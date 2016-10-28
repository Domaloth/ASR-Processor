
all: simu

sabote:  
	rm -Rf src_etudiants
	mkdir src_etudiants
	mkdir src_etudiants/ASM
	cp Makefile processor.hpp simu.cpp display.hpp display.cpp src_etudiants/
	python sabote.py

processor.o: processor.cpp processor.hpp
	g++ -c -O2 -Wall -Wextra -pedantic $< -o $@

display.o: display.cpp display.hpp
	g++ -c -O2 -Wall -Wextra -pedantic display.cpp -L/usr/X11R6/lib

simu.o: simu.cpp display.hpp processor.hpp 
	g++ -c -O2 -Wall -Wextra -pedantic $< -o $@

simu: processor.o display.o simu.o
	g++ -O2 -Wall -Wextra -pedantic processor.o display.o simu.o  -L/usr/X11R6/lib -lX11 -o $@

clean:
	rm -f *~ \#*\# *.o simu asm 
