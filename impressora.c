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






// Função para inicilizar a lista 
void inicializa_fila(Fila *f){
    f->inicio = NULL;
    f->fim = NULL;
    f->total = 0;
}
