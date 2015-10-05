#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/**
 * @description: program that sleeps for the amount of time in argv 
 */
int main(int argc, char *argv[]){

	int sum_time = 0;
  if(argc > 1){

		int i;
		printf("Times: \n");
		for(i = 0; i < argc; i++)
			sum_time = sum_time + atoi(argv[i]);

		printf("Process %d Will sleep %d seconds\n",(int) getpid(),sum_time);
		sleep(sum_time);
	}
	else{
		printf("No time given: <sem args>\n");
    exit(10);
  }
  printf("Process %d finished sleeping: %d seconds.\n", (int) getpid(), sum_time);
  exit(EXIT_SUCCESS);
}
