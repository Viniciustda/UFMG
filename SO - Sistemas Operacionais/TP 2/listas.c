#include "listas.h"
#include "structs.h"

// -------------------------------------------------- Funções de Listas --------------------------------------------------

void adicionaLista(estadoSimulacao *estado){
    node *no = malloc(sizeof(node));
    no->id = estado->atual_tam_memoria;
    no->anterior = NULL;
    no->proximo = NULL;

    if(estado->lista_id->tamanho == 0){
        estado->lista_id->primeiro = no;
        estado->lista_id->ultimo = no;
    }
    else if(estado->lista_id->tamanho == 1){
        estado->lista_id->ultimo = no;
        estado->lista_id->primeiro->proximo = no;
        no->anterior = estado->lista_id->primeiro;
    }
    else{
        no->anterior = estado->lista_id->ultimo;
        estado->lista_id->ultimo->proximo = no;
        estado->lista_id->ultimo = no;
    }
    estado->lista_id->tamanho++;
}

void atualizaLista(estadoSimulacao *estado, int id_memoria){
    node *aux = estado->lista_id->primeiro;
    if(estado->lista_id->tamanho == 1 || estado->lista_id->ultimo->id == id_memoria)
        return;

    if(estado->lista_id->primeiro->id == id_memoria){
        estado->lista_id->primeiro = estado->lista_id->primeiro->proximo;
        estado->lista_id->primeiro->anterior = NULL;
        aux->proximo = NULL;
        aux->anterior = estado->lista_id->ultimo;
        estado->lista_id->ultimo->proximo =  aux;
        estado->lista_id->ultimo = aux;
        return;
    }

    for(int i = 0; i < estado->lista_id->tamanho; i++){
        if(aux->id == id_memoria){
            aux->anterior->proximo = aux->proximo;
            aux->proximo->anterior = aux->anterior;

            aux->proximo = NULL;
            aux->anterior = estado->lista_id->ultimo;
            estado->lista_id->ultimo->proximo = aux;
            estado->lista_id->ultimo = aux;
            return;
        }
        aux = aux->proximo;
    }
}

void adicionaListaCircular(estadoSimulacao *estado){
    node *no = malloc(sizeof(node));
    no->id = estado->atual_tam_memoria;
    no->anterior = NULL;
    no->proximo = estado->lista_circular->primeiro;

    if(estado->lista_circular->tamanho == 0){
        estado->lista_circular->primeiro = no;
        estado->lista_circular->ultimo = no;
    }
    else if(estado->lista_circular->tamanho == 1){
        estado->lista_circular->ultimo = no;
        estado->lista_circular->primeiro->proximo = no;
        estado->lista_circular->primeiro->anterior = no;
        no->anterior = estado->lista_circular->primeiro;
    }
    else{
        no->anterior = estado->lista_circular->ultimo;
        estado->lista_circular->ultimo->proximo = no;
        estado->lista_circular->ultimo = no;
        estado->lista_circular->primeiro->anterior = no;
    }
    estado->lista_circular->tamanho++;
}
