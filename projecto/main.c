#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commandlinereader.h"

#define VECTOR_SIZE 5
#define ARG_LEN 100

int main(int argc, char *argv[]){
	
	int i;
	char input[ARG_LEN], command[ARG_LEN];
	char **argVector;
	
	while(1){
		fgets(input, ARG_LEN, stdin);
		sscanf(input, "%s", command);
		
		if(strcmp(command, "pathname") == 0){
			// enviar os comandos
			argVector = (char **) malloc(VECTOR_SIZE * sizeof(char*));
			readLineArguments(argVector, VECTOR_SIZE);
		}else if(strcmp(command, "exit") == 0){
			free(argVector);
			break;
		}else{
			printf("Command not found.\n");
		}
	}
	
	exit(EXIT_SUCCESS);
}
