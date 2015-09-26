#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commandlinereader.h"

#define VECTOR_SIZE 6
#define ARG_LEN 100

int main(int argc, char *argv[]){
	
	char **argVector;
	
	argVector = (char **) malloc(VECTOR_SIZE * sizeof(char*));
	
	while(1){
		/*fgets(input, ARG_LEN, stdin);
		sscanf(input, "%s", command);*/
		readLineArguments(argVector, VECTOR_SIZE);
		
		if(strcmp(argVector[0], "exit") == 0){
			break;
		}else{
			// qualquer comando que nao seja "exit" e considerado como
			// um comando para ser procurado na directoria de trabalho
			// e executado
			
			int status, pid = fork();
			
			if(pid != 0){
				// pai
				if(pid < 0){
					// erro ao criar o processo filho
					//write(stdout, "Error creating process\n", 23);
					perror("Error forking process\n");
				}
				wait(&status);
				
				// neste exercicio o pai nao monitoriza os filhos durante
				// a execucao, apenas quando termina
			}else{
				// filho
				// substitui a imagem do executavel actual
				// pelo especificado no comando
				if(execv(argVector[0], argVector)){
					perror("Error in execv\n");
				}
			}
			// espirito santo? novo banco?
		}
	}
	
	
	
	free(argVector);
	
	exit(EXIT_SUCCESS);
}
