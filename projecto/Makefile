fib: fibonacci.c commandlinereader.c
	gcc fibonacci.c commandlinereader.c -g -Wall -o fib

teste: teste.c
	gcc teste.c -g -Wall -o teste

par-shell: fib teste main.c commandlinereader.c
	gcc main.c commandlinereader.c -g -Wall -o par-shell

valgrind: par-shell
	valgrind --leak-check=full ./par-shell
	
gdb: par-shell
	gdb par-shell

clean:
	rm -f *.o *.c~ *.h~ Makefile~ par-shell fib teste
