#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commandlinereader.h"

#define VECTOR_SIZE 6
#define ARG_LEN 256
#define __DEBUG__ 0

int main(int argc, char *argv[]){
	char **argVector;
	int i, children = 0;
	int _exit = 0;
	argVector = (char **) malloc(VECTOR_SIZE * sizeof(char*));
	while(!_exit){
		readLineArguments(argVector, VECTOR_SIZE);

		if(argVector[0] == NULL)
			continue;

		if(strcmp(argVector[0], "exit") == 0){
			_exit = 1;
		}else{
			// qualquer comando que nao seja "exit" e considerado como
			// um comando para ser procurado

			int pid = fork();

			if(pid < 0){
				// erro ao criar o processo filho
				perror("Error forking process");

			}else if(pid > 0) {
				// pai
				// neste exercicio o pai nao monitoriza os filhos durante a execucao
				// apenas quando termina
				children++;

			}else{
				// filho
				// substitui a imagem do executavel actual
				// pelo especificado no comando
				if(execv(argVector[0], argVector)){
					if(__DEBUG__)
						printf("o comando nao existe na directoria actual\n");
				}
				if(execvp(argVector[0], argVector)){
					if(__DEBUG__)
						printf("o comando nao existe em lado nenhum..\n");

					fprintf(stderr, "o comando %s nao existe\n",argVector[0]);
					exit(EXIT_FAILURE);
				}
			}
			// espirito santo? novo banco?
		}
	}
	// quando sai, verifica se todos os filhos que criou ja terminaram
	// e espera pelos que ainda estao a correr
	printf("Waiting for child processes to finish...\n");
	int *outpid = malloc(sizeof(int)*children);
	int *outstatus = malloc(sizeof(int)*children);
	int status;
	for(i = 0; i < children; i++){
		if(__DEBUG__)
			printf("\t%d processes remaining\n", children - i);
		pid_t ret = wait(&status);
		outpid[i] = ret;
		outstatus[i] = status;
	}
	int currentStatus;
	for(i = 0; i < children; i++){
		currentStatus = outstatus[i];
		if(WIFEXITED(currentStatus))
			printf("Process %d terminated with status %d\n", outpid[i],WEXITSTATUS(currentStatus));
	}
	printf("All child processes finished\n");
	free(outpid);
	free(outstatus);
	free(argVector);

	printf("par-shell terminated\n");

	exit(EXIT_SUCCESS);
}
