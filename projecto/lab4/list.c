/*
* list.c - implementation of the integer list functions
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"

list_t* lst_new()
{
    list_t *list;
    list = (list_t*) malloc(sizeof(list_t));
    list->first = NULL;
    return list;
}

void lst_destroy(list_t *list)
{
    struct lst_iitem *item, *nextitem;

    item = list->first;
    while (item != NULL){
        nextitem = item->next;
        free(item);
        item = nextitem;
    }
    free(list);
}

void insert_new_process(list_t *list, int pid, time_t starttime)
{
    lst_iitem_t *item;

    item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
    item->pid = pid;
    item->starttime = starttime;
    item->endtime = 0;
    item->next = list->first;
    list->first = item;
}

// Implemente a função update_terminated_process. A função update_terminated_process recebe uma lista, um valor de pid e um tempo de fim,
// procura pelo elemento com esse valor de pid e atualiza esse elemento com o tempo de fim.
int update_terminated_process(list_t *list, int pid, time_t endtime, int status)
{

    int dif_time;
    lst_iitem_t *aux = list->first;
    while(aux != NULL){
        if(aux->pid == pid){
            aux->endtime = endtime;
	    dif_time = (int) aux->endtime - aux->starttime;
            aux->status = status;
            break;
        }
        aux = aux->next;
    }
    if( aux == NULL)
    printf("\e[31m[ ERROR ]\e[0m update_terminated_process: There is no process on the list with the pid: %d\n", pid );
    //   printf("teminated process with pid: %d\n", pid);

    return dif_time;
}

void delete_process(list_t *list, int pid){
    lst_iitem_t *currentItem, *previousItem;
    currentItem = list->first;
    if(list->first->pid == pid) {
      list->first = list->first->next;
      free(currentItem);
    }
    else{
      previousItem = list->first;
      while(currentItem != NULL) {
          if(currentItem->pid == pid) {
              previousItem->next = currentItem->next;
              free(currentItem);
              break;
          }
          else {
              previousItem = currentItem;
              currentItem = currentItem->next;
          }
      }
      if( currentItem == NULL)
        printf("\e[31m[ ERROR ]\e[0m delete_process: There is no process on the list with the pid: %d\n", pid );
    }
}

void lst_print(list_t *list){
    lst_iitem_t *item;

    printf("\e[33m[ INFO  ]\e[0m Process list with start and end time:\n\n");
    item = list->first;
    while (item != NULL){
        printf("Process with pid: %d terminated with status %d\n",item->pid,item->status );
        printf("\t Starting time:\t%s", ctime(&(item->starttime)));
        printf("\t   Ending time:\t%s", ctime(&(item->endtime)));

        int diff = (int) item->endtime - item->starttime;

        if(diff < 1){
            printf("\tExecution time:\t<1 s\n");
        }else{
            printf("\tExecution time:\t%d s\n", diff);
        }
        item = item->next;
    }
    printf("\n\e[33m[ INFO  ]\e[0m end of list.\n\n");
}

int get_dif_time_by_pid(list_t *list, int pid){
    lst_iitem_t *item;

    item = list->first;
    while(item != NULL){
	if(item->pid == pid){
	    item->time_dif = item->endtime - item->starttime;
	    return (int) item->time_dif;
	}
	item = item->next;
    }
    return -1;
}
