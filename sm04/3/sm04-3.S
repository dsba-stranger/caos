    //.global process, обработка, head, I, J
    .global обработка
/*process:
    push %ebp
    mov %esp, %ebp

    push %ebx
    push %esi
    push %edi

    mov head, %ebx
    movl I, %esi
    movl J, %edi

    call обработка
    pop %eax

    call writei32

    pop %edi
    pop %esi
    pop %ebx

    mov %ebp, %esp
    pop %ebp
    ret
*/
обработка:
    // push %ebp
    // mov %esp, %ebp

    // head of the list in ebx
    // index i in esi
    // index j in edi

    push %ebx

    // answer is stored here
    xor %eax, %eax

    .equ m1, 4 // shift of m1 relative to node address
    .equ m2, 52 // shift of m2 relative to node address
    .equ sz_m1, 4 // size of m1 field
    .equ sz_m2, 4 // size of m2 field

loop:
    cmp $0, %ebx
    jz loop_end

    mov m1(%ebx, %esi, 4), %ecx // ecx = p->m1[i]
    mov m2(%ebx, %edi, 4), %edx // edx = p->m2[j]

    // ecx = (p->m1[i] + p->m2[j]) / 2
    add %edx, %ecx

    mov %ecx, %edx
    shr $0x1f, %edx
    add %edx, %ecx
    sar %ecx

    add %ecx, %eax

    mov (%ebx), %ebx
    jmp loop
loop_end:
    pop %ebx

    // swap return address and eax in a stack frame
    pop %ecx
    push %eax
    push %ecx

    // mov %ebp, %esp
    // pop %ebp
    ret