#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int signal_counter=0;
int CATCH_SIGNAL;
int END_SIGNAL;
int signals_to_send;
char *mode_sig;

void sender_handler(int sig, siginfo_t *siginfo, void *context){
    if(sig == CATCH_SIGNAL){
        signal_counter++;
        if(strcmp(mode_sig, "sigqueue") == 0){
            printf("Received %d SIGQUEUE: %d Catcher idx = %d \n", signal_counter, sig,siginfo ->si_value.sival_int);
        }
    }
    else if(sig == END_SIGNAL){
       printf("END OF PROGRAM\n");
       printf("Sender received: %d signals and sensed %d signals\n",signal_counter,signals_to_send);
       exit(0);
    }
    else{
        printf("CANT HANDLE THIS SIGNAL! SOMETHINK IS NOT YES");
    }
}

int main(int no_args, char** arg_list){
    if(no_args != 4){
        printf("Wrong argument number");
        return 1;
    }
    int catcher_pid = atoi(arg_list[1]);
    signals_to_send = atoi(arg_list[2]);

    mode_sig = arg_list[3];

    
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

    sigset_t sender_mask;
    sigfillset(&sender_mask);
    sigdelset(&sender_mask, CATCH_SIGNAL);
    sigdelset(&sender_mask, END_SIGNAL);
    if(sigprocmask(SIG_BLOCK, &sender_mask,NULL)<0){
        perror("Nie udało się zablokować sygnału\n");
        return 1;
    }

    
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sender_handler;
    
    sigaction(CATCH_SIGNAL, &act, NULL);
    sigaction(END_SIGNAL, &act, NULL);

    printf("I WRITE FROM SENDER PID: %d\n", getpid());
    
    if(strcmp(mode_sig, "kill") == 0 || strcmp(mode_sig, "sigrt") == 0){
        for(int i = 0; i < signals_to_send; i++){
            kill(catcher_pid, CATCH_SIGNAL);
        }
        kill(catcher_pid, END_SIGNAL);
    }else{
        union sigval value;
        for(int i = 0; i < signals_to_send; i++){
            value.sival_int = i;
            sigqueue(catcher_pid, CATCH_SIGNAL,value); 
        }
        sigqueue(catcher_pid, END_SIGNAL, value);
    }
    while(1){
        sigsuspend(&sender_mask);
    }
}
