#include <sys/syscall.h>
    .global print_tb, main, bar, foo

print_tb:
    push %ebp
    mov %esp, %ebp

    sub $8, %esp
    .equ min_idx, -4
    .equ min_val, -8

    push %ebx
    push %esi
    push %edi

    // выводим название текущ функции
    lea my_name, %esi

    push $9
    push %esi
    call write_str
    add $8, %esp

    /*mov $main, %eax
    call writei32
    mov $bar, %eax
    call writei32
    mov $foo, %eax
    call writei32
    mov $-1, %eax
    call writei32*/

    // esi - адрес текущего ebp
    mov %ebp, %esi
bt_loop:
    lea fn_addrs, %ebx

    xor %edi, %edi

    movl $-1, min_idx(%ebp)
    movl $0x7fffffff, min_val(%ebp)

    //mov 4(%esi), %eax
    //call writei32
loop:
    cmp $3, %edi
    jge loop_end

    mov 4(%esi), %edx

    // ret_addr > fn_addr
    mov (%ebx), %eax
    cmp %eax, %edx
    jl loop_cont

    sub %eax, %edx
    cmp %edx, min_val(%ebp)
    jle loop_cont

    mov %edx, min_val(%ebp)
    mov %edi, min_idx(%ebp)
loop_cont:
    add $4, %ebx
    inc %edi
    jmp loop
loop_end:

    cmp $-1, min_idx(%ebp)
    jz end

    mov min_idx(%ebp), %eax
    lea fn_names(%eax, %eax, 4), %edi

    push $5
    push %edi
    call write_str

    mov (%esi), %eax
    mov %eax, %esi

    jmp bt_loop
end:
    pop %edi
    pop %esi
    pop %ebx

    add $8, %esp

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

    .data
fn_addrs:
    .int main
    .int bar
    .int foo
fn_names:
    .ascii "main\n"
    .ascii "bar \n"
    .ascii "foo \n"
my_name:
    .ascii "print_tb\n"