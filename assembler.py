#!/usr/bin/python

import sys

from types   import typesKey
from opcodes import instructions

equs = {}         # EQUs
labels = {}       # LABELs
actions = []      # actions in precompile
precompile = []   # order all the things in a internal structure


def resolve(x):
    if x in equs:
        return resolve(equs[x])
    elif x in labels:
        return labels[x]
    else:
        try:
            return int(x, 0)
        except ValueError:
            return float(x)

with open(sys.argv[1]) as sourceFile:

    print('Start Assembling: ', sys.argv[1])

    for line in sourceFile:

        # comment
        if('#') in line:
            line, _ = line.split('#', 1)

        line = line.strip()

        # labels
        if ':' in line:
            label, line = line.split(':', 1)
            labels[label] = len(precompile)
            line = line.strip()

        # blank lines
        if not line:
            continue

        # EQUs
        if len(line.split()) == 3 and line.split()[1].lower() == 'equ':
            equs[line.split()[0]] = line.split()[2]
            continue

        splitLine = line.split(None, 1)
        lineArgs = [x.strip() for x in (splitLine[1].split(','))] if len(splitLine) == 2 else []
        lineArgsCount = len(lineArgs)

        # instructions
        instructionKey = (splitLine[0].lower(), lineArgsCount)
        if instructionKey in instructions:
            command = instructions[instructionKey]
            precompile.append(lambda x=command[0]: x)
            for i, length in zip(range(lineArgsCount), command[1]):
                argValue = lambda x=lineArgs[i]: resolve(x)
                # -----------float ??!?!?!?!?!
                actions.append(argValue)
                for j in range(length):
                    precompile.append(lambda x=argValue, y=j: (x() >> (8 * y)) & 0xFF)
            continue

        # DCs and DSs
        if splitLine[0].lower().split('.')[0] == 'dc':
            opBytes = int(typesKey[splitLine[0].lower().split('.', 1)[1]][0])
            val_func = lambda x=splitLine[1]: resolve(x)
            actions.append(val_func)
            for j in range(opBytes):
                precompile.append(lambda x=val_func, y=j: (x() >> (8 * y)) & 0xFF)
            continue
        if splitLine[0].lower().split('.')[0] == 'ds':
            opBytes = int(typesKey[splitLine[0].lower().split('.', 1)[1]][0])
            for i in range(opBytes * resolve(splitLine[1])):
                precompile.append(lambda: 0)
            continue

        print("command not found", repr(line), splitLine, file=sys.stderr)
        sys.exit(1)

# make the action magic
for action in actions:
    action()

# prepare the code to write
bytecode = bytearray()
for singleByteLine in precompile:
    bytecode.append(singleByteLine())

# write the code to file
filename = sys.argv[1].rsplit(".", 1)[0] + '.neo'
with open(filename, 'wb') as output_file:
    output_file.write(bytecode)

print('Ready Assembling. Writing to: ', filename)
print('Bytecode size: ', len(bytecode))
