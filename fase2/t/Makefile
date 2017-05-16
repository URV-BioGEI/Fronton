all : memoria.o winsuport2.o pilota3 fronton3
winsuport2.o : winsuport2.c winsuport2.h
	gcc -Wall -c winsuport2.c -o winsuport2.o

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o

pilota3 : pilota3.c winsuport2.o memoria.o
	gcc -Wall pilota3.c winsuport2.o memoria.o -o pilota3 -lcurses -lpthread

fronton3 : fronton3.c winsuport2.o memoria.o
	gcc -Wall fronton3.c winsuport2.o memoria.o -o fronton3 -lcurses -lpthread


clean: 
	rm memoria.o fronton3 pilota3 winsuport2.o 
