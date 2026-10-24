/* x86-64 SysV ABI: rdi=wire, rsi=length, rdx=data_size_out.
 * Returns fb_kind in eax. No stack, libc, syscall or external calls.
 * Invalid leaves data_size untouched; DATA hex is validated before store.
 */
.text
.p2align 4
.globl fb_asm_classify_reply
.type fb_asm_classify_reply, @function
fb_asm_classify_reply:
    testq   %rdi, %rdi
    jz      .Lbad
    testq   %rdx, %rdx
    jz      .Lbad
    cmpq    $4, %rsi
    jb      .Lbad
    cmpq    $256, %rsi
    ja      .Lbad
    movl    (%rdi), %r8d
    cmpl    $0x59414b4f, %r8d
    je      .Lok
    cmpl    $0x4c494146, %r8d
    je      .Lfail
    cmpl    $0x4f464e49, %r8d
    je      .Linfo
    cmpl    $0x54584554, %r8d
    je      .Ltext
    cmpl    $0x41544144, %r8d
    jne     .Lbad
    cmpq    $12, %rsi
    jne     .Lbad
    xorl    %ecx, %ecx
    movl    $4, %r9d
.Lhex:
    movzbl  (%rdi,%r9), %r10d
    cmpl    $48, %r10d
    jb      .Lbad
    cmpl    $57, %r10d
    jbe     .Ldecimal
    cmpl    $65, %r10d
    jb      .Llower
    cmpl    $70, %r10d
    jbe     .Lupper
.Llower:
    cmpl    $97, %r10d
    jb      .Lbad
    cmpl    $102, %r10d
    ja      .Lbad
    subl    $87, %r10d
    jmp     .Laccumulate
.Lupper:
    subl    $55, %r10d
    jmp     .Laccumulate
.Ldecimal:
    subl    $48, %r10d
.Laccumulate:
    shll    $4, %ecx
    orl     %r10d, %ecx
    incq    %r9
    cmpq    $12, %r9
    jb      .Lhex
    movl    %ecx, (%rdx)
    movl    $4, %eax
    ret
.Lok:
    movl    $0, (%rdx)
    movl    $1, %eax
    ret
.Lfail:
    movl    $0, (%rdx)
    movl    $2, %eax
    ret
.Linfo:
    movl    $0, (%rdx)
    movl    $3, %eax
    ret
.Ltext:
    movl    $0, (%rdx)
    movl    $5, %eax
    ret
.Lbad:
    xorl    %eax, %eax
    ret
.size fb_asm_classify_reply, .-fb_asm_classify_reply
.section .note.GNU-stack,"",@progbits
