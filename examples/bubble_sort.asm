# bubble sort

# const
size_of_array		EQU		3000

# fill array with random 

main:
    const.i32 	0				# i
gen_loop:
    fetch.1		1				# i i

    const.a32   rndState     	# i i rndState
    const.i8    6				# i i rndState u32
    load						# i i [rndState]
    const.a32   random			# i i [rndState] random
    call						# i i rnd
    fetch.1		1				# i i rnd rnd
    const.a32   rndState		# i i rnd rnd rndState
    store						# i i rnd

    mix.2		1, 2			# i rnd i 
    const.i8	4				# i rnd i 4
    mul							# i rnd i*4
    const.a32	array			# i rnd i array
    store.2d					# i
    const.a32	arrayCounter	# i arrayCounter
    const.i8	2				# i arrayCounter 2
    load						# i [arrayCounter]
    const.i8	1				# i [arrayCounter] 1
    sub							# i [arrayCounter]-1
    fetch.1		2				# i [arrayCounter]-1 i
    eq							# i t/f
    const.a32	gen_end			# i t/f :gen_end
    jmp.con						# i
    const.i8	1				# i 1
    add							# i+1
    const.a32	gen_loop		# i+1 :gen_loop
    jmp							# i+1
gen_end:
    drop

    # 1234 ausgeben
    const.i32 	1234
    print
    drop

    # werte ausgeben
    const.a32 	output
    call 

    # 1235 ausgeben
    const.i32 	1235
    print
    drop

    # werte sortieren
    const.a32 	sort
    call

    # 1236 ausgeben
    const.i32 	1236
    print
    drop

    # werte ausgeben
    const.a32 	output
    call

    print.stack
    quit

# ----------------------------------------------------

# wikipedia:
# for(int i = 0; i < a.length; i++)
#     for(int j = 0; j < a.length - i - 1; j++)
#         if(a[j] > a[j + 1])
#             swap(a[j], a[j + 1]);

sort:
    const.i32 	0				# i
s_o_loop:
    # outer loop
    const.a32 	arrayCounter	# i arrayCounter
    const.i8 	2				# i arrayCounter i32
    load						# i [arrayCounter]
    const.i8 	1				# i [arrayCounter] 1
    sub							# i [arrayCounter]-1
    fetch.1 	2				# i [arrayCounter]-1 i
    eq							# i t/f
    const.a32 	sort_end		# i t/f :sort_end
    jmp.con						# i
    const.i32 	0				# i j
s_i_loop:
    # inner loop
    fetch.2 	2, 1			# i j i j
    const.a32 	arrayCounter	# i j i j size
    const.i8 	2				# i j i j size i32
    load						# i j i j [size]
    const.i8 	1				# i j i j [size] 1
    sub							# i j i j [size]-1
    mix.2 		1, 2			# i j i [size]-1 j
    sub							# i j i [size]-1-j
    eq							# i j t/f
    const.a32 	s_i_end			# i j t/f s_i_end 
    jmp.con						# i j
    fetch.2 	1, 1			# i j j j
    const.i8 	4				# i j j j 4
    mul							# i j j j*4
    const.a32 	array			# i j j j*4 array
    const.i8 	2				# i j j j*4 array i32
    load.2d						# i j j x
    mix.2 		1, 2			# i j x j
    const.i8 	1				# i j x j 1
    add							# i j x j+1
    const.i8 	4				# i j x j+1 4
    mul							# i j x (j+1)*4
    const.a32 	array			# i j x (j+1)*4 array
    const.i8 	2				# i j x (j+1)*4 array i32
    load.2d						# i j x x'
    fetch.2 	2, 1			# i j x x' x x'
    le							# i j x x' t/f
    const.a32 	cont			# i j x x' t/f cont
    jmp.con						# i j x x'
    # swap values in memory
    drop						# i j x
    mix.2 		1, 2			# i x j
    const.i8 	1				# i x j 1
    add							# i x j+1
    fetch.1 	1				# i x j+1 j+1
    const.i8 	4				# i x j+1 j+1 4
    mul							# i x j+1 (j+1)*4
    const.a32 	array			# i x j+1 (j+1)*4 array
    const.i8 	2				# i x j+1 (j+1)*4 array i32
    load.2d						# i x j+1 x'
    mix.2 		1, 2			# i x x' j+1
    const.i8 	1				# i x x' j+1 1
    sub							# i x x' j
    mix.2 		1, 2			# i x j x'
    fetch.2 	1, 2			# i x j x' x' j
    const.i8 	4				# i x j x' x' j 4
    mul							# i x j x' x' j*4
    const.a32 	array			# i x j x' x' j*4 array
    store.2d					# i x j x'
    drop						# i x j
    const.i8 	1				# i x j 1
    add							# i x j+1
    mix.2 		1, 2			# i j+1 x
    fetch.2 	1, 2			# i j+1 x x j+1
    const.i8 	4				# i j+1 x x j+1 4
    mul							# i j+1 x x (j+1)*4
    const.a32 	array			# i j+1 x x (j+1)*4 array
    store.2d					# i j+1 x
    drop						# i j+1
    const.a32 	s_i_loop		# i j+1 s_i_loop
    jmp 						# i j+1	
cont:
    drop						# i j x
    drop						# i j
    const.i8 	1				# i j 1
    add							# i j+1
    const.a32 	s_i_loop		# i j+1 s_i_loop
    jmp 						# i j+1
    s_i_end:
    drop						# i
    const.i8 	1				# i 1
    add							# i+1
    const.a32 	s_o_loop		# i+1 s_o_loop
    jmp 						# i+1
sort_end:
    drop
    ret

output:
    const.i32 	0				# i
output_loop:
    fetch.1 	1				# i i
    const.i8 	4				# i i 4
    mul							# i i*4
    const.a32 	array			# i i*4 array
    const.i8 	2				# i i*4 array i32
    load.2d						# i [array+i]
    print						# i [array+i]
    drop						# i
    const.a32 	arrayCounter	# i arrayCounter
    const.i8 	2				# i arrayCounter i32
    load						# i [arrayCounter]
    const.i8 	1				# i [arrayCounter] 1
    sub							# i [arrayCounter]-1
    fetch.1 	2				# i [arrayCounter]-1 i
    eq							# i t/f
    const.a32 	output_end		# i t/f :output_end
    jmp.con						# i
    const.i8 	1				# i 1
    add							# i+1
    const.a32 	output_loop		# i+1 output_loop
    jmp							# i+1		
output_end:
    drop
    ret


# funktion random input: state.u32, output: random.u32 -  Xorshift random number generators
random:                     
    fetch.1     1       		# x x
    const.u8    13      		# x x 13
    shl                 		# x x<<13
    xor                 		# x^(x<<13)
    fetch.1     1       		# x x
    const.u8    13      		# x x 17
    shr                 		# x x>>17
    xor                 		# x^(x>>17)
    fetch.1     1       		# x x
    const.u8    5       		# x x 5
    shr                 		# x x<<5
    xor                 		# x^(x<<5)
    ret



rndState:    		dc.u32  0xa1234567
arrayCounter: 		dc.i32	size_of_array
array: 				ds.i32	size_of_array
