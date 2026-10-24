/* ARMv7 AAPCS, ARM mode: r0=key, r1=length, r2=dst, r0=result.
 * Disjoint input/output buffers. No stack, libc, heap, syscall or OS call.
 * Validate before writing; return zero and leave dst untouched on error.
 */
.syntax unified
.arm
.text
.balign 4
.global fb_asm_make_getvar
.type fb_asm_make_getvar, %function
fb_asm_make_getvar:
    cmp     r0, #0
    beq     .Lbad
    cmp     r2, #0
    beq     .Lbad
    cmp     r1, #1
    blo     .Lbad
    cmp     r1, #57
    bhi     .Lbad
    mov     r3, #0
.Lverify:
    ldrb    r12, [r0, r3]
    cmp     r12, #97
    blo     .Lupper
    cmp     r12, #122
    bls     .Lvalid
.Lupper:
    cmp     r12, #65
    blo     .Ldigit
    cmp     r12, #90
    bls     .Lvalid
.Ldigit:
    cmp     r12, #48
    blo     .Lpunct
    cmp     r12, #57
    bls     .Lvalid
.Lpunct:
    cmp     r12, #45
    beq     .Lvalid
    cmp     r12, #95
    beq     .Lvalid
    cmp     r12, #46
    bne     .Lbad
.Lvalid:
    add     r3, r3, #1
    cmp     r3, r1
    blo     .Lverify
    mov     r12, #103
    strb    r12, [r2, #0]
    mov     r12, #101
    strb    r12, [r2, #1]
    mov     r12, #116
    strb    r12, [r2, #2]
    mov     r12, #118
    strb    r12, [r2, #3]
    mov     r12, #97
    strb    r12, [r2, #4]
    mov     r12, #114
    strb    r12, [r2, #5]
    mov     r12, #58
    strb    r12, [r2, #6]
    add     r2, r2, #7
    mov     r3, #0
.Lcopy:
    ldrb    r12, [r0, r3]
    strb    r12, [r2, r3]
    add     r3, r3, #1
    cmp     r3, r1
    blo     .Lcopy
    add     r0, r1, #7
    bx      lr
.Lbad:
    mov     r0, #0
    bx      lr
.size fb_asm_make_getvar, .-fb_asm_make_getvar
