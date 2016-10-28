/*
  processor.cpp du cours ASR1 2016
	Copyright (c) Florent de Dinechin / ENS-Lyon, 2016
	This file is distributed under the terms of the GPL V3 license
*/
#include <iostream>
#include <iomanip>
#include "processor.hpp"
#include "display.hpp"
using namespace std;


processor::processor(word* memory_) {
	// The memory is allocated outside of the processor class,
	// so that it can be pre-filled.
	memory = memory_;
	// Otherwise all the registers must be initialized to 0.
	pc=0;
	for(int i=0; i<16; i++)
		r[i]=0;
}



void processor::illegal_instruction(word address, word instruction) {
	cout << "Illegal instruction: " << hex << instruction
			 << " at address " << hex << address << endl;
}



void processor::step() {
	word instruction = memory[pc];
	int current_pc = pc;
	pc = pc+1; // Remember in the following that we already increased the PC
	
	// code op:
	word op = instruction >> 12;
	// number of destination register, when it is on 3 bits:
	word d3 = (instruction >> 8) & 0x7; 
	// number of destination register, when it is on 4 bits:
	word d4 = (instruction >> 8) & 0xf;
	word bit11 = (instruction >> 11) & 1;
	// number of first operand register
	word i = (instruction >> 4) & 0xf;
	// number of second operand: register or constant 
	word j = (instruction >> 0) & 0xf;
	// sign-extended constant
	word j_signed = j;

        // Better way may exist
        if((j & 0x8) != 0) 
          j_signed = (j | 0xfff0);  // adding 1 in bits 4 - 15
        
        // operand 2 is sometimes a register, sometimes an immediate constant
	// which is sometimes sign-extended
	// An instruction will use either op2 or op2s
	word op2, op2s; 
	if(bit11==0)	{
          op2 = r[j];
	}	
	else{
          op2 = j;
          op2s = j_signed;
	}
	word imm8;
#if 0
	if(verbose)
		cout << "pc=" << current_pc << "  instr: " << hex << instruction
				 << "   code_op=" << op
				 << " d3=" << d3
				 << " d4=" << d4
				 << " bit11=" << bit11
				 << " i=" << i
				 << " j=" << j
				 << " j_signed=" << j_signed << " (" << dec << (int16_t) j_signed << ")" << endl;
#endif
	switch(op) {
          
	case 0: // wmem
          if(d4!=0) {
            illegal_instruction(current_pc, instruction);
          }
          else {
            memory[ r[j] ]  = r[i];
            if ((r[j] >= STARTVIDEOMEM)) //   && (r[j] <= ENDVIDEOMEM) ) always true
              display_needs_refresh = true; // it is reset by refresh()
          }
          break;
          
	case 1: // add
          r[d3] = r[i] + op2;
          break;

        case 2: // sub
          r[d3] = r[i] - op2;
          break;
          
        case 3: // snif
          {
            bool skip_instr = false;
          
            switch(d3) {
            case 0: // eq
              skip_instr = r[i] == op2;
              break;
              
            case 1: // neq
              skip_instr = r[i] != op2;
              break;
              
            case 2: // sgt
              skip_instr = r[i] > op2s;
              break;
              
            case 3: // slt
              skip_instr = r[i] < op2s;
              break;
              
            case 4: // gt
              skip_instr = r[i] > op2;
            break;
            
            case 5: // ge
              skip_instr = r[i] >= op2;
              break;
              
            case 6: // lt
              skip_instr = r[i] < op2;
              break;
              
            case 7: // le
              skip_instr = r[i] <= op2;
              break;
              
            default:
              illegal_instruction(current_pc, instruction);
              break;
            }

            if(skip_instr)
              pc = pc + 1;
          }
          break;
          
        case 4: // and
          r[d3] = r[i] & op2;
          break;
          
        case 5: // or
          r[d3] = r[i] | op2;
          break;
        case 6: // xor
          r[d3] = r[i] ^ op2;
          break;
        case 7: // lsl
          r[d3] = r[i] << op2;
          break;
        case 8: // lsr
          r[d3] = r[i] >> op2;
          break;
        case 9: // asr
          // & Ox7fff to delete sign bit. >> op2 does the shift.
          // and then, put the sign back with the end
          r[d3] = (( r[i] & 0x7fff ) >> op2) | (r[i] & 0x8000);
          break;
        case 10: // call
          r[15] = current_pc +1;
          pc = instruction << 4;
          break;
        case 11: // jump
          if((instruction & 0x0fff) == 1) {
            pc = r[15];
          }
          else {
            int16_t c = instruction & 0x0fff;
            if((instruction & 0x0800) != 0) // If negative jump
              c = (int16_t)(instruction | 0xf000); // extends c          
            pc = current_pc + c; // int16_t conversion allows negative jump
          }
          break;
        case 12: // letl
          r[d4] = (instruction & 0x00ff);
          break;
        case 13: // leth
          r[d4] = ((r[d4] & 0x00ff) | (instruction & 0xff00));
          break;
        case 14: // EMPTY
          illegal_instruction(current_pc, instruction);
          break;
        case 15: // rmem
          r[d4] = memory[ r[j] ];
          break;
          
	default:
          illegal_instruction(current_pc, instruction);
          break;
          
	}
        
	
	if(verbose) {
          cout << "after instr:" << hex << setw(4) << setfill('0') << instruction
               << " at pc=" << hex << setw(4) << setfill('0') << current_pc
               << " newpc=" << hex << setw(4) << setfill('0') << pc;
          for (int i=0; i<16; i++)
            cout << " r"<< dec << i << "=" << hex << setw(4) << setfill('0') << r[i];
          cout << endl;
	}
	
}
