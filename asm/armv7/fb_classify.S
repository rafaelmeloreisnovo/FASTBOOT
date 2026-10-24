/* ARMv7 AAPCS: r0=wire, r1=length, r2=data_size_out; return r0 kind.
 * L0 never performs IO, syscall, heap allocation, or external calls.
 * No output mutation on malformed response. Uses caller-saved registers.
 */
.syntax unified
.arm
.text
.balign 4
.global fb_asm_classify_reply
.type fb_asm_classify_reply, %function
fb_asm_classify_reply:
    cmp     r0, #0
    beq     .Lbad
    cmp     r2, #0
    beq     .Lbad
    cmp     r1, #4
    blo     .Lbad
    cmp     r1, #256
    bhi     .Lbad
    ldrb    r3, [r0]
    cmp     r3, #79
    beq     .Lok
    cmp     r3, #70
    beq     .Lfail
    cmp     r3, #73
    beq     .Linfo
    cmp     r3, #84
    beq     .Ltext
    cmp     r3, #68
    bne     .Lbad
    ldrb    r3, [r0, #1]
    cmp     r3, #65
    bne     .Lbad
    ldrb    r3, [r0, #2]
    cmp     r3, #84
    bne     .Lbad
    ldrb    r3, [r0, #3]
    cmp     r3, #65
    bne     .Lbad
    cmp     r1, #12
    bne     .Lbad
    mov     r3, #0
    mov     r12, #4
.Lhex:
    ldrb    r1, [r0, r12]
    cmp     r1, #48
    blo     .Lbad
    cmp     r1, #57
    bls     .Ldecimal
    cmp     r1, #65
    blo     .Llower
    cmp     r1, #70
    bls     .Lupper
.Llower:
    cmp     r1, #97
    blo     .Lbad
    cmp     r1, #102
    bhi     .Lbad
    sub     r1, r1, #87
    b       .Laccumulate
.Lupper:
    sub     r1, r1, #55
    b       .Laccumulate
.Ldecimal:
    sub     r1, r1, #48
.Laccumulate:
    lsl     r3, r3, #4
    orr     r3, r3, r1
    add     r12, r12, #1
    cmp     r12, #12
    blo     .Lhex
    str     r3, [r2]
    mov     r0, #4
    bx      lr
.Lok:
    ldrb    r3, [r0, #1]
    cmp     r3, #75
    bne     .Lbad
    ldrb    r3, [r0, #2]
    cmp     r3, #65
    bne     .Lbad
    ldrb    r3, [r0, #3]
    cmp     r3, #89
    bne     .Lbad
    mov     r3, #0
    str     r3, [r2]
    mov     r0, #1
    bx      lr
.Lfail:
    ldrb    r3, [r0, #1]
    cmp     r3, #65
    bne     .Lbad
    ldrb    r3, [r0, #2]
    cmp     r3, #73
    bne     .Lbad
    ldrb    r3, [r0, #3]
    cmp     r3, #76
    bne     .Lbad
    mov     r3, #0
    str     r3, [r2]
    mov     r0, #2
    bx      lr
.Linfo:
    ldrb    r3, [r0, #1]
    cmp     r3, #78
    bne     .Lbad
    ldrb    r3, [r0, #2]
    cmp     r3, #70
    bne     .Lbad
    ldrb    r3, [r0, #3]
    cmp     r3, #79
    bne     .Lbad
    mov     r3, #0
    str     r3, [r2]
    mov     r0, #3
    bx      lr
.Ltext:
    ldrb    r3, [r0, #1]
    cmp     r3, #69
    bne     .Lbad
    ldrb    r3, [r0, #2]
    cmp     r3, #88
    bne     .Lbad
    ldrb    r3, [r0, #3]
    cmp     r3, #84
    bne     .Lbad
    mov     r3, #0
    str     r3, [r2]
    mov     r0, #5
    bx      lr
.Lbad:
    mov     r0, #0
    bx      lr
.size fb_asm_classify_reply, .-fb_asm_classify_reply
