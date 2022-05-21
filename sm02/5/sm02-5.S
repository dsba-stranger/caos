    //.global partition, partition_c
    .global partition
/*partition_c:
    push %ebp
    mov %esp, %ebp

    push %ebx
    push %esi
    push %edi

    mov 8(%ebp), %ebx
    mov 12(%ebp), %esi
    mov 16(%ebp), %edi
    call partition
    mov %edx, %eax

    pop %edi
    pop %esi
    pop %ebx

    mov %ebp, %esp
    pop %ebp
    ret*/
partition:
    push %ebp
    mov %esp, %ebp

    xor %ecx, %ecx // i
    xor %edx, %edx // j
    xor %eax, %eax
loop:
    cmp %esi, %edx // j < n
    jge loop_end

    mov (%ebx, %edx, 2), %ax // a[j]
    cmp %di, %ax // a[j] <= pivot
    jg loop_inc

    // swap a[i] and a[j]
    push %ax
    mov (%ebx, %ecx, 2), %ax // a[i]
    mov %ax, (%ebx, %edx, 2)
    pop %ax
    mov %ax, (%ebx, %ecx, 2) // a[j]

    inc %ecx
loop_inc:
    inc %edx
    jmp loop
loop_end:
    mov %ecx, %edx

    mov %ebp, %esp
    pop %ebp
    ret