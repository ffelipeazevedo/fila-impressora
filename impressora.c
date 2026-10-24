#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct solicitacao {
    char ip[16];                 // identificação: IP de quem pediu
    int prioridade;              // 1 = máxima, 2 = normal, 3 = baixa
    struct solicitacao *prox;    // endereço do próximo nodo da fila
} Solicitacao;

typedef struct fila {
    Solicitacao *inicio;         // primeiro da fila (próximo a sair)
    Solicitacao *fim;            // último da fila (onde o novo entra)
    int total;                   // quantidade de solicitações na fila
} Fila;

// Funcoes
void inicializa_fila(Fila *f);
Solicitacao *cria_nodo(int prioridade, char *ip);
void enfileira(Fila *f, int prioridade, char *ip);



// Funcao para inicilizar a lista 
void inicializa_fila(Fila *f){
    f->inicio = NULL;
    f->fim = NULL;
    f->total = 0;
}

// Funcao enfileira lista (Insere no inicio)
void enfileira(Fila *f, int prioridade, char *ip){
    Solicitacao *novo;
    novo = cria_nodo(prioridade, ip);

    if(f->inicio == NULL){
        f->inicio = novo;
        f->fim = novo;
    } else {
        f->fim->prox = novo;
        f->fim = novo;
        
    }

    f->total++;
}





// A funcao cria_nodo recebe a prioridade e o IP de uma solicitação e retorna o endereço de um novo nodo
Solicitacao *cria_nodo(int prioridade, char *ip){
    Solicitacao *aux;

    aux = (Solicitacao*)malloc(sizeof(Solicitacao));
    if (!aux){
        printf("Problema de alocacao");
        exit(1);
    }

    aux->prioridade = prioridade;
    // strncpy(destino, origem, máximo de caracteres): copia até 15, reservando espaço para o \0
    strncpy(aux->ip, ip, sizeof(aux->ip) - 1); 
    // garante o terminador da string
    aux->ip[sizeof(aux->ip) - 1] = '\0';        
    aux->prox = NULL;

    return aux;
}