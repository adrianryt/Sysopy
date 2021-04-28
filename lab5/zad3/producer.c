#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/file.h>


int main(int no_arg, char** arg_list){
    printf("===PRODUCENT===\n");
    if(no_arg != 5){
        printf("Wrong argument number");
        return 1;
    }

    char *fifo_pipe = arg_list[1];
    char *row_number = arg_list[2];
    char *file_name = arg_list[3];
    int N = atoi(arg_list[4]);

    FILE *f = fopen(file_name,"r");
    FILE *fifo = fopen(fifo_pipe, "w");

   
    char *buffer = malloc(sizeof (char) * N);
    while(fread(buffer, sizeof(char),N,f) == N){
        flock(fileno(fifo), LOCK_EX); //blokujemy pisanie do pliku dla innych procesow
        fwrite(row_number,sizeof(char),strlen(row_number), fifo);
        fwrite(buffer, sizeof (char), N, fifo);
        flock(fileno(fifo), LOCK_UN); //wywalamy blokowanie
        sleep(1);
    }
    fclose(f);
    fclose(fifo);
}