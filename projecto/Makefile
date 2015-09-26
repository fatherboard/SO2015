par-shell: main.c commandlinereader.c
	gcc main.c commandlinereader.c -g -Wall -o par-shell

valgrind: par-shell
	valgrind --leak-check=full ./par-shell
	
gdb: par-shell
	gdb par-shell
	
fib: fibonacci.c commandlinereader.c
	gcc fibonacci.c commandlinereader.c -g -Wall -o fib

clean:
	rm -f *.o *.c~ *.h~ Makefile~ par-shell fib
