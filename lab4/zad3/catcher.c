#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int signal_counter = 0;
int CATCH_SIGNAL;
int END_SIGNAL;
char *mode_sig;

void catcher_handler(int sig, siginfo_t *siginfo, void *context){
    if(sig == CATCH_SIGNAL){
        signal_counter++;
    }
    else if(sig == END_SIGNAL){
        if(strcmp(mode_sig, "kill") == 0 || strcmp(mode_sig, "sigrt") == 0){
            for(int i = 0; i < signal_counter; i++){
                kill(siginfo->si_pid, CATCH_SIGNAL); // dzieki siginfo-> si_pid dostajemy sie do pidu procesu z ktorego otrzymalismy sygnal(sendera)
            }
            kill(siginfo->si_pid, END_SIGNAL);
        }else{
            union sigval value;
            for(int i = 0; i < signal_counter; i++){
                value.sival_int = i;
                sigqueue(siginfo->si_pid, CATCH_SIGNAL,value); // dzieki siginfo-> si_pid dostajemy sie do pidu procesu z ktorego otrzymalismy sygnal(sendera)
            }
            sigqueue(siginfo->si_pid, END_SIGNAL, value);
        }
        printf("catcher handled: %d signals\n", signal_counter);
        exit(0);
    }else{
        printf("CANT HANDLE THIS SIGNAL! SOMETHINK IS NOT YES");
    }
}

int main(int no_args, char** arg_list){

    if(no_args != 2){
        printf("Wrong argument number");
        return 1;
    }

    mode_sig = arg_list[1];

    
    if(strcmp(mode_sig, "kill") == 0){
        CATCH_SIGNAL = SIGUSR1;
        END_SIGNAL = SIGUSR2;
    }
    else if(strcmp(mode_sig, "sigqueue") == 0){
        CATCH_SIGNAL = SIGUSR1;
        END_SIGNAL = SIGUSR2;
    }
    else if(strcmp(mode_sig, "sigrt") == 0){
        CATCH_SIGNAL = SIGRTMIN + 1;
        END_SIGNAL = SIGRTMIN + 2;
    }
    else{
        printf("Wrong argument");
        return 1;
    }
    
    sigset_t catcher_mask;
    sigfillset(&catcher_mask);
    sigdelset(&catcher_mask, CATCH_SIGNAL);
    sigdelset(&catcher_mask, END_SIGNAL);
    if(sigprocmask(SIG_BLOCK, &catcher_mask,NULL)<0){
        perror("Nie udało się zablokować sygnału\n");
        return 1;
    }


    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = catcher_handler;
    
    sigaction(CATCH_SIGNAL, &act, NULL);
    sigaction(END_SIGNAL, &act, NULL);

    printf("I WRITE FROM CATCHER PID: %d\n", getpid());
    while(1){
        sigsuspend(&catcher_mask);
    }
    return 0;
}