#include <stdio.h>
#include <stdlib.h>

	/**
	 * @description: function that prints the arguments in argVector
	 */
int main(int argc, char *argv[]){

	if(argc > 1){
		int i;
		printf("TESTE: ");
		for(i = 1; i < argc; i++){
			printf("%s ", argv[i]);
		}
		printf("\n");
	}
	else
		printf("TESTE: <sem args>\n");

	exit(EXIT_SUCCESS);
}
