/* AArch64 AAPCS64: x0=key, x1=length, x2=dst, x0=result.
 * Disjoint input/output buffers. No stack, heap, libc, syscall or OS call.
 */
.text
.balign 4
.global fb_asm_make_getvar
.type fb_asm_make_getvar, %function
fb_asm_make_getvar:
    cbz     x0, .Lbad
    cbz     x2, .Lbad
    cbz     x1, .Lbad
    cmp     x1, #57
    b.hi    .Lbad
    mov     x3, #0
.Lverify:
    ldrb    w4, [x0, x3]
    cmp     w4, #97
    b.lo    .Lupper
    cmp     w4, #122
    b.ls    .Lvalid
.Lupper:
    cmp     w4, #65
    b.lo    .Ldigit
    cmp     w4, #90
    b.ls    .Lvalid
.Ldigit:
    cmp     w4, #48
    b.lo    .Lpunct
    cmp     w4, #57
    b.ls    .Lvalid
.Lpunct:
    cmp     w4, #45
    b.eq    .Lvalid
    cmp     w4, #95
    b.eq    .Lvalid
    cmp     w4, #46
    b.ne    .Lbad
.Lvalid:
    add     x3, x3, #1
    cmp     x3, x1
    b.lo    .Lverify
    mov     w4, #103
    strb    w4, [x2, #0]
    mov     w4, #101
    strb    w4, [x2, #1]
    mov     w4, #116
    strb    w4, [x2, #2]
    mov     w4, #118
    strb    w4, [x2, #3]
    mov     w4, #97
    strb    w4, [x2, #4]
    mov     w4, #114
    strb    w4, [x2, #5]
    mov     w4, #58
    strb    w4, [x2, #6]
    add     x2, x2, #7
    mov     x3, #0
.Lcopy:
    ldrb    w4, [x0, x3]
    strb    w4, [x2, x3]
    add     x3, x3, #1
    cmp     x3, x1
    b.lo    .Lcopy
    add     x0, x1, #7
    ret
.Lbad:
    mov     x0, #0
    ret
.size fb_asm_make_getvar, .-fb_asm_make_getvar
