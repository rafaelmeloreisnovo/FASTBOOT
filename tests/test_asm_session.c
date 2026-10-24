/* Host-only reference harness. libc is NOT linked to Assembly objects. */
#include "fb_omega.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
_Static_assert(sizeof(fb_session)==8, "session ABI must be 8 bytes");
_Static_assert(offsetof(fb_session, messages)==4, "message counter ABI offset");
fb_state fb_asm_session_step(fb_session *, uint32_t);
static uint32_t seed=0x136b29a5u;
static unsigned long checks;
static uint32_t rnd(void) {
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  return seed;
}
static int check(fb_state state, uint32_t count, uint32_t kind) {
  fb_session baseline, machine;
  fb_packet p;
  fb_state a, b;
  baseline.state=state; baseline.messages=count;
  machine=baseline;
  memset(&p, 0, sizeof p);
  p.kind=(fb_kind)kind;
  a=fb_session_accept(&baseline, &p);
  b=fb_asm_session_step(&machine,kind);
  if(a!=b || baseline.state!=machine.state || baseline.messages!=machine.messages) {
    fprintf(stderr,"DIFF state=%u count=%u kind=%u expected=%u actual=%u count0=%u count1=%u\n",
       (unsigned)state,(unsigned)count,(unsigned)kind,(unsigned)a,(unsigned)b,
       baseline.messages,machine.messages);
    return 1;
  }
  ++checks;
  return 0;
}
int main(void) {
  unsigned int s,k,i;
  uint32_t n;
  fb_session session;
  for(s=0;s<=4;s++) for(n=0;n<40;n++) for(k=0;k<9;k++)
    if(check((fb_state)s,n,k)) return 1;
  for(i=0;i<100000;i++) {
    s=rnd()%5u; n=rnd(); k=rnd()%256u;
    if(check((fb_state)s,n,k)) return 2;
  }
  fb_session_init(&session);
  for(i=0;i<32;i++) {
    if(fb_asm_session_step(&session,FB_INFO)!=FB_WAIT) return 3;
  }
  if(session.messages!=32 || fb_asm_session_step(&session,FB_INFO)!=FB_PROTOCOL_ERROR) return 4;
  if(fb_asm_session_step(&session,FB_OKAY)!=FB_PROTOCOL_ERROR) return 5;
  session.state=(fb_state)0xffffffffu;session.messages=20;
  if(fb_asm_session_step(&session,FB_OKAY)!=FB_PROTOCOL_ERROR || session.messages!=20 || session.state!=FB_PROTOCOL_ERROR) return 6;
  if(fb_asm_session_step(NULL,FB_OKAY)!=FB_PROTOCOL_ERROR) return 7;
  fb_session_init(&session);
  if(fb_asm_session_step(&session,FB_DATA)!=FB_BLOCK_DATA) return 8;
  if(fb_asm_session_step(&session,FB_OKAY)!=FB_BLOCK_DATA) return 9;
  printf("PASS: %lu C-reference ASM state differential vectors + invalid-state quarantine and terminal tests\n",checks);
  return 0;
}
