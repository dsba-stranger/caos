    .global main
main:
    xor %ebx, %ebx
    xor %ecx, %ecx
loop:
    call readi32
    jc end
    test $1, %eax
    jnz odd
    add %eax, %ebx
    jmp loop
odd:
    add %eax, %ecx
    jmp loop
end:
    mov %ebx, %eax
    call writei32

    mov %ecx, %eax
    call writei32

    call finish