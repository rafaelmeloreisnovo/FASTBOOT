#include "fb_usb_desc.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/* Configuration(9), vendor-specific interface(9), BULK OUT(7), BULK IN(7) */
static const unsigned char valid[32] = {
  9,2,32,0,1,1,0,128,50,
  9,4,2,0,2,255,66,3,0,
  7,5,2,2,64,0,0,
  7,5,0x81,2,64,0,0
};
static unsigned long checks;
static int check_one(const unsigned char *a, unsigned int n,unsigned int target,
 unsigned int out,unsigned int in,unsigned int expected) {
 fb_usb_binding x={0xa5a5u,0xa5a5u,0xa5a5u};
 unsigned int result=fb_usb_match_config(a,n,target,out,in,&x);
 if(result!=expected || (expected && (x.interface_number!=target || x.ep_out!=out || x.ep_in!=in)) ||
    (!expected && (x.interface_number || x.ep_out || x.ep_in))) return 1;
 checks++;return 0;
}
int main(void) {
 unsigned char d[1024];unsigned int p,b;uint32_t seed=0x91e10da5u;
 if(check_one(valid,32,2,2,129,1)) return 1;
 if(check_one(NULL,32,2,2,129,0)) return 2;
 for(p=0;p<32u;p++) {
  for(b=0;b<256u;b++) {
   memcpy(d,valid,32);d[p]=(unsigned char)b;
   fb_usb_binding x;unsigned int got=fb_usb_match_config(d,32,2,2,129,&x);
   if(got && (x.interface_number!=2u || x.ep_out!=2u || x.ep_in!=129u)) return 4;
   checks++;
  }
 }
 for(p=0;p<12000u;p++) {
  unsigned int k,n;
  seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;
  n=seed%1025u;
  for(k=0;k<n;k++) {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;d[k]=(unsigned char)seed;}
  fb_usb_binding x={9u,9u,9u};
  unsigned int ok=fb_usb_match_config(d,n,2,2,129,&x);
  if(ok && (x.interface_number!=2u||x.ep_out!=2u||x.ep_in!=129u)) return 5;
  if(!ok && (x.interface_number||x.ep_out||x.ep_in)) return 6;
  checks++;
 }
 memcpy(d,valid,32);d[14]=8;if(check_one(d,32,2,2,129,0)) return 7;
 memcpy(d,valid,32);d[21]=3;if(check_one(d,32,2,2,129,0)) return 8;
 memcpy(d,valid,32);d[26]=0;if(check_one(d,32,2,2,129,0)) return 9;
 memcpy(d,valid,32);d[12]=1;if(check_one(d,32,2,2,129,0)) return 10;
 memcpy(d,valid,32);d[2]=31;if(check_one(d,32,2,2,129,0)) return 11;
 if(fb_usb_match_config(valid,32,2,2,129,NULL)!=0u) return 12;
 printf("PASS: %lu valid, invalid, exhaustive byte mutation and seeded USB descriptor checks\n",checks);
 return 0;
}
