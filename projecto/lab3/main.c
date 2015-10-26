#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "commandlinereader.h"
#include "list.h"


#define VECTOR_SIZE 6
#define ARG_LEN 256
#define MAXPAR 4
#define __DEBUG__ 1



sem_t num_processos;
sem_t num_filhos;
pthread_mutex_t children_mutex;
pthread_mutex_t lista_mutex;


// variaveis globais a serem partilhadas pelas threads
list_t *lista_processos;
int numChildren = 0;
int _exit_ctrl = 0;

void *tarefa_monitora(){
	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m Estamos na tarefa_monitora %d\n", (int) pthread_self() );
  }

	int status;

	while(1){
		pthread_mutex_lock(&children_mutex);
		if(numChildren > 0) {
			pthread_mutex_unlock(&children_mutex);
			// aguarda pela terminacao dos processos filhos
			pid_t ret = wait(&status);

			// regista o pid do processo acabado de terminar e o respectivo return status
			if(__DEBUG__)
				printf("\e[36m[ DEBUG ]\e[0m Process %d has just finished\n", (int) ret );

			if(WIFEXITED(status)){
				//atulizacao do tempo de fim do processo
				pthread_mutex_lock(&lista_mutex);
				update_terminated_process(lista_processos, ret, time(NULL), WEXITSTATUS(status));
				pthread_mutex_unlock(&lista_mutex);
			}else{
				//Eliminacao da lista de um processo no qual ocorreu um erro (ex. seg fault)
				pthread_mutex_lock(&lista_mutex);
				delete_process(lista_processos, ret);
				pthread_mutex_unlock(&lista_mutex);
				printf("\e[31m[ ERROR ]\e[0m Process %d terminated Abruptly\n", ret );
			}

			/*Assinalar que existe menos um filho em execucao*/
			sem_post(&num_processos);

			pthread_mutex_lock(&children_mutex);
			numChildren--;
			pthread_mutex_unlock(&children_mutex);
		}else{
			if(_exit_ctrl){
				pthread_mutex_unlock(&children_mutex);
				// terminar thread
				pthread_exit(0);
			}
			pthread_mutex_unlock(&children_mutex);
			sleep(1);
		}
	}
}

int main(int argc, char *argv[]){
	char **argVector;
	// o argVector ira guardar o input do utilizador na par-shell. O seu tamanho coincide
	// com o numero maximo de argumentos permitidos mais um, que corresponde ao nome do
	// proprio comando
	argVector = (char **) malloc(VECTOR_SIZE * sizeof(char*));
	lista_processos = lst_new();

	// Inicializacao dos mutex
	if (pthread_mutex_init(&children_mutex, NULL) != 0){
        printf("\e[31m[ ERROR ]\e[0m children_mutex init failed\n");
        exit(EXIT_FAILURE);
  }
	if (pthread_mutex_init(&lista_mutex, NULL) != 0){
        printf("\e[31m[ ERROR ]\e[0m lista_mutex init failed\n");
        exit(EXIT_FAILURE);
  }
	/* Inicializacao dos semaforos*/
	if(sem_init(&num_processos, 0, MAXPAR) != 0){
        printf("\e[31m[ ERROR ]\e[0m semaphore num_processos init failed\n");
        exit(EXIT_FAILURE);
  }
	if(sem_init(&num_filhos, 0,0) != 0){
        printf("\e[31m[ ERROR ]\e[0m semaphore num_filhos init failed\n");
        exit(EXIT_FAILURE);
  }

	/*Criaçao da thread*/
	pthread_t tid;
	if(pthread_create (&tid, 0,tarefa_monitora, NULL) == 0)	{
		if(__DEBUG__){
			printf ("\e[36m[ DEBUG ]\e[0m Criada a tarefa %d\e[0m\n",(int) tid);
		}
	}
	else {
		printf("\e[31m[ ERROR ]\e[0m na criação da tarefa\n");
		exit(EXIT_FAILURE);
	}

	// loop infinito de execucao da par-shell
	while(!_exit_ctrl){
		// le os argumentos atraves da funcao fornecida
		readLineArguments(argVector, VECTOR_SIZE);

		// caso nao tenha sido introduzido um comando, a par-shell prossegue a sua execucao
		if(argVector[0] == NULL){
			continue;
		}

		if(strcmp(argVector[0], "exit") == 0){
			_exit_ctrl = 1;
		}else{

			/* Espera que nao estejam a correr demasiados filhos */
			sem_wait(&num_processos);

			// Criacao do processo filho
			int pid = fork();

			if(pid < 0){
				// erro ao criar o processo filho
				perror("\e[31m[ Error ]\e[0m forking process");

			}else if(pid > 0) {
				// PROCESSO PAI

				pthread_mutex_lock(&lista_mutex);
				//Introducao de processo na lista
				insert_new_process(lista_processos, pid, time(NULL));
				pthread_mutex_unlock(&lista_mutex);

				pthread_mutex_lock(&children_mutex);
				numChildren++;
				pthread_mutex_unlock(&children_mutex);

			}else{
				// PROCESSO FILHO
				// substitui a imagem do executavel actual pelo especificado no comando introduzido

				if(execv(argVector[0], argVector)){
					if(__DEBUG__){
						printf("\e[36m[ DEBUG ]\e[0m O comando nao existe na directoria actual.\e[0m\n");
					}
				}
				// o processo continua se nao tiver sido possivel fazer a substituicao do executavel na directoria actual
				if(execvp(argVector[0], argVector)){
					if(__DEBUG__){
						printf("\e[36m[ DEBUG ]\e[0m O comando nao existe em lado nenhum.\e[0m\n");
					}

					// caso nao tenha sido possivel fazer a substituicao do executavel do processo,
					// escreve no stderr a razao e termina com status de falha
					fprintf(stderr, "o comando %s nao existe\n",argVector[0]);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	// quando sai, a thread principal sincroniza-se com a monitora
	printf("\n\e[33m[ INFO ]\e[0m Joining monitoring thread...\n\n");
	pthread_join(tid, NULL);

	pthread_mutex_destroy(&children_mutex);
	pthread_mutex_destroy(&lista_mutex);

	// liberta a memoria alocada
	lst_print(lista_processos);
	lst_destroy(lista_processos);
	free(argVector);

	// da a mensagem de fim do programa
	printf("\e[33m[ INFO ]\e[0m Par-shell terminated\n");
	printf("\e[33m[ INFO ]\e[0m exiting..\n");

	exit(EXIT_SUCCESS);
}
