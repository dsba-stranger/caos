.text
process:
    push %ebp
    mov %esp, %ebp

    xor %ecx, %ecx
loop:
    call readi32
    jc loop2
    push %eax
    inc %ecx
    jmp loop
loop2:
    cmp $0, %ecx
    jz end
    pop %eax
    call writei32
    dec %ecx
    jmp loop2
end:

    mov %ebp, %esp
    pop %ebp
    ret

.global process