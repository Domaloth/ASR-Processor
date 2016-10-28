#!/usr/bin/env python
import os
import sys
import re
import string
import argparse

line = 0  # global variable to make error reporting easier
current_instr = ""  # idem
labels = {}  # global because shared between the two passe


def error(e):
    raise BaseException("Error at line " + str(line) + " : " + e)


def asm_dest_reg(dest, max):
    """converts the string dest into its encoding in the machine instruction.
    max should be 7 or 15, depending on the instruction"""
    try:
        val = int(dest[1:])  # this removes the "r". TODO catch exception here
        if dest[0] != 'r' or val < 0 or val > max:
            error("invalid destination register: " + dest)
        else:
            return val << 8
    except:
        error("empty destination")


def asm_operand1(op1):
    "converts the string op1 into its encoding in the machine instruction"
    operand = ["wmem", "add", "sub", "snif", "and",
               "or", "xor", "lsl", "lsr", "asr", "call",
               "jump", "letl", "leth", "RESERVED", "rmem"]
    if op1 == "return":
        op1 = "jump"
    elif op1 == "copy":
        op1 = "rmem"
    try:
        val = operand.index(op1)
    except:
        error("invalid mnemonic" + op1)
    return val << 12


def asm_operand2(op2, signed_constant):
    "converts the string op2 into its encoding in the machine instruction"
    val = asm_operand1(op2)
    # Write the constant in bits 0 to 3. And switch bit 11 to 1
    # to show that it's a constant
    return val + (1 << 11) + (signed_constant)


def asm_three_op_instr(codeop, arguments, signed_constant):
    code = codeop << 12
    #extract registers
    code += asm_dest_reg(arguments[0], 7)
    code += asm
    code
    return code


def asm_memory_access(op):
    """converts the string op that should contain [rnn] into its encoding
    in the machine instruction"""


def asm_condition(cond):
    """converts the string cond into its encoding in the snif machine
    instruction. """
    comparisons = ["eq", "neq", "sgt", "slt", "gt", "ge", "lt", "le"]
    try:
        val = comparisons.index(cond)
    except:
        error("invalid condition: " + cond)
    return val << 8


def asm_snif(arguments):
    code = 3 << 12  # codeop
    #begin sabote
    #end sabote
    return code


def asm_wmem(arguments):
    "TODO"
    #begin sabote
    #end sabote


def asm_rmem(arguments):
    "TODO"
    #begin sabote
    #end sabote


def asm_jump(arguments, current_address, iteration):
    "TODO"
    #begin sabote
    #end sabote


def asm_call(arguments, iteration):
    "TODO"
    #begin sabote
    #end sabote


def asm_let(opcode, arguments):
    "TODO"
    #begin sabote
    #end sabote


def asm_pass(iteration, s_file):
    global line
    global labels
    code = []
    print "\n PASS " + str(iteration)
    current_address = 0
    source = open(s_file)
    for source_line in source:
        # just to get rid of the final newline
        print "processing " + source_line[0:-1]
        # \S means: any non-whitespace
        tokens = re.findall('[\S]+', source_line)
        # print tokens # to debug
        # if there is a label, consume it
        if tokens:
            token = tokens[0]
            if token[-1] == ":":  # last character
                label = token[0:-1]  # all the characters except last one
                labels[label] = current_address
                tokens = tokens[1:]
            if token == ".align16":
                while (current_address & 15) != 0:
                    code.append(0)
                    current_address += 1
                tokens = tokens[1:]
        # now we may have an instruction
        if tokens:
            machine_instr = -1
            operation = tokens[0]
            arguments = tokens[1:]
            if operation == ".word":
                machine_instr = int(arguments[0], 0)
            if operation == "rmem":
                machine_instr = asm_rmem(arguments)

            #  begin sabote
            #  end sabote
            if machine_instr != -1:
                code.append(machine_instr)
                print format(current_address, "04x") + " : " + format(machine_instr, "04x")
                current_address += 1
            elif tokens[0][0] != ";":
                error("don't know what to do with: " + tokens[0])
        line += 1
    source.close()
    return code


#  /* main */
if __name__ == '__main__':

    argparser = argparse.ArgumentParser(description='This is the assembler for the ASR2016 processor @ ENS-Lyon')
    argparser.add_argument('filename', help='name of the source file.  "python asm.py toto.s" assembles toto.s into toto.obj')

    options = argparser.parse_args()
    filename = options.filename
    basefilename, extension = os.path.splitext(filename)
    obj_file = filename+".obj"
    asm_pass(1, filename)  # first pass essentially builds the labels
    code = asm_pass(2, filename)  # second pass is for good

    outfile = open(obj_file, "w")
    for instr in code:
        outfile.write(format(instr, "04x"))
        outfile.write("\n")

    outfile.close()
