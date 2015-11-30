#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int create_fifo_read(char *name);
int create_fifo_write(char *name);
int open_pipe_write(char *pipe_name);
int open_pipe_read(char *pipe_name);
