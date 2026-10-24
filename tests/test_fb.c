#include "fb_omega.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;} ++passed; } while(0)
int main(void) {
  int passed=0; fb_packet p; fb_session s; uint8_t cmd[FB_COMMAND_MAX];
  static const uint8_t okay[]="OKAYpixel";
  static const uint8_t fail[]="FAILlocked";
  static const uint8_t info[]="INFOprogress";
  static const uint8_t text[]="TEXTdetails";
  static const uint8_t data[]="DATA00000010";
  static const uint8_t invalid[]="DATA0000000Z";
  static const uint8_t unknown[]="WRNG";
  static const uint8_t badcmd[]="flash:boot";
  static const uint8_t good[]="product";
  static const uint8_t long_reply[FB_REPLY_MAX+1]={0};
  CHECK(fb_make_getvar(good,7,cmd)==14u);
  CHECK(memcmp(cmd,"getvar:product",14)==0);
  CHECK(fb_make_getvar(badcmd,sizeof(badcmd)-1,cmd)==0);
  CHECK(fb_make_getvar(good,0,cmd)==0);
  CHECK(fb_make_getvar(0,7,cmd)==0);
  CHECK(fb_parse(okay,sizeof(okay)-1,&p)==FB_OKAY);
  CHECK(p.length==5 && memcmp(p.payload,"pixel",5)==0);
  fb_session_init(&s); CHECK(fb_session_accept(&s,&p)==FB_DONE);
  CHECK(fb_session_accept(&s,&p)==FB_DONE);
  CHECK(fb_parse(fail,sizeof(fail)-1,&p)==FB_FAIL);
  fb_session_init(&s); CHECK(fb_session_accept(&s,&p)==FB_DEVICE_FAIL);
  CHECK(fb_parse(info,sizeof(info)-1,&p)==FB_INFO);
  fb_session_init(&s); CHECK(fb_session_accept(&s,&p)==FB_WAIT);
  CHECK(fb_parse(text,sizeof(text)-1,&p)==FB_TEXT);
  CHECK(fb_session_accept(&s,&p)==FB_WAIT);
  CHECK(fb_parse(data,sizeof(data)-1,&p)==FB_DATA && p.data_size==16u);
  CHECK(fb_session_accept(&s,&p)==FB_BLOCK_DATA);
  CHECK(fb_parse(invalid,sizeof(invalid)-1,&p)==FB_BAD);
  CHECK(fb_parse(unknown,sizeof(unknown)-1,&p)==FB_BAD);
  CHECK(fb_parse(long_reply,sizeof(long_reply),&p)==FB_BAD);
  CHECK(fb_parse(okay,3,&p)==FB_BAD);
  CHECK(fb_parse(okay,sizeof(okay)-1,0)==FB_BAD);
  fb_session_init(&s); CHECK(s.state==FB_WAIT && s.messages==0);
  fb_parse(info,sizeof(info)-1,&p);
  {unsigned int i; for(i=0;i<FB_INFO_LIMIT;++i) CHECK(fb_session_accept(&s,&p)==FB_WAIT);}
  CHECK(fb_session_accept(&s,&p)==FB_PROTOCOL_ERROR);
  puts("PASS fb_omega L0 bounded protocol tests");printf("assertions=%d\n",passed);
  return 0;
}
