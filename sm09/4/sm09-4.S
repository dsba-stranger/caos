    .global myexp
myexp:
    push %ebp
    mov %esp, %ebp

    mov $1, %eax
    cvtsi2sd %eax, %xmm0
    addsd 8(%ebp), %xmm0
    movsd 8(%ebp), %xmm3

loop:
    mov $0, %edx
    cvtsi2sd %edx, %xmm1
    comisd %xmm1, %xmm3
    jz exit

    inc %eax
    cvtsi2sd %eax, %xmm2
    mulsd 8(%ebp), %xmm3
    divsd %xmm2, %xmm3

    addsd %xmm3, %xmm0
    jmp loop
fin:
    mov 16(%ebp), %eax
    movsd %xmm0, (%eax)
    mov %ebp, %esp
    pop %ebp
    ret