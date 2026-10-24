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
int main(int argc, char **argv) {
  unsigned long iface,epout,epin;int fd=-1, claimed=0,rc=1;
  uint8_t tx[FB_COMMAND_MAX],rx[FB_REPLY_MAX]; size_t txlen;
  fb_guard g;struct usbdevfs_bulktransfer x;struct timespec a,b;
  if(argc!=7 || !uint_arg(argv[2],255u,&iface) ||
      !uint_arg(argv[3],255u,&epout) || !uint_arg(argv[4],255u,&epin) ||
      epout<1u || epout>15u || epin<0x81u || epin>0x8fu ||
      strcmp(argv[5],"getvar")) {
    fprintf(stderr,"usage: %s /dev/bus/usb/BBB/DDD INTERFACE EP_OUT EP_IN getvar KEY\n",argv[0]);return 2;
  }
  txlen=fb_make_getvar((const uint8_t *)argv[6],strlen(argv[6]),tx);
  if(txlen==0u){fputs("getvar key rejected\n",stderr);return 2;}
  fd=open(argv[1],O_RDWR|O_CLOEXEC);
  if(fd<0){perror("open USB device");return 1;}
  if(!usb_verify_fastboot_iface(fd,(unsigned)iface,(unsigned)epout,(unsigned)epin)) {
    fputs("FAIL CLOSED: USB active config is not exactly a matching FF/42/03 two-BULK Fastboot interface\n",stderr);goto done;
  }
  {int num=(int)iface;if(ioctl(fd,USBDEVFS_CLAIMINTERFACE,&num)<0){perror("claim interface");goto done;}claimed=1;}
  fb_guard_init(&g,FB_GUARD_BUDGET_MS);
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
    if(g.reason!=FB_G_PROTOCOL && g.reason!=FB_G_TIMEOUT){
      printf("response ");print_bytes(rx,(size_t)got);putchar('\n');
    }
  }
  if(g.stage==FB_G_COMPLETE){rc=0;puts("GETVAR_DONE (read-only; physical response)");}
  else fprintf(stderr,"GETVAR_NOT_SUCCESS reason=%u stage=%u\n",(unsigned)g.reason,(unsigned)g.stage);
 done:
  if(claimed){int num=(int)iface;ioctl(fd,USBDEVFS_RELEASEINTERFACE,&num);}
  close(fd);
  if(claimed) fb_guard_step(&g,FB_EV_CLOSE,0,0,0);
  return rc;
}
