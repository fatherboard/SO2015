#include "pipes.h"

int create_fifo_read(char *name){
	
	int fifo_fd;
	
	if(mkfifo(name, S_IRUSR | S_IWUSR) != 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not create FIFO");
	    exit(EXIT_FAILURE);
	}
	
	fifo_fd = open(name, O_RDONLY);
	if(fifo_fd < 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not open FIFO");
	    exit(EXIT_FAILURE);
	}
	return fifo_fd;
}

int create_fifo_write(char *name){
	
	int fifo_fd;
	
	if(mkfifo(name, S_IRUSR | S_IWUSR) != 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not create FIFO");
	    exit(EXIT_FAILURE);
	}
	
	fifo_fd = open(name, O_WRONLY);
	if(fifo_fd < 0){
	    perror("\e[31m[ ERROR ]\e[0m Could not open FIFO");
	    exit(EXIT_FAILURE);
	}
	return fifo_fd;
}

int open_pipe_write(char *pipe_name){

	int shell_fifo = open(pipe_name, O_WRONLY);

	if(shell_fifo == -1){
		perror("Error opening pipe\n");
		exit(EXIT_FAILURE);
	}

	return shell_fifo;

}

int open_pipe_read(char *pipe_name){

	int shell_fifo = open(pipe_name, O_RDONLY);

	if(shell_fifo == -1){
		perror("Error opening pipe\n");
		exit(EXIT_FAILURE);
	}

	return shell_fifo;

}