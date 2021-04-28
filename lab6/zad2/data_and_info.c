#include "data_and_info.h"


void sigint_handler(int sig){
    exit(2); //we dont need to write this handler since we have atexit function that remove the queue
}

