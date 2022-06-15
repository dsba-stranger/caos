    .global spin_lock, spin_unlock
    .text

spin_lock:
    push %ebp
    mov %esp, %ebp
loop_begin:
    pause
    mov 8(%ebp), %ecx
    mov $1, %eax
    xchg %eax, (%ecx)
    cmp $0, %eax
    jnz loop_begin
after_loop:
    mov %ebp, %esp
    pop %ebp
    ret

spin_unlock:
    push %ebp
    mov %esp, %ebp

    mov 8(%ebp), %ecx
    xor %edx, %edx
    xchg %edx, (%ecx)

    mov %ebp, %esp
    pop %ebp
    ret