#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){

        if(argc > 1){

		int i;
		int time = 0;
		printf("Times: \n");
		for(i = 0; i < argc; i++){
			time = time + atoi(argv[i]);
		}
		printf("Will sleep %d seconds\n",time);
		sleep(time);
		printf("Sleept %d seconds\n",time);
	}
	else
		printf("No time given: <sem args>\n");
  printf("Process %d finished sleeping.\n", (int) getpid());
  exit(EXIT_SUCCESS);
}
