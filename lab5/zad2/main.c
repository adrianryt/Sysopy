#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>


int main(int no_args, char** args_list){
    if(no_args != 2 && no_args != 4){
        printf("Wrong argument number!");
        return 1;
    }
    if(no_args == 2){
        if(strcmp(args_list[1],"nadawca") == 0){
            FILE *popen_file = popen("echo q | mail | sort -k 3","w");
            pclose(popen_file);
        }
        else if(strcmp(args_list[1],"data") == 0){
            FILE *popen_file = popen("mail","w");
            pclose(popen_file);
        }
        else if(strcmp(args_list[1],"tytul") == 0){
           FILE *popen_file = popen("echo q | mail | sort -k 9","w");
           pclose(popen_file);
        }
        else{
            printf("WRONG ARGUMENT");
            return 1;
        }
    }
    else{
        char line[200];
        sprintf(line, "echo %s | mail -s %s %s", args_list[3],args_list[2],args_list[1]);
        FILE *popen_file = popen(line,"w");
        pclose(popen_file);
    }

    //echo "This is the body" | mail -s "Subject3" adrianryt
    // mail -s "Test Subject2" adrianryt <<< 'This is a message'
    //echo "This is the body" | mail -s "Subject" -aFrom:Harry\<harry@gmail.com\> someone@example.com

}