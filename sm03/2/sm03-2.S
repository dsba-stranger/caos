#include <sys/syscall.h>
    .global writeu32
    //.global writeu32c
    .text
/*writeu32c:
    push %ebp
    mov %esp, %ebp
    mov 8(%ebp), %ecx
    call writeu32
    mov %ebp, %esp
    pop %ebp
    ret*/
writeu32:
    push %ebp
    mov %esp, %ebp

    push %ebx
    push %esi
    push %edi

    .equ MAGIC_CONST, 0x66666666 // 2^34 / 10
    xor %esi, %esi // количество цифр

    .equ BUF_SIZE, 1
    sub $BUF_SIZE, %esp
    mov %esp, %edi

    cmp $0, %ecx
    jnz loop
print_zero:
    // если 0, то просто его выводим
    movl $'0', (%edi)
    push $BUF_SIZE
    push %edi
    call write_str
    add $8, %esp
    jmp fn_end
loop:
    cmp $0, %ecx
    jz backward_loop

    mov $MAGIC_CONST, %eax
    mul %ecx
    shr $2, %edx

    // 8 байт на стеке
    sub $8, %esp
    mov %edx, (%esp) // значение частного
    mov %ecx, 4(%esp) // остаток будет тут

    // пробуем увеличить частное, так как при кратных 10 идет округление вниз
    add $1, (%esp)
    mov $10, %eax
    mull (%esp)
    sub %eax, 4(%esp)

    cmp $0, 4(%esp)
    jz loop_ok
    // пробуем изначальное частное
    mov %ecx, 4(%esp)
    sub $1, (%esp)
    mov $10, %eax
    mull (%esp)
    sub %eax, 4(%esp)
loop_ok:
    mov (%esp), %ecx // ecx /= 10
    mov 4(%esp), %ebx
    add $8, %esp
    push %ebx

    add $1, %esi
    jmp loop
backward_loop:
    cmp %esp, %edi
    jz fn_end

    pop %eax
    add $'0', %eax
    mov %eax, (%edi)

    push $BUF_SIZE
    push %edi
    call write_str
    add $8, %esp

    jmp backward_loop
fn_end:
    add $BUF_SIZE, %esp

    pop %edi
    pop %esi
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