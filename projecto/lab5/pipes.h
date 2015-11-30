#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void create_fifo_read(char *name);
void create_fifo_write(char *name);
int open_pipe_write(char *pipe_name);
int open_pipe_read(char *pipe_name);
void deleteFifo(char * name);
