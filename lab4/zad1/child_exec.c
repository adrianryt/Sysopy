#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define _SIGNAL SIGUSR1

void signal_handler(int signal){
    printf("Sygnal zlapany\n SIG: %d PID %d PPID %d \n",signal, getpid(), getppid());
}


void check_ignore(){
    printf("Check_ignore function start in exec\n");
    // The default action of SIGUSR1 and SIGUSR2 is to terminate the process so we check if we terminate the process, if not out SIG_IGN handler works
    raise(_SIGNAL);
    printf("end of exec ignore check, if you see this exec works in child_exec\n");
}

void check_mask(){
    printf("Check_mask function start in exec\n");

    sigset_t newmask;/* sygnały do blokowania */
    sigset_t oldmask; /* aktualna maska sygnałów */
    sigemptyset(&newmask); /* wyczyść zbiór blokowanych sygnałów */
    /* Dodaj do zbioru sygnałów zablokowanych */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
        perror("Nie udało się zablokować sygnału");
    }

    if(sigismember(&oldmask,_SIGNAL)){
        printf("W masce w exec jest _SIGNAL\n");
    }
    else{
        printf("W masce w exec nie ma _SIGNAL\n");
    }

    printf("Raising in child_exec\n");
    raise(_SIGNAL);

    sigset_t test_mask;
    sigpending(&test_mask); //  sigpending() returns the set of signals that are pending for delivery to the calling thread (i.e., the signals which have beenraised while blocked).  The mask of pending signals is returnedin set.
    //^ zwraca nam set pending sygnalow
    if(sigismember(&test_mask,_SIGNAL)) {
        printf("Signal is in set of process pending sygnals\n");
    } else {
        printf("Signal is NOT in set of process pending sygnals\n");
    }
}
void check_pending(){
    printf("Check_pending function start in exec\n");


    sigset_t test_mask;
    sigpending (&test_mask);
    if(sigismember(&test_mask,_SIGNAL)) {
        printf("Signal is in set of pending sygnals\n");
    } else {
        printf("Signal is NOT in set of pending sygnals\n");
    }
}


int main(int no_args, char ** arg_list) {
    printf("EXEC CODE\n");
    if(no_args != 2){
        printf("Wrong number of argument");
    }

    if(strcmp(arg_list[1], "ignore") == 0){
        check_ignore();
    }
    else if(strcmp(arg_list[1], "mask") == 0){
        check_mask();
    }
    else if(strcmp(arg_list[1], "pending") == 0){
        check_pending();
    }
    else{
        printf("WRONG ARGUMENTS");
        return 1;
    }
    printf("END OF EXEC CODE\n");
    return 0;
}