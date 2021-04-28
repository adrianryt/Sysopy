#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>

int global_depth;

int lookForPhrase(char* file_name, char* looking_phrase){
    //printf("%s",file_name);
    FILE *f = fopen(file_name,"r");
    if(f == NULL){
        printf("Can't read this file: %s", file_name);
        return 0;
    }
    char *line_file = NULL;
    size_t line_file_size = 0;
    int c = 1;
    while(c){
        if(getline(&line_file, &line_file_size, f) != -1){
            //printf("%s",line_file);
            if(strstr(line_file, looking_phrase) != NULL){
                return 1;
            }
        }
        else{
            c= 0;
        }
    }
    return 0;
}

void main_function(char* path_start, char* looking_phrase, int depth){

    if(depth > global_depth){
        exit(0);
    }

    DIR* current_dirr = opendir(path_start);
    if(current_dirr == NULL){
        printf("Can't open this dir: %s", path_start);
        exit(0);
    }

    struct dirent* dir = readdir(current_dirr);

    while (dir != NULL){
        if(strcmp(dir->d_name,".") && strcmp(dir->d_name,"..") && dir->d_type == 8 && strstr(dir->d_name, ".txt")){ //if this is a regular txt file we check if he has our looking_phrase
            //printf("Name: %s Type: %d", dir->d_name, dir->d_type);
            char new_file_path[100]; //tworzymy juz koncowa sciezke do pliku ktory bedziemy sprawdzac
            strcpy(new_file_path, path_start);
            strcat(new_file_path,"/");
            strcat(new_file_path, dir->d_name);
            //printf("to jest nowy path do filesa: %s \n", new_file_path);
            if(lookForPhrase(new_file_path, looking_phrase) == 1){
                 printf("%s With son pid %d from parent pid %d\n", new_file_path, getpid(), getppid());
            }
            
        }
        else if(strcmp(dir->d_name,".") && strcmp(dir->d_name,"..") && dir->d_type == 4){
            //printf("Name: %s Type: %d", dir->d_name, dir->d_type);
            pid_t child_pid = fork();
            if(child_pid == 0){
                char new_path[100]; //tworzymy nowy path do katalogu
                strcpy(new_path, path_start);
                strcat(new_path,"/");
                strcat(new_path, dir->d_name);
                //printf("to jest nowy path: %s \n", new_path);
                main_function(new_path,looking_phrase,depth + 1);
                closedir(current_dirr);
                exit(0);
            }
        }
        wait(NULL);
        dir = readdir(current_dirr);
    }
    wait(NULL);
    closedir(current_dirr);
}

int main(int arg_n, char** arg_list){

    if(arg_n != 4){
        printf("give me 4 arguments or I wont work");
        return 1;
    }
    char path_start[100];
    char looking_phrase[100];
    global_depth = atoi(arg_list[3]);
    strcpy(path_start, arg_list[1]);
    strcpy(looking_phrase, arg_list[2]);
    //printf("%s %s %d",path_start,looking_phrase,global_depth);

    main_function(path_start, looking_phrase, 0);

    
}