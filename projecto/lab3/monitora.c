#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "commandlinereader.h"

int main(){

    int children = 0;

    while(1){
        if(children != 0){
            // wait
        }else{
            sleep(1);
        }
    }

}
