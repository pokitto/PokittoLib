
# coding: utf-8

# In[39]:

import os
#import fileinput
import sys


def parseparameters(paramstr):
    funcparams = []
    somethingfound = True
    while somethingfound :
        for dtype in datatypes:
            gotcha = paramstr.find(dtype)
            somethingfound = False
            if gotcha > -1:
                funcparams.append(dtype)
                #print('found',dtype,'at',gotcha)
                paramstr = paramstr[gotcha+len(dtype):]
                somethingfound = True
                break
    return funcparams
     

for p in sys.argv: print(p)
fdeclarations = []
usagemsg="Usage \"python romnify [romname][.nes]\""
outputname='nesrom.h'
inputname='rom.nes'
if len(sys.argv) > 0:
    if len(sys.argv) == 1:
        print(usagemsg)
        sys.exit()
    if '.nes' in str(sys.argv[1]) :
        inputname = str(sys.argv[1])
        outputname = str(sys.argv[1])[:len(sys.argv[1])-4]
        outputname += 'ROM.h'
    else:
        if sys.argv[1] != "-f":
            inputname = str(sys.argv[1])+ '.nes'
            outputname = str(sys.argv[1])+'.h'
        else:
            inputname = str(sys.argv[1])
            outputname = 'output.h'

print("Input file: " + inputname)
print("Outputfile: " + outputname)

fout = open(outputname,"w")

with open(inputname, mode='rb') as infile:
    print("*** CREATING HEADER FILE ***")
    fout.write("#ifndef ")
    headername = outputname[:len(outputname)-2].upper()
    fout.write(headername+"_H\n")
    fout.write("#define ")
    fout.write(headername + "_H\n")
    byte = infile.read(1)
    bytecounter = 0
    fout.write("const char romheader")
    while byte != "":
        byte = infile.read(1)
    print("")
    print(fdeclarations)
    print("*** ADDING FUNCTION DECLARATIONS TO .CPP FILE HEAD ***")
    fout = open(outputname,"w")
    includes_end_at = 1
    linenum = 1
    infile.seek(0)
    for line in infile: #iterate through lines in file
        if '#include' in line:
            includes_end_at = linenum
        linenum += 1
    linenum = 1
    print("includes end at line:",includes_end_at)
    fout.write("/* This file was automatically parsed from an Arduino sourcefile  */\n")
    fout.write("/* by PokittoParser v0.1 Copyright 2016 Jonne Valola              */\n")
    fout.write("/* USE AT YOUR OWN RISK - NO GUARANTEES GIVEN OF 100% CORRECTNESS */\n")
    infile.seek(0)
    for line in infile: #iterate through lines in file
        if linenum == includes_end_at + 1:
            fout.write("#include \"Pokitto.h\"\n")
            fout.write("/* Auto-generated function declarations */\n")
            for fd in fdeclarations:
                fout.write(fd+'\n') #write the declarations
            fout.write(line) #remember to add original line
        else:
            fout.write(line)
        linenum += 1
    fout.close();
    infile.close();
    print("*** DONE ! ***")






        

    
    

    




