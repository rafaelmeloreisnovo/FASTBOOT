#include "fb_guard.h"
#include <stdio.h>
#define CHECK(x) do { if(!(x)){printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}++passed;} while(0)
#define STEP(g,e,w,n,t) fb_guard_step(&(g),(e),(w),(n),(t))
int main(void) {
  int passed=0; unsigned int i;fb_guard g;fb_guard h; uint32_t seed=0x13579bdfu;
  static const uint8_t okay[]={'O','K','A','Y','p','i','x','e','l'};
  static const uint8_t fail[]={'F','A','I','L'};
  static const uint8_t info[]={'I','N','F','O'};
  static const uint8_t data[]={'D','A','T','A','0','0','0','0','0','0','1','0'};
  fb_guard_init(&g,100u);
  CHECK(g.stage==FB_G_NEW && g.remaining_ms==100u && g.reason==FB_G_NONE);
  STEP(g,FB_EV_BIND,0,0,1); CHECK(g.stage==FB_G_TX && g.remaining_ms==99u);
  STEP(g,FB_EV_TX_COMPLETE,0,0,2); CHECK(g.stage==FB_G_RX && g.remaining_ms==97u);
  STEP(g,FB_EV_REPLY,info,sizeof info,3); CHECK(g.stage==FB_G_RX && g.protocol.messages==1u);
  STEP(g,FB_EV_REPLY,okay,sizeof okay,2); CHECK(g.stage==FB_G_COMPLETE && g.reason==FB_G_NONE);
  STEP(g,FB_EV_IO_FAIL,0,0,1); CHECK(g.stage==FB_G_COMPLETE);
  STEP(g,FB_EV_CLOSE,0,0,0); CHECK(g.stage==FB_G_CLOSED && g.remaining_ms==0u);
  STEP(g,FB_EV_BIND,0,0,0); CHECK(g.stage==FB_G_CLOSED);
  fb_guard_init(&g,100); STEP(g,FB_EV_TX_COMPLETE,0,0,0);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_BAD_EVENT);
  STEP(g,FB_EV_BIND,0,0,0); CHECK(g.stage==FB_G_QUARANTINE);
  fb_guard_init(&g,100); STEP(g,FB_EV_BIND,0,0,0);
  STEP(g,FB_EV_BIND,0,0,0); CHECK(g.reason==FB_G_BAD_EVENT);
  fb_guard_init(&g,100); STEP(g,FB_EV_BIND,0,0,0);
  STEP(g,FB_EV_TX_COMPLETE,0,0,0);
  STEP(g,FB_EV_REPLY,fail,sizeof fail,0);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_DEVICE_FAIL);
  fb_guard_init(&g,100); STEP(g,FB_EV_BIND,0,0,0); STEP(g,FB_EV_TX_COMPLETE,0,0,0);
  STEP(g,FB_EV_REPLY,data,sizeof data,0);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_DATA_BLOCKED);
  fb_guard_init(&g,100); STEP(g,FB_EV_BIND,0,0,0);STEP(g,FB_EV_TX_COMPLETE,0,0,0);
  STEP(g,FB_EV_REPLY,0,0,0);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_PROTOCOL);
  fb_guard_init(&g,10); STEP(g,FB_EV_BIND,0,0,10);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_TIMEOUT);
  fb_guard_init(&g,10); STEP(g,FB_EV_BIND,0,0,9);
  CHECK(g.stage==FB_G_TX && g.remaining_ms==1);
  STEP(g,FB_EV_TX_COMPLETE,0,0,2); CHECK(g.reason==FB_G_TIMEOUT);
  fb_guard_init(&g,0); CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_BAD_BUDGET);
  fb_guard_init(&g,100); STEP(g,FB_EV_DETACH,0,0,1);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_DETACHED);
  fb_guard_init(&g,100); STEP(g,FB_EV_BIND,0,0,1);
  STEP(g,FB_EV_IO_FAIL,0,0,0);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_IO);
  fb_guard_init(&g,100);STEP(g,FB_EV_BIND,0,0,0);STEP(g,FB_EV_TX_COMPLETE,0,0,0);
  for(i=0;i<32;i++) STEP(g,FB_EV_REPLY,info,sizeof info,0);
  CHECK(g.stage==FB_G_RX && g.protocol.messages==32);
  STEP(g,FB_EV_REPLY,info,sizeof info,0);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_PROTOCOL);
  fb_guard_init(&g,100); STEP(g,FB_EV_REPLY,okay,sizeof okay,0);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_BAD_EVENT);
  fb_guard_init(&g,100);STEP(g,FB_EV_BIND,0,0,0); STEP(g,FB_EV_TX_COMPLETE,0,0,0);
  STEP(g,FB_EV_REPLY,okay,sizeof okay,100);
  CHECK(g.stage==FB_G_QUARANTINE && g.reason==FB_G_TIMEOUT);
  fb_guard_init(&h,100); fb_guard_step(0,FB_EV_BIND,0,0,1);
  CHECK(h.stage==FB_G_NEW);
  /* Deterministic malformed-event sweep: no reopening terminal states. */
  for(i=0;i<8192u;i++){
    fb_guard_stage terminal;fb_guard_reason reason;
    seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;
    fb_guard_init(&g,(seed&31u)+1u);
    STEP(g,FB_EV_BIND,0,0,0);STEP(g,FB_EV_TX_COMPLETE,0,0,0);
    STEP(g,(fb_guard_event)(seed&7u),okay,sizeof okay,seed>>24);
    terminal=g.stage;reason=g.reason;
    STEP(g,FB_EV_BIND,0,0,0);
    if(terminal==FB_G_QUARANTINE || terminal==FB_G_COMPLETE)
      CHECK(g.stage==terminal && g.reason==reason);
  }
  printf("PASS fb_guard deterministic watchdog, fail-closed and terminal fuzz; assertions=%d\n",passed);
  return 0;
}
