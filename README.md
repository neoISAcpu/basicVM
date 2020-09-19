# neoISA - basicVM 

this is the first simple variant of the stack based vector isa vm. it should serve as a playground for the idea described on [neoISA](https://neoisa.org). it is not about establishing a new isa, but agilely developing an isa that harmonizes with the llvm-ir. it is not the fastest interpreter and i don't use vector libraries. it is only for demonstration purposes.


# features:

*ssa stack based
*unified stack integer/float/bool and vector of these, up to a maximum length of 1 cache line - 512bit
*there is an arithmetic and a call stack
*scratchpad for the contents of the stack. stack holds the references and additional information such as the type 
*atomic 8bit commands, which can be bundled to several.
*constant feed the stack with parameters
*all commands which need operands, get them from the stack. 
*all commands can handle vectors as operands, also load, store, jump and call
*there are no addressing modes. all address calculations have to be done by yourself. 
*there is no rigid model regarding vector length or number of parallel units


# start

first you have to generate the files that were introduced as interface between python and c.
```bash
python3 predefines.py
```

then the interpreter should be compiled with:
```bash
cc -Wall -O3  stack_machine.c -o stack_machine
```

now one of the examples can be translated:
```bash
python3 assembler.py examples/quick_sort_block.asm
```

to execute it afterwards:
```bash
./stack_machine examples/quick_sort_block.neo
```




### have fun while trying ;)

