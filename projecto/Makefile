par-shell: main.c
	gcc main.c commandlinereader.c -g -o par-shell

valgrind: par-shell
	valgrind --leak-check=full ./par-shell
	
gdb: par-shell
	gdb par-shell

clean:
	rm -f *.o *.c~ *.h~ Makefile~ par-shell
