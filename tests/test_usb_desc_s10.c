/* Test-only harness; no libc is linked to src/fb_usb_desc.c in target. */
#include "fb_usb_desc.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static const unsigned char base[32]={
9,2,32,0,1,1,0,128,50,9,4,2,0,2,255,66,3,0,7,5,2,2,64,0,0,7,5,129,2,64,0,0};
static unsigned long checks;
static int test(const unsigned char *wire, unsigned int n, unsigned int expected){
 fb_usb_binding out={17u,18u,19u};
 unsigned int got=fb_usb_match_config(wire,n,2u,2u,129u,&out);
 if(got!=expected || (got && (out.interface_number!=2u || out.ep_out!=2u || out.ep_in!=129u)) ||
 (!got && (out.interface_number!=0u || out.ep_out!=0u || out.ep_in!=0u))) {
  fprintf(stderr,"FAIL S10 case=%lu got=%u want=%u\n",checks,got,expected);return 1;
 }
 checks++;return 0;
}
int main(void){
 unsigned char wire[48];unsigned int i;uint32_t seed=0x5896e7b1u;
 if(test(base,32u,1u))return 1;
 memcpy(wire,base,32);wire[4]=2; if(test(wire,32u,0u))return 2;
 /* Two alternate-zero instances of the same interface: duplicate, reject. */
 memcpy(wire,base,32);memcpy(wire+32,base+9,9);
 wire[2]=41;wire[4]=2;wire[32+4]=0;
 if(test(wire,41u,0u))return 3;
 /* Different second interface, alt=0 and zero endpoint count: valid composite. */
 wire[32+2]=3;wire[32+4]=0;
 if(test(wire,41u,1u))return 4;
 /* Interface count mismatch on composite. */
 wire[4]=1; if(test(wire,41u,0u))return 5;
 /* Orphan alternate setting before base: reject. */
 memcpy(wire,base,32);wire[12]=1;
 if(test(wire,32u,0u))return 6;
 /* Existing alt=0 followed by alt=1 for that interface: only one interface. */
 memcpy(wire,base,32);memcpy(wire+32,base+9,9);
 wire[2]=41;wire[32+3]=1;wire[32+4]=0;
 if(test(wire,41u,1u))return 7;
 /* A huge number of fuzzed, bounded, adversarial descriptor variants. */
 for(i=0;i<120000u;i++){
  unsigned int n,j;
  seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;
  n=seed%49u;
  memcpy(wire,base,32);
  for(j=0;j<48u;j++){
   seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;
   if((seed&7u)==0u) wire[j]=(unsigned char)seed;
  }
  /* Valid outputs must always match the selected interface/endpoints.
   * Negative results must zero output. This fuzz is a safety invariant,
   * not an oracle proving all configurations are correctly classified.
   */
  fb_usb_binding out={9u,9u,9u};
  unsigned int ok=fb_usb_match_config(wire,n,2u,2u,129u,&out);
  if((ok && (out.interface_number!=2u||out.ep_out!=2u||out.ep_in!=129u))||
     (!ok&&(out.interface_number||out.ep_out||out.ep_in)))return 8;
  checks++;
 }
 printf("PASS S10: %lu boundary, alternates, integrity and fuzz cases\n",checks);
 return 0;
}
