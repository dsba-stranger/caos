.global main

main:
    // A = ecx ebx
    // B = edx eax

    call readi64
    mov %eax, %ebx
    mov %edx, %ecx

    call readi64

    // A * 2

    shl $1, %ebx
    rcl $1, %ecx

    // B / 2

    shr $1, %edx
    rcr $1, %eax

    // A' - B'
    sub %eax, %ebx
    sbb %edx, %ecx

    mov %ebx, %eax
    mov %ecx, %edx

    call writei64
    call finish