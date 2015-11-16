all: fib par-shell fail sleeper seg_fault teste

par-shell: list.c main.c commandlinereader.c
	gcc main.c list.c commandlinereader.c  -g -lpthread -Wall -o par-shell

fib: fibonacci.c commandlinereader.c
	gcc fibonacci.c commandlinereader.c -g -Wall -o fib

fail: fail.c
	gcc fail.c -g -Wall -o fail

sleeper: sleeper.c
	gcc sleeper.c -g -Wall -o sleeper
teste: teste.c
	gcc teste.c -g -Wall -o teste
seg_fault: seg_fault.c
	gcc seg_fault.c -w -g -Wall -o seg_fault

valgrind: par-shell
	valgrind --leak-check=full ./par-shell

gdb: par-shell
	gdb par-shell

clean:
	rm -f *.o *.c~ *.h~ Makefile~ par-shell fib teste seg_fault fail time_test sleeper log.txt 

