    .text
list_reverse:
    push %ebp
    mov %esp, %ebp

    // eax = cur
    // ecx = next

    mov head, %eax
    cmp $0, %eax
    jz end

    mov 8(%eax), %ecx
    movl $0, 8(%eax)
loop:
    cmp $0, %ecx
    jz end

    // tmp = next->next
    mov 8(%ecx), %edx

    // next->next = cur
    mov %eax, 8(%ecx)
    mov %ecx, %eax
    mov %edx, %ecx
    jmp loop
end:
    mov %eax, head

    mov %ebp, %esp
    pop %ebp
    ret


    .global list_reverse