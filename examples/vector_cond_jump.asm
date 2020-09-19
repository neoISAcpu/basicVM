# this is a demonstration of a conditional vector jump

main:
    const.i32.4 1, 4, 3, 4
    const.i8    4
    eq
    const.a32.4 one, two, tree, four
    jmp.con
    quit


one:            
    const.i8    1      
    print
    drop
    quit

two:
    const.i8    2
    print
    drop
    quit

tree:
    const.i8    3
    print
    drop
    quit

four:
    const.i8    4
    print
    drop
    quit



