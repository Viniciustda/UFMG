#include <stdio.h>              // Entrada e saída padrão
#include <stdlib.h>             // Funções utilitárias (malloc, free, exit)
#include <string.h>             // Manipulação de strings (strncpy, strcmp)
#include <unistd.h>             // Funções POSIX (close, sleep)
#include <arpa/inet.h>          // Funções de rede (htons, inet_pton)
#include <pthread.h>            // Biblioteca de threads POSIX
#include <errno.h>              // Para verificar erros no accept()

#include "common.h"             // Arquivo com as definições de mensagens e comunicação

#define MAX_SENSORS 15

// Struct auxiliar para passar argumentos para a thread de inicialização do servidor
struct StartArgs {
    char ip[64];
    int peer_port;
    int client_port;
};

// Estrutura para armazenar informações dos sensores
typedef struct {
    char id[MAX_ID_SIZE];   
    int location;           
    int risk;               
    int socket_fd;        
} SensorInfo;

// Estrutura para gerenciar requisições pendentes de alerta
typedef struct {
    char sensor_id[MAX_ID_SIZE];
    int socket_fd;
} PendingRequest;

// Variaveis globais para o servidor
int peer_sock = -1;                     // Socket do peer conectado (SS ou SL)
int listen_fd = -1;                     // Socket usado para escutar conexões de peer
char my_id[MAX_ID_SIZE];                // ID deste servidor
char peer_id[MAX_ID_SIZE];              // ID do outro peer (servidor conectado)
char connected_peer_id[MAX_ID_SIZE];    // ID do peer atualmente conectado
int peer_connected = 0;                 // Flag: indica se há um peer conectado
int peer_port_global = 0;               // Porta para conexão entre peers
int awaiting_peer_confirmation = 0;     // Flag: aguardando confirmação do ID
char ip_global[64];                     // IP do peer alvo para reconexão
volatile int listening = 0;             // Flag: indica se o servidor está escutando (volátil por causa das threads)

pthread_mutex_t peer_lock = PTHREAD_MUTEX_INITIALIZER; // Mutex para proteger variáveis globais do peer

// Variáveis globais para rastrear o risco em cada área
int risk_norte = 0;
int risk_sul = 0;
int risk_leste = 0;
int risk_oeste = 0;

// Variáveis globais para sensores
int client_listen_fd = -1;                  // Socket de escuta para clientes (sensores)
int client_port = 0;                        // Porta de conexão com clientes (sensores)
int sensor_count = 0;                       // Número atual de sensores conectados
int area_sequence[5][11] = {0};             // Matriz para gerar IDs sequenciais de sensores por área e localização, [área 1-4][localização 1-10], índice 0 não usado
int pending_count = 0;                      // Número de requisições de alerta pendentes
SensorInfo sensors[MAX_SENSORS];            // Array para armazenar informações dos sensores conectados
PendingRequest pending_alerts[MAX_SENSORS]; // Array para armazenar requisições de alerta pendentes

pthread_mutex_t pending_lock = PTHREAD_MUTEX_INITIALIZER; // Mutex para proteger a lista de requisições pendentes
pthread_mutex_t sensor_lock = PTHREAD_MUTEX_INITIALIZER;  // Mutex para proteger o acesso aos dados dos sensores

// Funções para servidor
void *threaded_start_server(void *arg);                             // Thread que inicia o servidor
void generate_id_server(char *dest, int is_sl, int port, int seq);  // Gera um ID para o servidor
void print_sensor_list();                                           // Imprime a lista de sensores conectados
void *handle_peer(void *arg);                                       // Lida com a comunicação com o peer (outro servidor)
void *peer_listener(void *arg);                                     // Escuta por conexões de peers
void *stdin_thread(void *arg);                                      // Escuta comandos do teclado        
void start_server(const char *ip, int peer_port, int client_port);  // Função principal de inicialização do servidor

// Funções para sensores
void generate_id_sensor(char *dest, int loc);                       // Gera um ID para o sensor
int register_sensor(int sockfd, int loc_id, char *generated_id);    // Registra um novo sensor
void *handle_client(void *arg);                                     // Lida com a comunicação com um sensor cliente  
void *client_listener(void *arg);                                   // Escuta por conexões de sensores clientes    



// ---------- Funções servidores ----------



// Thread para iniciar o servidor, seja como SL (servidor de local) ou SS (servidor de status)
void *threaded_start_server(void *arg) {
    struct StartArgs *args = (struct StartArgs *)arg;
    start_server(args->ip, args->peer_port, args->client_port);
    free(args);
    return NULL;
}

// Gera o ID do servidor no formato "5<port><seq>" para SL ou "6<port><seq>" para SS
void generate_id_server(char *dest, int is_sl, int port, int seq) {
    snprintf(dest, 16, "%d%05d%04d", is_sl ? 5 : 6, port, seq);
}

// Função para imprimir a lista de sensores
void print_sensor_list() {
    pthread_mutex_lock(&sensor_lock);

    printf("=== Connected Sensors ===\n");

    const char *areas[] = {"North", "South", "East", "West"};
    int ranges[4][2] = {{1, 3}, {4, 5}, {6, 7}, {8, 10}};

    for (int a = 0; a < 4; a++) {
        printf("[%s]\n", areas[a]);
        int printed_in_area = 0; 

        for (int i = 0; i < sensor_count; i++) {
            int loc = sensors[i].location;
            if (loc >= ranges[a][0] && loc <= ranges[a][1]) {
                printf("ID: %s | Loc: %d | Risk: %d\n", sensors[i].id, sensors[i].location, sensors[i].risk);
                printed_in_area = 1;
            }
        }

        if (!printed_in_area) {
            printf("(no sensors in this area)\n");
        }
        printf("\n");
    }

    // Sensores fora da zona ativa (pela forma que foi implementado, não deveria haver sensores fora da zona ativa, mas é bom verificar)
    printf("[Out of active zone]\n");
    int printed_outside_area = 0;
    for (int i = 0; i < sensor_count; i++) {
        int loc = sensors[i].location;
        if (loc == -1) {
            printf("ID: %s | Loc: -1 | Risk: %d\n", sensors[i].id, sensors[i].risk);
            printed_outside_area = 1;
        }
    }

    if (!printed_outside_area) {
        printf("(no sensor out of zone)\n");
    }
    printf("===========================\n");

    pthread_mutex_unlock(&sensor_lock);
}

// Funcao para lidar com a conexão do peer (SS ou SL)
void *handle_peer(void *arg) {
    Message msg;

    // Loop principal: recebe mensagens do socket conectado com o peer
    while (recv_message(peer_sock, &msg) == 0) {

        // Caso o peer envie uma requisição de conexão (este servidor está atuando como SL)
        if (msg.code == REQ_CONNPEER) {
            pthread_mutex_lock(&peer_lock);

            peer_connected = 1; // Marca que há conexão ativa
            static int seq_counter = 1; // Contador para gerar IDs únicos

            // Gera um ID para o peer conectado (este servidor é SL => is_sl = 1)
            int is_sl = 1; 
            if (client_port == 60000) { // Se este for SL
                is_sl = 0; // Peer e SS
            }
            generate_id_server(peer_id, is_sl, peer_port_global, seq_counter++);

            // Armazena esse ID como o ID do peer conectado
            strncpy(connected_peer_id, peer_id, sizeof(connected_peer_id));

            // Envia a resposta de conexão com o ID gerado
            Message res = {RES_CONNPEER, ""};
            snprintf(res.payload, sizeof(res.payload), "%s", peer_id);
            send_message(peer_sock, &res);

            if (client_port == 60000) {
                printf("[SL] Peer %s connected\n", peer_id);
            } else {
                printf("[SS] Peer %s connected\n", peer_id);
            }
            
            // Marca que este servidor aguarda uma confirmação final (o ID do outro lado)
            awaiting_peer_confirmation = 1;

            pthread_mutex_unlock(&peer_lock);
        }

        // Caso receba a resposta de conexão (este servidor é SS)
        else if (msg.code == RES_CONNPEER) {
            pthread_mutex_lock(&peer_lock);

            // Primeiro caso: estava esperando o ID do outro lado
            if (awaiting_peer_confirmation) {
                strncpy(my_id, msg.payload, sizeof(my_id) - 1);
                // Imprime o ID atribuído ao próprio servidor
                if (client_port == 60000) {
                    printf("[SL] New Peer ID: %s\n", my_id);
                } else {
                    printf("[SS] New Peer ID: %s\n", my_id);
                }
                awaiting_peer_confirmation = 0;
            } else {
                // Segundo caso: recebi o ID do outro lado, agora envio meu ID como confirmação
                strncpy(my_id, msg.payload, sizeof(my_id) - 1);

                if (client_port == 60000) {
                    printf("[SL] New Peer ID: %s\n", my_id);
                } else {
                    printf("[SS] New Peer ID: %s\n", my_id);
                }
                static int seq_counter = 1;
                int is_sl = 1;
                if (client_port == 60000) { // Se este for SL
                    is_sl = 0; // Peer e SS
                }
                generate_id_server(peer_id, is_sl, peer_port_global, seq_counter++); 

                Message confirm = {RES_CONNPEER, ""};
                snprintf(confirm.payload, sizeof(confirm.payload), "%s", peer_id);
                send_message(peer_sock, &confirm);

                if (client_port == 60000) {
                    printf("[SL] Peer %s connected\n", peer_id);
                } else {
                    printf("[SS] Peer %s connected\n", peer_id);
                }
                strncpy(connected_peer_id, peer_id, sizeof(connected_peer_id));
                peer_connected = 1;
            }
            pthread_mutex_unlock(&peer_lock);
        }

        // Requisição de desconexão recebida do outro lado
        else if (msg.code == REQ_DISCPEER) {
            pthread_mutex_lock(&peer_lock);

            // Verifica se o ID enviado bate com o peer conectado
            if (strcmp(msg.payload, connected_peer_id) != 0) {
                Message err = {ERROR, "02"}; // Erro: ID não corresponde
                send_message(peer_sock, &err);
                pthread_mutex_unlock(&peer_lock);
                continue;
            }

            // Envia confirmação de desconexão
            Message ok = {OK, "01"};
            send_message(peer_sock, &ok);
            if (client_port == 60000) {
                printf("[SL] Peer %s disconnected\n", connected_peer_id);
            } else {
                printf("[SS] Peer %s disconnected\n", connected_peer_id);
            }

            // Fecha o socket e reseta variáveis
            close(peer_sock);
            peer_sock = -1;
            peer_connected = 0;

            // Se estiver atuando como SL, desativa o socket de escuta
            if (listening && listen_fd != -1) {
                shutdown(listen_fd, SHUT_RDWR); // Desbloqueia accept()
                close(listen_fd);
                listen_fd = -1;
                listening = 0;
            }

            // Reseta variáveis de estado
            awaiting_peer_confirmation = 0;
            connected_peer_id[0] = '\0';
            my_id[0] = '\0';
            peer_id[0] = '\0';

            pthread_mutex_unlock(&peer_lock);

            pthread_mutex_lock(&sensor_lock);
            memset(area_sequence, 0, sizeof(area_sequence)); // Reseta a sequência de áreas
            risk_norte = 0;
            risk_sul = 0;
            risk_leste = 0;
            risk_oeste = 0;
            pthread_mutex_unlock(&sensor_lock);

            // Cria uma nova thread para reiniciar o servidor (retorna ao estado inicial)
            struct StartArgs *args = malloc(sizeof(struct StartArgs));
            strncpy(args->ip, ip_global, sizeof(args->ip));
            args->peer_port = peer_port_global;
            args->client_port = client_port;

            pthread_t restart_tid;
            pthread_create(&restart_tid, NULL, threaded_start_server, args);
            pthread_detach(restart_tid);

            return NULL; // Encerra esta thread de comunicação com peer
        }

        // Requisição para verificar alerta (SS solicita ao SL a localização de um sensor em alerta)
        else if (msg.code == REQ_CHECKALERT) {
            printf("[SL] REQ_CHECKALERT %s\n", msg.payload);
        
            int found = 0;
            int loc = -1;
        
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    loc = sensors[i].location;
                    found = 1;
                    break;
                }
            }
        
            // Se o sensor foi encontrado, envia a localização de voltas
            if (found) {
                Message res = {RES_CHECKALERT, ""};
                snprintf(res.payload, MAX_MSG_SIZE, "%d", loc);
                send_message(peer_sock, &res);
            } else {
                Message err = {ERROR, "10"};
                send_message(peer_sock, &err);
            }
        }            

        // Resposta para REQ_CHECKALERT (SL responde ao SS com a localização de um sensor em alerta)
        else if (msg.code == RES_CHECKALERT) {
            int loc_id = atoi(msg.payload);
        
            pthread_mutex_lock(&pending_lock);
            for (int i = 0; i < pending_count; i++) {
                // Usa o primeiro cliente pendente (FIFO)
                int client_fd = pending_alerts[i].socket_fd;
        
                // Envia RES_SENSSTATUS(LocId) para o cliente correto
                Message res = {RES_SENSSTATUS, ""};
                snprintf(res.payload, MAX_MSG_SIZE, "%d", loc_id);
                send_message(client_fd, &res);
        
                // Remove entrada da lista de pendências
                pending_alerts[i] = pending_alerts[pending_count - 1];
                pending_count--;
                break;
            }
            pthread_mutex_unlock(&pending_lock);
        }               

        // Erro: excedeu o limite de peers permitidos
        else if (msg.code == ERROR && strcmp(msg.payload, "01") == 0) {
            pthread_mutex_lock(&peer_lock);

            if (client_port == 60000) {
                printf("[SL] Peer limit exceeded.\n");
            } else {
                printf("[SS] Peer limit exceeded.\n");
            }
            close(peer_sock);
            peer_sock = -1;
            pthread_mutex_unlock(&peer_lock);
            exit(0);
        }

        // Erro: peer não encontrado
        else if (msg.code == ERROR && strcmp(msg.payload, "02") == 0) {
            pthread_mutex_lock(&peer_lock);

            if (client_port == 60000) {
                printf("[SL] Peer not found.\n");
            } else {
                printf("[SS] Peer not found.\n");
            }

            pthread_mutex_unlock(&peer_lock);
        }

        // Erro: sensor nao encontrado
        else if (msg.code == ERROR && strcmp(msg.payload, "10") == 0) {
            pthread_mutex_lock(&pending_lock);
            for (int i = 0; i < pending_count; i++) {
                int client_fd = pending_alerts[i].socket_fd;
        
                Message err = {ERROR, "10"};
                send_message(client_fd, &err);
        
                // Remove da fila de pendentes
                pending_alerts[i] = pending_alerts[pending_count - 1];
                pending_count--;
                break;
            }
            pthread_mutex_unlock(&pending_lock);
        }
        
        // Confirmação de desconexão recebida (após o envio do comando "close connection" ou "kill")
        else if (msg.code == OK && strcmp(msg.payload, "01") == 0) {
            pthread_mutex_lock(&peer_lock);

            if (client_port == 60000) {
                printf("[SL] Successful disconnect.\n");
                printf("[SL] Peer %s disconnected\n", peer_id);
            } else {
                printf("[SS] Successful disconnect.\n");
                printf("[SS] Peer %s disconnected\n", peer_id);
            }
            // Finaliza a conexão e encerra o processo
            close(peer_sock);
            peer_sock = -1;

            if (listening && listen_fd != -1) {
                shutdown(listen_fd, SHUT_RDWR);
                close(listen_fd);
                listen_fd = -1;
                listening = 0;
            }

            pthread_mutex_unlock(&peer_lock);
            exit(0);
        }

        // Requisição de lista de status de sensores (entre servidores)
        else if (msg.code == REQ_LISTSTATUS_SERVER) {
            pthread_mutex_lock(&sensor_lock);
        
            // Formata resposta com: "ID Risco\n" para cada sensor
            char response[MAX_MSG_SIZE] = "";
            for (int i = 0; i < sensor_count; i++) {
                char line[64];
                snprintf(line, sizeof(line), "%s %d,", sensors[i].id, sensors[i].risk);
                strncat(response, line, sizeof(response) - strlen(response) - 1);
            }
            Message res = {RES_LISTSTATUS_SERVER, ""};
            strncpy(res.payload, response, sizeof(res.payload) - 1);
            send_message(peer_sock, &res);
        
            pthread_mutex_unlock(&sensor_lock);
        }

        // Resposta da lista de status de sensores (entre servidores)
        else if (msg.code == RES_LISTSTATUS_SERVER) {
            pthread_mutex_lock(&sensor_lock);
        
            // Copia do payload
            char buffer[MAX_MSG_SIZE];
            strncpy(buffer, msg.payload, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        
            char *token = strtok(buffer, ",");
            while (token != NULL) {
                char sensor_id[MAX_ID_SIZE];
                int risk;
        
                if (sscanf(token, "%s %d", sensor_id, &risk) == 2) {
                    // Atualiza o risco do sensor correspondente no vetor local
                    for (int i = 0; i < sensor_count; i++) {
                        if (strcmp(sensors[i].id, sensor_id) == 0) {
                            sensors[i].risk = risk;
                            break;
                        }
                    }
                }
        
                token = strtok(NULL, ",");
            }
        
            pthread_mutex_unlock(&sensor_lock);
        
            print_sensor_list(); 
        }        
        
        // Requisição de lista de localização de sensores (entre servidores)        
        else if (msg.code == REQ_LISTLOC_SERVER) {
            pthread_mutex_lock(&sensor_lock);
        
            // Formata resposta com: "ID Loc\n" para cada sensor
            char response[MAX_MSG_SIZE] = "";
            for (int i = 0; i < sensor_count; i++) {
                char line[64];
                snprintf(line, sizeof(line), "%s %d,", sensors[i].id, sensors[i].location);
                strncat(response, line, sizeof(response) - strlen(response) - 1);
            }
        
            Message res = {RES_LISTLOC_SERVER, ""};
            strncpy(res.payload, response, sizeof(res.payload) - 1);
            send_message(peer_sock, &res);
        
            pthread_mutex_unlock(&sensor_lock);
        }

        // Resposta da lista de localização de sensores (entre servidores)
        else if (msg.code == RES_LISTLOC_SERVER) {
            pthread_mutex_lock(&sensor_lock);
        
            // Atualiza localização dos sensores com base no payload recebido
            char buffer[MAX_MSG_SIZE];
            strncpy(buffer, msg.payload, sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';
        
            char *token = strtok(buffer, ",");
            while (token != NULL) {
                char id[MAX_ID_SIZE];
                int loc;
        
                if (sscanf(token, "%s %d", id, &loc) == 2) {
                    for (int i = 0; i < sensor_count; i++) {
                        if (strcmp(sensors[i].id, id) == 0) {
                            sensors[i].location = loc;
                            break;
                        }
                    }
                }
        
                token = strtok(NULL, ",");
            }
        
            pthread_mutex_unlock(&sensor_lock);
        
            print_sensor_list();
        } 
        
        // Requisição de localização de sensor específico (entre servidores)
        else if (msg.code == REQ_SENSLOC) {
            printf("[SL] REQ_SENSLOC %s\n", msg.payload);
        
            pthread_mutex_lock(&sensor_lock);
            int loc_id;
        
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    loc_id = sensors[i].location;
                    break;
                }
            }
        
            Message res = {RES_SENSLOC, ""};
            snprintf(res.payload, MAX_MSG_SIZE, "%d", loc_id);
            send_message(peer_sock, &res);
            
            pthread_mutex_unlock(&sensor_lock);
        }

        // Resposta da localização de sensor específico (entre servidores)
        else if (msg.code == RES_SENSLOC) {
            printf("[SS] Current sensor location: %s\n", msg.payload);
        }
        
        // Requisição do status (risco) de um sensor específico (entre servidores)
        else if (msg.code == REQ_SENSSTATUS) {
            printf("[SS] REQ_SENSSTATUS %s\n", msg.payload);
        
            pthread_mutex_lock(&sensor_lock);
            int status_id;
        
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    status_id = sensors[i].risk;
                    break;
                }
            }
        
            Message res = {RES_SENSSTATUS, ""};
            snprintf(res.payload, MAX_MSG_SIZE, "%d", status_id);
            send_message(peer_sock, &res);
            
            pthread_mutex_unlock(&sensor_lock);
        }

        // Resposta do status (risco) de um sensor específico (entre servidores)
        else if (msg.code == RES_SENSSTATUS) {
            printf("[SL] Current sensor status: %s\n", msg.payload);
        }

        // Requisição de localização de sensor ('report')
        else if (msg.code == REQ_LOCSENS_SERVER) {
            char id[MAX_ID_SIZE];
            int risk;
            sscanf(msg.payload, "%s %d", id, &risk);
        
            pthread_mutex_lock(&sensor_lock);
            int loc = -1;
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, id) == 0) {
                    loc = sensors[i].location;
                    break;
                }
            }
            pthread_mutex_unlock(&sensor_lock);
        
            Message res = {RES_LOCSENS_SERVER, ""};
            snprintf(res.payload, sizeof(res.payload), "%s %d %d", id, loc, risk);
            send_message(peer_sock, &res);
            
        } 

        // Requisição do risco de sensor ('report'))
        else if (msg.code == REQ_RISKSENS_SERVER) {
            char id[MAX_ID_SIZE];
            int loc;
            sscanf(msg.payload, "%s %d", id, &loc);
        
            pthread_mutex_lock(&sensor_lock);
            int risk = -1;
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, id) == 0) {
                    risk = sensors[i].risk;
                    break;
                }
            }
            pthread_mutex_unlock(&sensor_lock);
        
            Message res = {RES_RISKSENS_SERVER, ""};
            snprintf(res.payload, sizeof(res.payload), "%s %d %d", id, loc, risk);
            send_message(peer_sock, &res);
            
        }

        // Resposta de localização/risco (SS/SL recebe para o comando 'report')
        else if (msg.code == RES_LOCSENS_SERVER || msg.code == RES_RISKSENS_SERVER) {
            char id[MAX_ID_SIZE];
            int loc, risk;
            sscanf(msg.payload, "%s %d %d", id, &loc, &risk);
            printf("ID: %s | Loc: %d | Risk: %d\n", id, loc, risk);
        }  
        
        // Requisição de atualização de localização (SS solicita ao SL após um sensor atualizar seu risco)
        else if (msg.code == REQ_LOCUPDATE) {
            pthread_mutex_lock(&sensor_lock);
        
            // Extrai SensorID e risk_val
            char sensor_id[MAX_ID_SIZE];
            int risk_val;
            sscanf(msg.payload, "%s %d", sensor_id, &risk_val);

            char response[MAX_MSG_SIZE] = "";
            snprintf(response, sizeof(response), "%s %d ", sensor_id, risk_val);

            // Adiciona a localização de todos os sensores conectados ao payload da resposta
            for (int i = 0; i < sensor_count; i++) {
                char line[64];
                snprintf(line, sizeof(line), "%s %d,", sensors[i].id, sensors[i].location);
                strncat(response, line, sizeof(response) - strlen(response) - 1);
            }
        
            Message res = {RES_LOCUPDATE, ""};
            strncpy(res.payload, response, sizeof(res.payload) - 1);
            send_message(peer_sock, &res);
        
            pthread_mutex_unlock(&sensor_lock);
        }

        // Resposta de atualização de localização (SL responde ao SS com localizações e o SS atualiza contagens de risco)
        else if (msg.code == RES_LOCUPDATE) {
            char sensor_id[MAX_ID_SIZE];
            int risk_val;

            // Parse do início: ID + novo risco
            char buffer[MAX_MSG_SIZE];
            strncpy(buffer, msg.payload, sizeof(buffer));

            char *token = strtok(buffer, " ");
            strncpy(sensor_id, token, MAX_ID_SIZE);

            token = strtok(NULL, " ");
            risk_val = atoi(token);

            // Prepara estrutura para armazenar todos os pares ID/Loc
            typedef struct {
                char id[MAX_ID_SIZE];
                int loc;
            } SensorLoc;

            SensorLoc all[50];
            int count = 0;

            while ((token = strtok(NULL, ",")) != NULL && count < 50) {
                sscanf(token, "%s %d", all[count].id, &all[count].loc);
                count++;
            }

            // Verifica onde está o sensor que mudou
            int changed_loc = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(all[i].id, sensor_id) == 0) {
                    changed_loc = all[i].loc;
                    break;
                }
            }

            // Atualiza contadores da área
            const char *area = (changed_loc >= 1 && changed_loc <= 3) ? "North" :
                            (changed_loc >= 4 && changed_loc <= 5) ? "South" :
                            (changed_loc >= 6 && changed_loc <= 7) ? "East" :
                            (changed_loc >= 8 && changed_loc <= 10) ? "West" : "Unknown";
            
            pthread_mutex_lock(&sensor_lock);

            char alert_msg[128] = "";

            // Se o risco mudou, atualiza o contador de risco da área correspondente
            if (risk_val == 1) {
                printf("[SS] Alert received from location: %02d (%s)\n", changed_loc, area);
                if (changed_loc >= 1 && changed_loc <= 3 && ++risk_norte == 3) {

                    snprintf(alert_msg, sizeof(alert_msg), "ELECTRICAL FAILURE DETECTED IN THE AREA: NORTH");
                    printf("[SS] %s\n", alert_msg);

                    Message req = {REQ_MULTICAST_FAILURE, ""};
                    snprintf(req.payload, sizeof(req.payload), "%s %s", "NORTH", alert_msg);
                    send_message(peer_sock, &req);
                }

                else if (changed_loc >= 4 && changed_loc <= 5 && ++risk_sul == 3) {

                    snprintf(alert_msg, sizeof(alert_msg), "ELECTRICAL FAILURE DETECTED IN THE AREA: SOUTH");
                    printf("[SS] %s\n", alert_msg);

                    Message req = {REQ_MULTICAST_FAILURE, ""};
                    snprintf(req.payload, sizeof(req.payload), "%s %s", "SOUTH", alert_msg);
                    send_message(peer_sock, &req);
                }

                else if (changed_loc >= 6 && changed_loc <= 7 && ++risk_leste == 3) {

                    snprintf(alert_msg, sizeof(alert_msg), "ELECTRICAL FAILURE DETECTED IN THE AREA: EAST");
                    printf("[SS] %s\n", alert_msg);

                    Message req = {REQ_MULTICAST_FAILURE, ""};
                    snprintf(req.payload, sizeof(req.payload), "%s %s", "EAST", alert_msg);
                    send_message(peer_sock, &req);
                }

                else if (changed_loc >= 8 && changed_loc <= 10 && ++risk_oeste == 3) {

                    snprintf(alert_msg, sizeof(alert_msg), "ELECTRICAL FAILURE DETECTED IN THE AREA: WEST");
                    printf("[SS] %s\n", alert_msg);

                    Message req = {REQ_MULTICAST_FAILURE, ""};
                    snprintf(req.payload, sizeof(req.payload), "%s %s", "WEST", alert_msg);
                    send_message(peer_sock, &req);
                }

            } else {
                if (changed_loc >= 1 && changed_loc <= 3 && --risk_norte == 2) {

                    snprintf(alert_msg, sizeof(alert_msg), "ELECTRICAL FAILURE RESOLVED IN THE AREA: NORTH");
                    printf("[SS] %s\n", alert_msg);

                    Message req = {REQ_MULTICAST_FAILURE, ""};
                    snprintf(req.payload, sizeof(req.payload), "%s %s", "NORTH", alert_msg);
                    send_message(peer_sock, &req);
                }

                else if (changed_loc >= 4 && changed_loc <= 5 && --risk_sul == 2) {

                    snprintf(alert_msg, sizeof(alert_msg), "ELECTRICAL FAILURE RESOLVED IN THE AREA: SOUTH");
                    printf("[SS] %s\n", alert_msg);

                    Message req = {REQ_MULTICAST_FAILURE, ""};
                    snprintf(req.payload, sizeof(req.payload), "%s %s", "SOUTH", alert_msg);
                    send_message(peer_sock, &req);
                }

                else if (changed_loc >= 6 && changed_loc <= 7 && --risk_leste == 2) {

                    snprintf(alert_msg, sizeof(alert_msg), "ELECTRICAL FAILURE RESOLVED IN THE AREA: EAST");
                    printf("[SS] %s\n", alert_msg);

                    Message req = {REQ_MULTICAST_FAILURE, ""};
                    snprintf(req.payload, sizeof(req.payload), "%s %s", "EAST", alert_msg);
                    send_message(peer_sock, &req);
                }
                

                else if (changed_loc >= 8 && changed_loc <= 10 && --risk_oeste == 2) {

                    snprintf(alert_msg, sizeof(alert_msg), "ELECTRICAL FAILURE RESOLVED IN THE AREA: WEST");
                    printf("[SS] %s\n", alert_msg);

                    Message req = {REQ_MULTICAST_FAILURE, ""};
                    snprintf(req.payload, sizeof(req.payload), "%s %s", "WEST", alert_msg);
                    send_message(peer_sock, &req);
                }

            }

            pthread_mutex_unlock(&sensor_lock);
        }

        // Requisição de multicast de pane/estado seguro (SS solicita ao SL para notificar sensores)
        else if (msg.code == REQ_MULTICAST_FAILURE) {
            char area[16], message[128];
            sscanf(msg.payload, "%s %[^\n]", area, message);
        
            printf("[SL] %s\n", message);
        
            pthread_mutex_lock(&sensor_lock);
            for (int i = 0; i < sensor_count; i++) {
                int loc = sensors[i].location;
                int in_area =
                    (strcmp(area, "NORTH") == 0 && loc >= 1 && loc <= 3) ||
                    (strcmp(area, "SOUTH")   == 0 && loc >= 4 && loc <= 5) ||
                    (strcmp(area, "EAST") == 0 && loc >= 6 && loc <= 7) ||
                    (strcmp(area, "WEST") == 0 && loc >= 8 && loc <= 10);
        
                if (in_area) {
                    Message alert = {NOTIFY_FAILURE, ""};
                    strncpy(alert.payload, message, sizeof(alert.payload) - 1);
                    alert.payload[sizeof(alert.payload) - 1] = '\0';
                    send_message(sensors[i].socket_fd, &alert);
                }
            }
            pthread_mutex_unlock(&sensor_lock);
        }
        
    }

    // Se recv_message falhar (por desconexão inesperada)
    pthread_mutex_lock(&peer_lock);

    if (peer_sock != -1) {
        close(peer_sock);
        peer_sock = -1;
        peer_connected = 0;
        printf("Peer connection unexpectedly closed.\n");
    }

    // Se não estiver escutando, reinicia o servidor como SL
    if (!listening && listen_fd == -1) {
        printf("Attempting to restart listener after unexpected peer disconnect.\n");

        struct StartArgs *args = malloc(sizeof(struct StartArgs));
        strncpy(args->ip, ip_global, sizeof(args->ip));
        args->peer_port = peer_port_global;
        args->client_port = client_port;

        pthread_t restart_tid;
        pthread_create(&restart_tid, NULL, threaded_start_server, args);
        pthread_detach(restart_tid);
    }

    pthread_mutex_unlock(&peer_lock);
    return NULL;
}

// Funcao para escutar conexões de peers (SL)
void *peer_listener(void *arg) {
    int peer_port = *(int *)arg;
    free(arg); // Libera a memória alocada dinamicamente para a porta

    pthread_mutex_lock(&peer_lock);

    // Verifica se o socket de escuta já foi criado
    if (listen_fd != -1) {
        // Já está escutando ou não foi corretamente resetado — evita duplicidade
        pthread_mutex_unlock(&peer_lock);
        return NULL; // Encerra a thread para não iniciar outro listener
    }

    // Cria o socket TCP para escuta
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Define a opção SO_REUSEADDR para poder reusar a porta imediatamente após encerrar
    int optval = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Configura o endereço para escutar em todas as interfaces locais na porta fornecida
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(peer_port);

    // Associa o socket à porta
    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind failed in peer_listener");
        pthread_mutex_unlock(&peer_lock);
        exit(EXIT_FAILURE); // Encerra o programa em caso de erro crítico
    }

    // Começa a escutar por conexões, com backlog máximo de 1
    if (listen(listen_fd, 1) < 0) {
        perror("listen failed in peer_listener");
        pthread_mutex_unlock(&peer_lock);
        exit(EXIT_FAILURE); // Encerra o programa se falhar ao escutar
    }

    if (client_port == 60000) {
        printf("[SL] No peer found, starting to listen...\n");
    } else {
        printf("[SS] No peer found, starting to listen...\n");
    }
    listening = 1; // Marca que a thread está ativamente escutando
    pthread_mutex_unlock(&peer_lock);

    // Loop principal para aceitar conexões de outros servidores SS
    while (1) {
        // Bloqueia esperando uma conexão — pode ficar preso aqui se não tratado corretamente
        int new_sock = accept(listen_fd, NULL, NULL);

        pthread_mutex_lock(&peer_lock);

        // Verifica se o listener foi sinalizado para parar (ex: após desconexão)
        if (!listening) {
            // Se uma conexão foi aceita enquanto sinalizava para parar, fecha o socket
            if (new_sock != -1) close(new_sock);
            pthread_mutex_unlock(&peer_lock);
            break; // Sai do loop de escuta
        }

        // Trata erro na chamada de accept()
        if (new_sock < 0) {
            if (errno == EINTR) {
                // Se foi interrompido por um sinal, tenta aceitar de novo
                pthread_mutex_unlock(&peer_lock);
                continue;
            }
            if (errno == EBADF) {
                // Se o socket foi fechado por outra thread, encerra o listener
                pthread_mutex_unlock(&peer_lock);
                break;
            }

            // Outro erro mais grave — imprime e sai do loop
            perror("accept failed in peer_listener");
            pthread_mutex_unlock(&peer_lock);
            break;
        }

        // Se já há um peer conectado, rejeita a nova conexão
        if (peer_connected) {
            Message err = {ERROR, "01"}; // Código de erro: limite de peers excedido
            send_message(new_sock, &err);
            close(new_sock); // Fecha o novo socket
            pthread_mutex_unlock(&peer_lock);
            continue; // Continua escutando
        }

        // Nenhum peer conectado — aceita o novo peer
        peer_sock = new_sock;
        pthread_mutex_unlock(&peer_lock); // Libera antes de criar a thread

        // Cria uma nova thread para lidar com a conexão estabelecida
        pthread_t tid;
        pthread_create(&tid, NULL, handle_peer, NULL);
        pthread_detach(tid); // Não é necessário aguardar a thread
    }

    // Se o loop foi encerrado, realiza a limpeza final do listener
    pthread_mutex_lock(&peer_lock);
    if (listen_fd != -1) {
        close(listen_fd); // Fecha o socket de escuta
        listen_fd = -1;
    }
    listening = 0; // Garante que a flag de escuta foi resetada
    pthread_mutex_unlock(&peer_lock);

    return NULL; // Finaliza a thread listener
}

// Funcao para ler comandos do stdin
void *stdin_thread(void *arg) {
    char input[128];  // Buffer para armazenar a entrada do usuário pelo terminal

    // Loop que escuta continuamente comandos digitados no terminal
    while (fgets(input, sizeof(input), stdin)) {
        // Remove o caractere de nova linha ('\n') lido pelo fgets
        input[strcspn(input, "\n")] = 0;

        // Verifica se o comando digitado foi "close connection" ou "kill" - desconecta o peer
        if (strcmp(input, "kill") == 0 || strcmp(input, "close connection") == 0) {
            pthread_mutex_lock(&peer_lock);  // Garante acesso exclusivo às variáveis globais

            // Verifica se existe um peer conectado no momento
            if (!peer_connected || peer_sock == -1) {
                if (client_port == 60000) {
                    printf("[SL] No peer connected to close connection\n");
                } else {
                    printf("[SS] No peer connected to close connection\n");
                }
                pthread_mutex_unlock(&peer_lock); // Libera o mutex e continua esperando novo input
                continue;
            }

            pthread_mutex_lock(&sensor_lock);

            Message notify = {NOTIFY_DISCSERVER, ""};
            for (int i = 0; i < sensor_count; i++) {
                send_message(sensors[i].socket_fd, &notify);
                close(sensors[i].socket_fd);
            }

            sensor_count = 0;

            pthread_mutex_unlock(&sensor_lock);

            // Prepara uma mensagem de desconexão (REQ_DISCPEER) com o ID deste servidor
            Message msg = {REQ_DISCPEER, ""};
            snprintf(msg.payload, sizeof(msg.payload), "%s", my_id);  // Coloca o ID local no payload

            // Envia a mensagem de desconexão para o peer atual
            send_message(peer_sock, &msg);
            pthread_mutex_unlock(&peer_lock);  
        }

        // Verifica se o comando digitado foi "list sensors" - lista sensores
        else if (strcmp(input, "list sensors") == 0) {
            if (client_port == 60000) {
                // Sou SL → pedir status para SS
                Message req = {REQ_LISTSTATUS_SERVER, ""};
                send_message(peer_sock, &req);
            } else {
                // Sou SS → pedir localização para SL
                Message req = {REQ_LISTLOC_SERVER, ""};
                send_message(peer_sock, &req);
            }
        }    
        
        // Verifica se o comando digitado foi "locate" - localiza sensor especifico
        else if (strncmp(input, "locate ", 7) == 0) {
            char sensor_id[MAX_ID_SIZE];
            if (sscanf(input + 7, "%s", sensor_id) == 1) {
                if (client_port == 60000) {
                    // SL: tem acesso direto à localização
                    pthread_mutex_lock(&sensor_lock);
                    int found = 0;
                    for (int i = 0; i < sensor_count; i++) {
                        if (strcmp(sensors[i].id, sensor_id) == 0) {
                            printf("[SL] Current sensor location: %d\n", sensors[i].location);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("[SL] Sensor not found\n");
                    }
                    pthread_mutex_unlock(&sensor_lock);
                } else {
                    int found = 0;
                    for (int i = 0; i < sensor_count; i++) {
                        if (strcmp(sensors[i].id, sensor_id) == 0) {
                            // SS: não sabe a localização, solicita ao SL
                            Message req = {REQ_SENSLOC, ""};
                            snprintf(req.payload, sizeof(req.payload), "%s", sensor_id);
                            send_message(peer_sock, &req);
                            
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("[SS] Sensor not found\n");
                    }
                    
                }
            } 
            else {
                if (client_port == 60000) {
                    printf("[SL] Invalid command format. Use: locate <sensor_id>\n");
                } else {
                    printf("[SS] Invalid command format. Use: locate <sensor_id>\n");
                }
            }
        }
        
        // Verifica se o comando digitado foi "status" - consulta status de sensor especifico
        else if (strncmp(input, "status ", 7) == 0) {
            char sensor_id[MAX_ID_SIZE];
            if (sscanf(input + 7, "%s", sensor_id) == 1) {
                if (client_port == 60000) {
                    int found = 0;
                    for (int i = 0; i < sensor_count; i++) {
                        if (strcmp(sensors[i].id, sensor_id) == 0) {
                            // SL: não sabe o status, solicita ao SS
                            Message req = {REQ_SENSSTATUS, ""};
                            snprintf(req.payload, sizeof(req.payload), "%s", sensor_id);
                            send_message(peer_sock, &req);
                            
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("[SL] Sensor not found\n");
                    }
                } else {
                    // SS: tem acesso direto ao status
                    pthread_mutex_lock(&sensor_lock);
                    int found = 0;
                    for (int i = 0; i < sensor_count; i++) {
                        if (strcmp(sensors[i].id, sensor_id) == 0) {
                            printf("[SS] Current sensor status: %d\n", sensors[i].risk);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("[SS] Sensor not found\n");
                    }
                    pthread_mutex_unlock(&sensor_lock);
                    
                }
            } 
            else {
                if (client_port == 60000) {
                    printf("[SL] Invalid command format. Use: status <sensor_id>\n");
                } else {
                    printf("[SS] Invalid command format. Use: status <sensor_id>\n");
                }
            }
        }

        // Verifica se o comando digitado foi "report" - reporta localização e risco de sensor especifico
        else if (strncmp(input, "report ", 7) == 0) {
            char sensor_id[MAX_ID_SIZE];
            if (sscanf(input + 7, "%s", sensor_id) == 1) {
                if (client_port == 60000) {
                    // SL: sabe localização, precisa do risco
                    pthread_mutex_lock(&sensor_lock);
                    int found = 0, loc = -1;
                    for (int i = 0; i < sensor_count; i++) {
                        if (strcmp(sensors[i].id, sensor_id) == 0) {
                            loc = sensors[i].location;
                            found = 1;
                            break;
                        }
                    }
                    pthread_mutex_unlock(&sensor_lock);
        
                    if (!found) {
                        printf("[SL] Sensor not found\n");
                    } else {
                        // Envia requisição para SS com ID e Loc
                        Message msg = {REQ_RISKSENS_SERVER, ""};
                        snprintf(msg.payload, sizeof(msg.payload), "%s %d", sensor_id, loc);
                        send_message(peer_sock, &msg);
                    }
        
                } else {
                    // SS: sabe risco, precisa da localização
                    pthread_mutex_lock(&sensor_lock);
                    int found = 0, risk = -1;
                    for (int i = 0; i < sensor_count; i++) {
                        if (strcmp(sensors[i].id, sensor_id) == 0) {
                            risk = sensors[i].risk;
                            found = 1;
                            break;
                        }
                    }
                    pthread_mutex_unlock(&sensor_lock);
        
                    if (!found) {
                        printf("[SS] Sensor not found\n");
                    } else {
                        // Envia requisição para SL com ID e risco
                        Message msg = {REQ_LOCSENS_SERVER, ""};
                        snprintf(msg.payload, sizeof(msg.payload), "%s %d", sensor_id, risk);
                        send_message(peer_sock, &msg);
                    }
                }
            } 
            else {
                if (client_port == 60000) {
                    printf("[SL] Invalid command format. Use: report <sensor_id>\n");
                } else {
                    printf("[SS] Invalid command format. Use: report <sensor_id>\n");
                }
            }
        }
        
        // Verifica se o comando digitado foi "help" - exibe comandos disponíveis
        else if (strcmp(input, "help") == 0) {
            if (client_port == 60000) {
                printf("[SL] Available commands:\n");
                printf(" - close connection / kill: Close connection with peer\n");
                printf(" - locate <sensor_id>: Locates a specific sensor\n");
                printf(" - status <sensor_id>: Query the risk status of a specific sensor\n");
                printf(" - report <sensor_id>: Reports location and risk of a specific sensor\n");
                printf(" - list sensors: Lists all connected sensors, with their location and risk status\n");
            } else {
                printf("[SS] Available commands:\n");
                printf(" - close connection / kill: Close connection with peer\n");
                printf(" - locate <sensor_id>: Locates a specific sensor\n");
                printf(" - status <sensor_id>: Query the risk status of a specific sensor\n");
                printf(" - report <sensor_id>: Reports location and risk of a specific sensor\n");
                printf(" - list sensors: Lists all connected sensors, with their location and risk status\n");
            }
        }

        // Se o comando não for reconhecido, exibe mensagem de erro
        else {
            if (client_port == 60000) {
                printf("[SL] Unknown command: %s\n", input);
            } else {
                printf("[SS] Unknown command: %s\n", input);
            }
        }

    }

    return NULL;  // Encerra a thread quando a entrada padrão for fechada (EOF)
}

// Função que inicia o servidor como SS (tentando se conectar a outro peer) ou como SL (escutando conexões)
void start_server(const char *ip, int peer_port, int client_port_input) {
    // Copia os parâmetros recebidos para variáveis globais
    strncpy(ip_global, ip, sizeof(ip_global));
    peer_port_global = peer_port;
    client_port = client_port_input;

    pthread_mutex_lock(&peer_lock);
    // Verifica se deve tentar conectar como SS:
    // Só pode tentar se não estiver conectado, nem escutando e nem com socket ativo
    int should_try_connect = (!peer_connected && peer_sock == -1 && !listening);
    pthread_mutex_unlock(&peer_lock);

    // Se deve tentar conexão como SS (servidor que inicia conexão)
    if (should_try_connect) {
        int temp_sock = socket(AF_INET, SOCK_STREAM, 0); // Cria socket temporário para tentar conexão

        // Prepara estrutura de endereço do peer
        struct sockaddr_in peer_addr;
        peer_addr.sin_family = AF_INET;
        peer_addr.sin_port = htons(peer_port); // Converte porta para ordem de bytes da rede
        inet_pton(AF_INET, ip, &peer_addr.sin_addr); // Converte IP string para binário

        // Tenta conectar ao peer (SL)
        if (connect(temp_sock, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) == 0) {
            // Conexão bem-sucedida: atua como SS

            pthread_mutex_lock(&peer_lock);
            peer_sock = temp_sock; // Salva o socket na variável global
            pthread_mutex_unlock(&peer_lock);

            // Envia mensagem de requisição de conexão ao peer
            Message req = {REQ_CONNPEER, ""};
            snprintf(req.payload, sizeof(req.payload), "%s", my_id); // Envia seu ID
            send_message(peer_sock, &req);

            // Cria thread para lidar com mensagens do peer conectado
            pthread_t handle_peer_tid;
            pthread_create(&handle_peer_tid, NULL, handle_peer, NULL);
            pthread_detach(handle_peer_tid); // Detach pois não precisa aguardar retorno

            return; // Encerramos a função, pois já estamos conectados como SS
        } else {
            // Falha ao conectar: libera socket temporário e irá escutar como SL
            close(temp_sock);
        }
    }

    // Se não conectou como SS, inicia modo SL (servidor que escuta)
    pthread_mutex_lock(&peer_lock);
    // Garante que só vai escutar se ainda não estiver escutando, nem com socket ou conexão ativa
    if (!listening && peer_sock == -1 && listen_fd == -1) {
        pthread_mutex_unlock(&peer_lock); // Libera mutex antes de alocar memória e criar thread

        // Aloca memória para passar a porta como argumento para a thread
        int *arg_listener = malloc(sizeof(int));
        *arg_listener = peer_port;

        // Cria thread que ficará escutando conexões de peers (SS)
        pthread_t listener_tid;
        pthread_create(&listener_tid, NULL, peer_listener, arg_listener);
        pthread_detach(listener_tid); // Detach pois a thread será longa e independente
    } else {
        pthread_mutex_unlock(&peer_lock);
    }
}



// ---------- Funções sensores ----------



// Gera o ID do sensor no formato "<area><loc><seq>"
void generate_id_sensor(char *dest, int loc) {
    int area = (loc >= 1 && loc <= 3) ? 1 :
               (loc >= 4 && loc <= 5) ? 2 :
               (loc >= 6 && loc <= 7) ? 3 :
               (loc >= 8 && loc <= 10) ? 4 : 0;
    if (loc == -1) loc = 0; // Para sensores fora da zona ativa
    int seq = ++area_sequence[area][loc];
    snprintf(dest, MAX_ID_SIZE, "%1d%02d%06d", area, loc, seq);
}

// Registra um novo sensor no servidor
int register_sensor(int sockfd, int loc_id, char *generated_id) {
    if (sensor_count >= MAX_SENSORS) {
        return -1; // Limite atingido
    }

    generate_id_sensor(generated_id, loc_id);

    SensorInfo sc;
    sc.socket_fd = sockfd;
    if (client_port == 60000) { // Se for SL, define localização
        sc.location = loc_id;
        sc.risk = -1; // risco não definido para SL
    } else { // Se for SS, define risco
        sc.risk = 0; // rand() % 2; // Inicialmente, risco é 0
        sc.location = -1; // Localização não definida para SS
    }
    strncpy(sc.id, generated_id, MAX_ID_SIZE);

    sensors[sensor_count++] = sc;

    if (client_port == 60000) {
        printf("[SL] Client %s added (Loc %d)\n", generated_id, loc_id);
    } else {
        printf("[SS] Client %s added (Loc %d)\n", generated_id, loc_id);
    }

    return 0;
}

// Função que lida com a conexão de um cliente (sensor)
void *handle_client(void *arg) {;

    int client_sock = *(int *)arg;
    free(arg);

    Message msg;
    char sensor_id[MAX_ID_SIZE] = "";

    // Loop principal: continua recebendo mensagens do cliente enquanto a conexão estiver ativa
    while (recv_message(client_sock, &msg) == 0) {

        // Se a mensagem for uma requisição de conexão de sensor
        if (msg.code == REQ_CONNSEN) {
            pthread_mutex_lock(&sensor_lock);

            int loc_id = atoi(msg.payload);

            // Verifica se o total de sensores já atingiu o limite
            if (sensor_count >= MAX_SENSORS) {
                if (client_port == 60000) {
                    printf("[SL] Sensor limit exceeded\n");
                } else {
                    printf("[SS] Sensor limit exceeded\n");
                }
                Message err = {ERROR, "09"};
                send_message(client_sock, &err);
                pthread_mutex_unlock(&sensor_lock);
                continue;
            }

            register_sensor(client_sock, loc_id, sensor_id); // Registra o novo sensor

            Message res = {RES_CONNSEN, ""};
            strncpy(res.payload, sensor_id, MAX_MSG_SIZE - 1);
            send_message(client_sock, &res);

            Message response = {OK, "02"};
            send_message(client_sock, &response);

            pthread_mutex_unlock(&sensor_lock);
        }

        // Se a mensagem for uma requisição de desconexão de sensor
        else if (msg.code == REQ_DISCSEN) {
            pthread_mutex_lock(&sensor_lock);

            // Verifica se o ID enviado corresponde a algum sensor registrado
            int found = 0, i;
            for (i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    found = 1;
                    break;
                }
            }
        
            if (!found) {
                Message err = {ERROR, "10"};
                send_message(client_sock, &err);
                pthread_mutex_unlock(&sensor_lock);
                continue;
            }
        
            int loc = sensors[i].location;
        
            // Remove o sensor da lista 
            SensorInfo new_list[MAX_SENSORS];
            int new_count = 0;

            for (int j = 0; j < sensor_count; j++) {
                if (j != i) { // copia tudo, exceto o sensor a ser removido
                    new_list[new_count++] = sensors[j];
                }
            }

            // Copia nova lista para a lista original
            memcpy(sensors, new_list, sizeof(SensorInfo) * new_count);
            sensor_count = new_count;
            
            // Envia mensagem de confirmação de desconexão
            Message ok = {OK, "01"};
            send_message(client_sock, &ok);
        
            // Imprime mensagem de desconexão
            if (client_port == 60000)
                printf("[SL] Client %s removed (Loc %d)\n", msg.payload, loc);
            else
                printf("[SS] Client %s removed\n", msg.payload);
        
            pthread_mutex_unlock(&sensor_lock);
            break;  // encerra a conexão
        }    
        
        // Se a mensagem for uma requisição de status de sensor
        else if (msg.code == REQ_SENSSTATUS) {
            printf("[SS] REQ_SENSSTATUS %s\n", msg.payload);
        
            int found = 0;
        
            // Verifica se o sensor está na base de dados
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    found = 1;
                    if (sensors[i].risk == 1) {
                        pthread_mutex_lock(&pending_lock);
                        if (pending_count < MAX_SENSORS) {
                            strncpy(pending_alerts[pending_count].sensor_id, msg.payload, MAX_ID_SIZE);
                            pending_alerts[pending_count].socket_fd = client_sock;
                            pending_count++;
                        }
                        pthread_mutex_unlock(&pending_lock);
                        // Encaminha REQ_CHECKALERT para o SL
                        Message forward = {REQ_CHECKALERT, ""};
                        strncpy(forward.payload, msg.payload, MAX_MSG_SIZE - 1);
                        send_message(peer_sock, &forward);
                    }
                    else {
                        // Envia resposta com o status do sensor
                        Message res = {OK, "03"};
                        send_message(client_sock, &res);
                    }
                    break;
                }
            }
        
            if (!found) {
                Message err = {ERROR, "10"};
                send_message(client_sock, &err);
            }
        }     
        
        // Se a mensagem for uma requisição de localização de sensor 
        else if (msg.code == REQ_SENSLOC) {
            printf("[SL] REQ_SENSLOC %s\n", msg.payload);
        
            pthread_mutex_lock(&sensor_lock);
            int found = 0, loc_id;
        
            // Verifica se o sensor está na base de dados
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    loc_id = sensors[i].location;
                    found = 1;
                    break;
                }
            }
        
            // Se o sensor foi encontrado, envia a localização
            if (found) {
                Message res = {RES_SENSLOC, ""};
                snprintf(res.payload, MAX_MSG_SIZE, "%d", loc_id);
                send_message(client_sock, &res);
            } else {
                Message err = {ERROR, "10"};
                send_message(client_sock, &err);
            }
            pthread_mutex_unlock(&sensor_lock);
        }

        // Se a mensagem for uma requisição de lista de localização
        else if (msg.code == REQ_LOCLIST) {
            // Extrai SenID e LocId do payload
            char sen_id[MAX_ID_SIZE];
            int loc_id;
        
            if (sscanf(msg.payload, "%s %d", sen_id, &loc_id) != 2 || loc_id < 1 || loc_id > 10) {
                Message err = {ERROR, "11"};
                send_message(client_sock, &err);
                continue;
            }
        
            printf("[SL] REQ_LOCLIST %s %d\n", sen_id, loc_id);
        
            pthread_mutex_lock(&sensor_lock);
        
            // Monta lista de sensores da localização
            char response[MAX_MSG_SIZE] = "";
            int found = 0;
            for (int i = 0; i < sensor_count; i++) {
                if (sensors[i].location == loc_id) {
                    if (found) strncat(response, ", ", MAX_MSG_SIZE - strlen(response) - 1);
                    strncat(response, sensors[i].id, MAX_MSG_SIZE - strlen(response) - 1);
                    found = 1;
                }
            }
        
            // Se encontrou sensores na localização, envia a lista
            if (found) {
                Message res = {RES_LOCLIST, ""};
                snprintf(res.payload, MAX_MSG_SIZE, "%s", response);
                send_message(client_sock, &res);
            } else {
                Message err = {ERROR, "10"};
                send_message(client_sock, &err);
            }
        
            pthread_mutex_unlock(&sensor_lock);
        }

        // Se a mensagem for uma requisição de atualização de risco
        else if (msg.code == REQ_UPDRISK) {
            pthread_mutex_lock(&sensor_lock);
            int found = 0;
            int risk_val = atoi(msg.payload);
        
            for (int i = 0; i < sensor_count; i++) {
                if (sensors[i].socket_fd == client_sock) {
                    if (sensors[i].risk == risk_val && risk_val == 1) { // Se já está em alerta, não faz nada
                        printf("[SS] Sensor %s is already on alert!\n", sensors[i].id);
                    } else if (sensors[i].risk != risk_val) { // Se o risco mudou
                        sensors[i].risk = risk_val;
                        printf("[SS] Sensor %s risk updated to %d\n", sensors[i].id, risk_val);
                        // Envia mensagem de atualização de localização para o peer, para atualizar contadores de risco das areas
                        Message req = {REQ_LOCUPDATE, ""};
                        snprintf(req.payload, sizeof(req.payload), "%s %d", sensors[i].id, risk_val);
                        send_message(peer_sock, &req);
                    } 
                    found = 1;
                    break;
                }
            }
        
            if (!found) {
                Message err = {ERROR, "10"};
                send_message(client_sock, &err);
            }
        
            pthread_mutex_unlock(&sensor_lock);
        }
        
        // Se a mensagem for uma requisição de risco de sensor
        else if (msg.code == REQ_RISKSENS_SENSOR) {
            printf("[SS] REQ_RISKSENS_SENSOR %s\n", msg.payload);
        
            pthread_mutex_lock(&sensor_lock);
            int found = 0, risk_id;
        
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    risk_id = sensors[i].risk;
                    found = 1;
                    break;
                }
            }
        
            if (found) {
                Message res = {RES_RISKSENS_SENSOR, ""};
                snprintf(res.payload, MAX_MSG_SIZE, "%d", risk_id);
                send_message(client_sock, &res);
            } else {
                Message err = {ERROR, "10"};
                send_message(client_sock, &err);
            }
            pthread_mutex_unlock(&sensor_lock);
        }
        
        // Se a mensagem for uma requisição de lista de localização de sensores - SL responde com localizações
        else if (msg.code == REQ_LISTLOC_SENSOR) {
            pthread_mutex_lock(&sensor_lock);

            char response[MAX_MSG_SIZE] = "";
            for (int i = 0; i < sensor_count; i++) {
                char line[64];
                snprintf(line, sizeof(line), "%s %d,", sensors[i].id, sensors[i].location);
                strncat(response, line, sizeof(response) - strlen(response) - 1);
            }

            Message res = {RES_LISTLOC_SENSOR, ""};
            strncpy(res.payload, response, sizeof(res.payload) - 1);
            send_message(client_sock, &res);

            pthread_mutex_unlock(&sensor_lock);
        }

        // Se a mensagem for uma requisição de lista de status de sensores - SS responde com status
        else if (msg.code == REQ_LISTSTATUS_SENSOR) {
            pthread_mutex_lock(&sensor_lock);

            char response[MAX_MSG_SIZE] = "";
            for (int i = 0; i < sensor_count; i++) {
                char line[64];
                snprintf(line, sizeof(line), "%s %d,", sensors[i].id, sensors[i].risk);
                strncat(response, line, sizeof(response) - strlen(response) - 1);
            }

            Message res = {RES_LISTSTATUS_SENSOR, ""};
            strncpy(res.payload, response, sizeof(res.payload) - 1);
            send_message(client_sock, &res);

            pthread_mutex_unlock(&sensor_lock);
        }

        // Se a mensagem for uma requisição de relatório de localização de sensor - SL responde com localização
        else if (msg.code == REQ_REPORTLOC_SENSOR) {
            int found = 0, loc = -1;

            pthread_mutex_lock(&sensor_lock);
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    loc = sensors[i].location;
                    found = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&sensor_lock);

            if (found) {
                Message res = {RES_REPORTLOC_SENSOR, ""};
                snprintf(res.payload, sizeof(res.payload), "%d", loc);
                send_message(client_sock, &res);
            } else {
                Message err = {ERROR, "10"};
                send_message(client_sock, &err);
            }
        }

        // Se a mensagem for uma requisição de relatório de risco de sensor - SS responde com risco
        else if (msg.code == REQ_REPORTRISK_SENSOR) {
            int found = 0, risk = -1;

            pthread_mutex_lock(&sensor_lock);
            for (int i = 0; i < sensor_count; i++) {
                if (strcmp(sensors[i].id, msg.payload) == 0) {
                    risk = sensors[i].risk;
                    found = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&sensor_lock);

            if (found) {
                Message res = {RES_REPORTRISK_SENSOR, ""};
                snprintf(res.payload, sizeof(res.payload), "%d", risk);
                send_message(client_sock, &res);
            } else {
                Message err = {ERROR, "10"};
                send_message(client_sock, &err);
            }
        }
    }

    close(client_sock);
    pthread_exit(NULL);
}

// Função que escuta conexões de clientes (sensores)
void *client_listener(void *arg) {
    int port = *(int *)arg;
    free(arg);

    client_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(client_listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(client_listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(client_listen_fd, MAX_SENSORS); 

    while (1) {
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(client_listen_fd, NULL, NULL);

        if (*client_sock < 0) {
            perror("Erro no accept");
            free(client_sock);
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
    }

    return NULL;
}



// ---------- Main ----------



// Função principal
int main(int argc, char *argv[]) {
    // Verifica se o número de argumentos está correto (espera 3: IP, porta_peer, porta_clientes)
    if (argc != 4) {
        printf("Use: %s <ip_peer> <porta_peer> <porta_clientes>\n", argv[0]);
        return 1; // Encerra o programa com erro
    }

    // Aloca estrutura de argumentos para iniciar o servidor em uma nova thread
    struct StartArgs *args = malloc(sizeof(struct StartArgs));
    strncpy(args->ip, argv[1], sizeof(args->ip));         // IP do peer a se conectar
    args->peer_port = atoi(argv[2]);                      // Porta de conexão peer-to-peer
    args->client_port = atoi(argv[3]);                    // Porta para atender clientes (futuramente usada)

    // Cria thread que inicia a lógica do servidor (tentando conectar ou escutar)
    pthread_t server_init_tid;
    pthread_create(&server_init_tid, NULL, threaded_start_server, args);
    pthread_detach(server_init_tid); // Detach para que rode de forma independente

    // Cria thread que escuta conexões de sensores (clientes)
    int *port_ptr = malloc(sizeof(int));
    *port_ptr = atoi(argv[3]);
    client_port = *port_ptr;  // Salva no global também

    pthread_t client_tid;
    pthread_create(&client_tid, NULL, client_listener, port_ptr);
    pthread_detach(client_tid); // Também em detach, pois não precisa ser sincronizada

    // Cria thread que escuta comandos do usuário via stdin 
    pthread_t stdin_input_tid;
    pthread_create(&stdin_input_tid, NULL, stdin_thread, NULL);
    pthread_detach(stdin_input_tid); // Também em detach, pois não precisa ser sincronizada

    // Mantém o processo principal vivo para que as threads continuem rodando
    while (1) sleep(1);

    return 0; 
}