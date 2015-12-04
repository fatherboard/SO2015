#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "commandlinereader.h"
#include "list.h"
#include "pipes.h"

#define EXIT_COMMAND "exit"
#define EXIT_GLOBAL "exit-global"
#define MAIN_PIPE "par-shell-in"
#define NEW_TERMINAL_COMMAND   "ffmyqkdbjipyzgjxnbiatqhmpudxovas_new"
#define CLOSE_TERMINAL_COMMAND "hbtdqvkwcrxixyjbbkwjzqkylqhcdr_close"
#define VECTOR_SIZE 6
#define ARG_LEN 256
#define MAXPAR 4
#define __DEBUG__ 1

/* Ver ficheiro README.md para obter a sintaxe dos comandos
de comunicacao entre a par-shell e os terminais 	*/

/* variaveis de sincronizaçao */

/* variavel de condicao que garante que nao sao lancados mais processos que
o limite imposto pelo MAXPAR */
pthread_cond_t slots_processos_disponiveis;
/* variavel de condicao que ve se existem comandos pelos quais deve
esperar/processar */
pthread_cond_t comandos_escritos;

pthread_t tid;

pthread_mutex_t slots_processos_disponiveis_mutex;
pthread_mutex_t comandos_escritos_mutex;
pthread_mutex_t children_mutex;
pthread_mutex_t lista_mutex;

// variaveis globais a serem partilhadas pelas threads
/* Comandos tipo fib (validos) */
list_t *lista_processos;
// a lista_terminais mantem registo dos par-shell-terminal que estao com contacto com esta par-shell
list_t *lista_terminais;

int numChildren = 0;
int _exit_ctrl = 0;
int writtenCommands = 0;
int slotsAvaiable = MAXPAR;
static FILE *log;
int iteration_number = 0, total_exec_time = 0;
char **argVector;

void test_mutexes(){
	printf("\e[36m[ DEBUG ]\e[0m I will wait for children_mutex\n");
	pthread_mutex_lock(&children_mutex);
	pthread_mutex_unlock(&children_mutex);
	printf("\e[36m[ DEBUG ]\e[0m I waited for children_mutex\n");

	printf("\e[36m[ DEBUG ]\e[0m I will wait for comandos_escritos_mutex\n");
	pthread_mutex_lock(&comandos_escritos_mutex);
	pthread_mutex_unlock(&comandos_escritos_mutex);
	printf("\e[36m[ DEBUG ]\e[0m I waited for comandos_escritos_mutex\n");

	printf("\e[36m[ DEBUG ]\e[0m I will wait for slots_processos_disponiveis_mutex\n");
	pthread_mutex_lock(&slots_processos_disponiveis_mutex);
	pthread_mutex_unlock(&slots_processos_disponiveis_mutex);
	printf("\e[36m[ DEBUG ]\e[0m I waited for slots_processos_disponiveis_mutex\n");

	printf("\e[36m[ DEBUG ]\e[0m I will wait for lista_mutex\n");
	pthread_mutex_lock(&lista_mutex);
	pthread_mutex_unlock(&lista_mutex);
	printf("\e[36m[ DEBUG ]\e[0m I waited for lista_mutex\n");
}

void terminate_terminals(){
	lst_iitem_t *item;
	item = lista_terminais->first;
	//prev = lista_terminais->first;
	int pid;
	while(item != NULL){
		pid = item->pid;
		printf("\e[35m[ KILL  ]\e[0m Killing %d\n",item->pid);

		kill(pid, SIGINT);
		if(__DEBUG__){
			printf("\e[35m[ KILL  ]\e[0m Signal Sent to process number %d\n",pid);
		}
		item = item->next;
		delete_process(lista_terminais, pid);
	}
}

void end_sequence(){
	printf("\n\e[33m[ INFO  ]\e[0m Terminals hunting has begun! \n");
	terminate_terminals();
	printf("\e[33m[ INFO  ]\e[0m Terminals hunting is over for now! \n");
	deleteFifo(MAIN_PIPE);

	//needed for thread *monitora* completion
	_exit_ctrl = 1;
	pthread_mutex_lock(&comandos_escritos_mutex);
	writtenCommands++;
	pthread_cond_signal(&comandos_escritos);
	pthread_mutex_unlock(&comandos_escritos_mutex);

	printf("\n\e[33m[ INFO  ]\e[0m Joining monitoring thread...\n\n");
	if(__DEBUG__){
		printf("Then I will proceed to debug\n");
	}
	if(pthread_join(tid, NULL) != 0) {
		printf("\e[31m[ Error ]\e[0m joining thread.\n");
		exit(EXIT_FAILURE);
	}
	lst_destroy(lista_terminais);
	// liberta a memoria alocada
	printf("\e[33m[ INFO  ]\e[0m Par-shell before destroy \e[35mhere\e[0m\n");
	printf("\e[33m[ INFO  ]\e[0m __DEBUG__ = %d, so testing will ", __DEBUG__);
	if(!__DEBUG__){
		printf("not ");
	}
	printf("be executed\n");
	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m I will test the locks\n");
		test_mutexes();
		printf("\e[36m[ DEBUG ]\e[0m I finished testing the locks\n");
	}
	pthread_mutex_destroy(&children_mutex);
	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m children_mutex was destroyed\n");
	}
	pthread_mutex_destroy(&comandos_escritos_mutex);
	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m comandos_escritos_mutex was destroyed\n");
	}
	pthread_mutex_destroy(&slots_processos_disponiveis_mutex);
	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m slots_processos_disponiveis_mutex was destroyed\n");
	}
	lst_print(lista_processos);
	pthread_mutex_destroy(&lista_mutex);

	lst_destroy(lista_processos);
	free(argVector);
	pthread_cond_destroy(&comandos_escritos);
	fclose(log);

	//needed in case slots_processos_disponiveis is blocked
	//Not really working
	/*pthread_mutex_lock(&slots_processos_disponiveis_mutex);
	slotsAvaiable++;
	pthread_cond_signal(&slots_processos_disponiveis);
	pthread_mutex_unlock(&slots_processos_disponiveis_mutex);*/
	printf("\e[33m[ INFO  ]\e[0m Par-shell before destroy \e[36mhere\e[0m\n");
	pthread_cond_destroy(&slots_processos_disponiveis);

	printf("\e[33m[ INFO  ]\e[0m cond destroyed\n");
	// da a mensagem de fim do programa
	printf("\e[33m[ INFO  ]\e[0m Par-shell terminated\n");
	printf("\e[33m[ INFO  ]\e[0m exiting...\n");

	exit(EXIT_SUCCESS);
}

void *tarefa_monitora(){
	printf("\e[34m[ THREAD]\e[0m Estamos na tarefa_monitora (pthread_self) %d\n", (int) pthread_self());

	int status, dif;

	while(1){
		//Antigo sem_wait(&comandos_escritos);
		/* Esperar que tenha sido escrito um comando */
		pthread_mutex_lock(&comandos_escritos_mutex);
		while (writtenCommands == 0) {
			pthread_cond_wait(&comandos_escritos, &comandos_escritos_mutex);
		}
		writtenCommands--;
		pthread_mutex_unlock(&comandos_escritos_mutex);

		pthread_mutex_lock(&children_mutex);
		if(numChildren > 0) {
			pthread_mutex_unlock(&children_mutex);

			// aguarda pela terminacao dos processos filhos
			pid_t ret = wait(&status);

			/*Assinalar que existe mais um slot*/
			//Antigo sem_post(&slots_processos_disponiveis);
			pthread_mutex_lock(&slots_processos_disponiveis_mutex);
			slotsAvaiable++;
			pthread_cond_signal(&slots_processos_disponiveis);
			pthread_mutex_unlock(&slots_processos_disponiveis_mutex);

			printf("\e[33m[ INFO  ]\e[0m Process `\e[1m%d\e[0m` finished\n", (int) ret);

			// regista o pid do processo acabado de terminar e o respectivo return status na lista
			if(WIFEXITED(status)){
				//atulizacao do tempo de fim do processo
				pthread_mutex_lock(&lista_mutex);
				update_terminated_process(lista_processos, ret, time(NULL), WEXITSTATUS(status));
				dif = get_dif_time_by_pid(lista_processos, ret);
				total_exec_time += dif;
				// Escreve dados do processo no ficheiro log.txt
				fprintf(log, "iteracao %d\n", iteration_number);
				fprintf(log, "pid: %d execution time: %d\n", ret, dif);
				fprintf(log, "total execution time: %d s\n", total_exec_time);
				fflush(log);
				iteration_number++;
				pthread_mutex_unlock(&lista_mutex);
			}else{
				//Eliminacao da lista de um processo no qual ocorreu um erro (ex. seg fault)
				pthread_mutex_lock(&lista_mutex);
				delete_process(lista_processos, ret);
				pthread_mutex_unlock(&lista_mutex);
				printf("\e[31m[ ERROR ]\e[0m Process %d terminated Abruptly\n", ret);
			}

			pthread_mutex_lock(&children_mutex);
			numChildren--;
			pthread_mutex_unlock(&children_mutex);
		}else{
			if(_exit_ctrl){
				pthread_mutex_unlock(&children_mutex);
				// terminar thread
				printf("\e[33m[ INFO  ]\e[0m exiting thread \n");
				pthread_exit(0);
			}
			pthread_mutex_unlock(&children_mutex);
		}
	}
}

void ctrlCHandler(int ignored){
	fprintf(stderr, "\e[1;34m[ INFO  ]\e[0m SIGNAL received.. and handled by %d \n", (int) pthread_self());
	end_sequence();
}

void read_log_file(){
	/* Abrir FIcheiro */
	log = fopen("log.txt","a+");
	if(log == NULL){
		printf("\e[31m[ ERROR ]\e[0m could not open log.txt\n");
		exit(EXIT_FAILURE);
	}

	/*Ler dados do ficheiro*/
	char str_dummy[50], line[1024];
	int int_dummy;
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
	printf("\e[33m[ INFO  ]\e[0m Log file reading \e[32mComplete\e[0m\n");
}
void mutex_init(){
	// Inicializacao dos mutex
	if (pthread_mutex_init(&children_mutex, NULL) != 0){
		printf("\e[31m[ ERROR ]\e[0m children_mutex init failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_mutex_init(&lista_mutex, NULL) != 0){
		printf("\e[31m[ ERROR ]\e[0m lista_mutex init failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_mutex_init(&slots_processos_disponiveis_mutex, NULL) != 0){
		printf("\e[31m[ ERROR ]\e[0m slots_processos_disponiveis_mutex init failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_mutex_init(&comandos_escritos_mutex, NULL) != 0){
		printf("\e[31m[ ERROR ]\e[0m comandos_escritos_mutex init failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\e[33m[ INFO  ]\e[0m mutex init \e[32mComplete\e[0m\n");
}

void pthread_cond_initiation(){
	/* Inicializacao das variaveis de condicao*/
	if(pthread_cond_init(&slots_processos_disponiveis, NULL) != 0){
		printf("\e[31m[ ERROR ]\e[0m condition variable slots_processos_disponiveis init failed\n");
		exit(EXIT_FAILURE);
	}
	if(pthread_cond_init(&comandos_escritos, NULL) != 0){
		printf("\e[31m[ ERROR ]\e[0m condition variable comandos_escritos init failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\e[33m[ INFO  ]\e[0m pthread_cond init \e[32mComplete\e[0m\n");
}

void pthread_creation(){
	/*Criaçao da thread*/
	if(pthread_create (&tid, 0,tarefa_monitora, NULL) == 0){
		if(__DEBUG__){
			printf ("\e[36m[ DEBUG ]\e[0m Criada a tarefa %d\e[0m\n",(int) tid);
		}
	}else{
		printf("\e[31m[ ERROR ]\e[0m Creating Thread\n");
		exit(EXIT_FAILURE);
	}
	printf("\e[33m[ INFO  ]\e[0m pthread init \e[32mComplete\e[0m\n");
}

void changing_inpute_chanel(){
	create_fifo_read(MAIN_PIPE);
	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m fifo creation complete\n");
	}

	int fifo_fd = open_pipe_read(MAIN_PIPE);
	if(__DEBUG__){
		printf("\e[36m[ DEBUG ]\e[0m fifo opening complete\n");
	}

	if(dup2(fifo_fd,0) < 0){
		perror("\e[31m[ ERROR ]\e[0m Failed to redirect input\n");
		exit(EXIT_FAILURE);
	}
	printf("\e[33m[ INFO  ]\e[0m changing inpute chanel \e[32mComplete\e[0m\n");
}

int main(int argc, char *argv[]){
	//if(__DEBUG__)
	printf("\e[34m[ THREAD]\e[0m Main (pthread_self) %d\n", (int) pthread_self() );

	// Inicializacao das listas
	// o argVector ira guardar o input do utilizador na par-shell.
	argVector = (char **) malloc(VECTOR_SIZE * sizeof(char*));
	lista_processos = lst_new();
	lista_terminais = lst_new();

	// Booting Process

	// Instalacao do sinal
	signal(SIGINT, ctrlCHandler);
	read_log_file();
	mutex_init();
	pthread_cond_initiation();
	pthread_creation();
	changing_inpute_chanel();
	printf("\e[33m[ INFO  ]\e[0m Limite de processos filhos: %d\n", MAXPAR);
	printf("\e[33m[ INFO  ]\e[0m Booting Process \e[32mComplete\e[0m.\n");

	// loop infinito de execucao da par-shell
	while(!_exit_ctrl) {
		// le os argumentos atraves da funcao fornecida
		int args_read = readLineArguments(argVector, VECTOR_SIZE);
		if(args_read <= 0){
			//If it gets here no pipe is writing so we want it to get blocked
			if(__DEBUG__){
				printf("\e[36m[ DEBUG ]\e[0m niguem esta a escuta, vou ficar bloqueado atraves do open pipe\n");
			}
			create_fifo_read(MAIN_PIPE);
			if(__DEBUG__){
				printf("\e[36m[ DEBUG ]\e[0m fifo creation complete\n");
			}
			int fifo_fd = open_pipe_read(MAIN_PIPE);
			if(__DEBUG__){
				printf("\e[36m[ DEBUG ]\e[0m fifo opening complete\n");
			}
			if(dup2(fifo_fd,0) < 0){
				perror("\e[31m[ ERROR ]\e[0m Failed to redirect input\n");
				exit(EXIT_FAILURE);
			}
			continue;
		}

		if(strcmp(argVector[0], EXIT_COMMAND) == 0 || strcmp(argVector[0], EXIT_GLOBAL) == 0){
			end_sequence();
		}else if(strcmp(argVector[0], CLOSE_TERMINAL_COMMAND) == 0){
			int pstpid = atoi(argVector[1]);
			if(pstpid == 0){
				perror("\e[31m[ Error ]\e[0m CLOSE: terminal PID number invalid");
			}else{
				delete_process(lista_terminais, pstpid);
				printf("\e[33m[ INFO  ]\e[0m par-shell-terminal eliminado (PID %d)\n", pstpid);
			}
		}else if(strcmp(argVector[0], NEW_TERMINAL_COMMAND) == 0){
			// uma nova par-shell-terminal vai registar-se
			int pstpid = atoi(argVector[1]);
			if(pstpid == 0){
				perror("\e[31m[ Error ]\e[0m New: terminal PID number invalid");
			}
			else{
				insert_new_process(lista_terminais, pstpid, time(NULL));
				printf("\e[33m[ INFO  ]\e[0m Novo par-shell-terminal registado (PID %d)\n", pstpid);
			}
		}else if(strcmp(argVector[0], "stats") == 0){
			char pipe_name[512];
			char message_to_send[512];

			int pstpid = atoi(argVector[1]);
			if(pstpid == 0){
				perror("\e[31m[ Error ]\e[0m STATS: terminal PID number invalid");
			}else{
				sprintf(pipe_name, "par-shell-terminal-in-%s", argVector[1]);
				int terminal_fifo = open_pipe_write(pipe_name);
				sprintf(message_to_send, "%d %d", numChildren, total_exec_time);

				write(terminal_fifo, message_to_send, strlen(message_to_send));
				if(__DEBUG__){
					printf("\e[36m[ DEBUG ]\e[0m msg sent to %s: \'%s", argVector[1], message_to_send );
				}
				close(terminal_fifo);
			}
		}else{
			//Antigo sem_wait(&slots_processos_disponiveis);
			/* Esperar ate que a quota de numero de processos filhos nao seja ultrapassada */
			pthread_mutex_lock(&slots_processos_disponiveis_mutex);
			while (slotsAvaiable == 0) {
				printf("\e[33m[ INFO  ]\e[31m Nao\e[0m existem slots de processos disponiveis.\n");
				pthread_cond_wait(&slots_processos_disponiveis, &slots_processos_disponiveis_mutex);
			}
			slotsAvaiable--;
			pthread_mutex_unlock(&slots_processos_disponiveis_mutex);
			if(__DEBUG__){
				printf("\e[36m[ DEBUG ]\e[0m Processo pronto a ser lancado.\n");
			}

			// Criacao do processo filho
			int pid = fork();

			if(pid < 0){
				// erro ao criar o processo filho
				perror("\e[31m[ Error ]\e[0m forking process");
			}else if(pid > 0) {
				// PROCESSO PAI

				//Introducao de processo na lista
				pthread_mutex_lock(&lista_mutex);
				insert_new_process(lista_processos, pid, time(NULL));
				pthread_mutex_unlock(&lista_mutex);

				pthread_mutex_lock(&children_mutex);
				numChildren++;
				pthread_mutex_unlock(&children_mutex);

				//Antigo sem_post(&comandos_escritos);
				/* Assinalar que existe mais um filho em execuçao */
				pthread_mutex_lock(&comandos_escritos_mutex);
				writtenCommands++;
				pthread_cond_signal(&comandos_escritos);
				pthread_mutex_unlock(&comandos_escritos_mutex);
			}else{
				// PROCESSO FILHO
				printf("\e[33m[ INFO  ]\e[0m Process `\e[1m%d\e[0m` has just started.\n", getpid());
				printf("\e[33m[ INFO  ]\e[0m Executing: `\e[1m%s", argVector[0]);
				if(args_read > 1){
					printf(" %s", argVector[1] );
				}
				printf("\e[0m`\n");

				fclose(log);

				signal(SIGINT, SIG_IGN);

				// Creating name
				char str[25];
				char snum[8];
				strcpy(str, "par-shell-out-");
				sprintf(snum, "%d", getpid());
				strcat(str, snum);
				strcat(str, ".txt");
				if(__DEBUG__){
					printf("\e[36m[ DEBUG ]\e[0m ficheiro onde vai escrever: %s\e[0m\n", str);
				}
				int fd = open(str, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
				if (fd < 0){
					perror("\e[31m[ ERROR ]\e[0m Failed to open output file\n");
					exit(EXIT_FAILURE);
				}
				if(dup2(fd,1) < 0){
					perror("\e[31m[ ERROR ]\e[0m Failed to redirect output\n");
					exit(EXIT_FAILURE);
				}

				// substitui a imagem do executavel actual pelo especificado no comando introduzido
				if(execv(argVector[0], argVector)){
					
				}
				// o processo continua se nao tiver sido possivel fazer a substituicao do executavel na directoria actual
				if(execvp(argVector[0], argVector)){
					// caso nao tenha sido possivel fazer a substituicao do executavel do processo,
					// escreve no stderr a razao e termina com status de falha
					fprintf(stderr, "o comando %s nao existe\n",argVector[0]);
					exit(EXIT_FAILURE);
				}
			}
		}
	}

	// quando termina, a thread principal sincroniza-se com a monitora
	end_sequence();
	return(EXIT_SUCCESS);
}
