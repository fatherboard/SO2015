#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	int x = atoi(argv[1]);
	printf("Preparing to sleep %d seconds\n", x);
	sleep(x);
	printf("Sleeped %d seconds\n", x);
	exit(0);
}
