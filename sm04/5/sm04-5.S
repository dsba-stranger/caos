    .global sort_by_abs

sort_by_abs:
    push %ebp
    mov %esp, %ebp

    push $cmp
    push $2
    push 12(%ebp)
    push 8(%ebp)
    call qsort

    mov %ebp, %esp
    pop %ebp
    ret

cmp:
    push %ebp
    mov %esp, %ebp


    mov 8(%ebp),%eax
    movzwl (%eax),%eax // расширяем 16 бит до 32 бит
    mov %eax,%edx
    sar $0xf,%dx // либо 0xff..ff если отрицательное, либо 0 если положительное
    xor %edx,%eax // получим представление в дополнительном коде, если сделаем xor с 0xff..
    sub %edx,%eax // инвертировав биты и прибавим 1 (0xff.fff = -1)
    movzwl %ax,%ecx

    // тут то же самое для второго числа
    mov 12(%ebp),%eax
    movzwl (%eax),%eax
    mov %eax,%edx
    sar $0xf,%dx
    xor %edx,%eax
    sub %edx,%eax
    movzwl %ax,%eax

    // считаем разность a - b
    sub %eax,%ecx
    mov %ecx,%eax

    mov %ebp, %esp
    pop %ebp
    ret