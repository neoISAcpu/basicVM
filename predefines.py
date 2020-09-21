# Generates opcodes.h, defines.h and types.h in current directory
from gen_opcodes import opcodes_1, opcodes_2
from gen_types import types


with open('opcodes.h', 'w') as opcodes_header:
    opcodes_header.write('/* This file is generated by predefines.py script\n' +
                         'Please edit opcodes.txt instead of this file */\n')
    opcodes_header.write('#define OPCODES_COUNT ' + '256' + '\n')
    opcodes_header.write('void (*handlers[OPCODES_COUNT])(void) = {\n')
    for num, opcode in enumerate(opcodes_2):
        opcodes_header.write('\t' + opcode.lower() + ',\t// ' +
                             ("0x%x" % num) + '\n')

    # unused opcodes -> iop
    for num in range(len(opcodes_2), 96):
        opcodes_header.write('\t' + 'op_iop' + ',\t// ' + ("0x%x" % num) + '\n')

    # blk.xx commands
    for num in range(96, 128):
        opcodes_header.write('\t' + 'op_blk' + ',\t// ' + ("0x%x" % num) + '\n')

    # const.xxx commands
    for num in range(128, 256):
        opcodes_header.write('\t' + 'op_const' + ',\t// ' + ("0x%x" % num) + '\n')
    opcodes_header.write('};\n')

with open('defines.h', 'w') as opcodes_header:
    opcodes_header.write('/* This file is generated by predefines.py script\n' +
                         'Please edit opcodes.txt instead of this file */\n')
    for num, opcode in enumerate(opcodes_1):
        opcodes_header.write('\t#define def_' + opcode.lower().replace('.', '_') + '        ' + ("0x%x" % num) + '\n')

with open('types.h', 'w') as opcodes_header:
    opcodes_header.write('/* This file is generated by predefines.py script\n' +
                         'Please edit types.txt instead of this file */\n\n' +
                         '// definitions of all operational types\n\n' +
                         'typedef struct\n' +
                         '{\n' +
                         '    int numOfBytes;\n' +
                         '    int numOfBytesPerElement;\n' +
                         '    int numOfVectorElements;\n' +
                         '    int isBool;\n' +
                         '    char *typeName;\n' +
                         '} typeElement;\n\n' +
                         'const typeElement op_types[128] = {\n')
    for num, line in enumerate(types):
        opcodes_header.write('\t{' + line[0] + ', ' + line[1] + ', ' + line[2] + ', ' + line[3] + ', \"' + line[4] + '\"}, \n')

    opcodes_header.write('};\n')
