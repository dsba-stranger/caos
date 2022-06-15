    .text
    .global dot_product
dot_product:
    push %ebp
    mov %esp, %ebp

    push %esi
    push %ebx

    mov 8(%ebp), %eax   // n
    mov 12(%ebp), %ebx  // x
    mov 16(%ebp), %edx  // y

    cmp $0, %eax
    jz fin

    xorps %xmm2, %xmm2
for:
    cmp $2, %eax
    jl process_last

    movups (%ebx), %xmm0
    movups (%edx), %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm2

    add $8, %ebx
    add $8, %edx
    sub $2, %eax
    jmp for
process_last:
    haddps %xmm2, %xmm2
    cmp $1, %eax
    jne fin

    movss (%ebx), %xmm0
    movss (%edx), %xmm1
    mulss %xmm1, %xmm0
    addss %xmm0, %xmm2
fin:
    mov 20(%ebp), %ecx
    movss %xmm2, (%ecx)
    fldl (%ecx)

    pop %ebx
    pop %esi
    mov %ebp, %esp
    pop %ebp
    ret