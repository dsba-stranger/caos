#include <sys/syscall.h>
    .global main

    .text
main:
    push %ebp
    mov %esp, %ebp

    .equ BUF_SIZE, 1
    .equ num_symbols, -4
    .equ symbols_to_skip, -8
    sub $9, %esp
    mov %esp, %edi
    movl $0, num_symbols(%ebp)
    movl $0, symbols_to_skip(%ebp)

loop:
    // читаем символ из STDIN
    push $BUF_SIZE
    push %edi
    call read_str
    add $8, %esp

    // если дошли до конца, выходим
    cmp $0, %eax
    jz loop_end

    // нужно ли скипать символы
    cmp $0, symbols_to_skip(%ebp)
    jnz loop_jump

    // прибавляем к счетчику
    add $1, num_symbols(%ebp)

    mov (%edi), %al
    cmp $128, %al
    jl case_1

    // проверка масок

    and $240, %al
    cmp $240, %al
    jz case_4

    and $224, %al
    cmp $224, %al
    jz case_3

    and $192, %al
    cmp $192, %al
    jz case_2
case_1:
    movl $1, symbols_to_skip(%ebp)
    jmp loop_jump
case_2:
    movl $2, symbols_to_skip(%ebp)
    jmp loop_jump
case_3:
    movl $3, symbols_to_skip(%ebp)
    jmp loop_jump
case_4:
    movl $4, symbols_to_skip(%ebp)
    jmp loop_jump
loop_jump:
    sub $1, symbols_to_skip(%ebp)
    jmp loop

loop_end:
    mov num_symbols(%ebp), %eax
    call writei32

    mov $SYS_exit, %eax
    mov $0, %ebx
    int $0x80

    mov %ebp, %esp
    pop %ebp
    ret


read_str:
    push %ebp
    mov %esp, %ebp

    push %ebx

    mov $SYS_read, %eax
    mov $0, %ebx
    mov 8(%ebp), %ecx
    mov 12(%ebp), %edx
    int $0x80

    pop %ebx

    mov %ebp, %esp
    pop %ebp
    ret


write_str:
    push %ebp
    mov %esp, %ebp

    push %ebx

    mov $SYS_write, %eax
    mov $1, %ebx
    mov 8(%ebp), %ecx
    mov 12(%ebp), %edx
    int $0x80

    pop %ebx

    mov %ebp, %esp
    pop %ebp
    ret