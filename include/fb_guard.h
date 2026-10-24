#ifndef RAFAELIA_FB_GUARD_H
#define RAFAELIA_FB_GUARD_H
#include "fb_omega.h"
#if defined(FB_OMEGA_ENABLE_WRITE) && (FB_OMEGA_ENABLE_WRITE != 0)
#error "FASTBOOT OMEGA L0 is strictly read-only: write build rejected"
#endif
#define FB_GUARD_BUDGET_MS 12000u
/* L0: zero USB I/O, OS calls, allocation, global mutable data, automatic retries. */
typedef enum {
  FB_G_NEW=0, FB_G_TX=1, FB_G_RX=2,
  FB_G_COMPLETE=3, FB_G_QUARANTINE=4, FB_G_CLOSED=5
} fb_guard_stage;
typedef enum {
  FB_G_NONE=0, FB_G_TIMEOUT=1, FB_G_BAD_EVENT=2,
  FB_G_IO=3, FB_G_DETACHED=4, FB_G_PROTOCOL=5,
  FB_G_DEVICE_FAIL=6, FB_G_DATA_BLOCKED=7, FB_G_BAD_BUDGET=8
} fb_guard_reason;
typedef enum {
  FB_EV_BIND=1, FB_EV_TX_COMPLETE=2, FB_EV_REPLY=3,
  FB_EV_IO_FAIL=4, FB_EV_DETACH=5, FB_EV_CLOSE=6
} fb_guard_event;
typedef struct {
  fb_session protocol;
  uint32_t remaining_ms;
  fb_guard_stage stage;
  fb_guard_reason reason;
} fb_guard;
/* Re-initialize ONLY after physical transport release; host must verify identity
   and permission afresh before next FB_EV_BIND. Never reset a live link. */
void fb_guard_init(fb_guard *guard, uint32_t budget_ms);
/* elapsed_ms measured by host monotonic clock. L0 owns no timer or USB device. */
void fb_guard_step(fb_guard *guard, fb_guard_event event,
                   const uint8_t *wire, size_t length, uint32_t elapsed_ms);
#endif
