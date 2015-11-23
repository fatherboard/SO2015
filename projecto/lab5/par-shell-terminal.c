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

#define EXIT_COMMAND "exit"
#define VECTOR_SIZE 6
#define ARG_LEN 256
#define MAXPAR 4
#define __DEBUG__ 0

int main(int argc, char *argv[]){
  
  if(argc < 2){
      printf("Not enough arguments\n"),
      exit(EXIT_FAILURE);
  }
  
  int pipe_fd = open(argv[1], O_APPEND);
  
  if(pipe_fd == -1){
      printf("Error opening pipe\n");
      exit(EXIT_FAILURE);
  }
  
  char output[1024];
  sprintf(output,  "new terminal: %d", getpid());
  
  write(pipe_fd, output, strlen(output));
  
  printf("Enviou\n");
  
  close(pipe_fd);
  
}