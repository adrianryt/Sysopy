#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void function5sys(char *file1_name,char *file2_name){
    int f1 = open(file1_name,O_RDONLY);
    int f2 = open(file2_name, O_RDWR| O_CREAT);
    if(f1 == -1 || f2 == -1) {
        printf("Error in opening files");
        return;
    }
    char buffer[50];
    int count = read(f1,buffer,50);
    while(count !=0){
       int counter = 0;
       while(counter < count){
           write(f2,&buffer[counter],1);
           counter++;
           if(buffer[counter] == '\n'){
               write(f2,&buffer[counter],1);
               break;
           }
           if(counter == 50){
               write(f2,"\n",1);
               break;
           }
           
       }
       lseek(f1, counter - count + 1,SEEK_CUR);
       count = read(f1,buffer,50);
    }
    close(f1);
    close(f2);
}

void function5lib(char *file1_name,char *file2_name){
    FILE *f1 = fopen(file1_name, "r");
    FILE *f2 = fopen(file2_name, "w+");
    if(f1 == NULL || f2 == NULL) {
        printf("Error in opening files");
        return;
    }
    char buffer[50];
    int count = fread(buffer, sizeof(char), 50, f1);
    while(count !=0){
       int counter = 0;
       while(counter < count){
           fwrite(&buffer[counter], sizeof(char),1,f2);
           counter++;
           if(buffer[counter] == '\n'){
               fwrite(&buffer[counter], sizeof(char),1,f2);
               break;
           }
           if(counter == 50){
               fwrite("\n", sizeof(char),1,f2);
               break;
           }
           
       }
       fseek(f1, counter - count + 1,1);
       count = fread(buffer, sizeof(char), 50, f1);
    }
    fclose(f1);
    fclose(f2);
}



int main(int no_arg, char** arg_list){
    char *file1_name = calloc(100, sizeof(char));
    char *file2_name = calloc(100, sizeof(char));

    if(no_arg != 3){
        printf("Wrong number of arguments");
        return 1;
    }

    strcpy(file1_name, arg_list[1]);
    strcpy(file2_name, arg_list[2]);

   
    FILE *raport = fopen("pomiar_zad_5.txt","w+");
    clock_t operation_time[3];

    operation_time[0] = clock();
    function5lib(file1_name, file2_name);
    operation_time[1] = clock();

    function5sys(file1_name, file2_name);
    operation_time[2] = clock();



    fprintf(raport,"Czas mergowania: \n");
    fprintf(raport,"lib   %lf\n",(double) (operation_time[1] - operation_time[0]) / CLOCKS_PER_SEC );
    fprintf(raport,"sys   %lf\n",(double) (operation_time[2] - operation_time[1]) / CLOCKS_PER_SEC );
    fclose(raport);
    free(file1_name);
    free(file2_name);
}