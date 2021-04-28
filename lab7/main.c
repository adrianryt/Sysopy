#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "data_and_info.h"

void close_main(){

}


int main(){
    
    if (atexit(close_main) == -1){
        printf("MAIN atexit returned -1");
        return 1;
    }



}

