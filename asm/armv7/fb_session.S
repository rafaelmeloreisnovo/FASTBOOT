/* ARMv7 AAPCS, ARM mode. r0=fb_session* (8 writable bytes), r1=kind,
 * r0=result. Only r0-r3 altered; no libc/stack/system calls.
 * state: 0 WAIT, 1 DONE, 2 DEVICE_FAIL, 3 BLOCK_DATA, 4 PROTOCOL_ERROR.
 */
.syntax unified
.arm
.text
.balign 4
.global fb_asm_session_step
.type fb_asm_session_step, %function
fb_asm_session_step:
    cmp     r0, #0
    beq     .Lbadptr
    ldr     r2, [r0]
    cmp     r2, #4
    bhi     .Lprotocol
    cmp     r2, #0
    bne     .Lterminal
    cmp     r1, #3
    beq     .Linfo
    cmp     r1, #5
    beq     .Linfo
    cmp     r1, #1
    beq     .Lok
    cmp     r1, #2
    beq     .Lfail
    cmp     r1, #4
    beq     .Ldata
    b       .Lprotocol
.Linfo:
    ldr     r3, [r0, #4]
    cmp     r3, #32
    bhs     .Lprotocol
    add     r3, r3, #1
    str     r3, [r0, #4]
    mov     r0, #0
    bx      lr
.Lok:
    mov     r2, #1
    b       .Lstore
.Lfail:
    mov     r2, #2
    b       .Lstore
.Ldata:
    mov     r2, #3
    b       .Lstore
.Lprotocol:
    mov     r2, #4
.Lstore:
    str     r2, [r0]
.Lterminal:
    mov     r0, r2
    bx      lr
.Lbadptr:
    mov     r0, #4
    bx      lr
.size fb_asm_session_step, .-fb_asm_session_step
