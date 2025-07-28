// common.c
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

// Função para enviar uma mensagem através de um socket
int send_message(int sockfd, Message *msg) {
    char buffer[MAX_MSG_SIZE + 5];
    snprintf(buffer, sizeof(buffer), "%02d %s\n", msg->code, msg->payload);
    size_t len = strlen(buffer);
    return write(sockfd, buffer, len);
}

// Função para receber uma mensagem de um socket
int recv_message(int sockfd, Message *msg) {
    char buffer[MAX_MSG_SIZE + 5];
    int idx = 0;
    char c;
    while (read(sockfd, &c, 1) > 0) {
        if (c == '\n' || idx >= MAX_MSG_SIZE + 4) break;
        buffer[idx++] = c;
    }
    buffer[idx] = '\0';

    if (idx == 0) return -1; // conexão encerrada

    int code;
    if (sscanf(buffer, "%d %[^\n]", &code, msg->payload) < 1) return -1;

    msg->code = (MsgCode)code;
    return 0;
}