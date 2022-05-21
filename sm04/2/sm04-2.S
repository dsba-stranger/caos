#include <sys/syscall.h>
    // оставлю реализацию free_mem тут, которая использовалась для тестирования
    // .global alloc_mem, free_mem, freelist
    .global free_mem, freelist
alloc_mem:
    push %ebp
    mov %esp, %ebp

    push %ebx

    // в ebx необходимый размер
    mov 8(%ebp), %ebx

    // в ecx текущий блок
    mov freelist, %ecx

    cmp $0, %ecx
    jz system_call

    // в eax предыдущий блок
    xor %eax, %eax
loop:
    // если размер в блоке не меньше, чем надо, то блок найден
    // завершаем цикл
    cmp (%ecx), %ebx
    jle loop_end

    // сохраняем адрес предыдущего блока
    mov %ecx, %eax

    // если next = null, то мы дошли до конца списка и придется делать системный вызов
    cmp $0, 4(%ecx)
    jz system_call

    // иначе линейным поиском идем на следующий блок
    mov 4(%ecx), %ecx
    jmp loop
loop_end:
    // если это первый блок в списке, то можем не перекидывать указатели на следующий
    cmp $0, %eax
    jnz remove_node

    // меняем голову списка
    mov 4(%ecx), %edx
    mov %edx, freelist

    lea 4(%ecx), %eax
    jmp epilogue
remove_node:
    lea 4(%ecx), %ebx

    // prev->next = cur->next
    mov 4(%ecx), %ecx
    mov %ecx, 4(%eax)

    // в eax адрес
    mov %ebx, %eax
    jmp epilogue
system_call:
    mov heapend, %ebx
    cmp $0, %ebx
    jnz system_call_impl

    // если heapend = 0, то нужно узнать его с помощью системного вызова
    mov $SYS_brk, %eax
    mov $0, %ebx
    int $0x80

    mov %eax, heapend
    mov %eax, %ebx
system_call_impl:
    // увеличим конец хипа в соответствии с запросом + 4 байта под метаданные
    // max(4, align4(sz)) + 4
    // align4(x) = (x + 4 - 1) & -4

    mov 8(%ebp), %edx
    add $3, %edx
    jc system_call_failed
    and $-4, %edx
    jc system_call_failed

    // ebx = heapend + max(4, align4(sz)) + 4
    lea 4(%edx), %eax
    add %eax, %ebx
    jc system_call_failed

    // Системный вызов
    mov $SYS_brk, %eax
    int $0x80

    cmp %eax, %ebx
    jnz system_call_failed

    // в ecx старый конец, в eax новый
    mov heapend, %ecx
    mov %eax, heapend

    // кладем в начало блока размер
    mov %edx, (%ecx)
    lea 4(%ecx), %eax
    // теперь в eax необходимый адрес памяти

    jmp epilogue
system_call_failed:
    xor %eax, %eax
epilogue:
    pop %ebx

    mov %ebp, %esp
    pop %ebp
    ret

free_mem:
    push %ebp
    mov %esp, %ebp

    mov 8(%ebp), %ecx
    sub $4, %ecx

    // new_node->next = freelist
    mov freelist, %edx
    mov %edx, 4(%ecx)
    mov %ecx, freelist
free_mem_epilogue:
    mov %ebp, %esp
    pop %ebp
    ret

    .data
heapend:
    .int 0
freelist:
    .int 0