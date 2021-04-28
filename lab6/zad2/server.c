#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include "data_and_info.h"


client clients[MAX_CLIENTS];
int server_queue = -1;

void send_msg(struct message *msg, int idx){
    //printf("client_queue: %d\n",clients[idx].client_queue);
    if(mq_send(clients[idx].client_queue,(char *) msg, MAX_MESSAGE_LEN, 1) == -1){
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
            if(mq_close(clients[i].client_queue) == -1){
                printf("Cant close peer_queue: %d %d\n", clients[i].client_queue, errno);
            }
        }
    }  
    if (mq_unlink(SERVER_ATTR_NAME) == -1){
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

    //int client_queue_key;
    //sscanf(msg->message,"%d",&client_queue_key);
    int client_queue_id; //= msgget(client_queue_key, 0);
    if ((client_queue_id = mq_open(msg->message, O_WRONLY)) == -1){
        printf("Cant open client queue \n");
    }

    clients[idx].client_queue = client_queue_id;
    clients[idx].is_active = 1;
    clients[idx].is_avaible = 1;
    strcpy(clients[idx].client_queue_name,msg->message);
    
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

    printf("Client %d - %d and %d - %d are connected\n", client_id,clients[client_id].client_queue, client2_id, clients[client2_id].client_queue);
    char msg_with_queue[MAX_MESSAGE_LEN];

    strcpy(msg_with_queue,"");
    message msg;
    msg.msgType = CONNECT;
    msg.sender_pid = client2_id;
    sprintf(msg_with_queue, "%s", clients[client2_id].client_queue_name);
    sprintf(msg.message, "%s", msg_with_queue);
    printf("%s \n",clients[client2_id].client_queue_name);
    send_msg(&msg, client_id);

    strcpy(msg_with_queue,"");
    message msg2;
    msg2.msgType = CONNECT;
    msg2.sender_pid = client_id;
    sprintf(msg_with_queue, "%s", clients[client_id].client_queue_name);
    sprintf(msg2.message, "%s", msg_with_queue);
    printf("%s \n",clients[client_id].client_queue_name);
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
    if(mq_close(clients[client_id].client_queue) == -1){
            printf("Cant close client_queue: %d %d\n", clients[client_id].client_queue, errno);
    }
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

    struct mq_attr s_attr;
    s_attr.mq_msgsize = MAX_MESSAGE_LEN;
    s_attr.mq_maxmsg = MAX_MESSAGES;

    if((server_queue = mq_open(SERVER_ATTR_NAME, O_RDONLY | O_CREAT, 0666, &s_attr)) == -1){
        printf("SERVER: mq_open returned -1 \n %d", errno);
        exit(1);
    }
    printf("Server: server_queue: %d\n", server_queue);
    fflush(stdout);
    struct message buffer;
    while(1){
        if(mq_receive(server_queue, (char*) &buffer, MAX_MESSAGE_LEN ,NULL) == -1){ 
            printf("SERVER: MSGRCV returned -1 \n");
            exit(1);
        }
        make_action(&buffer);
    }
}
