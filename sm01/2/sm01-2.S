    .global main

main:
    call readi32
    cmp $0, %eax
loop:
    js end
    call writei32
    sub $1, %eax
    jmp loop
end:
    call finish