// Работает для отрицательных B, потому что
// A * |B| и потом взять дополнительный код
// то же самое, что умножить A на B в дополнительном коде
// (т.е чтобы получить отрицательное число, мы инвертируем биты и прибавляем 1)
    .text
process:
    push %ebp
    mov %esp, %ebp

    push %ebx

    mov A, %eax
    mov B, %ebx
    xor %ecx, %ecx
loop:
    cmp $0, %ebx
    jz end
    test $1, %ebx
    jz skip
    add %eax, %ecx
skip:
    shl $1, %eax
    shr $1, %ebx
    jmp loop
end:
    mov %ecx, R

    pop %ebx

    mov %ebp, %esp
    pop %ebp
    ret

    .data
A:
    .int 0
B:
    .int 0
R:
    .int 0

    .global process, A, B, R