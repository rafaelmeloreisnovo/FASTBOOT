/* L1 notebook adapter: kernel usbfs and POSIX calls are OUTSIDE freestanding L0. */
#include "fb_guard.h"
#include "fb_usb_desc.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/usbdevice_fs.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
static int uint_arg(const char *str, unsigned long max, unsigned long *v) {
  char *end=0; unsigned long n;
  if(!str || !*str || *str=='-') return 0;
  errno=0; n=strtoul(str,&end,0);
  if(errno || !end || *end || n>max) return 0;
  *v=n; return 1;
}
/* The only supported host path shape is the Linux usbfs device node.
 * This reduces accidental ioctl delivery to unrelated device files; it
 * cannot prove that the path names a trustworthy Fastboot device.
 */
static int usb_path_shape_valid(const char *path) {
  static const char prefix[]="/dev/bus/usb/";
  size_t i;
  if(!path) return 0;
  for(i=0u;i<sizeof(prefix)-1u;++i) if(path[i]!=prefix[i]) return 0;
  for(i=sizeof(prefix)-1u;i<sizeof(prefix)-1u+3u;++i) {
    if(path[i]<'0'||path[i]>'9') return 0;
  }
  if(path[i++]!='/') return 0;
  if(path[i]<'0'||path[i]>'9') return 0;
  if(path[i+1u]<'0'||path[i+1u]>'9') return 0;
  if(path[i+2u]<'0'||path[i+2u]>'9') return 0;
  return path[i+3u]=='\0';
}
static void print_bytes(const uint8_t *s,size_t n) {
  size_t i;
  for(i=0;i<n;++i){ unsigned char c=s[i];
    if(c>=32u && c<=126u) putchar(c);
    else printf("\\x%02X",(unsigned)c);
  }
}
static uint32_t elapsed_ms(const struct timespec *a, const struct timespec *b) {
  uint64_t sec, ms;int64_t ns;
  if(b->tv_sec<a->tv_sec || (b->tv_sec==a->tv_sec && b->tv_nsec<a->tv_nsec)) return UINT32_MAX;
  sec=(uint64_t)(b->tv_sec-a->tv_sec);
  if(sec>=((uint64_t)UINT32_MAX/1000u)) return UINT32_MAX;
  ms=sec*1000u;
  ns=(int64_t)b->tv_nsec-(int64_t)a->tv_nsec;
  if(ns<0) {ms-=1000u;ns+=1000000000LL;}
  ms+=(uint64_t)ns/1000000u;
  return ms>=UINT32_MAX ? UINT32_MAX : (uint32_t)ms;
}
/* L1 only: Linux usbfs control read. This code is NOT freestanding and
 * never changes configuration, detaches kernel drivers or writes firmware.
 * Verify ACTIVE configuration by value, not just config index zero.
 */
static int usb_control_read(int fd, unsigned int request, unsigned int value,
                             void *buf, unsigned int len) {
  struct usbdevfs_ctrltransfer c;
  int result;
  memset(&c,0,sizeof(c));
  c.bRequestType=0x80u;c.bRequest=(unsigned char)request;
  c.wValue=(unsigned short)value;c.wIndex=0u;
  c.wLength=(unsigned short)len;c.timeout=2500u;c.data=buf;
  result=ioctl(fd,USBDEVFS_CONTROL,&c);
  return result==(int)len;
}
static int usb_verify_fastboot_iface(int fd, unsigned int iface,
                                      unsigned int epout, unsigned int epin) {
  unsigned char active=0, device[18], head[9], conf[1024];
  unsigned int idx, count;
  fb_usb_binding binding;
  if(!usb_control_read(fd,8u,0u,&active,1u) || active==0u) return 0;
  if(!usb_control_read(fd,6u,0x0100u,device,18u) ||
     device[0]!=18u || device[1]!=1u) return 0;
  count=device[17];
  if(count<1u || count>4u) return 0;
  for(idx=0u;idx<count;++idx) {
    unsigned int total;
    if(!usb_control_read(fd,6u,0x0200u|idx,head,9u) ||
       head[0]!=9u || head[1]!=2u) return 0;
    total=(unsigned int)head[2]+((unsigned int)head[3]<<8u);
    if(total<9u || total>sizeof(conf)) return 0;
    if(!usb_control_read(fd,6u,0x0200u|idx,conf,total)) return 0;
    if(conf[5]!=active) continue;
    return fb_usb_match_config(conf,total,iface,epout,epin,&binding)==1u;
  }
  return 0;
}
/* Revalidate the claimed interface's CURRENT alternate setting before TX.
 * USB GET_INTERFACE: standard IN request to an interface. This is L1 only.
 * Treat unsupported/stalled requests as unsafe instead of assuming alt=0.
 */
static int usb_get_interface_alt_zero(int fd, unsigned int iface) {
  struct usbdevfs_ctrltransfer c;
  unsigned char alt=255u;
  int n;
  memset(&c,0,sizeof(c));
  c.bRequestType=0x81u; /* USB_DIR_IN | STANDARD | RECIP_INTERFACE */
  c.bRequest=10u;       /* GET_INTERFACE */
  c.wValue=0u;
  c.wIndex=(unsigned short)iface;
  c.wLength=1u;
  c.timeout=2500u;
  c.data=&alt;
  n=ioctl(fd,USBDEVFS_CONTROL,&c);
  return n==1 && alt==0u;
}
/* Clear potentially identifying responses and requested getvar key at exit.
 * Volatile writes are required so optimizers do not remove this scrub.
 * Does not claim full platform memory or swap sanitization.
 */
static void wipe_private(void *buffer, size_t n) {
  volatile uint8_t *p=(volatile uint8_t *)buffer;
  while(n>0u){--n;p[n]=0u;}
}
int main(int argc, char **argv) {
  unsigned long iface,epout,epin;int fd=-1, claimed=0,rc=1;
  int show_value=(argc==8 && strcmp(argv[7],"--show-value")==0);
  uint8_t tx[FB_COMMAND_MAX]={0},rx[FB_REPLY_MAX]={0}; size_t txlen;
  fb_guard g;struct usbdevfs_bulktransfer x;struct timespec a,b;
  if((argc!=7 && argc!=8) || (argc==8 && !show_value) ||
      !uint_arg(argv[2],255u,&iface) ||
      !uint_arg(argv[3],255u,&epout) || !uint_arg(argv[4],255u,&epin) ||
      epout<1u || epout>15u || epin<0x81u || epin>0x8fu ||
      strcmp(argv[5],"getvar") || !usb_path_shape_valid(argv[1])) {
    fprintf(stderr,"usage: %s /dev/bus/usb/BBB/DDD INTERFACE EP_OUT EP_IN getvar KEY [--show-value]\n",argv[0]);rc=2;goto done;
  }
  txlen=fb_make_getvar((const uint8_t *)argv[6],strlen(argv[6]),tx);
  if(txlen==0u){fputs("getvar key rejected\n",stderr);rc=2;goto done;}
  fd=open(argv[1],O_RDWR|O_CLOEXEC|O_NOFOLLOW);
  if(fd<0){perror("open USB device");goto done;}
  if(!usb_verify_fastboot_iface(fd,(unsigned)iface,(unsigned)epout,(unsigned)epin)) {
    fputs("FAIL CLOSED: USB active config is not exactly a matching FF/42/03 two-BULK Fastboot interface\n",stderr);goto done;
  }
  /* Initialize before claim: fail-closed paths after claim must not close an
   * uninitialized guard. A claimed interface always gets released at done.
   */
  fb_guard_init(&g,FB_GUARD_BUDGET_MS);
  {int num=(int)iface;if(ioctl(fd,USBDEVFS_CLAIMINTERFACE,&num)<0){perror("claim interface");goto done;}claimed=1;}
  /* Do not trust descriptor selection made before claim: recheck active
   * configuration and currently selected alternate setting before bulk OUT.
   * This mitigates (but cannot eliminate) host/device state races.
   */
  if(!usb_verify_fastboot_iface(fd,(unsigned)iface,(unsigned)epout,(unsigned)epin) ||
     !usb_get_interface_alt_zero(fd,(unsigned)iface)) {
    fputs("FAIL CLOSED: claimed USB Fastboot interface changed or alt setting is not zero\n",stderr);
    goto done;
  }
  fb_guard_step(&g,FB_EV_BIND,0,0,0u);
  if(g.stage!=FB_G_TX)goto done;
  memset(&x,0,sizeof(x));x.ep=(unsigned)epout;x.len=(unsigned)txlen;
  x.timeout=2500;x.data=tx;
  if(clock_gettime(CLOCK_MONOTONIC,&a)<0){perror("monotonic clock");fb_guard_step(&g,FB_EV_IO_FAIL,0,0,0);goto done;}
  {int sent=ioctl(fd,USBDEVFS_BULK,&x);
    if(clock_gettime(CLOCK_MONOTONIC,&b)<0){perror("monotonic clock");fb_guard_step(&g,FB_EV_IO_FAIL,0,0,0);goto done;}
    if(sent!=(int)txlen){perror("bulk OUT incomplete");fb_guard_step(&g,FB_EV_IO_FAIL,0,0,elapsed_ms(&a,&b));goto done;}
  }
  fb_guard_step(&g,FB_EV_TX_COMPLETE,0,0,elapsed_ms(&a,&b));
  while(g.stage==FB_G_RX){int got;
    memset(&x,0,sizeof(x));x.ep=(unsigned)epin;x.len=sizeof(rx);
    x.timeout=2500;x.data=rx;
    if(clock_gettime(CLOCK_MONOTONIC,&a)<0){fb_guard_step(&g,FB_EV_IO_FAIL,0,0,0);break;}
    got=ioctl(fd,USBDEVFS_BULK,&x);
    if(clock_gettime(CLOCK_MONOTONIC,&b)<0){fb_guard_step(&g,FB_EV_IO_FAIL,0,0,0);break;}
    if(got<0){perror("bulk IN");fb_guard_step(&g,FB_EV_IO_FAIL,0,0,elapsed_ms(&a,&b));break;}
    fb_guard_step(&g,FB_EV_REPLY,rx,(size_t)got,elapsed_ms(&a,&b));
    /* The response payload can contain serial, account/device identifiers,
     * carrier data or arbitrary OEM values. Never emit it by default.
     * DATA is explicitly blocked and must not be printed as accepted output.
     * An explicit --show-value is the only path to raw payload display.
     */
    if(g.reason!=FB_G_PROTOCOL && g.reason!=FB_G_TIMEOUT &&
       g.reason!=FB_G_DATA_BLOCKED && got>=4){
      if(show_value){
        printf("response ");print_bytes(rx,(size_t)got);putchar('\n');
      } else {
        printf("response %.4s [payload redacted]\n",(const char *)rx);
      }
    }
  }
  if(g.stage==FB_G_COMPLETE){rc=0;puts("GETVAR_DONE (read-only transport response; physical transport not attested)");}
  else fprintf(stderr,"GETVAR_NOT_SUCCESS reason=%u stage=%u\n",(unsigned)g.reason,(unsigned)g.stage);
 done:
  if(claimed){int num=(int)iface;ioctl(fd,USBDEVFS_RELEASEINTERFACE,&num);}
  if(fd>=0) close(fd);
  wipe_private(tx,sizeof(tx));wipe_private(rx,sizeof(rx));
  if(claimed) fb_guard_step(&g,FB_EV_CLOSE,0,0,0);
  return rc;
}
