#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void function4sys(char *file1_name,char *file2_name, char *n1, char *n2){
    int f1 = open(file1_name,O_RDONLY);
    int f2 = open(file2_name, O_RDWR);
    if(f1 == -1|| f2 == -1) {
        printf("Error in opening files");
        return;
    }
    char buffer;
    int count = read(f1,&buffer,1);
    while(count == 1){
        if(buffer != n1[0]){
            write(f2,&buffer,1);
        }
        else{
            int idx = 1;
            count = read(f1,&buffer,1);
            while(count !=0 && idx < strlen(n1)){
                if(buffer == n1[idx]){
                    idx++;
                    count = read(f1,&buffer,1);
                }
                else{
                    break;
                }
            }
            if(idx == strlen(n1)){
                write(f2,n2,strlen(n2));
                write(f2,&buffer,1);
            }
            else{
                write(f2,n1,idx);
                write(f2,&buffer,1);
            }
            
        }
        count = read(f1,&buffer,1);
    }
    close(f1);
    close(f2);
}

void function4lib(char *file1_name,char *file2_name, char *n1, char *n2){
    FILE *f1 = fopen(file1_name, "r");
    FILE *f2 = fopen(file2_name, "w+");
    if(f1 == NULL || f2 == NULL) {
        printf("Error in opening files");
        return;
    }
    char buffer;
    int count = fread(&buffer, sizeof(char), 1, f1); 
    while(count !=0){
        if(buffer != n1[0]){
            fwrite(&buffer, sizeof(char),1,f2);
        }
        else{
            int idx = 1;
            count = fread(&buffer, sizeof(char), 1, f1);
            while(count !=0 && idx < strlen(n1)){
                if(buffer == n1[idx]){
                    idx++;
                    count = fread(&buffer, sizeof(char), 1, f1);
                }
                else{
                    break;
                }
            }
            if(idx == strlen(n1)){
                fwrite(n2, sizeof(char),strlen(n2),f2);
                fwrite(&buffer, sizeof(char),1,f2);
            }
            else{
                fwrite(n1, sizeof(char),idx,f2);
                fwrite(&buffer, sizeof(char),1,f2);
            }
        }
        count = fread(&buffer, sizeof(char), 1, f1);
    }
    fclose(f1);
    fclose(f2);
}



int main(int no_arg, char** arg_list){
    char *file1_name = calloc(100, sizeof(char));
    char *file2_name = calloc(100, sizeof(char));

    if(no_arg != 5){
        printf("Wrong number of arguments");
        return 1;
    }

    strcpy(file1_name, arg_list[1]);
    strcpy(file2_name, arg_list[2]);

   
    FILE *raport = fopen("pomiar_zad_4.txt","w+");
    clock_t operation_time[3];

    operation_time[0] = clock();
    function4lib(file1_name, file2_name, arg_list[3], arg_list[4]);
    operation_time[1] = clock();

    function4sys(file1_name, file2_name, arg_list[3], arg_list[4]);
    operation_time[2] = clock();



    fprintf(raport,"Czas mergowania: \n");
    fprintf(raport,"lib   %lf\n",(double) (operation_time[1] - operation_time[0]) / CLOCKS_PER_SEC );
    fprintf(raport,"sys   %lf\n",(double) (operation_time[2] - operation_time[1]) / CLOCKS_PER_SEC );
    fclose(raport);
    free(file1_name);
    free(file2_name);
}