#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "defines.h"
#include "types.h"

// pseudo include stack_machine.h
void next_step();
void die(char *message);

#define MEM_SIZE         65536        // bytecode loads in memory at 0 address, and 0 address takes control
#define SCRATCH_MEM_SIZE 1024 * 1024  // bytecode loads in memory at 0 address, and 0 address takes control
#define ARITH_STACK_SIZE 2048         // separated stack to make calculations
#define CALL_STACK_SIZE  2048         // call stack to store return addresses
#define PROG_STACK_SIZE  2048         // program stack to store code sequences
#define STACK_SIZE       2048         // indirect new stack

unsigned char mem[MEM_SIZE];          // main memory as byte representation

int i_pointer;                        // instruction pointer

int call_stack[CALL_STACK_SIZE];
int call_stack_pointer;               // call stack cursor.

unsigned int stack[STACK_SIZE];       // real size in bytes is ARITH_STACK_SIZE * sizeof(int)
int stack_pointer;

unsigned char scratch[SCRATCH_MEM_SIZE]; // scratchpad memory as byte representation

unsigned char tempArray[64 * 64];     // calc temp line

long long int instructionCounter, blockCounter;

void load_prg(int argc, char **argv)
{
    FILE *bfile;
    srand(time(NULL));
    if (argc < 2)
        die("bytecode file not specified");
    printf("Bytecode file is: %s\n", argv[1]);
    bfile = fopen(argv[1], "r");
    if (bfile == NULL)
        die("cannot open bytecode file");
    fread(mem, 1, MEM_SIZE, bfile);
    if (!feof(bfile))
        die("bytecode file too big");
}

void inc_i_pointer(int i)
{
    if (i_pointer + i >= MEM_SIZE)
        die("unexpected end of code");
    i_pointer += i;
}

int resultType(int type1, int type2)
{
    int result = 0;

    if (((type1 & 0x70) == 0x70) | ((type2 & 0x70) == 0x70)) //bool
    {
        if (((type1 & 0x70) == 0x70) & ((type2 & 0x70) == 0x70))
        {
            if (type1 == type2)
                return type1;
            else
                return -1;
        }
        else
            return -1;
    }

    if (((type1 & 0x0c) == 0x08) | ((type2 & 0x0c) == 0x08)) //float - no bool
    {
        if (((type1 & 0x0c) == 0x08) & ((type2 & 0x0c) == 0x08))
        {
            int floatSize1, floatSize2;
            int vectorType1, vectortype2;
            result |= 0x08;
            floatSize1 = (type1 & 0x03);
            floatSize2 = (type2 & 0x03);
            vectorType1 = (type1 & 0x70);
            vectortype2 = (type2 & 0x70);
            result |= (vectorType1 > vectortype2) ? vectorType1 : vectortype2;
            result |= (floatSize1 > floatSize2) ? floatSize1 : floatSize2;
            // return the bigger float type and the bigger vector type, but the result must be <=512bit (op_types check)
            return (op_types[result].numOfBytes == -1) ? -1 : result;
        }
        else
            return -1;
    }

    if (((type1 & 0x0c) == 0x0c) | ((type2 & 0x0c) == 0x0c)) // i, u, a - no float, no bool
    {
        int addressSize1, addressSize2;
        int vectorType1, vectortype2;
        result |= 0x0c;
        addressSize1 = (type1 & 0x03);
        addressSize2 = (type2 & 0x03);
        vectorType1 = (type1 & 0x70);
        vectortype2 = (type2 & 0x70);
        result |= (vectorType1 > vectortype2) ? vectorType1 : vectortype2;
        result |= (addressSize1 > addressSize2) ? addressSize1 : addressSize2;
        // return the bigger address type and the bigger vector type, but the result must be <=512bit (op_types check)
        return (op_types[result].numOfBytes == -1) ? -1 : result;
    }
    else
    {
        int intSize1, intSize2;
        int intType1, intType2;
        int vectorType1, vectortype2;
        intSize1 = (type1 & 0x03);
        intSize2 = (type2 & 0x03);
        intType1 = (type1 & 0x0c);
        intType2 = (type2 & 0x0c);
        vectorType1 = (type1 & 0x70);
        vectortype2 = (type2 & 0x70);
        if (intSize1 == intSize2)
        {
            if (intType1 > intType2)
            {
                result |= intType1;
                result |= intSize1;
            }
            else
            {
                result |= intType2;
                result |= intSize2;
            }
        }
        else
        {
            if (intSize1 > intSize2)
            {
                result |= intType1;
                result |= intSize1;
            }
            else
            {
                result |= intType2;
                result |= intSize2;
            }
        }
        result |= (vectorType1 > vectortype2) ? vectorType1 : vectortype2;
        // return the bigger address type and the bigger vector type, but the result must be <=512bit (op_types check)
        return (op_types[result].numOfBytes == -1) ? -1 : result;
    }

    return -2;
}

int resultBoolType(int type1, int type2)
{
    int result, targetType = 0;
    int vectorType1, vectortype2;

    if (((type1 & 0x70) == 0x70) | ((type2 & 0x70) == 0x70)) //bool
    {
        return -1;
    }

    result = 0x70;
    vectorType1 = op_types[type1].numOfVectorElements;
    vectortype2 = op_types[type2].numOfVectorElements;
    result = (vectorType1 > vectortype2) ? vectorType1 : vectortype2;

    for (int i = 0x70; i < 0x7f; i++)
        if (op_types[i].numOfVectorElements == result)
        {
            targetType = i;
            break;
        }

    return (op_types[targetType].numOfBytes == -1) ? -1 : targetType;
}

// mem access ---------------------------------------------
long long unsigned int get_mem(int size, unsigned char *memory, int index)
{
    long long unsigned int wert = 0;
    for (int i = 0; i < size; i++)
        wert |= (long long unsigned int)(memory[index + i] << (long long unsigned int)i * 8);

    if (memory[index + size - 1] & 0x80)
        for (int i = size; i < 8; i++)
            wert |= (0xffLL << (i * 8));

    return wert;
}

void set_mem(int size, unsigned char *memory, long long unsigned int value)
{
    for (int i = 0; i < size; i++)
        memory[i] = (long long unsigned int)((value >> i * 8) & 0xff);
}

// stack access ----------------------------------------------------------------
// stack element: 32bit - 2bit stack_elemt_typ, 1bit valid, 1bit NaN, 7bit type, 20bit scratch adresse

void inc_stack_pointer()
{
    if (stack_pointer == STACK_SIZE - 1)
        die("stack overflow");
    stack_pointer++;
}

void drop_stack(int amount)
{
    if (stack_pointer - amount < 0)
        die("drop from empty stack or to many");
    stack_pointer -= amount;
}

void inc_call_stack_pointer()
{
    call_stack_pointer++;
    if (call_stack_pointer >= CALL_STACK_SIZE)
        die("call stack overflow");
}

void dec_call_stack_pointer()
{
    call_stack_pointer--;
    if (call_stack_pointer < -1)
        die("call stack underflow");
}

void check_scratch_pointer(int i)
{
    if (i >= SCRATCH_MEM_SIZE)
        die("unexpected end: scratch_pointer to big");
}

int get_type_from_stackElement(int stackPointer)
{
    if (stackPointer < 0)
        die("unexpected end: stack to small in get_type_from_stackElement");

    return stack[stackPointer] >> 20 & 0x7f;
}

int get_size_from_stackElement(int stackPointer)
{
    if (stackPointer < 0)
        die("unexpected end: stack to small in get_size_from_stackElement");

    return op_types[stack[stackPointer] >> 20 & 0x7f].numOfBytes;
}

int get_address_from_stackElement(int stackPointer)
{
    if (stackPointer < 0)
        die("unexpected end: stack to small in get_address_from_stackElement");

    return stack[stackPointer] & 0xfffff;
}

int add_empty_element_to_stack(int type)
{
    int temp;
    inc_stack_pointer();
    temp = get_address_from_stackElement(stack_pointer - 1) + get_size_from_stackElement(stack_pointer - 1);
    check_scratch_pointer(temp);
    stack[stack_pointer] = ((type << 20) | (temp & 0xfffff));
    return temp;
}

int copy_from_stack(int stackpointer, unsigned char *memory)
{
    int stackElementAddress, stackElementType, targetSize;
    stackElementAddress = get_address_from_stackElement(stackpointer);
    stackElementType = get_type_from_stackElement(stackpointer);
    targetSize = op_types[stackElementType].numOfBytes;

    for (int i = 0; i < targetSize; i++)
    {
        memory[i] = scratch[stackElementAddress + i];
    }

    return stackElementType;
}

void push_to_stack(unsigned char type, unsigned char *memory)
{
    int count, scratch_address;

    count = op_types[type].numOfBytes;
    scratch_address = add_empty_element_to_stack(type);
    for (int i = 0; i < count; i++)
    {
        scratch[scratch_address + i] = memory[i];
    }
}

// ----------------------------------------------------------------

void succ_exit()
{
    // maybe you need to replace %I64i with %lli
    printf("End of program, success\nInstructions: \t%I64lli\nBlocks: \t%I64lli\ntime: \t\t%.3f\n", instructionCounter, blockCounter, (clock() * 1.0) / CLOCKS_PER_SEC);
    exit(0);
}

void alu(int operation)
{
    int opType1, opType2, resultType1;

    opType1 = get_type_from_stackElement(stack_pointer);
    opType2 = get_type_from_stackElement(stack_pointer - 1);
    resultType1 = resultType(opType1, opType2);
    if (resultType1 != -1 && !op_types[resultType1].isBool)
    {
        int op1Address, op2Address;
        int addressCounterOp1, addressCounterOp2, addressCounterR;
        long long int x, y, wert;

        op1Address = get_address_from_stackElement(stack_pointer);
        op2Address = get_address_from_stackElement(stack_pointer - 1);

        for (int i = 0; i < op_types[resultType1].numOfVectorElements; i++)
        {
            addressCounterOp1 = (i % op_types[opType1].numOfVectorElements) * op_types[opType1].numOfBytesPerElement;
            addressCounterOp2 = (i % op_types[opType2].numOfVectorElements) * op_types[opType2].numOfBytesPerElement;
            addressCounterR = (i % op_types[resultType1].numOfVectorElements) * op_types[resultType1].numOfBytesPerElement;

            x = get_mem(op_types[opType1].numOfBytesPerElement, scratch, op1Address + addressCounterOp1);
            y = get_mem(op_types[opType2].numOfBytesPerElement, scratch, op2Address + addressCounterOp2);
            // and finaly the operation ----------------------------------------------------------------------xxxxxx----
            switch (operation)
            {
            case 1: //add
                wert = y + x;
                break;
            case 2: //sub
                wert = y - x;
                break;
            case 3: //mul
                wert = y * x;
                break;
            case 4: //div
                wert = y / x;
                break;
            case 5: //rem
                wert = y % x;
                break;

            case 16: //and
                wert = y & x;
                break;
            case 17: //or
                wert = y | x;
                break;
            case 18: //xor
                wert = y ^ x;
                break;
            case 19: //shl
                wert = y << x;
                break;
            case 20: //shr
                wert = y >> x;
                break;

            default:
                die("wrong alu operation! ");
                break;
            }

            set_mem(op_types[resultType1].numOfBytesPerElement, tempArray + addressCounterR, wert);
        }
        drop_stack(2);
        push_to_stack(resultType1, tempArray);
    }
    else
        die("add: result type not supported");
}

void compare(int operation)
{
    // bool as input dont funktional
    int opType1, opType2, resultType1;

    opType1 = get_type_from_stackElement(stack_pointer);
    opType2 = get_type_from_stackElement(stack_pointer - 1);
    resultType1 = resultBoolType(opType1, opType2);
    if (resultType1 != -1)
    {
        int op1Address, op2Address, targetsize;
        int addressCounterOp1, addressCounterOp2;
        long long int x, y;
        long long unsigned int wert = 0;

        op1Address = get_address_from_stackElement(stack_pointer);
        op2Address = get_address_from_stackElement(stack_pointer - 1);

        for (int i = 0; i < op_types[resultType1].numOfVectorElements; i++)
        {
            addressCounterOp1 = (i % op_types[opType1].numOfVectorElements) * op_types[opType1].numOfBytesPerElement;
            addressCounterOp2 = (i % op_types[opType2].numOfVectorElements) * op_types[opType2].numOfBytesPerElement;

            x = get_mem(op_types[opType1].numOfBytesPerElement, scratch, op1Address + addressCounterOp1);
            y = get_mem(op_types[opType2].numOfBytesPerElement, scratch, op2Address + addressCounterOp2);
            // and finaly the operation -----xxxxxx----
            switch (operation)
            {
            case 1: // eq
                wert |= (long long unsigned int)(y == x) << i;
                break;
            case 2: // ne
                wert |= (long long unsigned int)(y != x) << i;
                break;
            case 3: // lt
                wert |= (long long unsigned int)(y < x) << i;
                break;
            case 4: // gt
                wert |= (long long unsigned int)(y > x) << i;
                break;
            case 5: // le
                wert |= (long long unsigned int)(y <= x) << i;
                break;
            case 6: // ge
                wert |= (long long unsigned int)(y >= x) << i;
                break;

            default:
                die("wrong compare operation");
                break;
            }
        }
        drop_stack(2);
        targetsize = op_types[resultType1].numOfBytes;
        set_mem(targetsize, tempArray, wert);
        push_to_stack(resultType1, tempArray);
    }
    else
        die("add: result type not supported");
}

// start opcodes --------------------------------------------------------------------------------------------------------------------

void op_iop()
{
    long long unsigned int wert = 0;
    int opType, opAddress;

    printf("iop \n");
    printf("Illegal operation at 0x%x\n", i_pointer);
    printf("--------------------------------------\n");
    printf("Call stack depth: %i\n", call_stack_pointer + 1);
    printf("Stack depth: %i\n", stack_pointer + 1);
    printf("Stack content:\n");
    for (int i = stack_pointer; i >= 0; i--)
    {
        opType = get_type_from_stackElement(i);
        opAddress = get_address_from_stackElement(i);
        printf("pos: %i type=%s   ", i, op_types[opType].typeName);
        for (int i = 0; i < op_types[opType].numOfVectorElements; i++)
        {
            wert = get_mem(op_types[opType].numOfBytesPerElement, scratch, opAddress + op_types[opType].numOfBytesPerElement * i);
            printf("0x%llx oder %lli  -  ", wert, wert);
        }
        printf("\n");
    }
    printf("--------------------------------------\n");
    getchar();
}

void op_nop()
{
}

// arithmetik stuff

void op_add()
{
    alu(1);
}

void op_sub()
{
    alu(2);
}

void op_mul()
{
    alu(3);
}

void op_divide()
{
    alu(4);
}

void op_rem()
{
    alu(5);
}

void op_and()
{
    alu(16);
}

void op_or()
{
    alu(17);
}

void op_xor()
{
    alu(18);
}

void op_shl()
{
    alu(19);
}

void op_shr()
{
    alu(20);
}

void op_load()
{
    // load: TOS=type, next=address

    unsigned char opType1, opType2, resultType, targetSize;
    int op1Address, op2Address, targetAddress;

    if (stack_pointer < 1)
        die("not enough elements in stack for load");

    opType1 = get_type_from_stackElement(stack_pointer);
    opType2 = get_type_from_stackElement(stack_pointer - 1);

    if (opType1 != 0x00)
        die("Parameter 1 must be a i8 - load");
    if ((opType2 & 0x0c) != 0x0c)
        die("Parameter 2 must be a address - load");

    op1Address = get_address_from_stackElement(stack_pointer);
    op2Address = get_address_from_stackElement(stack_pointer - 1);
    resultType = get_mem(op_types[opType1].numOfBytesPerElement, scratch, op1Address);

    targetSize = op_types[resultType].numOfBytes;

    drop_stack(2);
    for (int i = 0; i < op_types[opType2].numOfVectorElements; i++)
    {
        targetAddress = get_mem(op_types[opType2].numOfBytesPerElement, scratch, op2Address + op_types[opType2].numOfBytesPerElement * i);

        for (int j = 0; j < targetSize; j++)
        {
            tempArray[j] = mem[targetAddress + j];
        }
        push_to_stack(resultType, tempArray);
    }
}

void op_load_2d()
{
    // load: TOS=type, next1=address1, next2= address2
    int opType1;
    unsigned char tempA1[64];

    opType1 = copy_from_stack(stack_pointer, tempA1);
    drop_stack(1);
    op_add();
    push_to_stack(opType1, tempA1);
    op_load();
}

void op_load_mem()
{
}

void op_store()
{
    int opType1, opType2, targetSize;
    int op1Address, op2Address, targetAddress;

    if (stack_pointer < 0)
        die("not enough elements in stack for store");

    opType1 = get_type_from_stackElement(stack_pointer);
    if ((opType1 & 0x0c) != 0x0c)
        die("Parameter 1 must be a address - store");
    op1Address = get_address_from_stackElement(stack_pointer);

    drop_stack(1);
    for (int i = 0; i < op_types[opType1].numOfVectorElements; i++)
    {
        targetAddress = get_mem(op_types[opType1].numOfBytesPerElement, scratch, op1Address + op_types[opType1].numOfBytesPerElement * i);
        op2Address = get_address_from_stackElement(stack_pointer);
        opType2 = get_type_from_stackElement(stack_pointer);
        targetSize = op_types[opType2].numOfBytes;

        for (int j = 0; j < targetSize; j++)
        {
            mem[targetAddress + j] = scratch[op2Address + j];
        }
        drop_stack(1);
    }
}

void op_store_2d()
{
    op_add();
    op_store();
}

void op_store_mem()
{
}

void op_convert()
{
}

void op_fetch()
{
    int counter = 0, elementCount = 0;
    int temp1, type1;
    unsigned char tempA1[64];

    switch ((unsigned char)get_mem(1, mem, i_pointer))
    {
    case def_fetch_1:
        counter = 1;
        break;
    case def_fetch_2:
        counter = 2;
        break;
    case def_fetch_4:
        counter = 4;
        break;
    case def_fetch_8:
        counter = 8;
        break;
    }

    for (int i = 0; i < counter; i++)
    {
        inc_i_pointer(1);
        temp1 = (unsigned char)get_mem(1, mem, i_pointer);
        if (stack_pointer - temp1 + 1 - elementCount < 0)
            die("Not enough elements in stack for fetch");
        type1 = copy_from_stack(stack_pointer - temp1 + 1 - elementCount, tempA1);
        if (temp1 > 0)
        {
            push_to_stack(type1, tempA1);
            elementCount++;
        }
    }
}

void op_mix()
{
    int counter = 0, elementCount = 0;
    int elementAddress[8], type1[8];
    unsigned char tempA1[8][64];

    switch ((unsigned char)get_mem(1, mem, i_pointer))
    {
    case def_mix_2:
        counter = 2;
        break;
    case def_mix_4:
        counter = 4;
        break;
    case def_mix_8:
        counter = 8;
        break;
    }

    for (int i = 0; i < counter; i++)
    {
        inc_i_pointer(1);
        elementAddress[i] = (unsigned char)get_mem(1, mem, i_pointer);
        if (stack_pointer - elementAddress[i] + 1 < 0)
            die("Not enough elements in stack for mix");
        type1[i] = copy_from_stack(stack_pointer - elementAddress[i] + 1, tempA1[i]);
        if (elementAddress[i] > 0)
            elementCount++;
    }

    drop_stack(elementCount);

    for (int i = 0; i < counter; i++)
    {
        if (elementAddress[i] > 0)
            push_to_stack(type1[i], tempA1[i]);
    }
}

void op_drop()
{
    drop_stack(1);
}

void op_eqz()
{
    // bool as input dont work
    long long unsigned int wert = 0;

    int opType, opAddress, opVectors, opSizePerElement;
    int targetsize, targetType;

    if (stack_pointer < 0)
        die("not enough elements in stack for eqz");

    opType = get_type_from_stackElement(stack_pointer);
    opAddress = get_address_from_stackElement(stack_pointer);
    opVectors = op_types[opType].numOfVectorElements;
    opSizePerElement = op_types[opType].numOfBytesPerElement;
    for (int i = 0; i < opVectors; i++)
    {
        wert |= (long long unsigned int)(get_mem(opSizePerElement, scratch, opAddress + opSizePerElement * i) == 0) << i;
    }

    targetType = resultBoolType(opType, 0x00);

    drop_stack(1);
    targetsize = op_types[targetType].numOfBytes;
    set_mem(targetsize, tempArray, wert);
    push_to_stack(targetType, tempArray);
}

void op_eqnz()
{
    // bool as input dont work
    long long unsigned int wert = 0;

    int opType, opAddress, opVectors, opSizePerElement;
    int targetsize, targetType;

    if (stack_pointer < 0)
        die("not enough elements in stack for eqz");

    opType = get_type_from_stackElement(stack_pointer);
    opAddress = get_address_from_stackElement(stack_pointer);
    opVectors = op_types[opType].numOfVectorElements;
    opSizePerElement = op_types[opType].numOfBytesPerElement;
    for (int i = 0; i < opVectors; i++)
    {
        wert |= (long long unsigned int)(get_mem(opSizePerElement, scratch, opAddress + opSizePerElement * i) != 0) << i;
    }

    targetType = resultBoolType(opType, 0x00);

    drop_stack(1);
    targetsize = op_types[targetType].numOfBytes;
    set_mem(targetsize, tempArray, wert);
    push_to_stack(targetType, tempArray);
}

void op_eq()
{
    compare(1);
}

void op_ne()
{
    compare(2);
}

void op_lt()
{
    compare(3);
}

void op_gt()
{
    compare(4);
}

void op_le()
{
    compare(5);
}

void op_ge()
{
    compare(6);
}

void op_jmp()
{
    int opType1;
    long long int x;
    unsigned char tempA1[64];

    if (stack_pointer < 0)
        die("not enough elements in stack for jmp.con");

    opType1 = copy_from_stack(stack_pointer, tempA1);
    drop_stack(1);

    if ((opType1 & 0x0c) != 0x0c)
        die("Parameter 1 must be a address - jmp.con");

    for (int i = 0; i < op_types[opType1].numOfVectorElements; i++)
    {
        x = get_mem(op_types[opType1].numOfBytesPerElement, tempA1, op_types[opType1].numOfBytesPerElement * i);
        if (x > 0)
        {
            i_pointer = x - 1;
            if (i_pointer >= MEM_SIZE || i_pointer < -1)
                die("segmentation fault after jmp");
            break;
        }
    }
}

void op_jmp_con()
{
    int opType1, opType2;
    long long int x;
    long long unsigned int wert = 0;
    unsigned char tempA1[64], tempA2[64];

    if (stack_pointer < 1)
        die("not enough elements in stack for jmp.con");

    opType1 = copy_from_stack(stack_pointer, tempA1);
    opType2 = copy_from_stack(stack_pointer - 1, tempA2);
    drop_stack(2);

    if ((opType1 & 0x0c) != 0x0c)
        die("Parameter 1 must be a address - jmp.con");
    if ((opType2 & 0x70) != 0x70)
        die("Parameter 2 must be a bool - jmp.con");
    if (op_types[opType1].numOfVectorElements != op_types[opType2].numOfVectorElements)
        die("both parameter must be the same vectorsize");

    for (int i = 0; i < op_types[opType1].numOfVectorElements; i++)
    {
        x = get_mem(op_types[opType1].numOfBytesPerElement, tempA1, op_types[opType1].numOfBytesPerElement * i);
        wert = get_mem(op_types[opType2].numOfBytesPerElement, tempA2, 0);

        if (wert & 0x01LL << i)
        {
            i_pointer = x - 1;
            if (i_pointer >= MEM_SIZE || i_pointer < -1)
                die("segmentation fault after jmp");
            break;
        }
    }
}

void op_call()
{
    int opType1;
    long long int x;
    unsigned char tempA1[64];

    if (stack_pointer < 0)
        die("not enough elements in stack for call");

    opType1 = copy_from_stack(stack_pointer, tempA1);
    drop_stack(1);

    if ((opType1 & 0x0c) != 0x0c)
        die("Parameter 1 must be a address - call");

    for (int i = 0; i < op_types[opType1].numOfVectorElements; i++)
    {
        x = get_mem(op_types[opType1].numOfBytesPerElement, tempA1, op_types[opType1].numOfBytesPerElement * i);

        if (x > 0)
        {
            inc_call_stack_pointer();
            call_stack[call_stack_pointer] = i_pointer + 1;

            i_pointer = x;
            if (i_pointer >= MEM_SIZE || i_pointer < -1)
                die("segmentation fault after call");
            next_step();
        }
    }
}

void op_call_con()
{
    int opType1, opType2;
    long long int x;
    long long unsigned int wert = 0;
    unsigned char tempA1[64], tempA2[64];

    if (stack_pointer < 1)
        die("not enough elements in stack for call.con");

    opType1 = copy_from_stack(stack_pointer, tempA1);
    opType2 = copy_from_stack(stack_pointer - 1, tempA2);
    drop_stack(2);

    if ((opType1 & 0x0c) != 0x0c)
        die("Parameter 1 must be a address - call.con");
    if ((opType2 & 0x70) != 0x70)
        die("Parameter 2 must be a bool - call.con");
    if (op_types[opType1].numOfVectorElements != op_types[opType2].numOfVectorElements)
        die("both parameter must be the same vectorsize");

    for (int i = 0; i < op_types[opType1].numOfVectorElements; i++)
    {
        x = get_mem(op_types[opType1].numOfBytesPerElement, tempA1, op_types[opType1].numOfBytesPerElement * i);
        wert = get_mem(op_types[opType2].numOfBytesPerElement, tempA2, 0);

        if ((wert & 0x01LL << i) && (x > 0))
        {
            inc_call_stack_pointer();
            call_stack[call_stack_pointer] = i_pointer + 1;

            i_pointer = x;
            if (i_pointer >= MEM_SIZE || i_pointer < -1)
                die("segmentation fault after jmp");
            next_step();
        }
    }
}

void op_ret()
{
    if (call_stack_pointer < 0)
        die("cannot ret: call stack is empty");
    if (call_stack[call_stack_pointer] < 0 || call_stack[call_stack_pointer] >= MEM_SIZE)
        die("segmentation fault in ret");
    i_pointer = call_stack[call_stack_pointer] - 1;
    dec_call_stack_pointer();
}

void op_mem()
{
}

void op_blk()
{
    blockCounter -= ((unsigned char)get_mem(1, mem, i_pointer) - 95);
    instructionCounter--;
}

//------------------------

void op_bp()
{
    long long unsigned int wert = 0;
    int opType, opAddress;

    printf("--------------------------------------\n");
    printf("Breakpoint reached at 0x%x\n", i_pointer);
    printf("Call stack depth: %i\n", call_stack_pointer + 1);
    printf("Stack depth: %i\n", stack_pointer + 1);
    printf("--------------------\n");
    printf("Stack content:\n");
    for (int i = stack_pointer; i >= 0; i--)
    {
        opType = get_type_from_stackElement(i);
        opAddress = get_address_from_stackElement(i);
        printf("pos: %i type=%s   ", i, op_types[opType].typeName);
        for (int i = 0; i < op_types[opType].numOfVectorElements; i++)
        {
            wert = get_mem(op_types[opType].numOfBytesPerElement, scratch, opAddress + op_types[opType].numOfBytesPerElement * i);
            printf("0x%llx oder %lli  -  ", wert, wert);
        }
        printf("\n");
    }
    printf("Press enter to continue\n");
    printf("--------------------------------------\n");

    getchar();
}

void op_print_stack()
{
    long long unsigned int wert = 0;
    int opType, opAddress;

    printf("--------------------------------------\n");
    printf("Call stack depth: %i\n", call_stack_pointer + 1);
    printf("Stack depth: %i\n", stack_pointer + 1);
    printf("Stack content:\n");
    for (int i = stack_pointer; i >= 0; i--)
    {
        opType = get_type_from_stackElement(i);
        opAddress = get_address_from_stackElement(i);
        printf("pos: %i type=%s   ", i, op_types[opType].typeName);
        for (int i = 0; i < op_types[opType].numOfVectorElements; i++)
        {
            wert = get_mem(op_types[opType].numOfBytesPerElement, scratch, opAddress + op_types[opType].numOfBytesPerElement * i);
            printf("0x%llx or %lli  -  ", wert, wert);
        }
        printf("\n");
    }
    printf("--------------------------------------\n");
}

void op_print()
{
    long long unsigned int wert = 0;
    int opType, opAddress;
    if (stack_pointer < 0)
        printf("Stack is empty, continue...\n");
    else
    {
        opType = get_type_from_stackElement(stack_pointer);
        opAddress = get_address_from_stackElement(stack_pointer);
        printf("-=> type=%s   ", op_types[opType].typeName);
        for (int i = 0; i < op_types[opType].numOfVectorElements; i++)
        {
            wert = get_mem(op_types[opType].numOfBytesPerElement, scratch, opAddress + op_types[opType].numOfBytesPerElement * i);
            printf("0x%llx or %lli  -  ", wert, wert);
        }
        printf("\n");
    }
}

void op_quit()
{
    succ_exit();
}

void op_const()
{
    int type;

    type = (unsigned char)get_mem(1, mem, i_pointer) & 0x7f;
    push_to_stack(type, mem + i_pointer + 1);

    inc_i_pointer(get_size_from_stackElement(stack_pointer));

}

// end opcodes -----------------------------------------------------------------------------------------------------------------------


// All handlers should be declared before including opcodes.h
#include "opcodes.h"

void die(char *message)
{
    long long unsigned int wert = 0;
    int opType, opAddress;

    printf("--------------------------------------\n");
    printf("VM-Error: %s at 0x%x, exiting\n", message, i_pointer);
    printf("Call stack depth: %i\n", call_stack_pointer + 1);
    printf("Stack depth: %i\n", stack_pointer + 1);
    printf("Stack content:\n");
    for (int i = stack_pointer; i >= 0; i--)
    {
        opType = get_type_from_stackElement(i);
        opAddress = get_address_from_stackElement(i);
        printf("pos: %i type=%s   ", i, op_types[opType].typeName);
        for (int i = 0; i < op_types[opType].numOfVectorElements; i++)
        {
            wert = get_mem(op_types[opType].numOfBytesPerElement, scratch, opAddress + op_types[opType].numOfBytesPerElement * i);
            printf("0x%llx or %lli  -  ", wert, wert);
        }
        printf("\n");
    }
    printf("--------------------------------------\n");

    exit(1);
}

void next_step()
{
    instructionCounter++;
    blockCounter++;
    handlers[(unsigned char)get_mem(1, mem, i_pointer)]();
}

void main_loop()
{
    int scratchStartAddress = 0;
    i_pointer = 0;
    call_stack_pointer = -1;

    stack_pointer = -1;

    instructionCounter = 0;
    blockCounter = 0;

    // putting one initial "void" on the stack with zero length
    inc_stack_pointer();
    stack[stack_pointer] = ((0x7f << 20) | (scratchStartAddress & 0xfffff));

    while (1)
    {
        next_step();
        inc_i_pointer(1);
    }
}

int main(int argc, char **argv)
{
    load_prg(argc, argv);
    main_loop();
}
