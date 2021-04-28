#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/file.h>

char BIGBUFF[1000];

int main(int no_arg, char** arg_list){
    printf("====CONSUMER====\n");
    if(no_arg != 4){
        printf("Wrong argument number");
        return 1;
    }

    char c = '\n';
    char *fifo_pipe = arg_list[1];
    char *file_name = arg_list[2];
    int N = atoi(arg_list[3]);

    FILE *f = fopen(file_name, "rw+");
    FILE *fifo = fopen(fifo_pipe, "r");

    char buffer[N+2];
    while (fgets(buffer, N+2, fifo) != NULL)
    {   
        int row = buffer[0] - '0';
        int counter = 0;
        fseek(f,0,SEEK_SET);
        while(fread(&c, sizeof (char), 1, f)) {
            if (c == '\n') counter++;
            if (counter >= row) {
                fseek(f, -1, SEEK_CUR);
                int readed = fread(BIGBUFF, sizeof(char), 1000, f);
                fseek(f, -readed, SEEK_CUR);
                char *tmp = malloc((N+2 )* sizeof(char));
                sprintf(tmp, "%.*s",(int)(strlen(buffer))-1,buffer + 1);
                fwrite(tmp, sizeof(char), N, f);  
                fwrite(BIGBUFF, sizeof(char), readed, f);
                break;
            }

        }
        if(counter < row) {
            
            for(int i = counter + 1; i <= row; i++) {
                char str[5];
                sprintf(str, "%d \n", i);
                fwrite(str, sizeof (char), strlen(str), f);
            }
            fseek(f, -1, SEEK_CUR);
            char *tmp = malloc((N+2 )* sizeof(char));
            sprintf(tmp, "%.*s", (int)(strlen(buffer))-1,buffer + 1);
            fwrite(tmp, sizeof(char), N, f);
            fwrite("\n", sizeof(char), 1, f);
        }

    }
    fclose(f);
    fclose(fifo);
}
