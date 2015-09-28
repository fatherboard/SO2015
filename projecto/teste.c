#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	if(argc > 1){
		int i;
		printf("TESTE: ");
		for(i = 0; i < argc; i++){
			printf("%s ", argv[i]);
		}
		printf("\n");
	}
	else
		printf("TESTE: <sem args>\n");

	exit(EXIT_SUCCESS);

}
