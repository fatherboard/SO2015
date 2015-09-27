#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	if(argc > 1)
		printf("TESTE: %s\n",argv[1]);
	else
		printf("TESTE: <sem args>\n");
	
	exit(EXIT_SUCCESS);

}
