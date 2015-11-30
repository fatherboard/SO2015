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

#define EXIT_COMMAND "exit\n"
#define GLOBAL_EXIT  "exit-global"
#define VECTOR_SIZE 6
#define ARG_LEN 256
#define MAXPAR 4
#define __DEBUG__ 0

int _exit_ctrl = 0;

// don't forget to close with close(fd)
int open_pipew(char *pipe_name){

	int pipe_fd = open(pipe_name, O_WRONLY);

	if(pipe_fd == -1){
			perror("Error opening pipe\n");
			exit(EXIT_FAILURE);
	}

	return pipe_fd;

}

int main(int argc, char *argv[]){

	char output[1024], input[1024], aux[1024];

	if(argc < 2){
			printf("Not enough arguments\n"),
			exit(EXIT_FAILURE);
	}

	int pipe_fd = open_pipew(argv[1]);

	sprintf(output,	"REG %d", getpid());
	write(pipe_fd, output, strlen(output));
	//close(pipe_fd);

	while(!_exit_ctrl) {
		fgets(input, 1024, stdin);

		// caso o utilizador tenha introduzido o comando stats
		if(strcmp(input, "stats") == 0){
			write(pipe_fd, input, strlen(input));
			//close(pipe_fd);
		}else if(strcmp(input, EXIT_COMMAND) == 0){
			sprintf(aux,	"%s %d", input, getpid());
			fprintf(stderr, "vou sair.. %s\n", aux );

			_exit_ctrl = 1;
			write(pipe_fd, aux, strlen(aux));
			//close(pipe_fd);
		}else{
			//pipe_fd = open_pipew(argv[1]);
			write(pipe_fd, input, strlen(input));
			//close(pipe_fd);
		}

	}
	fprintf(stderr, "vou sair..\n" );
	close(pipe_fd);
}
