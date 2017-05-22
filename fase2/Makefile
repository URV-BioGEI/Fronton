all : memoria.o semafor.o missatge.o winsuport2.o pilota3 fronton3
winsuport2.o : winsuport2.c winsuport2.h
	gcc -Wall -c winsuport2.c -o winsuport2.o

missatge.o : missatge.c missatge.h
	gcc -c -Wall missatge.c -o missatge.o

semafor.o : semafor.c semafor.h
	gcc -c -Wall semafor.c -o semafor.o

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o

pilota3 : pilota3.c winsuport2.o memoria.o
	gcc -Wall pilota3.c winsuport2.o memoria.o semafor.o -o pilota3 -lcurses -lpthread

fronton3 : fronton3.c winsuport2.o memoria.o missatge.o semafor.o 
	gcc -Wall fronton3.c winsuport2.o missatge.o memoria.o semafor.o -o fronton3 -lcurses -lpthread

pilota4 : pilota4.c winsuport2.o memoria.o
	gcc -Wall pilota4.c winsuport2.o memoria.o semafor.o -o pilota4 -lcurses -lpthread

fronton4 : fronton4.c winsuport2.o memoria.o missatge.o semafor.o 
	gcc -Wall fronton4.c winsuport2.o missatge.o memoria.o semafor.o -o fronton4 -lcurses -lpthread

clean: 
	rm memoria.o semafor.o missatge.o fronton3 pilota3 winsuport2.o 
