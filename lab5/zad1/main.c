#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_LINE_LENGTH 200 // ograniczenie na dlugosc lini
#define MAX_ARGUMENTS 15 // ograniczenie na ilosc argumentow w lini
#define MAX_SKLADNIK 8
#define MAX_FUNCTIONS 5

typedef struct skladnik_arrs 
{
    char *words[MAX_ARGUMENTS];
    int words_counter;

} skladnik_arr;

typedef struct lines_to_exec_arrays
{
    char *words[(MAX_ARGUMENTS * MAX_SKLADNIK)+MAX_SKLADNIK-1];
    int words_counter;
}line_to_exec_array;


skladnik_arr *create_skladnik_arr(int n){
    skladnik_arr *result = calloc(n,sizeof(skladnik_arr));
    return result;
}

line_to_exec_array *create_line_to_exec_array(int n){
    line_to_exec_array *result = calloc(n,sizeof(line_to_exec_array));
    return result;
}


void execute_line(line_to_exec_array line){
    int prog_count = 0;
    for(int i = 0; i<line.words_counter; i++){
        if(strcmp(line.words[i],"|") == 0) prog_count++;
    }
    prog_count++;
    
    //tworzymy tablice programow ktora posluzy nam do uruchomienia execow
    char *programs[prog_count][MAX_ARGUMENTS];
    for (int i = 0; i < prog_count; i++)
    {
        for (int j = 0; j < MAX_ARGUMENTS; j++)
        {
            programs[i][j] = NULL;
        }
    }

    int tmp_prog = 0;
    int idx = 0;
    for(int i = 0; i< line.words_counter; i++){
        if(strcmp(line.words[i],"|") == 0){
            tmp_prog++;
            idx = 0;
        }else{
            programs[tmp_prog][idx] = line.words[i];
            idx++;
        }
    }
    
    /*for (int i = 0; i < prog_count; i++)
    {
        for (int j = 0; j < MAX_ARGUMENTS; j++)
        {
            printf("%s ",programs[i][j]);
        }
    }*/


    int pipes[prog_count][2];
    for (int i = 0; i < prog_count - 1; i++)
    {

        if (pipe(pipes[i]) < 0)
        {
            fprintf(stderr, "cant make pipe\n");
            exit(1);
        }
    }
    for (int i = 0; i<prog_count; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            if (i > 0)
            {
                dup2(pipes[i-1][0], STDIN_FILENO); //wlot
            }
            if (i+1 < prog_count)
            {
                dup2(pipes[i][1], STDOUT_FILENO); //wylot
            }

            for (int j = 0; j< prog_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
           
            execvp(programs[i][0], programs[i]);
            exit(0);
        }
    }
    for (int j = 0; j < prog_count - 1; j++)
    {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
    for (int i = 0; i < prog_count; i++)
    {
        wait(NULL);
    }
}

int main(int no_args, char** args_list){
    if(no_args != 2){
        printf("Wrong argument number!");
        return 1;
    }

    char command_line[MAX_LINE_LENGTH];
    FILE *file = fopen(args_list[1],"r");
    fgets(command_line, MAX_LINE_LENGTH, file);
    skladnik_arr *mainMemoryArr = create_skladnik_arr(MAX_SKLADNIK);

    int line_counter = 0;
    char* arg;
    while(command_line[0] != 10) //idziemy do entera i zbieramy skladniki
    {
        arg = strtok(command_line, " ");
        arg = strtok(NULL, " "); //przechodzimy do faktycznych komend
        arg = strtok(NULL, " ");
        int words_counter = 0;
        while(arg != NULL && arg != "\n"){
            arg[strcspn(arg, "\n")] = 0; // pozbywa, sie enterow na koncu japierdole czemy jebie sie 4 godzine ze stringami?????
            mainMemoryArr[line_counter].words[words_counter] = malloc((strlen(arg)+1) * sizeof(char));
            strcpy(mainMemoryArr[line_counter].words[words_counter],arg);
            words_counter++;
            arg = strtok(NULL, " ");
        }
        mainMemoryArr[line_counter].words_counter = words_counter;
        line_counter++;
        fgets(command_line, MAX_LINE_LENGTH, file);
    }

    for(int i = 0; i<line_counter; i++){
        for(int j=0; j< mainMemoryArr[i].words_counter;j++){
             printf("%s",mainMemoryArr[i].words[j]);
        }
        printf("\n");
    }
    int mainArrlines = line_counter;

    puts("=================");
    //przygotujmy sobie tablice ktora bedzie miala komendy do wykonania z kazdej lini

    line_to_exec_array *taskArray = create_line_to_exec_array((MAX_ARGUMENTS * MAX_SKLADNIK)+MAX_SKLADNIK-1);
    line_counter = 0;

    while(fgets(command_line, MAX_LINE_LENGTH, file) != NULL) //idziemy do konca i zbieramy polaczenia
    {
        arg = strtok(command_line, " ");
        int words_counter = 0;
        while(arg != NULL ){
            int a = arg[8] - '0';
            if(strcmp(arg,"|") == 0){
                taskArray[line_counter].words[words_counter] = malloc((strlen(arg)+1)* sizeof(char));
                strcpy(taskArray[line_counter].words[words_counter],arg);
                words_counter++;
            }
            else{
                for(int i=0; i < mainMemoryArr[a].words_counter; i++){
                    taskArray[line_counter].words[words_counter] = malloc(strlen(mainMemoryArr[a].words[i]) * sizeof(char));
                    taskArray[line_counter].words[words_counter] = mainMemoryArr[a].words[i];
                    words_counter++;
                }
            }
            arg = strtok(NULL, " ");
        }

        taskArray[line_counter].words_counter = words_counter;
        line_counter++;
    }

    fclose(file);

    for(int i = 0; i<line_counter; i++){
        for(int j=0; j< taskArray[i].words_counter;j++){
             printf("%s",taskArray[i].words[j]);
        }
        printf("\n");
    }
    printf("===================\n");
    for(int i = 0; i < line_counter;i++){
        execute_line(taskArray[i]);
    }
    //tutaj jakies free ze niby nie mam leakow XD
    for(int i = 0; i < MAX_SKLADNIK; i++){
        for (int j = 0; j < mainArrlines; j++)
        {
            free(mainMemoryArr[i].words[j]);
        }
    }

    free(mainMemoryArr);

    for(int i = 0; i < ((MAX_ARGUMENTS * MAX_SKLADNIK)+MAX_SKLADNIK-1); i++){
        for (int j = 0; j < line_counter; j++)
        {
            //free(taskArray[i].words[j]); a czemu to nie dziala to nie wiem
        }
    }
    free(taskArray);
    
}