#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commandlinereader.h"

int main(int argc, char *argv[]){
	
	char input[101], command[101];
	
	while(1){
		fgets(input, 100, stdin);
		sscanf(input, "%s", command);
		
		if(strcmp(command, "exit") == 0){
			break;
		}
	}
	
	return EXIT_SUCCESS;
}
