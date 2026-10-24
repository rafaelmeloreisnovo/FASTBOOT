/* Host-only differential harness; libc belongs to tests, NOT the Assembly modules. */
#include "fb_omega.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
fb_kind fb_asm_classify_reply(const uint8_t *, size_t, uint32_t *);
static unsigned long checks;
static int check(const uint8_t *s, size_t n) {
    fb_packet p;
    uint32_t d=0xa5a5a5a5u;
    fb_kind want=fb_parse(s,n,&p);
    fb_kind got=fb_asm_classify_reply(s,n,&d);
    if (want!=got || (got && p.data_size!=d) || (!got && d!=0xa5a5a5a5u)) {
        fprintf(stderr,"DIFF n=%zu want=%u got=%u size=%08x (want=%08x)\n",
                n,(unsigned)want,(unsigned)got,(unsigned)d,(unsigned)p.data_size);
        return 1;
    }
    checks++;
    return 0;
}
int main(void) {
    static const char *const prefixes[]={"OKAY", "FAIL", "INFO", "TEXT", "DATA", "XXX!"};
    uint8_t b[257]; uint32_t state=0x9e3779b9u;
    size_t n, p, c, i, k;
    for(p=0;p<sizeof(prefixes)/sizeof(prefixes[0]);++p) {
        for(n=0;n<=257;++n) {
            memset(b,'x',sizeof b);
            memcpy(b,prefixes[p],4);
            if(n==12 && p==4) memcpy(b,"DATA0012faB3",12);
            if(check(b,n)) return 1;
        }
    }
    for(i=0;i<8;++i) {
        memcpy(b,"DATAffffffff",12);
        b[4+i]=0;
        if(check(b,12)) return 2;
        b[4+i]='g';
        if(check(b,12)) return 2;
        b[4+i]='G';
        if(check(b,12)) return 2;
        b[4+i]='0';
        if(check(b,12)) return 2;
    }
    for(c=0;c<100000u;++c) {
        state^=state<<13; state^=state>>17;state^=state<<5;
        n=(size_t)(state%258u);
        for(k=0;k<257u;k++) {
            state^=state<<13;state^=state>>17;state^=state<<5;
            b[k]=(uint8_t)state;
        }
        if((c&7u)==0u) {
            memcpy(b,prefixes[c%6u],4);
            if((c&15u)==0u) memcpy(b,"DATA000000fF",12);
        }
        if(check(b,n)) return 3;
    }
    if(fb_asm_classify_reply(NULL,4,&state)!=FB_BAD) return 4;
    if(fb_asm_classify_reply(b,4,NULL)!=FB_BAD) return 5;
    if(fb_asm_classify_reply(NULL,0,NULL)!=FB_BAD) return 6;
    printf("PASS: %lu differential parse vectors + 3 pointer tests\n",checks);
    return 0;
}
