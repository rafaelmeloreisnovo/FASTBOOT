/* Host-only mock of Linux kernel usbfs. The mock invokes NO real USB I/O.
 * This test is NOT freestanding, unlike fb_usb_match_config L0.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
int fb_linux_usbfs_main(int argc, char **argv);
int __wrap_open(const char *path, int flags, ...);
int __wrap_close(int fd);
int __wrap_clock_gettime(clockid_t clock_id, struct timespec *tp);
int __wrap_ioctl(int fd, unsigned long cmd, ...);
static const unsigned char config[32]={9,2,32,0,1,1,0,128,50,9,4,2,0,2,255,66,3,0,7,5,2,2,64,0,0,7,5,129,2,64,0,0};
static int scenario, claimed, released, bulkout, bulkin, config_reads, alt_reads;
static int testfail(const char *s) {fprintf(stderr,"HOST MOCK FAIL %s scenario=%d\n",s,scenario);return 1;}
int __wrap_open(const char *path, int flags, ...){
 if(strcmp(path,"/dev/bus/usb/001/002")!=0 || !(flags&O_RDWR)) return -1;
 return 37;
}
int __wrap_close(int fd){return fd==37?0:-1;}
int __wrap_clock_gettime(clockid_t clock_id, struct timespec *tp){
 static unsigned int tick;
 if(clock_id!=CLOCK_MONOTONIC)return -1;
 tp->tv_sec=2;tp->tv_nsec=(long)((tick++%1000u)*1000000u);
 return 0;
}
int __wrap_ioctl(int fd, unsigned long cmd, ...){
 va_list args;void *arg;
 va_start(args,cmd);arg=va_arg(args,void*);va_end(args);
 if(fd!=37)return -1;
 if(cmd==USBDEVFS_CONTROL){
  struct usbdevfs_ctrltransfer *c=(struct usbdevfs_ctrltransfer *)arg;
  if(c->bRequest==8u && c->bRequestType==0x80u && c->wLength==1u){
   config_reads++;
   ((unsigned char*)c->data)[0]=(scenario==3 && claimed)?2u:1u;
   return 1;
  }
  if(c->bRequest==6u && c->bRequestType==0x80u){
   if(c->wValue==0x0100u && c->wLength==18u){
    unsigned char dev[18]={18,1,0,2,0,0,0,64,0,0,0,0,1,0,0,0,0,1};
    memcpy(c->data,dev,18);return 18;
   }
   if(c->wValue==0x0200u && c->wLength==9u){memcpy(c->data,config,9);return 9;}
   if(c->wValue==0x0200u && c->wLength==32u){
    unsigned char d[32];memcpy(d,config,32);
    if(scenario==4)d[4]=2u; /* reject inconsistent bNumInterfaces */
    memcpy(c->data,d,32);return 32;
   }
  }
  if(c->bRequest==10u && c->bRequestType==0x81u && c->wLength==1u){
   alt_reads++;
   if(scenario==2){errno=EPIPE;return -1;} /* GET_INTERFACE stall */
   ((unsigned char*)c->data)[0]=(scenario==1)?1u:0u;
   return 1;
  }
  errno=EINVAL;return -1;
 }
 if(cmd==USBDEVFS_CLAIMINTERFACE){claimed++;return 0;}
 if(cmd==USBDEVFS_RELEASEINTERFACE){released++;return 0;}
 if(cmd==USBDEVFS_BULK){
  struct usbdevfs_bulktransfer *bulk=(struct usbdevfs_bulktransfer*)arg;
  if(bulk->ep==2u){
   if(bulk->len!=14u || memcmp(bulk->data,"getvar:version",14u))return -1;
   bulkout++;return 14;
  }
  if(bulk->ep==129u){
   memcpy(bulk->data,"OKAY1",5);bulkin++;return 5;
  }
  return -1;
 }
 errno=EINVAL;return -1;
}
int main(void){
 int s;
 char *argv[]={(char*)"fb-linux",(char*)"/dev/bus/usb/001/002",(char*)"2",(char*)"2",
 (char*)"129",(char*)"getvar",(char*)"version",NULL};
 for(s=0;s<5;s++){
  int rc;
  scenario=s;claimed=0;released=0;bulkout=0;bulkin=0;config_reads=0;alt_reads=0;
  rc=fb_linux_usbfs_main(7,argv);
  if(s==0){
   if(rc!=0||claimed!=1||released!=1||bulkout!=1||bulkin!=1||config_reads!=2||alt_reads!=1)return testfail("positive gate");
  } else if(s==4){
   if(rc==0||claimed!=0||released!=0||bulkout!=0||bulkin!=0)return testfail("preclaim negative gate");
  } else {
   if(rc==0||claimed!=1||released!=1||bulkout!=0||bulkin!=0)return testfail("postclaim negative gate");
  }
 }
 puts("PASS S10: mock USB Linux 5 scenarios, zero real USB I/O, rejects alt1/stall/reconfiguration and malformed count before TX");
 return 0;
}
