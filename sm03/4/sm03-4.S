include <sys/syscall.h>
    .global main

    .text
main:
    push %ebp
    mov %esp, %ebp

    .equ BUF_SIZE, 1
    .equ line_cnt, -4
    .equ word_cnt, -8
    .equ symb_cnt, -12

    // выделяю 13 байт на стеке под счетчики и буфер
    sub $13, %esp

    movl $0, line_cnt(%ebp) // счетчик строк
    movl $0, word_cnt(%ebp) // слов
    movl $0, symb_cnt(%ebp) // символов
    lea -13(%ebp), %edi

    // ebx = 1, если пробельный, 0 - иначе
    mov $1, %ebx
loop:
    // читаем символ из stdin
    push $BUF_SIZE
    push %edi
    call read_str
    add $8, %esp

    // дошли до конца, выходим
    cmp $0, %eax
    jz handle_end_of_str

    mov $0, %ecx

    // увеличиваем счетчик символов
    add $1, symb_cnt(%ebp)

    // увеличиваем счетчик строк, если встретили перенос
    mov (%edi), %al
    cmp $'\n', %al
    jnz loop_check_for_word
    add $1, line_cnt(%ebp)
loop_check_for_word:
    // проверяем текущий символ на "пробельность"
    cmp $' ', %al
    jz loop_encounter_ws
    cmp $'\n', %al
    jnz loop_end
loop_encounter_ws:
    // если предыдущий не пробельный, то увеличиваем счетчик слов
    cmp $0, %ebx
    mov $1, %ecx
    jnz loop_end
    add $1, word_cnt(%ebp)
loop_end:
    mov %ecx, %ebx
    jmp loop
handle_end_of_str:
    // если в конце стоит не пробел, значит нужно обработать отдельно
    cmp $0, %ebx
    jnz print
    add $1, word_cnt(%ebp)
print:
    // выводим счетчики
    mov line_cnt(%ebp), %eax
    call writei32
    mov word_cnt(%ebp), %eax
    call writei32
    mov symb_cnt(%ebp), %eax
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