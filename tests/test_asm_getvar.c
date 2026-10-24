/* Host-only test: its libc is not linked into the Assembly module. */
#include "fb_omega.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
size_t fb_asm_make_getvar(const uint8_t *, size_t, uint8_t *);
int main(void) {
    uint8_t key[64], plain[FB_COMMAND_MAX], machine[FB_COMMAND_MAX];
    size_t n, b, expected, actual;
    unsigned long assertions=0;
    for (n=1; n<=58; ++n) {
        for (b=0; b<256; ++b) {
            memset(key, 'a', sizeof key);
            key[0]=(uint8_t)b;
            memset(plain, 0xa5, sizeof plain);
            memset(machine, 0xa5, sizeof machine);
            expected=fb_make_getvar(key,n,plain);
            actual=fb_asm_make_getvar(key,n,machine);
            if (expected!=actual || memcmp(plain,machine,sizeof plain)!=0) {
                fprintf(stderr,"FAIL n=%zu b=%zu c=%zu asm=%zu\n",n,b,expected,actual);
                return 1;
            }
            ++assertions;
        }
    }
    memset(plain,0xa5,sizeof plain);
    if(fb_asm_make_getvar(0,3,plain)!=0 || plain[0]!=0xa5) return 2;
    if(fb_asm_make_getvar(key,0,plain)!=0 || plain[0]!=0xa5) return 3;
    if(fb_asm_make_getvar(key,1,0)!=0) return 4;
    if(fb_asm_make_getvar(key,~(size_t)0,plain)!=0 || plain[0]!=0xa5) return 5;
    printf("PASS asm x86_64 equivalence: %lu byte-for-byte vectors + 4 invalid pointer/bound tests\n",assertions);
    return 0;
}
