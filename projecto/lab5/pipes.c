#include "pipes.h"
#include <unistd.h>

void deleteFifo(char * name){
  unlink(name);
//  system("rm -rf par-shell-in");
//  system("ls -l ");
  system("echo -e -n '\e[31m'");
  system("ls -l  | grep par-shell-in");
  system("echo -e -n '\e[0m'");
  printf("\e[35m[ FIXME ]\e[0m Remove SYSTEM CALLS\n");

  printf("\e[33m[ INFO ]\e[0m Removed par-shell-in\n");
}

int create_fifo_read(char *name){

	deleteFifo(name);

	if(mkfifo(name, S_IRUSR | S_IWUSR) != 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not create FIFO");
	    exit(EXIT_FAILURE);
	}

/*
	fifo_fd = open(name, O_RDONLY);
	if(fifo_fd < 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not open FIFO");
	    exit(EXIT_FAILURE);
	}*/
	return open_pipe_read(name);
}

int create_fifo_write(char *name){

	deleteFifo(name);

	if(mkfifo(name, S_IRUSR | S_IWUSR) != 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not create FIFO");
	    exit(EXIT_FAILURE);
	}
/*
	fifo_fd = open(name, O_WRONLY);
	if(fifo_fd < 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not open FIFO");
	    exit(EXIT_FAILURE);
	}*/
	return open_pipe_write(name);
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
