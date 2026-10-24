/* x86-64 System V ABI: rdi=key, rsi=length, rdx=dst, rax=result.
 * No stack, heap, libc, syscall or OS call; invalid dst remains intact.
 */
.text
.p2align 4
.globl fb_asm_make_getvar
.type fb_asm_make_getvar, @function
fb_asm_make_getvar:
    testq   %rdi, %rdi
    jz      .Lbad
    testq   %rdx, %rdx
    jz      .Lbad
    testq   %rsi, %rsi
    jz      .Lbad
    cmpq    $57, %rsi
    ja      .Lbad
    xorl    %ecx, %ecx
.Lverify:
    movzbl  (%rdi,%rcx), %r8d
    cmpl    $97, %r8d
    jb      .Lupper
    cmpl    $122, %r8d
    jbe     .Lvalid
.Lupper:
    cmpl    $65, %r8d
    jb      .Ldigit
    cmpl    $90, %r8d
    jbe     .Lvalid
.Ldigit:
    cmpl    $48, %r8d
    jb      .Lpunct
    cmpl    $57, %r8d
    jbe     .Lvalid
.Lpunct:
    cmpl    $45, %r8d
    je      .Lvalid
    cmpl    $95, %r8d
    je      .Lvalid
    cmpl    $46, %r8d
    jne     .Lbad
.Lvalid:
    incq    %rcx
    cmpq    %rsi, %rcx
    jb      .Lverify
    movb    $103, 0(%rdx)
    movb    $101, 1(%rdx)
    movb    $116, 2(%rdx)
    movb    $118, 3(%rdx)
    movb    $97, 4(%rdx)
    movb    $114, 5(%rdx)
    movb    $58, 6(%rdx)
    leaq    7(%rdx), %rdx
    xorl    %ecx, %ecx
.Lcopy:
    movb    (%rdi,%rcx), %r8b
    movb    %r8b, (%rdx,%rcx)
    incq    %rcx
    cmpq    %rsi, %rcx
    jb      .Lcopy
    leaq    7(%rsi), %rax
    ret
.Lbad:
    xorl    %eax, %eax
    ret
.size fb_asm_make_getvar, .-fb_asm_make_getvar
.section .note.GNU-stack,"",@progbits
