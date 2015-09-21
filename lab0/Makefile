# Makefile de exemplo

main: list.o main.o
	gcc -o main list.o main.o
	./main

list.o: list.c list.h
	gcc -Wall -g -c list.c

main.o: main.c list.h
	gcc -Wall -g -c main.c

clean:
	rm -f *.o main
