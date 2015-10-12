par-shell: main.c commandlinereader.c 
	gcc main.c commandlinereader.c -g -Wall -o par-shell

fib: fibonacci.c commandlinereader.c
	gcc fibonacci.c commandlinereader.c -g -Wall -o fib

teste: teste.c 
	gcc teste.c -g -Wall -o teste

fail: teste_failure.c
	gcc teste_failure.c -g -Wall -o fail

time_test: time_test.c
	gcc time_test.c -g -Wall -o time_test

seg_fault: seg_fault.c
	gcc seg_fault.c -w -g -Wall -o seg_fault
	
valgrind: par-shell
	valgrind --leak-check=full ./par-shell
	
gdb: par-shell
	gdb par-shell

clean:
	rm -f *.o *.c~ *.h~ Makefile~ par-shell fib teste seg_fault fail time_test
