#include <stdio.h>
#include "library.h"
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>

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


void run_test(int number_of_blocks, int number_of_rows, FILE *raport){

    struct tms operation_time[4]; //usr and sys takty procesora
    clock_t operation_time_real[4]; //real 

    int numberOfFiles = number_of_blocks * 2;
    char **fileNames = calloc(numberOfFiles,sizeof(char*));
    
    for(int i = 0; i < numberOfFiles; i++){
        char file_name[30] = "file_number_";
        char nr[30];
        sprintf(nr, "%d", i); //zamienia nam i na "stringa"
        strcat(file_name, nr);
        strcat(file_name, ".txt");
        fileNames[i] = file_name;
        generateFile(file_name, number_of_rows);
    }
    pair *pairs = NULL;
    block *result = NULL;
    //start pomiaru mergowania
    times(&operation_time[0]);
    operation_time_real[0] = clock();

    pairs = fill_pairs_with_data(numberOfFiles, fileNames); //poprzez pomiar "mergowania plikow rozumiem czas tworzenia sekwencji par oraz ich mergowania"
    result = allFilesMerge(pairs, number_of_blocks);//number of pairs

    times(&operation_time[1]);
    operation_time_real[1] = clock();
    //koniec pomiaru mergowania
    //start pomiaru usuwania wszystkich blokow

    for(int i = 0; i < number_of_blocks; i++) {
        deleteBlock(i,result);
    }


    times(&operation_time[2]);
    operation_time_real[2] = clock();
    //koniec pomiaru usuwania wszystkich blokow
    
    pairs = fill_pairs_with_data(numberOfFiles, fileNames); 
    result = allFilesMerge(pairs, number_of_blocks);//number of pairs
    for(int i = 0; i < number_of_blocks; i++) {
        deleteBlock(i,result);
    }
    times(&operation_time[3]);
    operation_time_real[3] = clock();
    //pomiar dodania i usuniecia blokow liczymy jako suma poprzednich dwoch pomiarow 
    
    fprintf(raport,"         REAL         USER         SYS\n");
    fprintf(raport,"MERGE    %lf",(double) (operation_time_real[1] - operation_time_real[0]) / CLOCKS_PER_SEC );
    fprintf(raport,"     %lf",(double) (operation_time[1].tms_utime - operation_time[0].tms_utime) / sysconf(_SC_CLK_TCK));
    fprintf(raport,"     %lf \n",(double) (operation_time[1].tms_stime - operation_time[0].tms_stime) / sysconf(_SC_CLK_TCK));

    fprintf(raport,"DELETE   %lf",(double) (operation_time_real[2] - operation_time_real[1]) / CLOCKS_PER_SEC );
    fprintf(raport,"     %lf",(double) (operation_time[2].tms_utime - operation_time[1].tms_utime) / sysconf(_SC_CLK_TCK));
    fprintf(raport,"     %lf \n",(double) (operation_time[2].tms_stime - operation_time[1].tms_stime) / sysconf(_SC_CLK_TCK) );

    fprintf(raport,"Add-DEL  %lf",(double) (operation_time_real[3] - operation_time_real[2]) / CLOCKS_PER_SEC );
    fprintf(raport,"     %lf",(double) (operation_time[3].tms_utime - operation_time[2].tms_utime) / sysconf(_SC_CLK_TCK));
    fprintf(raport,"     %lf \n",(double) (operation_time[3].tms_stime - operation_time[2].tms_stime) / sysconf(_SC_CLK_TCK) );


    printf("%s", "         REAL         USER         SYS\n");
    printf("MERGE    %lf",(double) (operation_time_real[1] - operation_time_real[0]) / CLOCKS_PER_SEC );
    printf("     %lf",(double) (operation_time[1].tms_utime - operation_time[0].tms_utime) / sysconf(_SC_CLK_TCK) );
    printf("     %lf \n",(double) (operation_time[1].tms_stime - operation_time[0].tms_stime) / sysconf(_SC_CLK_TCK) );

    printf("DELETE   %lf",(double) (operation_time_real[2] - operation_time_real[1]) / CLOCKS_PER_SEC );
    printf("     %lf",(double) (operation_time[2].tms_utime - operation_time[1].tms_utime) / sysconf(_SC_CLK_TCK) );
    printf("     %lf \n",(double) (operation_time[2].tms_stime - operation_time[1].tms_stime) / sysconf(_SC_CLK_TCK) );

    printf("Add-DEL  %lf",(double) (operation_time_real[3] - operation_time_real[2]) / CLOCKS_PER_SEC );
    printf("     %lf",(double) (operation_time[3].tms_utime - operation_time[2].tms_utime) / sysconf(_SC_CLK_TCK) );
    printf("     %lf \n",(double) (operation_time[3].tms_stime - operation_time[2].tms_stime) / sysconf(_SC_CLK_TCK) );
    
    printf("\n");
    free(fileNames);
    fileNames = NULL;
}

int main(int no_arguments, char ** arg_list) {
    // w programie uzytkownik musi poprawnie podac 4 pierwsze argumenty tj. 1. create_table 2.rozmiar(ilosc par) 
    //3. merge_files 4. lista plikow. Nastepne argumenty mowia o czynnosciach ktore uzytkownik chce wykonac na plikach
    //podczas przeprowadzania testow na wiekszej ilosci plikow nalezy wpisac ./output test 


    //!!!! make test - komenda realizujaca testy i mierzenie czasu zadanie 2
    
    srand((unsigned int) time(NULL));

    if(no_arguments >= 2 && strcmp(arg_list[1], "test") == 0) {
        FILE *raport = fopen("raport2.txt","w+");
        printf("%s", "Test 5 blocks, each 15 rows\n");
        fprintf(raport,"Test 5 blocks, each 15 rows\n");
        run_test(5, 15,raport);

        printf("%s", "Test 5 blocks, each 150 rows\n");
        fprintf(raport,"Test 5 blocks, each 150 rows\n");
        run_test(5, 150, raport);
    
        printf("%s", "Test 20 blocks, each 100 rows\n");
        fprintf(raport,"Test 20 blocks, each 100 rows\n");
        run_test(20, 15,raport);

        printf("%s", "Test 20 blocks, each 500 rows\n");
        fprintf(raport,"Test 20 blocks, each 500 rows\n");
        run_test(20, 500,raport);

        printf("%s", "Test 100 blocks, each 100 rows\n");
        fprintf(raport,"Test 100 blocks, each 100 rows\n");
        run_test(100, 100,raport);

        printf("%s", "Test 100 blocks, each 300 rows\n");
        fprintf(raport,"Test 100 blocks, each 300 rows\n");
        run_test(100, 300,raport);

        printf("%s", "Test 300 blocks, each 800 rows\n");
        fprintf(raport,"Test 300 blocks, each 800 rows\n");
        run_test(100, 800,raport);
    
        fclose(raport);
    
        return 0;
    }


    if(no_arguments <= 4){
        printf("%s", " not enough arguments \n");
        return -1;
    }
    if(strcmp(arg_list[1],"create_table") != 0){
        printf("%s","first argument must create table \n");
        return -1;
    }
    if(strcmp(arg_list[3],"merge_files") != 0){
        printf("%s","use merge_file as 4'th argument \n");
        return -1;
    }

    int numberOfFiles = strtol(arg_list[2], NULL, 10);
    numberOfFiles *=2;
    char **fileNames = calloc(numberOfFiles,sizeof(char*)); //tablica z nazwami plikow
    int j=0;

    for(int i = 0; i < numberOfFiles/2; i++){//nazwy plikow zaczynaja sie od 4 argumentu
        fileNames[j] = strtok(arg_list[i+4],":");
        j++;
        fileNames[j] = strtok(NULL,"");
        j++;
    } //zapisane w postaci tablicy stringow
    
    j=j/2 + 4; //pliki podajemy w parach dlatego /2 a +4 bo pierwsze 4 argumenty to nie pliki - takim sposobem nasze j wskazuje na kolejny argument(jezeli istnieje)
    
    pair *pairs = NULL;
    block *result = NULL;

    pairs = fill_pairs_with_data(numberOfFiles, fileNames);
    result = allFilesMerge(pairs, numberOfFiles/2);//number of pairs
    //^ to zawsze ma sie wykonac jezeli podamy dobre dane

    printf("%s", "blocks after merging: \n\n");
    listMainTable(result, numberOfFiles/2); //wypisujemy zawartosc po zmergowaniu
    printf("%s","\n");

    while(j<no_arguments){ //sprawdzanie kolejnych argumentow
         if(strcmp(arg_list[j], "remove_block") == 0){
             deleteBlock(strtol(arg_list[j+1],NULL,10),result);
             j++;
         }
         else if (strcmp(arg_list[j], "remove_row") == 0)
         {
             deleteRowFromBlock(strtol(arg_list[j+1],NULL,10),strtol(arg_list[j+2],NULL,10),result);
             j+=2;
         }
         else{
             printf("%s", "You wrote wrong argument");
         }
         j++;
         
    }

    printf("%s", "Block after arguments:\n \n");
    listMainTable(result, numberOfFiles/2);
    printf("%s","\n");


    free(pairs);
    free(result);
    free(fileNames);
    fileNames = NULL;
    

    return 0;
}