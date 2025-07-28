// common.h
#ifndef COMMON_H
#define COMMON_H

#define MAX_MSG_SIZE 495
#define MAX_ID_SIZE 11  // 10 chars + '\0'

// Enumeração que define os códigos das mensagens utilizadas na comunicação entre cliente e servidor
typedef enum {
    REQ_CONNPEER = 20,       // Requisição de conexão entre peers (servidores)
    RES_CONNPEER = 21,       // Resposta de conexão entre peers
    REQ_DISCPEER = 22,       // Requisição de desconexão entre peers
    REQ_CONNSEN  = 23,       // Requisição de conexão de um sensor
    RES_CONNSEN  = 24,       // Resposta de conexão de um sensor
    REQ_DISCSEN  = 25,       // Requisição de desconexão de um sensor
    REQ_CHECKALERT = 36,     // Requisição para verificar um alerta
    RES_CHECKALERT = 37,     // Resposta para a verificação de alerta
    REQ_SENSLOC = 38,        // Requisição da localização de um sensor
    RES_SENSLOC = 39,        // Resposta com a localização de um sensor
    REQ_SENSSTATUS = 40,     // Requisição do status (risco) de um sensor
    RES_SENSSTATUS = 41,     // Resposta com o status (risco) de um sensor
    REQ_LOCLIST = 42,        // Requisição da lista de sensores em uma localização
    RES_LOCLIST = 43,        // Resposta com a lista de sensores em uma localização
    ERROR = 255,             // Código de erro genérico
    OK = 0,                  // Código de sucesso genérico

    REQ_LISTSTATUS_SERVER = 60, // Requisição da lista de status de sensores (entre servidores)
    RES_LISTSTATUS_SERVER = 61, // Resposta da lista de status de sensores (entre servidores)
    REQ_LISTLOC_SERVER = 62,    // Requisição da lista de localização de sensores (entre servidores)
    RES_LISTLOC_SERVER = 63,    // Resposta da lista de localização de sensores (entre servidores)
    REQ_RISKSENS_SERVER = 64,   // Requisição do risco de um sensor específico (entre servidores)
    RES_RISKSENS_SERVER = 65,   // Resposta com o risco de um sensor específico (entre servidores)
    REQ_LOCSENS_SERVER = 66,    // Requisição da localização de um sensor específico (entre servidores)
    RES_LOCSENS_SERVER = 67,    // Resposta com a localização de um sensor específico (entre servidores)
    REQ_UPDRISK = 68,           // Requisição para atualizar o risco de um sensor
    REQ_LOCUPDATE = 69,         // Requisição para atualizar a localização de um sensor
    RES_LOCUPDATE = 70,         // Resposta da atualização de localização de um sensor
    NOTIFY_DISCSERVER = 71,     // Notificação de desconexão do servidor
    NOTIFY_FAILURE = 72,        // Notificação de falha (ex: falha elétrica)
    REQ_MULTICAST_FAILURE = 73, // Requisição de multicast de falha para sensores em uma área
    REQ_RISKSENS_SENSOR = 74,   // Requisição do risco de um sensor (cliente para servidor)
    RES_RISKSENS_SENSOR = 75,   // Resposta com o risco de um sensor (servidor para cliente)
    REQ_LISTSTATUS_SENSOR = 76, // Requisição da lista de status de sensores (cliente para servidor)
    RES_LISTSTATUS_SENSOR = 77, // Resposta da lista de status de sensores (servidor para cliente)
    REQ_LISTLOC_SENSOR = 78,    // Requisição da lista de localização de sensores (cliente para servidor)
    RES_LISTLOC_SENSOR = 79,    // Resposta da lista de localização de sensores (servidor para cliente)
    REQ_REPORTLOC_SENSOR = 80,  // Requisição para reportar a localização de um sensor (cliente para servidor)
    RES_REPORTLOC_SENSOR = 81,  // Resposta com a localização de um sensor (servidor para cliente)
    REQ_REPORTRISK_SENSOR = 82, // Requisição para reportar o risco de um sensor (cliente para servidor)
    RES_REPORTRISK_SENSOR = 83  // Resposta com o risco de um sensor (servidor para cliente)

} MsgCode;

// Estrutura que representa uma mensagem genérica, contendo um código e um payload
typedef struct {
    MsgCode code;
    char payload[MAX_MSG_SIZE];
} Message;

// Funções são responsáveis por enviar e receber mensagens através de um socket
int send_message(int sockfd, Message *msg);
int recv_message(int sockfd, Message *msg);

#endif