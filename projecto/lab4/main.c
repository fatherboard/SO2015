// Say your prayers, little one
// Don't forget, my son to
//#include <everyone.h>
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
#define __DEBUG__ 0



/* variaveis de sincronizaçao */
sem_t lim_processos;
sem_t filhos_em_execucao;
pthread_mutex_t children_mutex;
pthread_mutex_t lista_mutex;

// variaveis globais a serem partilhadas pelas threads
list_t *lista_processos;
int numChildren = 0;
int _exit_ctrl = 0;

static FILE *log;
int iteration_number = 0, total_exec_time = 0;

void *tarefa_monitora(){
	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m Estamos na tarefa_monitora %d\n", (int) pthread_self() );
	}
	int status, dif;

	while(1){
		/* Esperar que existam filhos em execucao */
		sem_wait(&filhos_em_execucao);
	      
		pthread_mutex_lock(&children_mutex);
		if(numChildren > 0) {
			pthread_mutex_unlock(&children_mutex);

			// aguarda pela terminacao dos processos filhos
			pid_t ret = wait(&status);
			/*Assinalar que existe menos um filho em execucao*/
			sem_post(&lim_processos);

			if(__DEBUG__)
				printf("\e[36m[ DEBUG ]\e[0m Process %d finished\n", (int) ret );

			// regista o pid do processo acabado de terminar e o respectivo return status na lista
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
			
			dif = get_dif_time_by_pid(lista_processos, ret);
			total_exec_time += dif;
			fprintf(log, "iteracao %d\n", iteration_number);
			fprintf(log, "pid: %d execution time: %d\n", ret, dif);
			fprintf(log, "total execution time: %d s\n", total_exec_time);
			fflush(log);
			iteration_number++;

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
	
	char str_dummy[50], line[1024];
	int int_dummy;
	
	log = fopen("log.txt","a+");

	if(log == NULL){
	  printf("\e[31m[ ERROR ]\e[0m could not open log.txt\n");
	  exit(EXIT_FAILURE);
	}

	while(fgets(line, 1024, log) != NULL){
	  if(sscanf(line, "%s %d", str_dummy, &iteration_number) == 2){
	    iteration_number++;
	  }
	  fgets(line, 1024, log);
	  if(sscanf(line, "%s %d %s %s %d %s", str_dummy, &int_dummy, str_dummy, str_dummy, &int_dummy, str_dummy) == 6){
	    // nothing
	  }
	  fgets(line, 1024, log);
	  if(sscanf(line, "%s %s %s %d ", str_dummy, str_dummy, str_dummy, &total_exec_time) == 4){
	   // exec_time 
	  }
	}

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
	if(sem_init(&lim_processos, 0, MAXPAR) != 0){
        printf("\e[31m[ ERROR ]\e[0m semaphore lim_processos init failed\n");
        exit(EXIT_FAILURE);
  }
	if(sem_init(&filhos_em_execucao, 0,0) != 0){
        printf("\e[31m[ ERROR ]\e[0m semaphore filhos_em_execucao init failed\n");
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
		printf("\e[31m[ ERROR ]\e[0m Creating Thread\n");
		exit(EXIT_FAILURE);
	}


	printf("\e[33m[ INFO ]\e[0m Limite de processos filhos: %d\n", MAXPAR);
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
			sem_post(&filhos_em_execucao);
		}else{

			/* Esperar para que a quota de numero de processos filhos nao seja ultrapassada */
			sem_wait(&lim_processos);

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

				/* Assinalar que existe menos um filho em execuçao */
				sem_post(&filhos_em_execucao);
			}else{
				// PROCESSO FILHO
				// substitui a imagem do executavel actual pelo especificado no comando introduzido
				if(__DEBUG__)
					printf("\e[36m[ DEBUG ]\e[0m Process %d has just started.\n\e[36m[ DEBUG ]\e[0m Executing: %s\n", getpid(), argVector[0] );

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
	// quando termina, a thread principal sincroniza-se com a monitora
	printf("\n\e[33m[ INFO ]\e[0m Joining monitoring thread...\n\n");
	pthread_join(tid, NULL);

	// liberta a memoria alocada
	pthread_mutex_destroy(&children_mutex);
	pthread_mutex_destroy(&lista_mutex);
	lst_print(lista_processos);
	lst_destroy(lista_processos);
	free(argVector);
	sem_destroy(&filhos_em_execucao);
	sem_destroy(&lim_processos);
	fclose(log);

	// da a mensagem de fim do programa
	printf("\e[33m[ INFO ]\e[0m Par-shell terminated\n");
	printf("\e[33m[ INFO ]\e[0m exiting..\n");

	exit(EXIT_SUCCESS);
}
