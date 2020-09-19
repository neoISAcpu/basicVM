# test from the random generator

main:
    const.i8    0       # 0
    const.i8    20       # 0 5
label:
    const.a32   rnd     
    const.i8    6
    load
    const.a32   random
    call
    print
    const.a32   rnd
    store

    const.i8    1       # 0 5 1
    sub                 # 0 4
    fetch.2     2, 1    # 0 4 0 4
    le                  # 0 4 true/false
    const.a32   label   # 0 true/false label
    jmp.con
    quit




# funktion random input: state.u32, output: random.u32 -  Xorshift random number generators
random:                     
    fetch.1     1       # x x
    const.u8    13      # x x 13
    shl                 # x x<<13
    xor                 # x^(x<<13)
    fetch.1     1       # x x
    const.u8    13      # x x 17
    shr                 # x x>>17
    xor                 # x^(x>>17)
    fetch.1     1       # x x
    const.u8    5       # x x 5
    shr                 # x x<<5
    xor                 # x^(x<<5)
    ret


rnd:    dc.u32      0xa1234567

