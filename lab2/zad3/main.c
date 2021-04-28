#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int is_square(int n){
    int i = 0;
    while(i * i < n){
        i++;
        if(i * i == n){
            return 1;
        }
    }
    return 0;
}

void function3sys(){
    int dane = open("dane.txt", O_RDONLY);
    int a = open("asys.txt", O_CREAT| O_RDWR,0777);
    int b = open("bsys.txt", O_CREAT| O_RDWR,0777);
    int c = open("csys.txt", O_CREAT| O_RDWR,0777);
    if(a == -1 || c == -1 || b == -1 || dane == -1){
        printf("Blad odczytu pliku\n");
        return;
    }
    char buffer;
    int even = 0;
    int count = read(dane,&buffer,1);
    while(count == 1){
        int number = -1;

        if(count != 0){ //jezeli jest jakas kolejna liczba to wczytuj
            number = 0;
        }
        while(buffer != '\n' && count != 0 ){
            number *=10;
            number += (buffer - '0');
            count = read(dane,&buffer,1);
        }
        
        if(number > -1){
            char tmp_arr[16];
            sprintf(tmp_arr, "%d", number);
            if(number%2 == 0) even++; //zliczmy ilosc parzystych liczb

            if(is_square(number)){
                write(c,tmp_arr,strlen(tmp_arr));
                write(c,"\n",1);
            }

            if(number <= 9 || (number % 100) / 10 == 7 || (number % 100) / 10 == 0){
                write(b,tmp_arr,strlen(tmp_arr));
                write(b,"\n",1);
            }

        }
        count = read(dane,&buffer,1);
    }
    char tmp[10];
    write(a,"Liczb parzystych jest:",22); 
    sprintf(tmp, "%d", even);
    write(a,tmp,strlen(tmp));
   

    close(dane);
    close(a);
    close(b);
    close(c);
}

void function3lib(){
    FILE *dane = fopen("dane.txt","r");
    FILE *a = fopen("a.txt","w+");
    FILE *b = fopen("b.txt","w+");
    FILE *c = fopen("c.txt","w+");
    if(a == NULL || c == NULL || b == NULL || dane == NULL){
        printf("Blad odczytu pliku\n");
        return;
    }
    char buffer;
    int even = 0;

    while(!feof(dane)){
        int number = -1;
        int count = fread(&buffer, sizeof(char),1,dane);
        if(count != 0){ //jezeli jest jakas kolejna liczba to wczytuj
            number = 0;
        }
        while(buffer != '\n' && count != 0 ){
            number *=10;
            number += (buffer - '0');
            count = fread(&buffer, sizeof(char),1,dane);
        }
        
        if(number > -1){
            char tmp_arr[16];
            sprintf(tmp_arr, "%d", number);
            if(number%2 == 0) even++; //zliczmy ilosc parzystych liczb

            //najpierw sprawdzamy czy jest kwadratem zeby potem moc podzielic number przez 10
            if(is_square(number)){
                fwrite(tmp_arr,sizeof(char),strlen(tmp_arr),c);
                fwrite("\n",sizeof(char), 1, c);
            }
            if(number <= 9 || (number % 100) / 10 == 7 || (number % 100) / 10 == 0 ){
                fwrite(tmp_arr,sizeof(char),strlen(tmp_arr),b);
                fwrite("\n",sizeof(char), 1, b);
            }

        }
        
    }
    char tmp[10];
    fwrite("Liczb parzystych jest:", 1, 22, a); 
    sprintf(tmp, "%d", even);
    fwrite(tmp,sizeof(char),strlen(tmp),a);

    fclose(dane);
    fclose(a);
    fclose(b);
    fclose(c);
}

int main(){
    FILE *raport = fopen("pomiar_zad_3.txt","w+");
    clock_t operation_time[3];
    
    operation_time[0] = clock();
    function3sys();
    operation_time[1] = clock();
    function3lib();
    operation_time[2] = clock();

    fprintf(raport,"Czas mergowania: \n");
    fprintf(raport,"sys   %lf\n",(double) (operation_time[1] - operation_time[0]) / CLOCKS_PER_SEC );
    fprintf(raport,"lib   %lf\n",(double) (operation_time[2] - operation_time[1]) / CLOCKS_PER_SEC );
    fclose(raport);
}


