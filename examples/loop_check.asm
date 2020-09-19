# loop check

main:
    const.i8    1       # 1
    const.i8    8       # 1 8
    fetch.1     1       # 1 8 8
    const.a32   99      # 1 8 8 99
    store               # 1 8
label:
    Mul                 # 1*8
    const.a32   99      # 1*8 99
    const.i8    2       # 1*8 99 2
    load                # 1*8 [99]
    const.i8    1       # 1*8 [99] 1
    Sub                 # 1*8 [99]-1
    fetch.1     1       # 1*8 [99]-1 [99]-1
    const.a32   99      # 1*8 [99]-1 [99]-1 99
    store               # 1*8 [99]-1
    print
    fetch.1     1       # 1*8 [99]-1 [99]-1
    const.i8    0       # 1*8 [99]-1 [99]-1 0
    ne                  # 1*8 [99]-1 true/false
    const.a32   label   # 1*8 [99]-1 true/false :label
    jmp.con
    drop                # 1*8
    Print
    quit
