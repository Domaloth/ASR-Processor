/*
  simu.cpp du cours ASR1 2016
	Copyright (c) Florent de Dinechin / ENS-Lyon, 2016
	This file is distributed under the terms of the GPL V3 license
*/
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>

#include "display.hpp"
#include "processor.hpp"

using namespace std;

#define REFRESHRATE 32 // for the -f mode; should be a power of two

#define RAMSIZE   0x10000 // in words
// also a few display-related constants defined in display.h


word memory[RAMSIZE];


bool debug_output;
bool frame_skip;
bool step_by_step;
uint64_t cycle;

void read_obj_file(string filename) {
	cerr << "loading" ;
  word adr = 0x0000;
	ifstream file;
	file.open(filename.c_str());
  while (!file.eof()) {
    // read a line
    char buf[256];
    file.getline(buf, 256);
    string line(buf);
    istringstream sstr(line);
    sstr >> hex >> memory[adr];
		adr++;
		cerr << ".";
  }
	cerr << "done" << endl;
	file.close();
}

void usage(char* argv[]) {
		cerr << "Usage: " << argv[0] << " s file    for step by step simulation" <<endl;
		cerr << "       " << argv[0] << " r file    for running simulation with debug output" <<endl;
		cerr << "       " << argv[0] << " f file    for fast simulation (no debug output, display refresh every " << REFRESHRATE <<" cycles)" <<endl;
		exit(0);
}

int main(int argc, char* argv[])
{

	if (argc!=3){
		usage(argv);
	}
	string mode = argv[1];
	string filename = argv[2];

	if(mode=="s") {
		step_by_step = true;
		debug_output=true;
		frame_skip = false;
	}
	else if(mode=="r") {
		step_by_step = false;
		debug_output=true;
		frame_skip = false;
	}
	else if(mode=="f") {
		step_by_step = false;
		debug_output=false;
		frame_skip = true;
	}
	else
		usage(argv);

	// From here on we assume that arg parsing was successful
	
	cerr << "ASR1 2016 simulator. Start of videomem is at 0x" << hex << STARTVIDEOMEM<<endl;
	// initialize the memory
	for (int i=0; i<RAMSIZE; i++)
		memory[i]=17*17*i; // to have a nice screen; +1 to check RW

	// initialize the graphics display
	display* window = new display(memory);

	// read the program and place it at address 0
	read_obj_file(filename);
	
	// initialize the processor
	processor* proc = new processor(memory);
		proc->verbose = debug_output;

	cycle=0;
	
	// von Neumann cycle
	while (1+1==2) {
		proc->step();
		cycle ++;

		// The following is related to the user interface
		if(proc->display_needs_refresh && (!frame_skip || (cycle & (REFRESHRATE-1)) == 0)) {
			window->refresh();
			proc->display_needs_refresh=false;
		}
		if(step_by_step)
			getchar();
	}
}


