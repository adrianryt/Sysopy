#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>



void mergeFilessys(char *file1, char* file2){
    int a;
    int b;
    a = open(file1,O_RDONLY);
    b = open(file2, O_RDONLY);
    char buffer;
    int count1 = 1;
    int count2 = 1;
    while(count1 && count2){
        count1 = read(a,&buffer,1);
        if(count1 == 0){
            break;
        }
        while(buffer != '\n')
        {
            printf("%c",buffer);
            count1 = read(a,&buffer,1);
            if(count1 == 0){
                break;
            }
            
        }
        printf("\n");
        count2 = read(b,&buffer,1);
        if(count2 == 0){
            break;
        }
        
        while (buffer != '\n')
        {
            printf("%c",buffer);
            count2 = read(b,&buffer,1);
            if(count2 == 0){
                break;
            }
        }
        printf("\n");
    }
    while(count1)
    {
        count1 = read(a,&buffer,1);
        if(count1 == 0){
            break;
        }
        printf("%c",buffer);
        
    }

    while (count2)
    {
        count2 = read(b,&buffer,1);
        if(count2 == 0){
            break;
        }
        printf("%c",buffer);
        
    }
    


}

void mergeFileslib(char *file1, char* file2){
    FILE *f1 = fopen(file1,"r");
    FILE *f2 = fopen(file2, "r");
    char buffer;
    int a = 1;
    int b = 1;
    int count;
    while(a || b){ //podwojne ify sprawdzaja przypadki w ktorych na koncu pliku jest enter
        count = fread(&buffer, sizeof(char), 1, f1); 
        if(count == 0){
            a = 0;
            break;
        }
        while(a && buffer != '\n'){
            printf("%c", buffer);
            count = fread(&buffer, sizeof(char), 1, f1);
            if(count == 0){
                a = 0;
                break;
            }
        }
        printf("\n");

        count = fread(&buffer, sizeof(char), 1, f2);
        if(count == 0){
            b = 0;
            break;
        }
        while(b && buffer != '\n'){
            printf("%c", buffer);
            count = fread(&buffer, sizeof(char), 1, f2);
            if(count == 0){
                b = 0;
                break;
            }
        }
        printf("\n");
    }
    while(a){
        count = fread(&buffer, sizeof(char), 1, f1);
         if(count == 0){
            a = 0;
            break;
        }
        printf("%c", buffer);
    }
    while(b){
        count = fread(&buffer, sizeof(char), 1, f2);
        if(count == 0){
            b = 0;
            break;
        }
        printf("%c", buffer);
    }
    fclose(f1);
    fclose(f2);
}



int main(int no_arg, char** arg_list){
    char *file1_name = calloc(100, sizeof(char));
    char *file2_name = calloc(100, sizeof(char));
    FILE *fp = NULL;

    if(no_arg > 3){
        printf("TOO MANY ARGUMENTS");
        return 1;
    }

    if(no_arg == 1){
        printf("Please enter the name of first file");
        scanf("%s", file1_name);
        if((fp = fopen(file1_name,"r")) == NULL){
            printf("Openning file %s error\n",file1_name);
            return 1;
        }
        fclose(fp);
        fp = NULL;
        printf("Please enter the name of second file");
        scanf("%s", file2_name);

        if((fp = fopen(file2_name,"r")) == NULL){
            printf("Openning file %s error",file2_name);
            return 1;
        }
        fclose(fp);
        fp = NULL;
    }

    else if(no_arg == 2){
        strcpy(file1_name, arg_list[1]);
        printf("Please enter the name of second file");
        scanf("%s", file2_name);
        if((fp = fopen(file2_name,"r")) == NULL){
            printf("Openning file %s error",file2_name);
            return 1;
        }
        fclose(fp);
        fp = NULL;
    }
    else
    {
        strcpy(file1_name, arg_list[1]);
        strcpy(file2_name, arg_list[2]);
    }
    FILE *raport = fopen("pomiar_zad_1.txt","w+");
    clock_t operation_time[3];

    operation_time[0] = clock();
    mergeFileslib(file1_name, file2_name);
    operation_time[1] = clock();

    mergeFilessys(file1_name, file2_name);
    operation_time[2] = clock();



    fprintf(raport,"Czas mergowania: \n");
    fprintf(raport,"lib   %lf\n",(double) (operation_time[1] - operation_time[0]) / CLOCKS_PER_SEC );
    fprintf(raport,"sys   %lf\n",(double) (operation_time[2] - operation_time[1]) / CLOCKS_PER_SEC );
    fclose(raport);
    free(file1_name);
    free(file2_name);
}