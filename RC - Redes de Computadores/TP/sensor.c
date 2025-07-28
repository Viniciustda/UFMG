#include <stdio.h>      // Funções de entrada/saída padrão
#include <stdlib.h>     // Funções de propósito geral 
#include <string.h>     // Funções de manipulação de strings 
#include <unistd.h>     // Funções POSIX (close, sleep)
#include <arpa/inet.h>  // Funções de rede (htons, inet_pton)
#include <pthread.h>    // Biblioteca de threads POSIX

#include "common.h"     // Arquivo com as definições de mensagens e comunicação

// Struct para manter informações de contexto de cada servidor (SS/SL)
typedef struct {
    int sockfd;
    char label[4];         // "SS" ou "SL"
} ServerContext;

// Estrutura para armazenar informações de localização de um sensor (ID e Localização)
typedef struct {
    char id[MAX_ID_SIZE];
    int loc;
} SensorLoc;

// Estrutura para armazenar informações de risco de um sensor (ID e Risco)
typedef struct {
    char id[MAX_ID_SIZE];
    int risk;
} SensorRisk;

char my_id[MAX_ID_SIZE] = "";   // ID do sensor atribuído (compartilhado entre threads)
int sock_sl;                    // Socket para o servidor SL
int sock_ss;                    // Socket para o servidor SS
int last_requested_loc_id = -1; // Último ID de localização solicitado pelo cliente
int status = 0;                 // Status do sensor (0: seguro, 1: alerta)

char listloc_payload[MAX_MSG_SIZE] = "";      // Buffer para armazenar o payload da lista de localização
char liststatus_payload[MAX_MSG_SIZE] = "";   // Buffer para armazenar o payload da lista de status
int received_listloc = 0;                     // Flag que indica se a lista de localização foi recebida
int received_liststatus = 0;                  // Flag que indica se a lista de status foi recebida

char report_id[MAX_ID_SIZE] = ""; // ID do sensor para o qual um relatório foi solicitado
int report_loc = -1;              // Localização do sensor a ser reportada
int report_risk = -1;             // Risco do sensor a ser reportado
int got_report_loc = 0;           // Flag que indica se a localização para o relatório foi recebida
int got_report_risk = 0;          // Flag que indica se o risco para o relatório foi recebido
int got_create_SL = 0;           // Flag que indica se o sensor foi criado no SL
int got_create_SS = 0;          // Flag que indica se o sensor foi criado no SS

pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER; // Mutex para proteger acesso às listas
pthread_mutex_t create_lock = PTHREAD_MUTEX_INITIALIZER; // Mutex para proteger acesso à criação de sensores

// Conecta ao servidor dado IP e porta
int connect_to_server(const char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    return sock;
}

// Imprime a lista de sensores conectados
void print_combined_sensor_list(const char *loc_payload, const char *status_payload) {
    SensorLoc locs[50];
    SensorRisk risks[50];
    int loc_count = 0, risk_count = 0;

    char buffer1[MAX_MSG_SIZE], buffer2[MAX_MSG_SIZE];
    strncpy(buffer1, loc_payload, MAX_MSG_SIZE);
    strncpy(buffer2, status_payload, MAX_MSG_SIZE);

    char *token = strtok(buffer1, ",");
    while (token && loc_count < 50) {
        sscanf(token, "%s %d", locs[loc_count].id, &locs[loc_count].loc);
        loc_count++;
        token = strtok(NULL, ",");
    }

    token = strtok(buffer2, ",");
    while (token && risk_count < 50) {
        sscanf(token, "%s %d", risks[risk_count].id, &risks[risk_count].risk);
        risk_count++;
        token = strtok(NULL, ",");
    }

    printf("=== Connected Sensors ===\n");

    const char *areas[] = {"North", "South", "East", "West"};
    int ranges[4][2] = {{1, 3}, {4, 5}, {6, 7}, {8, 10}};

    for (int a = 0; a < 4; a++) {
        printf("[%s]\n", areas[a]);
        int printed = 0;

        for (int i = 0; i < loc_count; i++) {
            int loc = locs[i].loc;
            if (loc >= ranges[a][0] && loc <= ranges[a][1]) {
                int risk = -1;
                for (int j = 0; j < risk_count; j++) {
                    if (strcmp(locs[i].id, risks[j].id) == 0) {
                        risk = risks[j].risk;
                        break;
                    }
                }

                if (risk != -1) {
                    printf("ID: %s | Loc: %d | Risk: %d\n", locs[i].id, loc, risk);
                    printed = 1;
                }
            }
        }

        if (!printed) printf("(no sensors in this area)\n");
        printf("\n");
    }

    printf("===========================\n");
}

// Thread que escuta as respostas dos servidores (SS e SL)
void *handle_server_response(void *arg) {
    ServerContext *ctx = (ServerContext *)arg;
    Message msg;

    // Loop principal: continua recebendo mensagens enquanto a conexão estiver ativa
    while (recv_message(ctx->sockfd, &msg) == 0) {

        // Se a mensagem for uma resposta de conexão de sensor
        if (msg.code == RES_CONNSEN) {
            printf("[CLIENT] %s New ID: %s\n", ctx->label, msg.payload);
            strncpy(my_id, msg.payload, MAX_ID_SIZE); // Salva o ID global
        }

        // Se a mensagem for uma resposta de status de sensor
        else if (msg.code == RES_SENSSTATUS) {
            int loc = atoi(msg.payload);
            const char *region = (loc >= 1 && loc <= 3) ? "Norte" :
                                 (loc >= 4 && loc <= 5) ? "Sul" :
                                 (loc >= 6 && loc <= 7) ? "Leste" :
                                 (loc >= 8 && loc <= 10) ? "Oeste" : "Desconhecida";
            printf("[CLIENT] Alert received from location: %d (%s)\n", loc, region);
        } 
        
        // Se a mensagem for uma resposta de localização de sensor
        else if (msg.code == RES_SENSLOC) {
            printf("[CLIENT] Current sensor location: %s\n", msg.payload);
        }

        // Se a mensagem for uma resposta de lista de localização (diagnóstico)
        else if (msg.code == RES_LOCLIST) {
            extern int last_requested_loc_id;
            printf("[CLIENT] Sensors at location %d: %s\n", last_requested_loc_id, msg.payload);
        }
        
        // Se a mensagem for um erro de limite de sensor excedido
        else if (msg.code == ERROR && strcmp(msg.payload, "09") == 0) {
            printf("[CLIENT] %s ERROR: Sensor limit exceeded\n", ctx->label);
        }

        // Se a mensagem for um erro de sensor não encontrado
        else if (msg.code == ERROR && strcmp(msg.payload, "10") == 0) {
            printf("[CLIENT] %s ERROR: Sensor not found\n", ctx->label);
        }

        // Se a mensagem for um erro de localização não encontrada
        else if (msg.code == ERROR && strcmp(msg.payload, "11") == 0) {
            printf("[CLIENT] SL ERROR: Location not found\n");
        }

        // Se a mensagem for uma confirmação de sucesso de desconexão
        else if (msg.code == OK && strcmp(msg.payload, "01") == 0) {
            printf("[CLIENT] %s Successful disconnect\n", ctx->label);
            close(ctx->sockfd);
            exit(0); // Encerra o cliente ao receber confirmação dos dois lados
        }

        else if (msg.code == OK && strcmp(msg.payload, "02") == 0) {
            pthread_mutex_lock(&create_lock);
            if (strcmp(ctx->label, "SS") == 0) {
                got_create_SS = 1; // Marca que o SS criou o sensor
            } else if (strcmp(ctx->label, "SL") == 0) {
                got_create_SL = 1; // Marca que o SL criou o sensor
            }
            if (got_create_SL && got_create_SS) {
                printf("[CLIENT] Successful create\n");
            } 
            pthread_mutex_unlock(&create_lock);
        }

        // Se a mensagem for uma confirmação do Check failure
        else if (msg.code == OK && strcmp(msg.payload, "03") == 0) {
            printf("[CLIENT] Status do sensor 0\n");
        }
       
        // Se a mensagem for uma notificação de desconexão de servidor
        else if (msg.code == NOTIFY_DISCSERVER) {
            printf("[CLIENT] Server %s disconnected. Shutting down...\n", ctx->label);
            printf("[CLIENT] %s Successful disconnect\n", ctx->label);
            // Se o SS desconectou, pede para desconectar de SL
            if (strcmp(ctx->label, "SS") == 0) {
                close(sock_ss);
                Message mensage = {REQ_DISCSEN, ""};
                snprintf(mensage.payload, sizeof(mensage.payload), "%s", my_id);
                send_message(sock_sl, &mensage);

            } 
            // Se o SL desconectou, pede para desconectar de SS
            else if (strcmp(ctx->label, "SL") == 0) {
                close(sock_sl);
                Message mensage = {REQ_DISCSEN, ""};
                snprintf(mensage.payload, sizeof(mensage.payload), "%s", my_id);
                send_message(sock_ss, &mensage);
            }
            
        }

        // Se a mensagem for uma notificação de falha (pane ou estado seguro)
        else if (msg.code == NOTIFY_FAILURE) {
            printf("[CLIENT] %s\n", msg.payload);
        }

        // Se a mensagem for uma resposta de risco de sensor (status)
        else if (msg.code == RES_RISKSENS_SENSOR) {
            printf("[CLIENT] Current sensor status: %s\n", msg.payload);
        }

        // Se a mensagem for uma resposta da lista de localização para o sensor (list sensors)
        else if (msg.code == RES_LISTLOC_SENSOR) {
            pthread_mutex_lock(&list_lock);
            strncpy(listloc_payload, msg.payload, MAX_MSG_SIZE - 1);
            received_listloc = 1; // marca que SL respondeu

            if (received_liststatus)  // verifica se o SS já respondeu
                print_combined_sensor_list(listloc_payload, liststatus_payload);

            pthread_mutex_unlock(&list_lock);
        }

        // Se a mensagem for uma resposta da lista de status para o sensor (list sensors)
        else if (msg.code == RES_LISTSTATUS_SENSOR) {
            pthread_mutex_lock(&list_lock);
            strncpy(liststatus_payload, msg.payload, MAX_MSG_SIZE - 1);
            received_liststatus = 1; // marca que SS respondeu

            if (received_listloc)  // verifica se o SL já respondeu
                print_combined_sensor_list(listloc_payload, liststatus_payload);

            pthread_mutex_unlock(&list_lock);
        }

        // Se a mensagem for uma resposta de localização para o comando 'report'
        else if (msg.code == RES_REPORTLOC_SENSOR) {
            pthread_mutex_lock(&list_lock);
            report_loc = atoi(msg.payload);
            got_report_loc = 1;
            // Se o risco também já foi recebido, imprime o relatório completo
            if (got_report_risk) {
                printf("ID: %s | Loc: %d | Risk: %d\n", report_id, report_loc, report_risk);
            }
            pthread_mutex_unlock(&list_lock);
        }

        // Se a mensagem for uma resposta de risco para o comando 'report'
        else if (msg.code == RES_REPORTRISK_SENSOR) {
            pthread_mutex_lock(&list_lock);
            report_risk = atoi(msg.payload);
            got_report_risk = 1;
            // Se a localização também já foi recebida, imprime o relatório completo
            if (got_report_loc) {
                printf("ID: %s | Loc: %d | Risk: %d\n", report_id, report_loc, report_risk);
            }
            pthread_mutex_unlock(&list_lock);
        }

        // Se a mensagem tiver um código desconhecido
        else {
            printf("[CLIENT] Unknown message code: %d\n", msg.code);
        }
    }

    return NULL;
}

// Thread que escuta comandos do teclado (stdin)
void *stdin_thread(void *arg) {
    ServerContext **servers = (ServerContext **)arg;
    ServerContext *ss = servers[0];
    ServerContext *sl = servers[1];
    char input[128];

    // Loop principal: continua lendo comandos do stdin
    while (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;

        // Se o comando for "kill" ou "close connection" - desconecta do servidor
        if (strcmp(input, "kill") == 0 || strcmp(input, "close connection") == 0) {

            // Verifica se o sensor já possui um ID atribuído
            if (strlen(my_id) == 0) {
                printf("[CLIENT] Cannot disconnect: ID not yet assigned.\n");
                continue;
            }

            // Se o sensor estiver em estado de alerta (status = 1), reseta o status de risco para safe (status = 0) antes de desconectar
            if (status) {
                status = 0; 
                char risk_val = '0'; 
                Message msg = {REQ_UPDRISK, ""};
                snprintf(msg.payload, sizeof(msg.payload), "%c", risk_val);
                send_message(ss->sockfd, &msg);
                sleep(1); // Aguarda para garantir que o risco foi atualizado
            }

            Message mensage = {REQ_DISCSEN, ""};
            snprintf(mensage.payload, sizeof(mensage.payload), "%s", my_id);

            send_message(ss->sockfd, &mensage); // Envia a requisição de desconexão para o SS
            send_message(sl->sockfd, &mensage); // Envia a requisição de desconexão para o SL
        }

        // Se o comando for "Check failure" ou "check failure" - verifica falha de sensor
        else if (strcmp(input, "Check failure") == 0 || strcmp(input, "check failure") == 0) {
            Message check = {REQ_SENSSTATUS, ""};
            snprintf(check.payload, sizeof(check.payload), "%s", my_id);
            send_message(sock_ss, &check);
        }

        // Se o comando começar com "locate" - localiza um sensor específico
        else if (strncmp(input, "locate ", 7) == 0) {
            char sensor_id[MAX_ID_SIZE];
            strncpy(sensor_id, input + 7, MAX_ID_SIZE - 1);
            sensor_id[MAX_ID_SIZE - 1] = '\0';
        
            Message req = {REQ_SENSLOC, ""};
            snprintf(req.payload, sizeof(req.payload), "%s", sensor_id);
            send_message(sock_sl, &req);
        }

        // Se o comando começar com "diagnose" - diagnostica sensores em uma localização específica
        else if (strncmp(input, "diagnose ", 9) == 0) {
            int loc_id = atoi(input + 9);

            last_requested_loc_id = loc_id;
        
            Message req = {REQ_LOCLIST, ""};
            snprintf(req.payload, sizeof(req.payload), "%s %d", my_id, loc_id);
            send_message(sock_sl, &req);
        } 
        
        // Se o comando for "alert" ou "safe" - atualiza o status de risco do sensor
        else if (strcmp(input, "alert") == 0 || strcmp(input, "safe") == 0) {
            char risk_val = (strcmp(input, "alert") == 0) ? '1' : '0';

            if (risk_val) {
                status = 1; // Alerta   
            }
            else {
                status = 0; // Seguro
            }
        
            Message msg = {REQ_UPDRISK, ""};
            snprintf(msg.payload, sizeof(msg.payload), "%c", risk_val);
            send_message(ss->sockfd, &msg);
        }        

        // Se o comando começar com "status" - consulta o status de risco de um sensor específico
        else if (strncmp(input, "status ", 7) == 0) {
            char sensor_id[MAX_ID_SIZE];
            strncpy(sensor_id, input + 7, MAX_ID_SIZE - 1);
            sensor_id[MAX_ID_SIZE - 1] = '\0';
        
            Message req = {REQ_RISKSENS_SENSOR, ""};
            snprintf(req.payload, sizeof(req.payload), "%s", sensor_id);
            send_message(sock_ss, &req);
        }

        // Se o comando for "list sensors" - lista todos os sensores conectados
        else if (strcmp(input, "list sensors") == 0) {
            pthread_mutex_lock(&list_lock);
            received_listloc = 0;
            received_liststatus = 0;
            listloc_payload[0] = '\0';
            liststatus_payload[0] = '\0';
            pthread_mutex_unlock(&list_lock);

            Message req1 = {REQ_LISTLOC_SENSOR, ""};
            Message req2 = {REQ_LISTSTATUS_SENSOR, ""};
            send_message(sock_sl, &req1);
            send_message(sock_ss, &req2);
        }

        // Se o comando começar com "report" - reporta localização e risco de um sensor específico
        else if (strncmp(input, "report ", 7) == 0) {
            char sensor_id[MAX_ID_SIZE];
            sscanf(input + 7, "%s", sensor_id);

            pthread_mutex_lock(&list_lock);
            strncpy(report_id, sensor_id, MAX_ID_SIZE);
            report_loc = -1;
            report_risk = -1;
            got_report_loc = 0;
            got_report_risk = 0;
            pthread_mutex_unlock(&list_lock);

            Message m1 = {REQ_REPORTLOC_SENSOR, ""};
            Message m2 = {REQ_REPORTRISK_SENSOR, ""};
            strncpy(m1.payload, sensor_id, sizeof(m1.payload));
            strncpy(m2.payload, sensor_id, sizeof(m2.payload));
            send_message(sock_sl, &m1);
            send_message(sock_ss, &m2);
        }

        // Se o comando for "help" - exibe a lista de comandos disponíveis
        else if (strcmp(input, "help") == 0) {
            printf("[CLIENT] Available commands:\n");
            printf(" - close connection / kill: Close connection with servers\n");
            printf(" - Check failure: Checks for sensor failures\n");
            printf(" - diagnose <loc_id>: Diagnoses sensors at a location\n");
            printf(" - locate <sensor_id>: Locates a specific sensor\n");
            printf(" - status <sensor_id>: Query the risk status of a specific sensor\n");
            printf(" - report <sensor_id>: Reports location and risk of a specific sensor\n");
            printf(" - list sensors: Lists all connected sensors, with their location and risk status\n");
            printf(" - alert: Sets sensor status to alert (risk 1)\n");
            printf(" - safe: Sets the sensor status to safe (risk 0)\n");
        }

        // Se o comando for desconhecido
        else {
            printf("[CLIENT] Unknown command: %s\n", input);
        }
    }

    return NULL;
}

// ---------- Função principal ----------

int main(int argc, char *argv[]) {

    // Verifica o número de argumentos da linha de comandos
    if (argc < 3 || argc > 5) {
        printf("Use: %s <ip> <port_1> [port_2] [LocId]\n", argv[0]);
        return 1;
    }

    // Variaveis
    const char *ip = argv[1];
    int port_1 = atoi(argv[2]);
    int port_2 = -1;
    int port_sl = -1;
    int port_ss = -1;
    int loc_id = -1;


    // Se 3 argumentos: IP, porta_1
    if (argc == 3) {
        if (port_1 == 60000 ) {
            port_2 = 61000;
        } 
        else if (port_1 == 61000) {
            port_2 = 60000;
        } else {
            printf("[CLIENT] Error: Port must be 60000 (SL) or 61000 (SS)\n");
            return 1;
        }
    } 
    // Se 4 argumentos: pode ser IP, porta_1, porta_2 OU IP, porta_1, LocId
    else if (argc == 4) {
        int value = atoi(argv[3]);
        if (value == 60000 || value == 61000) {
            port_2 = value;
        }
        else if (value >= 1 && value <= 10) {
            loc_id = value;
            if (port_1 == 60000 ) {
                port_2 = 61000;
            } 
            else if (port_1 == 61000) {
                port_2 = 60000;
            } else {
                printf("[CLIENT] Error: Port must be 60000 (SL) or 61000 (SS)\n");
                return 1;
            }
        }
        else {
            printf("[CLIENT] Error: Invalid argument: %d\n", value);
            return 1;
        }
    }
    // Se 5 argumentos: IP, porta_1, porta_2, LocId
    else if (argc == 5) {
        port_2 = atoi(argv[3]);
        loc_id = atoi(argv[4]);
    }

    // Validação e atribuição final das portas SL e SS
    if (port_1 == 60000 && port_2 == 61000) {
        port_sl = port_1;
        port_ss = port_2;
    }
    else if (port_1 == 61000 && port_2 == 60000) {
        port_sl = port_2;
        port_ss = port_1;
    }
    else {
        printf("[CLIENT] Error: Port must be 60000 (SL) or 61000 (SS)\n");
        return 1;
    }

    // Se a localização não foi fornecida ou é inválida, geramos uma aleatória
    if (loc_id == -1) {
        srand(time(NULL));
        loc_id = (rand() % 10) + 1;
        // printf("[CLIENT] Random location assigned: %d\n", loc_id);
    }
    else if (loc_id < 1 || loc_id > 10) {
        printf("[CLIENT] Error: Location must be between 1 and 10\n");
        srand(time(NULL));
        loc_id = (rand() % 10) + 1;
        printf("[CLIENT] Random location assigned: %d\n", loc_id);
    }

    // Conecta aos dois servidores
    sock_ss = connect_to_server(ip, port_ss);
    sock_sl = connect_to_server(ip, port_sl);

    // Envia REQ_CONNSEN com a localização
    pthread_mutex_lock(&create_lock);
    got_create_SL = 0;
    got_create_SS = 0;
    pthread_mutex_unlock(&create_lock);
    Message req = {REQ_CONNSEN, ""};
    snprintf(req.payload, sizeof(req.payload), "%d", loc_id);
    send_message(sock_ss, &req);
    send_message(sock_sl, &req);

    // Contexto para SS
    ServerContext *ctx_ss = malloc(sizeof(ServerContext));
    ctx_ss->sockfd = sock_ss;
    strncpy(ctx_ss->label, "SS", sizeof(ctx_ss->label));

    // Contexto para SL
    ServerContext *ctx_sl = malloc(sizeof(ServerContext));
    ctx_sl->sockfd = sock_sl;
    strncpy(ctx_sl->label, "SL", sizeof(ctx_sl->label));

    // Thread para escutar resposta do SS
    pthread_t thread_ss;
    pthread_create(&thread_ss, NULL, handle_server_response, ctx_ss);
    pthread_detach(thread_ss);

    // Thread para escutar resposta do SL
    pthread_t thread_sl;
    pthread_create(&thread_sl, NULL, handle_server_response, ctx_sl);
    pthread_detach(thread_sl);

    // Cria thread para entrada de teclado
    ServerContext *servers[2] = {ctx_ss, ctx_sl};
    pthread_t stdin_tid;
    pthread_create(&stdin_tid, NULL, stdin_thread, servers);
    pthread_detach(stdin_tid);

    // Mantém o processo do cliente vivo
    while (1) sleep(1);

    return 0;
}