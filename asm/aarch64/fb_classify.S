/* AArch64 AAPCS64: x0=wire, x1=length, x2=data_size_out;
 * w0=fb_kind. Uses x3/w3,x4/w4,x5 as caller-saved scratch.
 * No libc, heap, stack or OS calls. Byte-addressed input.
 */
.text
.balign 4
.global fb_asm_classify_reply
.type fb_asm_classify_reply, %function
fb_asm_classify_reply:
    cbz     x0, .Lbad
    cbz     x2, .Lbad
    cmp     x1, #4
    b.lo    .Lbad
    cmp     x1, #256
    b.hi    .Lbad
    ldrb    w3, [x0]
    cmp     w3, #79
    b.eq    .Lok
    cmp     w3, #70
    b.eq    .Lfail
    cmp     w3, #73
    b.eq    .Linfo
    cmp     w3, #84
    b.eq    .Ltext
    cmp     w3, #68
    b.ne    .Lbad
    ldrb    w3, [x0, #1]
    cmp     w3, #65
    b.ne    .Lbad
    ldrb    w3, [x0, #2]
    cmp     w3, #84
    b.ne    .Lbad
    ldrb    w3, [x0, #3]
    cmp     w3, #65
    b.ne    .Lbad
    cmp     x1, #12
    b.ne    .Lbad
    mov     w3, #0
    mov     x5, #4
.Lhex:
    ldrb    w4, [x0, x5]
    cmp     w4, #48
    b.lo    .Lbad
    cmp     w4, #57
    b.ls    .Ldecimal
    cmp     w4, #65
    b.lo    .Llower
    cmp     w4, #70
    b.ls    .Lupper
.Llower:
    cmp     w4, #97
    b.lo    .Lbad
    cmp     w4, #102
    b.hi    .Lbad
    sub     w4, w4, #87
    b       .Laccumulate
.Lupper:
    sub     w4, w4, #55
    b       .Laccumulate
.Ldecimal:
    sub     w4, w4, #48
.Laccumulate:
    lsl     w3, w3, #4
    orr     w3, w3, w4
    add     x5, x5, #1
    cmp     x5, #12
    b.lo    .Lhex
    str     w3, [x2]
    mov     w0, #4
    ret
.Lok:
    ldrb    w3, [x0, #1]
    cmp     w3, #75
    b.ne    .Lbad
    ldrb    w3, [x0, #2]
    cmp     w3, #65
    b.ne    .Lbad
    ldrb    w3, [x0, #3]
    cmp     w3, #89
    b.ne    .Lbad
    str     wzr, [x2]
    mov     w0, #1
    ret
.Lfail:
    ldrb    w3, [x0, #1]
    cmp     w3, #65
    b.ne    .Lbad
    ldrb    w3, [x0, #2]
    cmp     w3, #73
    b.ne    .Lbad
    ldrb    w3, [x0, #3]
    cmp     w3, #76
    b.ne    .Lbad
    str     wzr, [x2]
    mov     w0, #2
    ret
.Linfo:
    ldrb    w3, [x0, #1]
    cmp     w3, #78
    b.ne    .Lbad
    ldrb    w3, [x0, #2]
    cmp     w3, #70
    b.ne    .Lbad
    ldrb    w3, [x0, #3]
    cmp     w3, #79
    b.ne    .Lbad
    str     wzr, [x2]
    mov     w0, #3
    ret
.Ltext:
    ldrb    w3, [x0, #1]
    cmp     w3, #69
    b.ne    .Lbad
    ldrb    w3, [x0, #2]
    cmp     w3, #88
    b.ne    .Lbad
    ldrb    w3, [x0, #3]
    cmp     w3, #84
    b.ne    .Lbad
    str     wzr, [x2]
    mov     w0, #5
    ret
.Lbad:
    mov     w0, #0
    ret
.size fb_asm_classify_reply, .-fb_asm_classify_reply
