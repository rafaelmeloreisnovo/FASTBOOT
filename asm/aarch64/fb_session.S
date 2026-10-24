/* AArch64 AAPCS64. x0=fb_session*, w1=kind, w0=state.
 * No stack, libc, heap, system calls or external symbols.
 */
.text
.balign 4
.global fb_asm_session_step
.type fb_asm_session_step, %function
fb_asm_session_step:
    cbz     x0, .Lbadptr
    ldr     w2, [x0]
    cmp     w2, #4
    b.hi    .Lprotocol
    cbnz    w2, .Lterminal
    cmp     w1, #3
    b.eq    .Linfo
    cmp     w1, #5
    b.eq    .Linfo
    cmp     w1, #1
    b.eq    .Lok
    cmp     w1, #2
    b.eq    .Lfail
    cmp     w1, #4
    b.eq    .Ldata
    b       .Lprotocol
.Linfo:
    ldr     w3, [x0, #4]
    cmp     w3, #32
    b.hs    .Lprotocol
    add     w3, w3, #1
    str     w3, [x0, #4]
    mov     w0, #0
    ret
.Lok:
    mov     w2, #1
    b       .Lstore
.Lfail:
    mov     w2, #2
    b       .Lstore
.Ldata:
    mov     w2, #3
    b       .Lstore
.Lprotocol:
    mov     w2, #4
.Lstore:
    str     w2, [x0]
.Lterminal:
    mov     w0, w2
    ret
.Lbadptr:
    mov     w0, #4
    ret
.size fb_asm_session_step, .-fb_asm_session_step
