#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/file.h>
#include <sys/types.h> 
#include <sys/stat.h>


int main(int no_arg, char** arg_list){

    char *consumer[] = {"./consumer","fifo","b.txt","5",NULL};
    char *producer1[] = {"./producer","fifo","1","a1.txt","5",NULL};
    char *producer2[] = {"./producer","fifo","2","a2.txt","5",NULL};
    char *producer3[] = {"./producer","fifo","3","a3.txt","5",NULL};

    mkfifo("fifo", 0777);
    pid_t pid_tab[4];
    if ((pid_tab[0] = fork()) == 0)
        execvp(consumer[0], consumer);

    if ((pid_tab[1] = fork()) == 0)
        execvp(producer1[0], producer1);

    if ((pid_tab[2] = fork()) == 0)
        execvp(producer2[0], producer2);
    if ((pid_tab[3] = fork()) == 0)
        execvp(producer3[0], producer3);

    for (int i = 0; i < 6; i++)
        waitpid(pid_tab[i], NULL, 0);

    printf("====MAIN PROGRAM ENDED====\n");
}
