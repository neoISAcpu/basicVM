# recursion check

main:
    const.i32   8          # 8
    const.a32   fack
    call
    print
    quit

fack:                       # count
    print.stack
    fetch.1     1           # count    
    const.i8    1           # count count 1
    eq                      # count true/false
    const.a32   label       # count true/false :label
    jmp.con
    fetch.1     1           # count count
    const.i8    1           # count count 1
    sub                     # count count-1
    const.a32   fack
    call           
    mul
    ret

label:
    ret
