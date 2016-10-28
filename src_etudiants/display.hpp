#ifndef __DISPLAY_HPP
#define __DISPLAY_HPP

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdint.h>

#include "processor.hpp" // defines the word size by typedefing word

#define WIDTH 160   //in pixels
#define HEIGHT 128
#define PIXEL_DOUBLING 2 // can be 1 or 2

// The video memory ranges from addresses (in word) STARTVIDEOMEM to ENDVIDEOMEM
#define STARTVIDEOMEM (0x10000 - WIDTH*HEIGHT) 
#define ENDVIDEOMEM (0xFFFF)

class display {	
public:
	/** constructor */
	display(word* memory);
	
	/** to be called to update the display when the screen memory is modified */
	void refresh();


	/** to be used for testing */
	
	void plot (int x, int y, char c);
private:
	word* memory; // the memory of the processor, one pixel per word
	Display* dpy;
	Window w;
	GC gc;
	XImage *img;
	uint32_t image[PIXEL_DOUBLING*WIDTH * PIXEL_DOUBLING*HEIGHT]; 
};
	
#endif
