    .text
    .global rop
rop:
    push $rop+16
    push $str1
    push $f1+1
    ret
    push $str2
    push $f1+1
    ret