
# coding: utf-8

# In[39]:

import os
#import fileinput
import sys

for p in sys.argv: print(p)
fdeclarations = []
usagemsg="Usage \"python gb2c [romname][.gb]\""
outputname='gbrom.h'
inputname='gbrom.gb'
if len(sys.argv) > 0:
    if len(sys.argv) == 1:
        print(usagemsg)
        sys.exit()
    if '.gb' in str(sys.argv[1]) :
        inputname = str(sys.argv[1])
        outputname = str(sys.argv[1])[:len(sys.argv[1])-3]
        outputname += 'ROM.h'
    else:
        if sys.argv[1] != "-f":
            inputname = str(sys.argv[1])+ '.gb'
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
    fout.write("#include <stdint.h>\n")
    fout.write("const uint8_t gbrom[] = {\n")
    byte = infile.read(1)
    counter = 0
    while byte:
        fout.write("0x"+byte.encode("hex") + ",")
        counter += 1
        if counter == 40:
            counter = 0
            fout.write("\n")
        byte = infile.read(1)
    fout.write("};\n\n#endif\n\n")
    fout.close()
    infile.close()
    print("*** DONE ! ***")






        

    
    

    




