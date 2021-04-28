#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


void checkRowslib(char a, char *file_name){
    printf("LIBRARY\n");
    int n = 256;
    char* buffer = malloc(256*sizeof(char));
    int check_char = 0;
    int pointer;
    FILE *f = fopen(file_name,"r");
    if( f == NULL){
        printf("WRONG FILE NAME");
        return;
    }
    size_t count = fread(buffer, sizeof(char), n, f); 
    while (count != 0){
        pointer = 0;
        check_char = 0;
        while(buffer[pointer] != '\n')
        {
            if(buffer[pointer] == a){
                check_char = 1;
            }
            pointer++;
        }
        pointer++;
        if(check_char){
            printf("%.*s", pointer , buffer);
        }
        fseek(f, pointer - count ,1);
        count = fread(buffer, sizeof(char), n, f); 
    }
    fclose(f);
    free(buffer);
}


void checkRowssys(char a, char *file_name){
    printf("SYSTEM\n");
    int n = 256;
    char* buffer = malloc(256*sizeof(char));
    int check_char = 0;
    int pointer;
    int b = open(file_name,O_RDONLY);
    if( b== 0){
        printf("WRONG FILE NAME");
        return;
    }
    int count = read(b,buffer,n); 
    while (count != 0){
        pointer = 0;
        check_char = 0;
        while(buffer[pointer] != '\n')
        {
            if(buffer[pointer] == a){
                check_char = 1;
            }
            pointer++;
        }
        pointer++;
        if(check_char){
            printf("%.*s", pointer , buffer);
        }
        lseek(b, pointer - count ,SEEK_CUR);
        count = read(b,buffer,n); 
    }
    close(b);
    free(buffer);
}


int main(int no_arg, char** arg_list){
    char *file_name = calloc(100, sizeof(char));

    if(no_arg > 3){
        printf("TOO MANY ARGUMENTS");
        return 1;
    }

    if(no_arg < 3){
        printf("We need more arguments");
        return 1;
    }
    if(strlen(arg_list[1]) != 1){
        printf("Invalid argument 1");
        return 1;
    }
    char znak;
    znak = *arg_list[1];
    strcpy(file_name, arg_list[2]);

    FILE *raport = fopen("pomiar_zad_2.txt","w+");
    clock_t operation_time[3];
    
    operation_time[0] = clock();
    checkRowslib(znak, file_name);
    operation_time[1] = clock();
    checkRowssys(znak, file_name);
    operation_time[2] = clock();



    fprintf(raport,"Czas mergowania: \n");
    fprintf(raport,"lib   %lf\n",(double) (operation_time[1] - operation_time[0]) / CLOCKS_PER_SEC );
    fprintf(raport,"sys   %lf\n",(double) (operation_time[2] - operation_time[1]) / CLOCKS_PER_SEC );
    fclose(raport);
    free(file_name);
}