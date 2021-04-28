#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "library.h"
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <dlfcn.h>
#include <unistd.h>

char *random_line(int i){
    int size = 20 + (rand() % 10); 

    char* res = calloc(size, sizeof(char));
    for(int i = 0; i < size-2; i++){\
        if(rand() % 2 == 0){
            res[i] = (rand() % 26) + 65;
        }else{
            res[i] = (rand() % 26) + 97;
        }
        
    }
    res[size-2] = '\n';//nowa linia
    res[size-1] = '\0';//zamkniecie chara
    return res;
}

void generateFile(char *fileName, int number_of_rows){
    FILE *file = fopen(fileName, "w+");
     for(int i = 0; i < number_of_rows; i++) {
        fprintf(file, "%s", random_line(i));
    }
    fclose(file);
}


void run_test(int number_of_blocks, int number_of_rows,FILE *raport){

    struct tms operation_time[2]; //usr and sys takty procesora
    clock_t operation_time_real[2]; //real 

    pair *pairs = NULL;
    int numberOfFiles = number_of_blocks * 2;
    char **fileNames = calloc(numberOfFiles,sizeof(char*));

    for(int i = 0; i < numberOfFiles; i++){ //tworze pliki
        char file_name[30] = "file_number_";
        char nr[30];
        sprintf(nr, "%d", i); //zamienia nam i na "stringa"
        strcat(file_name, nr);
        strcat(file_name, ".txt");
        fileNames[i] = file_name;
        generateFile(file_name, number_of_rows);
    }

    pid_t child_pid;
    pairs = fill_pairs_with_data(numberOfFiles, fileNames);
    fflush(stdout);
    times(&operation_time[0]);
    operation_time_real[0] = clock();
    
    for(int i=0;i<number_of_blocks;i++) 
    {
        child_pid = fork();
        if(child_pid == 0){
            //printf("son pid %d from parent pid %d\n",getpid(),getppid());
            mergeTwoFiles(pairs,i);
            exit(0);
        }
    }
    for(int i=0;i<number_of_blocks;i++){
        wait(NULL);
    }
    times(&operation_time[1]);
    operation_time_real[1] = clock();

    //pomiar dodania i usuniecia blokow liczymy jako suma poprzednich dwoch pomiarow 

    /*fprintf(raport,"         REAL         USER         SYS\n");
    fprintf(raport,"MERGE    %lf",(double) (operation_time_real[1] - operation_time_real[0]) / CLOCKS_PER_SEC );
    fprintf(raport,"     %lf",(double) (operation_time[1].tms_utime - operation_time[0].tms_utime) / sysconf(_SC_CLK_TCK));
    fprintf(raport,"     %lf \n",(double) (operation_time[1].tms_stime - operation_time[0].tms_stime) / sysconf(_SC_CLK_TCK));*/

    printf("%s", "         REAL         USER         SYS\n");
    printf("MERGE    %lf",(double) (operation_time_real[1] - operation_time_real[0]) / CLOCKS_PER_SEC );
    printf("     %lf",(double) (operation_time[1].tms_utime - operation_time[0].tms_utime) / sysconf(_SC_CLK_TCK) );
    printf("     %lf \n",(double) (operation_time[1].tms_stime - operation_time[0].tms_stime) / sysconf(_SC_CLK_TCK) );
    
    printf("\n");
    free(fileNames);
    free(pairs);
    fileNames = NULL;
}

int main(int arg_n, char** arg_list) 
{
   
    if( strcmp(arg_list[1],"test") == 0){
        FILE *raport = fopen("raport2.txt","w+");
        run_test(100,100,raport);
        printf("%s", "Test 100 blocks, each 100 rows\n");
        //fprintf(raport,"Test 100 blocks, each 100 rows\n");
        run_test(100,100,raport);

        printf("%s", "Test 100 blocks, each 300 rows\n");
        //fprintf(raport,"Test 100 blocks, each 300 rows\n");
        run_test(100,300,raport);

        printf("%s", "Test 300 blocks, each 800 rows\n");
        //fprintf(raport,"Test 300 blocks, each 800 rows\n");
        run_test(300,800,raport);

        printf("%s", "Test 500 blocks, each 300 rows\n");
        //fprintf(raport,"Test 500 blocks, each 300 rows\n");
        run_test(500,300,raport);

        printf("%s", "Test 500 blocks, each 1000 rows\n");
        //fprintf(raport,"Test 500 blocks, each 1000 rows\n");
        run_test(500,1000,raport);

        printf("%s", "Test 1000 blocks, each 1000 rows\n");
        //fprintf(raport,"Test 1000 blocks, each 1000 rows\n");
        run_test(1000,1000,raport);

        printf("%s", "Test 5000 blocks, each 3000 rows\n");
        //fprintf(raport,"Test 5000 blocks, each 3000 rows\n");
        //run_test(3000,3000,raport);

        fclose(raport);
        return 0;
    }

    if (arg_n < 3){
        printf("Wrong argument number");
        return 1;
    }

    int numberOfFiles = strtol(arg_list[1], NULL, 10);
    numberOfFiles *=2;
    char **fileNames = calloc(numberOfFiles,sizeof(char*)); //tablica z nazwami plikow
    int j=0;

    for(int i = 0; i < numberOfFiles/2; i++){//nazwy plikow zaczynaja sie od 3 argumentu
        fileNames[j] = strtok(arg_list[i+2],":");
        j++;
        fileNames[j] = strtok(NULL,"");
        j++;
    } //zapisane w postaci tablicy stringow
    
    //for( int i = 0; i< numberOfFiles; i++){
      //  printf("%s \n",fileNames[i]);
    //}
    j=j/2 + 4; //pliki podajemy w parach dlatego /2 a +4 bo pierwsze 4 argumenty to nie pliki - takim sposobem nasze j wskazuje na kolejny argument(jezeli istnieje)
    
    pair *pairs = NULL;

    pairs = fill_pairs_with_data(numberOfFiles, fileNames);
    pid_t child_pid;
    //fflush(stdout); //usuwam buffor ktory procesy potomne dziecicza po procesie macierzystym - wypisywalo mi fliseNames po kilka razy
    for(int i=0;i<numberOfFiles/2;i++) 
    {
        child_pid = fork();
        if(child_pid == 0){
            printf("son pid %d from parent pid %d\n",getpid(),getppid());
            mergeTwoFiles(pairs,i);
            exit(0);
        }
    }

    for(int i=0;i<numberOfFiles/2;i++){
        wait(NULL);
    }

} 