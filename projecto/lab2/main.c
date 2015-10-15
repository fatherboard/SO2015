#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "commandlinereader.h"
#include "list.h"


#define VECTOR_SIZE 6
#define ARG_LEN 256
#define __DEBUG__ 0
/*
	list_t* lst_new()
	void insert_new_process(list_t *list, int pid, time_t starttime)
	void update_terminated_process(list_t *list, int pid, time_t endtime)
	void lst_print(list_t *list)
	void lst_destroy(list_t *list)
*/
/**/


void *tarefa_monitora(){
	//if(__DEBUG__)
		printf("\e[36m[ DEBUG ]\e[0m Estamos na tarefa_monitora %d\n", (int) pthread_self() );
		printf("\e[36m[ DEBUG ]\e[0m %d\n", _exit );
	while(!_exit){sleep(1);
		printf("\e[36m[ DEBUG ]\e[0m Inseide loop" );
	}
		printf("\n" );
		printf("\e[36m[ DEBUG ]\e[0m Exiting %d\n", (int) pthread_self() );

		/*
	int status;
	time_t * endtime;
	while(1){
		if(children > 0) {
			pid_t ret = wait(&status);

			/*lista_mutex.lock()*/
			//			time( endtime );
			//			update_terminated_process(lista_processos, ret, *endtime);

			/*lista_mutex.unlock()*/
			/* children_mutex.lock() FIXME*/
			//			children--;
			/* children_mutex.unlock() FIXME*/
			/*		}
		else{
			sleep(1);
		}
	}
	*/
	return 0;
}

int main(int argc, char *argv[]){
	list_t* lista_processos;
	lista_processos= lst_new();
	char **argVector;
	int i;
	int _exit = 0;
	int children = 0;
	// o argVector ira guardar o input do utilizador na par-shell. O seu tamanho coincide
	// com o numero maximo de argumentos permitidos mais um, que corresponde ao nome do
	// proprio comando
	argVector = (char **) malloc(VECTOR_SIZE * sizeof(char*));

	/*Aula teorica */
	pthread_t tid;
	if(pthread_create (&tid, 0,tarefa_monitora, NULL) == 0)	{
		//if(__DEBUG__){
			printf ("\e[36m[ DEBUG ]\e[0m Criada a tarefa %d\e[0m\n",(int) tid);
		//}
	}
	else {
		printf("\e[31mErro \e[0m na criação da tarefa\n");
		exit(1);
	}

  /**/

	// loop infinito de execucao da par-shell
	while(!_exit){
		// le os argumentos atraves da funcao fornecida
		readLineArguments(argVector, VECTOR_SIZE);

		// caso nao tenha sido introduzido um comando, a par-shell prossegue a sua execucao
		if(argVector[0] == NULL){
			continue;
		}

		if(strcmp(argVector[0], "exit") == 0){
			_exit = 1;
		}else{
			// Criacao do processo filho
			int pid = fork();

			if(pid < 0){
				// erro ao criar o processo filho
				perror("Error forking process");

			}else if(pid > 0) {
				// PROCESSO PAI

				/* children_mutex.lock() FIXME*/
				children++;
				/* children_mutex.unlock() FIXME*/
				/*lista_mutex.lock()*/
				insert_new_process(lista_processos, pid, time(NULL));
				/*lista_mutex.unlock()*/

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
	// quando sai, verifica se todos os filhos que criou ja terminaram e espera pelos que ainda estao a correr
	printf("Waiting for child processes to finish...\n");

	// aloca memoria necessaria para a monitorizacao da terminacao dos processos filho
	int *outpid = (int *) malloc(sizeof(int) * children);
	int *outstatus = (int *) malloc(sizeof(int) * children);
	int status;
	time_t * endtime;

	for(i = 0; i < children; i++){
		if(__DEBUG__){
			printf("\e[36m[ DEBUG ]\e[0m\t%d processes remaining\n", children - i);
		}
		// aguarda pela terminacao dos processos filhos
		pid_t ret = wait(&status);
		// regista o pid do processo acabado de terminar e o respectivo return status
		outpid[i] = ret;
		outstatus[i] = status;
		if(WIFEXITED(status)){
			update_terminated_process(lista_processos, ret, time(NULL), WEXITSTATUS(status));
		}
		else{
			printf("\e[31mProcess %d terminated Abruptly\e[0m\n", ret );
			delete_process(lista_processos, ret);
		}
	}
	//pthread_join (tid, NULL);

	/*TODO probably garbage*/
	int currentStatus;
	for(i = 0; i < children; i++){
		// apresenta no ecra o exit status reportado por cada processo filho e o respectivo pid
		currentStatus = outstatus[i];
		if(WIFEXITED(currentStatus)){
			printf("Process %d terminated with status %d\n", outpid[i], WEXITSTATUS(currentStatus));
		}
	}
	/*TODO end ^ */

  lst_print(lista_processos);
	lst_destroy(lista_processos);

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
