    .global main
main:
    xor %ebx, %ebx
    xor %ecx, %ecx
loop:
    call readi32
    jc end
    add %eax, %ebx
    jno loop
    adc $0x7fffffff, %ecx
    mov %ecx, %ebx
end:
    mov %ebx, %eax
    call writei32
    call finish