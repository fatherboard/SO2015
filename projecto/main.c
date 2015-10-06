#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commandlinereader.h"

#define VECTOR_SIZE 6
#define ARG_LEN 256
#define __DEBUG__ 0

int main(int argc, char *argv[]){

	char **argVector;
	int i, children = 0;
	int _exit = 0;
	// o argVector ira guardar o input do utilizador na par-shell. O seu tamanho coincide
	// com o numero maximo de argumentos permitidos mais um, que corresponde ao nome do
	// proprio comando
	argVector = (char **) malloc(VECTOR_SIZE * sizeof(char*));
	
	// loop infinito de execucao da par-shell
	while(!_exit){
		// le os argumentos atraves da funcao fornecida
		readLineArguments(argVector, VECTOR_SIZE);

		// caso nao tenha sido introduzido um comando, a par-shell prossegue a sua execucao
		if(argVector[0] == NULL){
			continue;
		}

		if(strcmp(argVector[0], "exit") == 0){
			// caso tenha sido introduzido o comando "exit", a par-shell prepara-se para terminar
			_exit = 1;
		}else{
			// caso tenha sido introduzido um comando que nao seja "exit", a par-shell prepara-se
			// para executar o binario indicado
			
			// faz fork para criar o processo filho
			int pid = fork();

			if(pid < 0){
				// erro ao criar o processo filho
				perror("Error forking process");

			}else if(pid > 0) {
				// PROCESSO PAI
				// neste exercicio o pai nao monitoriza os filhos durante a execucao
				// apenas quando termina
				children++;

			}else{
				// PROCESSO FILHO
				// substitui a imagem do executavel actual pelo especificado no comando introduzido
				
				// comeca por procurar o executavel na directoria de trabalho actual
				if(execv(argVector[0], argVector)){
					if(__DEBUG__){
						printf("o comando nao existe na directoria actual.\n");
					}
				}
				// o processo continua se nao tiver sido possivel fazer a substituicao do executavel
				// verifica se a imagem se encontra numa directoria indicada pela PATH
				if(execvp(argVector[0], argVector)){
					if(__DEBUG__){
						printf("o comando nao existe em lado nenhum.\n");
					}

					// caso nao tenha sido possivel fazer a substituicao do executavel do processo,
					// escreve no stderr a razao e termina com status de falha
					fprintf(stderr, "o comando %s nao existe\n",argVector[0]);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	// quando sai, verifica se todos os filhos que criou ja terminaram e espera pelos que ainda estao a correr
	printf("Waiting for child processes to finish...\n");
	
	// aloca memoria necessaria para a monitorizacao da terminacao dos processos filho
	int *outpid = (int *) malloc(sizeof(int) * children);
	int *outstatus = (int *) malloc(sizeof(int) * children);
	int status;
	
	for(i = 0; i < children; i++){
		if(__DEBUG__){
			printf("\t%d processes remaining\n", children - i);
		}
		// aguarda pela terminacao dos processos filhos
		pid_t ret = wait(&status);
		// regista o pid do processo acabado de terminar e o respectivo return status
		outpid[i] = ret;
		outstatus[i] = status;
	}
	
	int currentStatus;
	for(i = 0; i < children; i++){
		// apresenta no ecra o exit status reportado por cada processo filho e o respectivo pid
		currentStatus = outstatus[i];
		if(WIFEXITED(currentStatus)){
			printf("Process %d terminated with status %d\n", outpid[i], WEXITSTATUS(currentStatus));
		}
	}
	
	printf("All child processes finished\n");
	
	// liberta a memoria alocada
	free(outpid);
	free(outstatus);
	free(argVector);

	// da a mensagem de fim do programa
	printf("par-shell terminated\n");

	// termina com o estado de execucao bem sucedida
	exit(EXIT_SUCCESS);
}
