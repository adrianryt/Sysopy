#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int arg_n, char** arg_list) 
{
    if ( arg_n != 2){
        printf("Wrong argument number");
        return 1;
    }
    
    int n = atoi(arg_list[1]);
    for(int i=0;i<n;i++) 
    { 
        if(fork() == 0) 
        { 
            printf("son pid %d from parent pid %d\n",getpid(),getppid()); 
            exit(0);
        } 
    } 
    for(int i=0;i<n;i++)
    wait(NULL);
} 