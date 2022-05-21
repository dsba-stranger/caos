#include <sys/syscall.h>
    .global main

    .text
main:
    push %ebp
    mov %esp, %ebp

    // выделили место под символ на стеке
    .equ BUF_SIZE, 1
    sub $BUF_SIZE, %esp
    mov %esp, %edi

    // счетчик символов
    xor %ebx, %ebx
loop:
    // читаем символ из STDIN
    push $BUF_SIZE
    push %edi
    call read_str
    add $8, %esp

    // если дошли до конца, выходим
    cmp $0, %eax
    jz loop_end

    // Если написали 10 символов и текущий прочитанный не перенос,
    // добавляем перенос
    cmp $10, %ebx
    jnz loop_write_symbol
    cmp $'\n', (%edi)
    jz loop_write_symbol

    // сохраним текущий символ
    mov (%edi), %esi
    movl $'\n', (%edi)

    // выводим перенос
    push $BUF_SIZE
    push %edi
    call write_str
    add $8, %esp

    // восстанавливаем текущий символ и обнуляем счетчик
    mov %esi, (%edi)
    xor %ebx, %ebx
loop_write_symbol:
    // вывод символа
    push $BUF_SIZE
    push %edi
    call write_str
    add $8, %esp

    // если встретили перенос, то нужно обнулить счетчик
    cmp $'\n', (%edi)
    jnz loop_inc_counter
    xor %ebx, %ebx
    jmp loop
loop_inc_counter:
    inc %ebx
    jmp loop
loop_end:
    cmp $0, %ebx
    jz prog_exit

    // добавим перенос, если его не было в конце строки
    cmp $'\n', (%edi)
    jz prog_exit

    movl $'\n', (%edi)
    push $BUF_SIZE
    push %edi
    call write_str
    add $8, %esp
prog_exit:
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