# quick sort

# const
size_of_array		EQU		3000

# fill array with random 

main:
	const.i32 	0				#i
gen_loop:
 	fetch.1 	1				#i i

    const.a32   rndState     	#i i rndState
    const.i8    6				#i i rndState u32
    load						#i i [rndState]
    const.a32   random			#i i [rndState] random
    call						#i i rnd
	fetch.1		1				#i i rnd rnd
    const.a32   rndState		#i i rnd rnd rndState
    store						#i i rnd

 	mix.2 		1, 2			#i rnd i 
	const.i8 	4				#i rnd i 4
	mul							#i rnd i*4
 	const.a32 	array			#i rnd i array
	store.2d					#i
 	const.a32 	arrayCounter	#i arrayCounter
	const.i8 	2				#i arrayCounter i32
	load						#i [arrayCounter]
 	const.i8 	1				#i [arrayCounter] 1
	sub							#i [arrayCounter]-1
 	fetch.1 	2				#i [arrayCounter]-1 i
	eq							#i t/f
 	const.a32 	gen_end			#i t/f gen_end
	jmp.con						#i
 	const.i8 	1				#i 1
	add							#i+1
 	const.a32 	gen_loop		#i+1 gen_loop
	jmp 						#i+1
gen_end:
	drop

	# print 	0 
	const.i8 	0
	print
	drop

	# show array
	const.a32 output
	call

	# print 	1
	const.i8 	1
	print
	drop

	# call sort - low, high on the stack
	const.i32 	0				#low
	const.a32 	arrayCounter	#low *arrayCounter
	const.i8 	2				#low *arrayCounter i32
	load						#low high
	const.i32 	1				#low high 1
	sub							#low high-1
	const.a32 	sort			#low high-1 sort
	call						#low high-1

	# print 	2
	const.i8 	2
	print
	drop

	# show sorted array
	const.a32 output
	call

	quit

# ----------------------------------------------------

# procedure sort(var ar: array of real; low, high: integer);
#  var i, j: integer;
#      m, wsp: real;
#  begin
#      i:=low; j:=high; m:=ar[(i+j) div 2];
#      repeat
#        while ar[i]<m do Inc(i);
#        while ar[j]>m do Dec(j);
#        if i<=j then begin
#          wsp:=ar[i]; ar[i]:=ar[j]; ar[j]:=wsp;
#          Inc(i); Dec(j);
#         end;
#      until i>j;
#      if low<j then sort(ar, low, j);
#      if i<high then sort(ar, i, high);
#  end;

store_mid:						#i j
	add							#i+j
 	const.i8 	2				#i+j 2
	div							#i+j/2
 	const.i8 	4				#i+j/2 4
	mul							#(i+j/2)*4
 	const.a32 	array			#(i+j/2)*4 array
	const.i8 	2				#(i+j/2)*4 array i32
	load.2d						#array[i+j/2]
 	const.a32 	val_mid			#array[i+j/2] val_mid
	store						#
	ret


exch_old:						#a1 a2 - original
 	fetch.1 	1				#a1 a2 a2
	const.i8 	4				#a1 a2 a2 4
	mul							#a1 a2 a2*4
 	const.a32 	array			#a1 a2 a2*4 array
	const.i8 	2				#a1 a2 a2*4 array i32
	load.2d						#a1 a2 array[a2*4+array]
 	const.a32 	temp			#a1 a2 array[a2*4+array] temp
	store						#a1 a2
 	mix.2 		1, 2			#a2 a1
	fetch.1 	1				#a2 a1 a1	
	const.i8 	4				#a2 a1 a1 4
	mul							#a2 a1 a1*4
 	const.a32 	array			#a2 a1 a1*4 array
	const.i8 	2				#a2 a1 a1*4 array i32
	load.2d						#a2 a1 array[a1*4+array]
 	mix.2 		1, 2			#a2 array[a1*+array] a1
	const.a32 	temp			#a2 array[a1*+array] a1 temp
	const.i8 	2				#a2 array[a1*+array] a1 temp i32
	load						#a2 array[a1*4+array] a1 array[a2*4+array]
 	mix.2 		1, 2			#a2 array[a1*4+array] array[a2*4+array] a1
	const.i8 	4				#a2 array[a1*4+array] array[a2*4+array] a1 4
	mul							#a2 array[a1*4+array] array[a2*4+array] a1*4
 	const.a32 	array			#a2 array[a1*4+array] array[a2*4+array] a1*4 array
	store.2d					#a2 array[a1*4+array]
 	mix.2 		1, 2			#array[a1*4+array] a2
	const.i8 	4				#array[a1*4+array] a2 4
	mul							#array[a1*4+array] a2*4
 	const.a32 	array			#array[a1+array] a2*4 array
	store.2d					#
	ret


exch:							#a1 a2 - 1st try to optimize
	const.i8	4				#a1 a2 4
	mul							#a1 a2*4
	const.a32	array			#a1 a2*4 array
	add							#a1 a2*4+array
	mix.2		1, 2			#a2*4+array a1
	const.i8	4				#a2*4+array a1 4
	mul							#a2*4+array a1*4
	const.a32	array			#a2*4+array a1*4 array
	add							#a2*4+array a1*4+array
	fetch.1		1				#a2*4+array a1*4+array a1*4+array
	const.i8	2				#a2*4+array a1*4+array a1*4+array i32
	load						#a2*4+array a1*4+array array[a1*4+array]
	fetch.1		3				#a2*4+array a1*4+array array[a1*4+array] a2*4+array
	const.i8	2				#a2*4+array a1*4+array array[a1*4+array] a2*4+array i32
	load						#a2*4+array a1*4+array array[a1*4+array] array[a2*4+array]
	mix.4		1, 3, 2, 4		#array[a2*4+array] a1*4+array array[a1*4+array] a2*4+array
	store						#array[a2*4+array] a1*4+array
	store						#
	ret



sort:							#low high
 	fetch.4 	2, 1, 2, 1		#low high i j i j
	const.a32 	store_mid		#low high i j i j store_mid 
	call						#low high i j 
main_cycle:
	mix.2 		1, 2			#low high j i
i_cycle:
 	fetch.1 	1				#low high j i i
	const.i8 	4				#low high j i i 4
	mul							#low high j i i*4
 	const.a32 	array			#low high j i i*4 array
	const.i8 	2				#low high j i i*4 array i32
	load.2d						#low high j i array[i*4]
 	const.a32 	val_mid			#low high j i array[i*4] val_mid
	const.i8 	2				#low high j i array[i*4] array[m] i32
	load						#low high j i array[i*4] array[m]
	ge							#low high j i t/f
 	const.a32 	end_i_cycle		#low high j i t/f end_i_cycle
	jmp.con						#low high j i
 	const.i8 	1				#low high j i 1
	add							#low high j i+1
 	const.a32 	i_cycle			#low high j i+1 i_cycle
	jmp 						#low high j i+1
end_i_cycle:
	mix.2 			1, 2		#low high i j
j_cycle:
 	fetch.1 	1				#low high i j j
	const.i8 	4				#low high i j j 4
	mul							#low high i j j*4
 	const.a32 	array			#low high i j j*4 array
	const.i8 	2				#low high i j j*4 array i32
	load.2d						#low high i j array[j*4]
 	const.a32 	val_mid			#low high i j array[j*4] val_mid
	const.i8 	2				#low high i j array[j*4] val_mid i32
	load						#low high i j array[j*4] array[m]
	le							#low high i j t/f
 	const.a32 	end_j_cycle		#low high i j t/f end_j_cycle
	jmp.con						#low high i j
 	const.i8 	1				#low high i j 1
	sub							#low high i j-1
 	const.a32 	j_cycle			#low high i j-1 j_cycle
	jmp 						#low high i j-1
end_j_cycle:
 	fetch.2 		2, 1		#low high i j i j
	gt							#low high i j t/f
 	const.a32 		not_swap	#low high i j t/f not_swap
	jmp.con						#low high i j
 	fetch.2 	2, 1			#low high i j i j
	const.a32 	exch			#low high i j i j exch
	call						#low high i j
 	const.i8 	1				#low high i j 1
	sub							#low high i j-1
 	mix.2 		1, 2			#low high j-1 i
	const.i8 	1				#low high j-1 i 1
	add							#low high j-1 i+1
	mix.2 		1, 2			#low high i+1 j-1
not_swap:
 	fetch.2 		2, 1		#low high i+1 j-1 i+1 j-1 
	le							#low high i+1 j-1 t/f
 	const.a32 		main_cycle	#low high i+1 j-1 t/f main_cycle
	jmp.con						#low high i+1 j-1
 	fetch.1 	4				#low high i j low
	mix.2 		1, 2			#low high i low j
	fetch.2 	2, 1			#low high i low j low j 
	ge							#low high i low j t/f
 	const.a32 	do_not_call_1	#low high i low j t/f do_not_call_1
	jmp.con						#low high i low j
 	const.a32 	sort			#low high i low j sort
	call						#low high i
 	const.a32 	call_2			#low high i call_2
	jmp							#low high i
do_not_call_1:
	drop 						#low high i low
	drop						#low high i
call_2:
 	mix.2 		1, 2			#low i high
	fetch.2 	2, 1			#low i high i high
	ge							#low i high t/f
 	const.a32 	do_not_call_2	#low i high t/f do_not_call_2 
	jmp.con						#low i high
 	const.a32 	sort			#low i high sort
	call						#low
	drop						#
	ret
do_not_call_2:
	drop						#low i
	drop						#low	
	drop						#
	ret

output:
	const.i32 	0				#i
output_loop:
 	fetch.1 	1				#i i
	const.i8 	4				#i i 4
	mul							#i i*4
 	const.a32 	array			#i i*4 array
	const.i8 	2				#i i*4 array i32
	load.2d						#i [array+i*4]
	print						#i [array+i*4]
 	drop						#i
	const.a32 	arrayCounter	#i arrayCounter
	const.i8 	2				#i arrayCounter i32
	load						#i [arrayCounter]
 	const.i8 	1				#i [arrayCounter] 1
	sub							#i [arrayCounter]-1
 	fetch.1 	2				#i [arrayCounter]-1 i
	eq							#i t/f
 	const.a32 	output_end		#i t/f output_end
	jmp.con						#i
 	const.i8 	1				#i 1
	add							#i+1
 	const.a32 	output_loop		#i+1 output_loop
	jmp 						#i+1
output_end:
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


# Variables
rndState:    		dc.u32  0xa1234567
arrayCounter: 		dc.i32	size_of_array
val_mid:			dc.i32  0
temp:				dc.i32  0
array: 				ds.i32	size_of_array
