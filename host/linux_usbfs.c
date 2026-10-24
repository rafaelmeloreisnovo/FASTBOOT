/* L1 adapter: Linux-specific and NOT freestanding. Only getvar; no flash paths. */
#include "fb_omega.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/usbdevice_fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
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
int main(int argc, char **argv) {
  unsigned long iface,epout,epin;int fd=-1, claimed=0,rc=1;
  uint8_t tx[FB_COMMAND_MAX],rx[FB_REPLY_MAX]; size_t txlen;
  fb_packet pkt; fb_session s; struct usbdevfs_bulktransfer x;
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
  {int num=(int)iface; if(ioctl(fd,USBDEVFS_CLAIMINTERFACE,&num)<0){perror("claim interface");goto done;}claimed=1;}
  memset(&x,0,sizeof(x));x.ep=(unsigned)epout;x.len=(unsigned)txlen;
  x.timeout=2500;x.data=tx;
  if(ioctl(fd,USBDEVFS_BULK,&x)!=(int)txlen){perror("bulk OUT");goto done;}
  fb_session_init(&s);
  while(s.state==FB_WAIT){int got;
    memset(&x,0,sizeof(x));x.ep=(unsigned)epin;x.len=sizeof(rx);
    x.timeout=2500;x.data=rx;
    got=ioctl(fd,USBDEVFS_BULK,&x);
    if(got<0){perror("bulk IN");goto done;}
    if(fb_parse(rx,(size_t)got,&pkt)==FB_BAD){fputs("invalid response\n",stderr);goto done;}
    printf("response ");print_bytes(rx,(size_t)got);putchar('\n');
    fb_session_accept(&s,&pkt);
  }
  if(s.state==FB_DONE){rc=0;puts("GETVAR_DONE (device response, no firmware write)");}
  else fprintf(stderr,"GETVAR_NOT_SUCCESS state=%d\n",(int)s.state);
 done:
  if(claimed){int num=(int)iface;ioctl(fd,USBDEVFS_RELEASEINTERFACE,&num);}
  close(fd);return rc;
}
