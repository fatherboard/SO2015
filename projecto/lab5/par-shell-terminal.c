#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "commandlinereader.h"
#include "list.h"
#include "pipes.h"

#define EXIT_COMMAND "exit\n"
#define GLOBAL_EXIT  "exit-global"
#define NEW_TERMINAL_COMMAND  "NEW_TERMINAL"
#define CLOSE_TERMINAL_COMMAND "CLOSE_TERMINAL"
#define VECTOR_SIZE 6
#define ARG_LEN 256
#define MAXPAR 4
#define __DEBUG__ 1

int _exit_ctrl = 0, shell_fifo, my_fifo;
char fifo_name[512];

void ctrlCHandler(int derp){

	printf("\e[33m[ INFO ]\e[0m I received SIGINT (by another process or by ctrl+c)\n");
	
	close(shell_fifo);
	close(my_fifo);
	
	sprintf(fifo_name, "rm -rf %s", fifo_name);
	system(fifo_name);

	exit(0);
}

int main(int argc, char *argv[]){

	char output[1024], input[1024], aux[1024];

	signal(SIGINT, ctrlCHandler);
	
	if(argc < 2){
		perror("\e[31m[ ERROR ]\e[0m Not enough arguments\n");
		exit(EXIT_FAILURE);
	}

	sprintf(fifo_name, "par-shell-terminal-in-%d", getpid());
	shell_fifo = open_pipe_write(argv[1]);
	my_fifo = create_fifo_read(fifo_name);

	sprintf(output,	"%s %d\n",NEW_TERMINAL_COMMAND, getpid());

	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m New msg sent: \'%s\'", output );
	}
	write(shell_fifo, output, strlen(output));

	while(!_exit_ctrl) {
		fgets(input, 1024, stdin);

		// caso o utilizador tenha introduzido o comando stats
		if(strcmp(input, "stats\n") == 0){
			sprintf(aux,	"%s %d\n", "stats ", getpid());
			write(shell_fifo, aux, strlen(aux));
			if(__DEBUG__){
				printf("\e[36m[ DEBUG ]\e[0m msg sent: \'%s\'", aux );
			}

		}else if(strcmp(input, EXIT_COMMAND) == 0){
			sprintf(aux,	"%s %d\n", CLOSE_TERMINAL_COMMAND, getpid());
			_exit_ctrl = 1;
			write(shell_fifo, aux, strlen(aux));
			if(__DEBUG__){
				printf("\e[36m[ DEBUG ]\e[0m msg sent: \'%s\'", aux );
			}
		}else{
			write(shell_fifo, input, strlen(input));
			if(__DEBUG__){
				printf("\e[36m[ DEBUG ]\e[0m msg sent: \'%s\'", input );
			}
		}

	}
	printf("\e[33m[ INFO ]\e[0m Exiting \n");
	
	exit(EXIT_SUCCESS);
}
