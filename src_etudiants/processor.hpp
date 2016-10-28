#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <stdint.h>
typedef uint16_t word;
// this defines "word" as a type for a 16-bit word with unsigned arithmetic.
// if you need signed arithmetic, use int16_t, and if needed extend to uint32_t and int32_t

class processor {
public:
	/** The processor constructor receives a pointer to its memory, allocated in simu.cpp  */
	processor(word* memory);

	/** One step of the von Neumann cycle.
	 This is the method you have to edit */ 
	void step();

	/** cal this method when the processor encounters an illegal instruction */ 
	void illegal_instruction(word address, word instruction);


	/*  The following 3 attributes hold the state of the von Neumann machine */
	word pc;      /** The program counter */
	word r[16];   /** The 16 registers, accessed as r[0] to r[15] */
	word* memory; /** The simulator accesses memory location at address a by memory[a] */

	/* You may ignore the following. But feel free to understand it */ 
	bool verbose;
	bool display_needs_refresh;
};

#endif // PROCESSOR_HPP
