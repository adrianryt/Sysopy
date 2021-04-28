#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include "data_and_info.h"

int server_queue = -1;
int client_queue = -1;
int my_client_id = -1;
int connected = 0;
int peer_queue = -1;


void send_msg(struct message *msg){
    if(msgsnd(server_queue, msg, sizeof(struct message), 0) == -1){
        printf("Cant send message to server: %d\n %d", server_queue, errno);
        fflush(stdout);
    }
}


void init_client(key_t key){
    message msg;
    msg.msgType = INIT;
    msg.sender_pid = getpid();
    //za pierwszym razem wysylamy pid potem juz my_klient_id
    sprintf(msg.message,"%d", key);
    send_msg(&msg);
}

void list(){
    message msg;
    msg.msgType = LIST;
    msg.sender_pid = my_client_id;
    sprintf(msg.message,"%s", "");
    send_msg(&msg);
}

void connect(char* client_id){
    message msg;
    msg.msgType = CONNECT;
    msg.sender_pid = my_client_id;
    sprintf(msg.message,"%s", client_id);
    send_msg(&msg);
}

void disconnect(){
    message msg;
    msg.msgType = DISCONNECT;
    msg.sender_pid = my_client_id;
    sprintf(msg.message,"%s", "");
    send_msg(&msg);
}

void stop(){
    message msg;
    msg.msgType = STOP;
    msg.sender_pid = my_client_id;
    sprintf(msg.message,"%s", "");
    send_msg(&msg);
    printf("Thats all for you\n");
    exit(0);
}

void close_client(){
    message msg;
    msg.msgType = STOP;
    msg.sender_pid = my_client_id;
    sprintf(msg.message,"%s", "");
    send_msg(&msg);
    printf("Thats all for you\n");
    if (msgctl(client_queue, IPC_RMID, NULL) == -1){
        printf("CANT DELETE CLIENT QUEUE\n");
        exit(1);
    }
    else{
        printf("DELETED CLIENT QUEUE\n");
        exit(0);
    } 
}

void execute_command(char *command){
    char* tmp;
    tmp = strtok(command, " ");
    if(strcmp(tmp, "CONNECT") == 0){
        char *tmp2;
        tmp2 = strtok(NULL,"\n");
        connect(tmp2);
    }else{
        tmp = strtok(command, "\n");
        if(strcmp(tmp, "LIST") == 0){
            list();
        }
        else if(strcmp(tmp, "DISCONNECT") == 0){
            printf("You are not connected to anyone\n");
        }
        else if(strcmp(tmp, "STOP") == 0){
            stop();
        }
        else{
            printf("Wrong command\n");
        }
    }
}

void command_handler(struct message *msg){
    switch(msg->msgType){
        case INIT:
            sscanf(msg->message, "%d", &my_client_id);
            break;
        case LIST:
            printf("%s", msg->message);
            break;
        case CONNECT:
            peer_queue = atoi(msg->message);
            connected = 1;
            printf("You are connected with client ID: %d \n", msg->sender_pid);
            break;
        case DISCONNECT:
            peer_queue = -1;
            connected = 0;
            printf("Client %d disconnected chat, you will disconnect aswell\n",msg->sender_pid);
            fflush(stdout);
            break;
        case STOP:
            exit(0);
            break;
    }
}

void send_msg_to_peer(char* mess){
    if(strcmp(mess, "DISCONNECT\n") == 0){
        message msg;
        msg.msgType = DISCONNECT;
        msg.sender_pid = my_client_id;
        strcpy(msg.message, mess);
        if(msgsnd(peer_queue, &msg, sizeof(struct message), 0) == -1){
            printf("Cant send message to client: %d %d\n", peer_queue, errno);
        }
        peer_queue = -1;
        connected = 0;
        disconnect();
    }else{
        message msg;
        msg.msgType = MSG;
        msg.sender_pid = my_client_id;
        strcpy(msg.message, mess);
        if(msgsnd(peer_queue, &msg, sizeof(struct message), 0) == -1){
            printf("Cant send message to client: %d %d\n", peer_queue, errno);
        }
    }
}

int main(){
    
    if (atexit(close_client) == -1){
        printf("CLIENT: atexit returned -1");
        return 1;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR){
        printf("CLIENT: signal function returned -1");
        return 1;
    }

    char* path = HOME;

    key_t server_key = ftok(path, PROJ);
    if(server_key == -1){
        printf("SERVER: ftok returned -1");
        exit(0);
    }

    key_t client_key = ftok(path, getpid());
    if(client_key == -1){
        printf("CLIENT: ftok returned -1");
        exit(0);
    }

    if((server_queue = msgget(server_key, 0)) == -1){
        printf("CLIENT: MSGGET ON SERVER QUEUE RETURNED -1");
        exit(1);
    }

    client_queue = msgget(client_key, IPC_CREAT | IPC_EXCL | 0666);
    if(client_queue == -1){
        printf("SERVER: MSGGET returned -1 \n");
        exit(1);
    }
    init_client(client_key);

    char command[200];
    message msg;
    printf("You can use chat: \n");
    while(1){
        if(msgrcv(client_queue,&msg, sizeof(struct message),0,IPC_NOWAIT) >= 0){
            if(connected == 1){
                if(msg.msgType == DISCONNECT){
                     peer_queue = -1;
                     connected = 0;
                }else{
                    printf("Friend: %s",msg.message);
                }
                
            }else{
                command_handler(&msg);
            }
        }

        fd_set *read_set = malloc(sizeof(fd_set)); //set deskryptorow plikow
        FD_ZERO(read_set); // zeruje set
        FD_SET(STDIN_FILENO, read_set); // dodajemy STDIN_FILENO
        struct timeval *timeout = malloc(sizeof(struct timeval)); // struktura mowiaca ile ma czekac na wczytanie
        timeout->tv_sec = 0;
        timeout->tv_usec = 0; //nie bedzie czekal
        //read line if won't block
        select(STDIN_FILENO + 1, read_set, NULL, NULL, timeout); // STDIN_FILENO + 1 <- tak ma byc XD, desktryptor na ktory czekamy do czytania, NULL,NULL <- inne niepotrzebne deskryptory

        if (FD_ISSET(STDIN_FILENO, read_set)){ // jezeli cos jest to mi to sprobuj wczytac
            if(fgets(command, sizeof(command),stdin) == NULL){
                continue;
            }
            if(connected == 1){
                send_msg_to_peer(command);
            }else{
                execute_command(command);
            }
            
        }

        free(timeout);
        free(read_set);
    }
}   