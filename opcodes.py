# Reads opcodes from opcodes.txt
import os
import sys
from types import types

opcodes_1 = []
opcodes_2 = []
instructions = {}

instructionNumber = 0

fname = os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), 'opcodes.txt')

with open(fname) as opcodes_file:
    for num, line in enumerate(opcodes_file):
        if not line.strip():
            continue
        if not line.startswith("#"):
            opcodes_1.append(line.split()[0])
            opcodes_2.append(line.split()[1])
            instructions[line.split()[0].lower(), int(line.split()[2])] = instructionNumber, [int(line.split()[3]) for x in range(int(line.split()[2]))]
            instructionNumber += 1

    # inserts the blk.xy instruction
    for num in range(96, 128):
        instructions['blk.' + str(num-95), 0] = num, []

    # inserts the const.x.y instruction
    for num, line in enumerate(types):
        instructions['const.' + line[4].lower(), int(line[2])] = 128 + num, [int(line[1]) for x in range(int(line[2]))]
