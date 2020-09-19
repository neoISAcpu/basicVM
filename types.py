# Reads types from types.txt file at script directory
import os, sys
        
types = []
typesKey ={}


fname = os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), 'types.txt')

with open(fname) as types_file:
    for line in types_file:
        if not line.strip():
            continue
        if not line.split()[0]=="#":
            types.append(line.split())
            typesKey[line.split()[4]]=line.split()[0], line.split()[1],line.split()[2],line.split()[3],line.split()[5]