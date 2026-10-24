#ifndef RAFAELIA_FB_OMEGA_H
#define RAFAELIA_FB_OMEGA_H
#include <stddef.h>
#include <stdint.h>
#define FB_COMMAND_MAX 64u
#define FB_REPLY_MAX 256u
#define FB_INFO_LIMIT 32u
/* L0 contains no I/O, allocation, platform calls or shared mutable state. */
typedef enum { FB_BAD=0, FB_OKAY=1, FB_FAIL=2, FB_INFO=3,
               FB_DATA=4, FB_TEXT=5 } fb_kind;
typedef enum { FB_WAIT=0, FB_DONE=1, FB_DEVICE_FAIL=2,
               FB_BLOCK_DATA=3, FB_PROTOCOL_ERROR=4 } fb_state;
typedef struct { fb_kind kind; const uint8_t *payload; size_t length;
                 uint32_t data_size; } fb_packet;
typedef struct { fb_state state; uint32_t messages; } fb_session;
/* Returns exact command byte length; zero means invalid. No NUL on wire. */
size_t fb_make_getvar(const uint8_t *name, size_t n, uint8_t output[FB_COMMAND_MAX]);
fb_kind fb_parse(const uint8_t *wire, size_t n, fb_packet *out);
void fb_session_init(fb_session *session);
fb_state fb_session_accept(fb_session *session, const fb_packet *packet);
#endif
