/* x86-64 SysV: rdi=fb_session*, esi=kind, eax=state.
 * Only caller-saved eax/esi/edi are used. No heap, stack or syscall.
 */
.text
.p2align 4
.globl fb_asm_session_step
.type fb_asm_session_step, @function
fb_asm_session_step:
    testq   %rdi, %rdi
    jz      .Lbadptr
    movl    (%rdi), %eax
    cmpl    $4, %eax
    ja      .Lprotocol
    testl   %eax, %eax
    jnz     .Ldone
    cmpl    $3, %esi
    je      .Linfo
    cmpl    $5, %esi
    je      .Linfo
    cmpl    $1, %esi
    je      .Lok
    cmpl    $2, %esi
    je      .Lfail
    cmpl    $4, %esi
    je      .Ldata
    jmp     .Lprotocol
.Linfo:
    cmpl    $32, 4(%rdi)
    jae     .Lprotocol
    incl    4(%rdi)
    xorl    %eax, %eax
    ret
.Lok:
    movl    $1, %eax
    jmp     .Lstore
.Lfail:
    movl    $2, %eax
    jmp     .Lstore
.Ldata:
    movl    $3, %eax
    jmp     .Lstore
.Lprotocol:
    movl    $4, %eax
.Lstore:
    movl    %eax, (%rdi)
.Ldone:
    ret
.Lbadptr:
    movl    $4, %eax
    ret
.size fb_asm_session_step, .-fb_asm_session_step
.section .note.GNU-stack,"",@progbits
