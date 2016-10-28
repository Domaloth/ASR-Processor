/*
  display.cpp du cours ASR1 2016
	Copyright (c) Florent de Dinechin / ENS-Lyon, 2016
	This file is distributed under the terms of the GPL V3 license
*/
#include "display.hpp"
#include <iostream>
#include <sstream>
#include <stdio.h>

using namespace std;

// This class manages the display in an X window of the bitmap part of the memory
// TODO: add sizeof(word) wherever relevant to make it robust to a change of the word size

// the constructor
display::display(word* memory_) :
	memory(memory_)
{
	
  dpy = XOpenDisplay(NULL);
	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 
																 0, 0, PIXEL_DOUBLING*WIDTH, PIXEL_DOUBLING*HEIGHT,
																 0, BlackPixel(dpy, DefaultScreen(dpy)), 
																 BlackPixel(dpy, DefaultScreen(dpy)));
  XMapWindow(dpy, w);
  XStoreName(dpy, w, "ASR1 2016 processor simulator");
	// XSelectInput(dpy, w, ExposureMask | KeyPressMask);
	
	gc = XCreateGC(dpy, w, 0, NULL);
	// Create an image of depth 24 because at least we know where are the RGB
	// And create it twice as large in both dimensions because screens are huge these days
	img = XCreateImage(dpy, DefaultVisual(dpy, DefaultScreen(dpy)), 
										 24, ZPixmap, 0,
										 (char*)image, PIXEL_DOUBLING*WIDTH, PIXEL_DOUBLING*HEIGHT, 32, 0);
	
  refresh();
	// cout << dpy << " " << w << " " << gc << " " << img << "\n";
}



void display::refresh()
{
	for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
			word address = STARTVIDEOMEM + (y*WIDTH + x);
      word pixel = memory[address]; 
			// convert this 8-bit pixel to a 24-bit one
			//uint32_t rgb_pixel = ((c&0xe0)<<16) + ((c&0x18)<<11) + ((c&0x7)<<5);

			uint32_t blue = pixel & ((1<<5)-1); // 5 bits blue
			uint32_t green = (pixel>>5) & ((1<<5)-1); // 5 bits green
			uint32_t red = (pixel>>10) ; // 6 bits red because I am a communist
			// Now shift each color to its place in the 24-bit word 
			uint32_t rgb_pixel = (red << (2+16)) + (green << (3+8)) + (blue << 3);
#if PIXEL_DOUBLING==2
			// then copy this pixel 4 times in the picture. These days there is hardware for that.
      image[y*4*WIDTH + 2*x] =       rgb_pixel;      
      image[y*4*WIDTH + 2*x+1] =     rgb_pixel;
			image[(y+1)*4*WIDTH + 2*x] =	 rgb_pixel;
			image[(y+1)*4*WIDTH + 2*x+1] = rgb_pixel;
#else
      image[y*WIDTH+x] =       rgb_pixel;      
#endif
    }
  }
	XPutImage(dpy, w, gc, img, 0, 0, 0, 0, PIXEL_DOUBLING*WIDTH, PIXEL_DOUBLING*HEIGHT);
	//	XFlush(dpy);

}


void display::plot (int x, int y, char c){
	memory[x + WIDTH*y + STARTVIDEOMEM] = c;
}



