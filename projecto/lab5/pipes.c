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