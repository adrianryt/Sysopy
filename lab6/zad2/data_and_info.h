#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_CLIENTS  15
#define MAX_MESSAGE_LEN 500
#define MAX_MESSAGES 10
#define HOME getenv("HOME")
#define PROJ 0x099
#define SERVER_ATTR_NAME "/server_queue"
#define MESSAGE_SIZE sizeof(message)
#define INIT 1
#define STOP 2
#define DISCONNECT 3
#define LIST 4
#define CONNECT 5
#define MSG 6


typedef struct client
{
    int client_queue;
    int client_connected_pair;
    int is_active;
    int is_avaible;
    char client_queue_name[20];
}client;

typedef struct message
{
    int msgType;
    pid_t sender_pid;
    char message[MAX_MESSAGE_LEN];
}message;


void sigint_handler(int sig);
