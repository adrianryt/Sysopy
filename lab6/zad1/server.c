#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "data_and_info.h"


client clients[MAX_CLIENTS];
int server_queue = -1;

void send_msg(struct message *msg, int idx){
    //printf("client_queue: %d\n",clients[idx].client_queue);
    if(msgsnd(clients[idx].client_queue, msg, sizeof(struct message), 0) == -1){
        printf("Cant send message to client: %d\n", idx);
        printf("%d", clients[idx].client_queue);
        fflush(stdout);
    }
}

void stop_and_send(int client_id){
    message msg_to_client;
    sprintf(msg_to_client.message, "%s", "");
    msg_to_client.sender_pid = getpid();
    msg_to_client.msgType = STOP;
    send_msg(&msg_to_client,client_id);
}

void close_server(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].client_queue != -1){
            stop_and_send(i);
        }
    }  
    if (msgctl(server_queue, IPC_RMID, NULL) == -1){
        printf("CANT DELETE SERVER QUEUE\n");
        exit(1);
    }
    else{
        printf("DELETED SERVER QUEUE\n");
        exit(0);
    } 
}

void init_client(struct message *msg){
    int idx = MAX_CLIENTS;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].client_queue == -1){
            idx = i;
            break;
        }
    }
    if(idx == MAX_CLIENTS){
        printf("TOO MANY CLIENTS");
        exit(0);
        
    }

    int client_queue_key;
    sscanf(msg->message,"%d",&client_queue_key);
    int client_queue_id = msgget(client_queue_key, 0);
    clients[idx].client_queue = client_queue_id;
    clients[idx].is_active = 1;
    clients[idx].is_avaible = 1;
    //printf("%d \n", client_queue_id);
    printf("SERVER: Added new client with index: %d\n", idx);
    fflush(stdout);
    // time to send info to client

    message msg_to_client;
    sprintf(msg_to_client.message, "%d", idx);
    msg_to_client.sender_pid = getpid();
    msg_to_client.msgType = INIT;
    send_msg(&msg_to_client,idx);
}

void list(int id){
    printf("SERVER: Listing active clients for client %d\n", id);
    char msg_to_client[MAX_MESSAGE_LEN];
    char tmp[MAX_MESSAGE_LEN];
    strcpy(msg_to_client, "");
    strcat(msg_to_client,"SERVER: ACTIVE LIST: \n");
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].client_queue >= 0){
            strcpy(tmp,"");
            sprintf(tmp,"ID %d  :  AVAILABLE ", i);
            if(clients[i].is_avaible == 1)
                strcat(tmp,"YES\n");
            else
                strcat(tmp,"NO\n");
            strcat(msg_to_client,tmp);
        }
    }
    message msg;
    sprintf(msg.message, "%s", msg_to_client);
    msg.sender_pid = getpid();
    msg.msgType = LIST;
    send_msg(&msg,id);
}

void connect(int client_id, char* peer_to_client){
    int client2_id = atoi(peer_to_client);
    if(client2_id >= MAX_CLIENTS || client2_id < 0 || client2_id == client_id || clients[client2_id].is_avaible == 0){
        printf("Cant connect with this client\n");
        return;
    }
    clients[client_id].is_avaible = 0;
    clients[client2_id].is_avaible = 0;
    clients[client_id].client_connected_pair = client2_id;
    clients[client2_id].client_connected_pair = client_id;

    printf("Clients %d and %d are connected\n", client_id, client2_id);

    char msg_with_queue[MAX_MESSAGE_LEN];

    strcpy(msg_with_queue,"");
    message msg;
    msg.msgType = CONNECT;
    msg.sender_pid = client2_id;
    sprintf(msg_with_queue, "%d", clients[client2_id].client_queue);
    sprintf(msg.message, "%s", msg_with_queue);
    send_msg(&msg, client_id);

    strcpy(msg_with_queue,"");
    message msg2;
    msg2.msgType = CONNECT;
    msg2.sender_pid = client_id;
    sprintf(msg_with_queue, "%d", clients[client_id].client_queue);
    sprintf(msg2.message, "%s", msg_with_queue);
    send_msg(&msg2, client2_id);

}

void disconnect(int client_id){
    printf("Clients %d and %d disconnected their chat\n",client_id,clients[client_id].client_connected_pair);
    message msg;
    msg.sender_pid = client_id;
    msg.msgType = DISCONNECT;
    sprintf(msg.message,"%s", "disc_mess");
    send_msg(&msg,clients[client_id].client_connected_pair);


    clients[client_id].is_avaible = clients[clients[client_id].client_connected_pair].is_avaible = 1;
    clients[client_id].client_connected_pair = clients[clients[client_id].client_connected_pair].client_connected_pair = -1;
}

void stop(int client_id){
    printf("Client %d left server\n", client_id);
    clients[client_id].client_connected_pair = clients[client_id].client_queue = -1;
    clients[client_id].is_active = clients[client_id].is_avaible = 0;
}

void make_action(struct message *msg){
    switch (msg->msgType)
    {
        case INIT:
            init_client(msg);
            break;
        case CONNECT:
            connect(msg->sender_pid, msg->message);
            break;
        case LIST:
            list(msg->sender_pid);
            break;
        case DISCONNECT:
            disconnect(msg->sender_pid);
            break;
        case STOP:
            stop(msg->sender_pid);
            break;
    }
    
}

int main(){
    if (atexit(close_server) == -1){
        printf("SERVER: atexit returned -1");
        return 1;
    }
    

    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].is_active = clients[i].is_avaible = 0;
        clients[i].client_queue = clients[i].client_connected_pair = -1;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR){
        printf("SERVER: signal function returned -1");
        return 1;
    }

    char* path = HOME;

    key_t server_key = ftok(path, PROJ);
    if(server_key == -1){
        printf("SERVER: ftok returned -1");
        exit(0);
    }

    if((server_queue = msgget(server_key, 0666)) > -1){
        msgctl(server_queue, IPC_RMID, NULL);
    }

    server_queue = msgget(server_key, IPC_CREAT | IPC_EXCL | 0666);
    if(server_queue == -1){
        printf("SERVER: MSGGET returned -1 \n");
        exit(1);
    }
    struct message buffer;
    while(1){
        if(msgrcv(server_queue, &buffer, sizeof(struct message),0,0) == -1){ // mtype na -7 zeby ogarnac priorytet sie totalnie wykrzacza
            printf("SERVER: MSGRCV returned -1 \n");
            exit(1);
        }
        make_action(&buffer);
    }
}