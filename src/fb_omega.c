#include "fb_omega.h"
static int valid_key(uint8_t c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') || c=='-' || c=='_' || c=='.';
}
size_t fb_make_getvar(const uint8_t *name, size_t n, uint8_t out[FB_COMMAND_MAX]) {
  static const uint8_t prefix[7] = {'g','e','t','v','a','r',':'};
  size_t i;
  if (!name || !out || n==0u || n>FB_COMMAND_MAX-7u) return 0u;
  for (i=0;i<n;++i) if (!valid_key(name[i])) return 0u;
  for (i=0;i<7u;++i) out[i]=prefix[i];
  for (i=0;i<n;++i) out[7u+i]=name[i];
  return n+7u;
}
static int prefix(const uint8_t *b, const char *x) {
  return b[0]==(uint8_t)x[0] && b[1]==(uint8_t)x[1]
    && b[2]==(uint8_t)x[2] && b[3]==(uint8_t)x[3];
}
fb_kind fb_parse(const uint8_t *wire, size_t n, fb_packet *out) {
  fb_kind k=FB_BAD; uint32_t size=0u; size_t i;
  if (!out) return FB_BAD;
  out->kind=FB_BAD; out->payload=0; out->length=0u; out->data_size=0u;
  if (!wire || n<4u || n>FB_REPLY_MAX) return FB_BAD;
  if (prefix(wire,"OKAY")) k=FB_OKAY;
  else if (prefix(wire,"FAIL")) k=FB_FAIL;
  else if (prefix(wire,"INFO")) k=FB_INFO;
  else if (prefix(wire,"TEXT")) k=FB_TEXT;
  else if (prefix(wire,"DATA")) {
    if (n!=12u) return FB_BAD;
    for(i=4u;i<12u;++i) {
      uint8_t c=wire[i]; uint32_t d;
      if(c>='0' && c<='9') d=(uint32_t)(c-'0');
      else if(c>='A' && c<='F') d=(uint32_t)(c-'A'+10u);
      else if(c>='a' && c<='f') d=(uint32_t)(c-'a'+10u);
      else return FB_BAD;
      size=(size<<4u)|d;
    }
    k=FB_DATA;
  } else return FB_BAD;
  out->kind=k; out->payload=wire+4u; out->length=n-4u; out->data_size=size;
  return k;
}
void fb_session_init(fb_session *s) { if(s){s->state=FB_WAIT;s->messages=0u;} }
fb_state fb_session_accept(fb_session *s, const fb_packet *p) {
  if(!s) return FB_PROTOCOL_ERROR;
  if(s->state!=FB_WAIT) return s->state;
  if(!p || p->kind==FB_BAD) return (s->state=FB_PROTOCOL_ERROR);
  if(p->kind==FB_INFO || p->kind==FB_TEXT) {
    if(s->messages>=FB_INFO_LIMIT) return (s->state=FB_PROTOCOL_ERROR);
    ++s->messages; return s->state;
  }
  if(p->kind==FB_OKAY) return (s->state=FB_DONE);
  if(p->kind==FB_FAIL) return (s->state=FB_DEVICE_FAIL);
  if(p->kind==FB_DATA) return (s->state=FB_BLOCK_DATA);
  return (s->state=FB_PROTOCOL_ERROR);
}
