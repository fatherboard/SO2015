#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

int main(){
  
  FILE *log = fopen("log.txt","r");
  char linha_fich[1024], lixo1[50];
  int iter, ex_time, lixo_int;
  
  if(log == NULL){
      printf("Nao deu para abrir o ficheiro\n");
      exit(EXIT_FAILURE);
  }
  
  while(fgets(linha_fich, 1024, log) != NULL){
    if(sscanf(linha_fich, "%s %d", lixo1, &iter) == 2){
	printf("Iteracao: %d\n", iter);
    }
    fgets(linha_fich, 1024, log);
    if(sscanf(linha_fich, "%s %d %s %s %d %s", lixo1, &lixo_int, lixo1,lixo1,&lixo_int,lixo1) == 6){
	printf("Lixo read\n");
    }
    fgets(linha_fich, 1024, log);
    if(sscanf(linha_fich, "%s %s %s %d ", lixo1, lixo1, lixo1, &ex_time) == 4){
	printf("Tempo execucao: %d\n", ex_time);
    }
  }
  
  fclose(log);
  
  exit(EXIT_SUCCESS);
}