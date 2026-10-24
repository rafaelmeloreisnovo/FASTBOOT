#include "fb_guard.h"
void fb_guard_init(fb_guard *g, uint32_t budget) {
  if (!g) return;
  fb_session_init(&g->protocol);
  g->remaining_ms=budget;
  g->stage=budget ? FB_G_NEW : FB_G_QUARANTINE;
  g->reason=budget ? FB_G_NONE : FB_G_BAD_BUDGET;
}
void fb_guard_step(fb_guard *g, fb_guard_event e,
                   const uint8_t *wire, size_t length, uint32_t elapsed) {
  fb_packet packet;
  fb_state state;
  if (!g || g->stage==FB_G_CLOSED) return;
  if (e==FB_EV_CLOSE) { g->stage=FB_G_CLOSED; g->remaining_ms=0u; return; }
  if (g->stage==FB_G_QUARANTINE || g->stage==FB_G_COMPLETE) return;
  if (elapsed >= g->remaining_ms) {
    g->stage=FB_G_QUARANTINE; g->reason=FB_G_TIMEOUT;
    g->remaining_ms=0u; return;
  }
  g->remaining_ms-=elapsed;
  if (e==FB_EV_DETACH || e==FB_EV_IO_FAIL) {
    g->stage=FB_G_QUARANTINE;
    g->reason=(e==FB_EV_DETACH) ? FB_G_DETACHED : FB_G_IO;
    return;
  }
  if (e==FB_EV_BIND && g->stage==FB_G_NEW) {g->stage=FB_G_TX; return;}
  if (e==FB_EV_TX_COMPLETE && g->stage==FB_G_TX) {g->stage=FB_G_RX; return;}
  if (e==FB_EV_REPLY && g->stage==FB_G_RX) {
    if (fb_parse(wire,length,&packet)==FB_BAD) {
      g->stage=FB_G_QUARANTINE;g->reason=FB_G_PROTOCOL;return;
    }
    state=fb_session_accept(&g->protocol,&packet);
    if(state==FB_WAIT) return;
    if(state==FB_DONE) {g->stage=FB_G_COMPLETE;return;}
    g->stage=FB_G_QUARANTINE;
    g->reason=(state==FB_DEVICE_FAIL) ? FB_G_DEVICE_FAIL :
              (state==FB_BLOCK_DATA) ? FB_G_DATA_BLOCKED : FB_G_PROTOCOL;
    return;
  }
  g->stage=FB_G_QUARANTINE;g->reason=FB_G_BAD_EVENT;
}
