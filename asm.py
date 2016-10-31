#!/usr/bin/env python
import os
import sys
import re
import string
import argparse

line=0 # global variable to make error reporting easier
current_instr="" # idem
labels={} # global because shared between the two passe


def error(e):
    raise BaseException("Error at line " + str(line) + " : " + e)

        
def asm_dest_reg(dest, max):#Registre de destination.
    """converts the string op1 into its encoding in the machine instruction. 
    max should be 7 or 15, depending on the instruction"""
    val = int(dest[1:]) # this removes the "r". TODO catch exception here
    if dest[0]!='r' or val<0 or val>max:
        error("invalid destination register: " + dest)
    else:
        return val<<8


def asm_operand1(op1): #1ere operande
    """converts the string op1 into its encoding in the machine instruction"""
    #begin sabote
    val = int(op1[1:])
    if op1[0]!='r' or val<0 or val>15:
        error("invalid first operand :" + op1)
    else:
        return val<<4
    #end sabote
    
def asm_operand2(op2, signed_constant): #2eme operande
    """converts the string op2 into its encoding in the machine instruction"""
    #begin sabote
    if op2[0]!='r':
        val=int(op2)
        if signed_constant:
            if val< -8 or val > 7:
                error("constant out of range: " + op2)
            else:
                if val<0:
                    return (1<<11)+16+val
                else:
                    return (1<<11)+val
        else:
            if val<0 or val>255:
                error("constant out of range: "+op2)
            else:
                return val+(1<<11)
    else:
        val = int(op2[1:])
        if val<0 or val>15:
            error("invalid second operand: "+op1)
        else:
            return val
    #end sabote

def asm_memory_access(op): #Acces memoire pour wmem et rmem
    """converts the string op that should contain [rnn] into its encoding in the machine instruction"""
    #begin sabote
    val = int(op[2:-1])
    if (op[0]!='[' and op[1]!='r' and op[-1]!=']') or val<0 or val>15:
        error("invalid memory access: " + op)
    else:
        return val
    #end sabote


    
def asm_condition(cond):
    """converts the string cond into its encoding in the snif machine instruction. """
    comparisons = ["eq", "neq", "sgt", "slt", "gt", "ge", "lt", "le"]
    try:
        val = comparisons.index(cond)
    except:
        error("invalid condition: " + cond)
    return val<<8


    
def asm_three_op_instr(codeop, arguments, signed_constant): #Les operations type add, sub, etc.
    code = codeop<<12
    #extract registers
    #begin sabote
    dest = asm_dest_reg(arguments[0], 7)
    op1 = asm_operand1(arguments[1])
    op2 = asm_operand2(arguments[2], signed_constant)
    code = code + dest + op1 + op2
    #end sabote
    return code


def asm_snif(arguments):
    code = 3<<12 # codeop
    #begin sabote
    op1 = asm_operand1(arguments[0])
    cond = asm_condition(arguments[1])
    op2 = asm_operand2(arguments[2], 4)
    code=code+op1+cond+op2
    #end sabote
    return code

    
def asm_wmem(arguments):
    "TODO"
    #begin sabote
    code = 0
    op1 = asm_operand1(arguments[0])
    reg = asm_memory_access(arguments[1])
    code = op1+reg
    return code
    #end sabote

def asm_rmem(arguments):
    "TODO"
    #begin sabote
    code = 15<<12
    dest = asm_dest_reg(arguments[0], 15)
    reg = asm_memory_access(arguments[1])
    code = code + dest + reg
    return code
    #end sabote

def asm_jump(arguments, current_address):
    "TODO"
    #begin sabote
    code = 11<<12
    val=int(arguments[0])
    if val<0 or val>4095:
        error("invalid call")
    else:
        return code+val
    #end sabote



def asm_call(arguments):
    "TODO"
    #begin sabote
    code = 10<<12
    val = int(arguments[0])
    if val<0 or val>4095:
        error("invalid call")
    else:
        return code+val
    #end sabote

def asm_let(opcode, arguments):
    "TODO"
    #begin sabote
    code=opcode<<12
    dest = asm_dest_reg(arguments[0],15)
    val=int(arguments[1])
    if val<(-128) or val>127:
        error("invalid constant in let")
    else:
        if val<0:
            const=256+val
        else:
            const=val
    code = code + dest + const
    return code
    #end sabote

def asm_copy(arguments):
    code= 14<<12
    dest=asm_dest_reg(arguments[0],15)
    op=asm_operand1(arguments[1])
    code=code+dest+op
    return code

def asm_pass(iteration, s_file):
    global line
    global labels
    code =[]
    print "\n PASS " + str(iteration)
    current_address = 0
    source = open(s_file)
    for source_line in source:
        print "processing " + source_line[0:-1] # just to get rid of the final newline
        tokens = re.findall('[\S]+', source_line) # \S means: any non-whitespace
        # print tokens # to debug
        # if there is a label, consume it
        if tokens:
            token=tokens[0]
            if token[-1] == ":": # last character
                label = token[0: -1] # all the characters except last one
                labels[label] = current_address
                tokens = tokens[1:]
            if token == ".align16":
                while (current_address & 15) !=0:
                    code.append(0);
                    current_address += 1;
                tokens = tokens[1:]
        # now we may have an instruction
        if tokens:
            machine_instr=-1
            operation = tokens[0]
            arguments = tokens[1:]
            if operation == ".word":
                machine_instr = int(arguments[0], 0)
            if operation == "rmem":
                machine_instr = asm_rmem(arguments)
            #begin sabote
            if operation == "wmem":
                machine_instr = asm_wmem(arguments)
            if operation == "add":
                machine_instr = asm_three_op_instr(1, arguments, True)
            if operation == "sub":
                machine_instr = asm_three_op_instr(2, arguments, True)
            if operation == "snif":
                machine_instr = asm_snif(arguments)
            if operation == "and":
                machine_instr = asm_three_op_instr(4, arguments, True)
            if operation == "or":
                machine_instr = asm_three_op_instr(5, arguments, True)
            if operation == "xor":
                machine_instr = asm_three_op_instr(6, arguments, True)
            if operation == "lsl":
                machine_instr = asm_three_op_instr(7, arguments, False)
            if operation == "lsr":
                machine_instr = asm_three_op_instr(8, arguments, False)
            if operation == "asr":
                machine_instr = asm_three_op_instr(9, arguments, False)
            if operation == "call":
                machine_instr = asm_call(arguments)
            if operation == "jump":
                machine_instr = asm_jump(arguments)
            if operation == "letl":
                machine_instr = asm_let(12, arguments)
            if operation == "leth":
                machine_instr = asm_let(13, arguments)
            if operation == "copy":
                machine_instr = asm_copy(arguments)
            #end sabote
                
            if machine_instr != -1 :
                code.append(machine_instr)
                print format(current_address, "04x") + " : " + format(machine_instr, "04x")
                current_address += 1
            elif tokens[0][0]!=";":
                error("don't know what to do with: " + tokens[0])
            
        line += 1
        
    source.close()
    return code

#/* main */
if __name__ == '__main__':

    argparser = argparse.ArgumentParser(description='This is the assembler for the ASR2016 processor @ ENS-Lyon')
    argparser.add_argument('filename', help='name of the source file.  "python asm.py toto.s" assembles toto.s into toto.obj')

    options=argparser.parse_args()
    filename = options.filename
    basefilename, extension = os.path.splitext(filename)
    obj_file = filename+".obj"
    asm_pass(1, filename) # first pass essentially builds the labels
    code = asm_pass(2, filename) # second pass is for good

    outfile = open(obj_file, "w")
    for instr in code:
        outfile.write(format(instr, "04x"))
        outfile.write("\n")

    outfile.close()
