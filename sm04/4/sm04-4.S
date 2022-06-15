#include <sys/syscall.h>
    .global print_tb

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

    .equ TB_NAME_LEN, 9
    .equ NUM_FUNCS, 3
    .equ FN_NAME_MAX_LEN, 5

    push $TB_NAME_LEN
    push %esi
    call write_str
    add $8, %esp

    // esi - адрес текущего ebp
    mov %ebp, %esi
bt_loop:
    lea fn_addrs, %ebx

    xor %edi, %edi

    movl $-1, min_idx(%ebp)
    movl $0x7fffffff, min_val(%ebp)
loop:
    cmp $NUM_FUNCS, %edi
    jge loop_end

    mov 4(%esi), %edx

    // ret_addr > fn_addr
    mov (%ebx), %eax
    cmp %eax, %edx
    jle loop_cont

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
    mov min_idx(%ebp), %eax
    lea fn_names(%eax, %eax, 4), %edi

    push $FN_NAME_MAX_LEN
    push %edi
    call write_str
    add $8, %esp

    cmp $0, min_idx(%ebp)
    jz end

    mov (%esi), %eax
    mov %eax, %esi

    jmp bt_loop
end:
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