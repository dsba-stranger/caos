    .text

main:
loop:
    call readi32
    jc end
    cmp $0, %eax
    jl call_pop
    mov %eax, %ebx
    call readi32
    push %eax
    push %ebx
    call stack_push
    jmp loop
call_pop:
    push %eax
    call stack_pop
    jmp loop
end:
    call finish

stack_push:
    push %ebp
    mov %esp, %ebp

    push %ebx
    push %esi

    mov 8(%ebp), %eax
    mov 12(%ebp), %ebx

    // ecx = &array[%eax]
    lea array(, %eax, 8), %ecx

    // edx = &array[n_vers+1]
    mov n_vers, %esi
    inc %esi
    mov %esi, n_vers

    lea array(, %esi, 8), %edx

    mov %ebx, (%edx)
    mov %ecx, 4(%edx)

    pop %esi
    pop %ebx

    mov %ebp, %esp
    pop %ebp
    ret

stack_pop:
    push %ebp
    mov %esp, %ebp

    push %ebx

    mov 8(%ebp), %eax
    neg %eax

    // ecx = &array[%eax]
    lea array(, %eax, 8), %ecx
    mov 4(%ecx), %ebx
    cmp $0, %ebx
    jz empty_stack

    mov (%ecx), %eax
    call writei32

    // edx = &array[n_vers+1]
    mov n_vers, %ebx
    inc %ebx
    mov %ebx, n_vers

    mov 4(%ecx), %ecx

    lea array(, %ebx, 8), %edx
    mov (%ecx), %ebx
    mov %ebx, (%edx)
    mov 4(%ecx), %ebx
    mov %ebx, 4(%edx)

    jmp stack_pop_end
empty_stack:
    mov $-1, %eax
    call writei32
stack_pop_end:
    pop %ebx

    mov %ebp, %esp
    pop %ebp
    ret

    .data
n_vers:
    .skip 4, 0
array:
    .skip 8008

    .global main