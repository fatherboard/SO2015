#include "pipes.h"
#include <unistd.h>

void deleteFifo(char * name){
  if(unlink(name) == -1){
    printf("\e[31m[ ERROR ]\e[0m could not remove pipe %s\n", name);
    //exit(EXIT_FAILURE);
  }
  else{
    printf("\e[33m[ INFO  ]\e[0m Removed pipe %s\n", name);
  }
}

void create_fifo_read(char *name){

	deleteFifo(name);

	if(mkfifo(name, S_IRUSR | S_IWUSR) != 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not create FIFO");
	    exit(EXIT_FAILURE);
	}

	//return open_pipe_read(name);
}

void create_fifo_write(char *name){

	deleteFifo(name);

	if(mkfifo(name, S_IRUSR | S_IWUSR) != 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not create FIFO");
	    exit(EXIT_FAILURE);
	}
	//return open_pipe_write(name);
}

int open_pipe_write(char *pipe_name){

	int shell_fifo = open(pipe_name, O_WRONLY);

	if(shell_fifo == -1){
		perror("\e[31m[ ERROR ]\e[0m opening pipe\n");
		exit(EXIT_FAILURE);
	}

	return shell_fifo;

}

int open_pipe_read(char *pipe_name){

	int shell_fifo = open(pipe_name, O_RDONLY);

	if(shell_fifo == -1){
		perror("\e[31m[ ERROR ]\e[0m opening pipe\n");
		exit(EXIT_FAILURE);
	}

	return shell_fifo;

}
