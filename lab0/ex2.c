#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct list_s{
    char nome[51];
    time_t data;
    int prioridade;
    list_s *next;
} list;

list* criarLista(){
  
  return (list*) malloc(sizeof(list));
  
}

void adicionarTarefa(list *base, char nome[51], prioridade){
  
  list *aux = base->next;
  
  while(aux->next != NULL){
      aux = aux->next;
  }
  
}

int main(){
  
  list lista = criarLista();
  
}