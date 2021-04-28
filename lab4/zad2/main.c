#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void siginfo_handler (int sig, siginfo_t *siginfo, void *context)
{
	printf ("Sending PID: %d, UID: %d\nReal user ID of sending process: %d\nSignal number being delivered: %d\nSignal code: %d\n",
    siginfo->si_pid, siginfo->si_uid, siginfo->si_uid, siginfo->si_signo, siginfo->si_code);
}
void nodefer_handler(int signal){

    printf("Into handler\n");
    while(1);
}

void nocldstop_handler(int signal){
    printf("notification from: SIG: %d, PID: %d, PPID: %d\n", signal, getpid(),getppid());
}


void signal_one(){
    printf("====SA_SIGINFO====\n");
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_handler = &siginfo_handler;
    sigaction(SIGUSR1, &act, NULL);
    raise(SIGUSR1);
}

void signal_two(){
    printf("====SA_NOCLDSTOP====\n");
    struct sigaction act;
    act.sa_handler = &nocldstop_handler;
    sigaction(SIGCHLD, &act, NULL);

    pid_t child_pid = fork();
    if(child_pid == 0){
        while(1);
    }
    else{
        printf("SIGSTOP without flag\n");
        sleep(1);
        kill(child_pid, SIGSTOP);
        wait(NULL);
    }
    
    child_pid = fork();
    if(child_pid == 0){
        while(1);
    }
    else{
        printf("SIGKILL without flag\n");
        sleep(1);
        kill(child_pid, SIGKILL);
        wait(NULL);
    }

    act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act, NULL);

    child_pid = fork();
    if(child_pid == 0){
        while(1);
    }
    else{
        printf("SIGSTOP with flag\n");
        sleep(1);
        kill(child_pid, SIGSTOP);
        //wait(NULL);
    }
    child_pid = fork();
    if(child_pid == 0){
        while(1);
    }
    else{
        printf("SIGKILL with flag\n");
        sleep(1);
        kill(child_pid, SIGKILL);
        wait(NULL);
    }
}

void signal_three(){
    printf("====SA_NODEFER====\n");
    struct sigaction act;
    //Funkcje niezawodnej obsługi sygnałów muszą się charakteryzować:
    //• blokowaniem tego samego sygnału podczas jego obsługi;
    // no i dzieki tej fladze mamy na to wywalone
    //: Do not prevent the signal from being received from within its own signal handler.
    act.sa_flags = SA_NODEFER;
    act.sa_handler = &nodefer_handler;
    sigaction(SIGINT, &act, NULL); //SIGINT czyli interupt from keyboard
    while(1);
}

int main(){

    printf("Test first signal\n");
    signal_one();

    printf("Test second signal\n");
    signal_two();

    printf("Test third signal\n");
    signal_three();


}
