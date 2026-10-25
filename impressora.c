#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TOTAL_COMPUTADORES 15

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
void inicializa_fila(Fila *f);                         // iniciliza a fila
void imprime_fila(Fila *f, char *nome);                // imprimi fila
Solicitacao *cria_nodo(int prioridade, char *ip);      // cria um no 
void enfileira(Fila *f, int prioridade, char *ip);     // adiciona no fim
int desenfileira(Fila *f, int *prioridade, char *ip);  // retira do inicio
void liberar_fila(Fila *f);                            // libera memoria
int gera_arquivo(char *nome_arquivo, int quantidade);   // gera solicitacoes


// preenche o arquivo txt com as solicitacoes - simulando pessoas 
// Gera um arquivo com solicitações de impressão aleatórias
// Retorna 1 se gerou, 0 se não conseguiu criar o arquivo
int gera_arquivo(char *nome_arquivo, int quantidade){
    FILE *arq;
    int i, prioridade, sorteio;
    char ip[16];

    arq = fopen(nome_arquivo, "w");              // abre o arquivo para ESCRITA
    if (arq == NULL) {                     
        printf("Erro ao criar o arquivo %s\n", nome_arquivo);
        return 0;
    }

    for (i = 0; i < quantidade; i++) {
        prioridade = rand() % 3 + 1;              // sorteia de 1 a 3

        sorteio = (rand() % TOTAL_COMPUTADORES + 1);
        sprintf(ip, "192.168.0.%d", sorteio);     // monta o IP no vetor ip (sprintf)

        fprintf(arq, "%d, %s; ", prioridade, ip); // escreve "prioridade, ip; " no arquivo (fprintf)

        if ((i + 1) % 5 == 0) {  // a cada 5 solicitações...   
            fprintf(arq, "\n");
        }                           
    }

    fclose(arq);                 
    return 1;
}



// Funcao para inicilizar a lista 
void inicializa_fila(Fila *f){
    f->inicio = NULL;
    f->fim = NULL;
    f->total = 0;
}

// Insere uma solicitação no FIM da fila
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


// Imprime todas as solicitações da fila, do início ao fim
void imprime_fila(Fila *f, char *nome){
    Solicitacao *aux;

    printf("%s (%d): ", nome, f->total);        // cabeçalho: nome e quantidade

    if (f->inicio == NULL) {                    // fila vazia
        printf("vazia\n");
        return;
    }

    // percorre do primeiro nodo até o fim (quando aux vira NULL)
    for (aux = f->inicio; aux != NULL; aux = aux->prox) {
        printf("[%d | %s] ", aux->prioridade, aux->ip);
    }
    printf("\n");                             
}


// Imprimi a solicitação do inicio da fila (Remove do Inicio)
int desenfileira(Fila *f, int *prioridade, char *ip){
    Solicitacao *aux;

    if(f->inicio == NULL){
        return 0;
    }

    aux = f->inicio;
    *prioridade = aux->prioridade;
    strcpy(ip, aux->ip);

    f->inicio = f->inicio->prox;
    if (f->total == 0){
        f->fim = NULL;
    }
    
    free(aux);
    f->total--;

    return 1;
}

// Libera a memoria dos Nos alocados
void liberar_fila(Fila *f){
    Solicitacao *aux, *prox;

    aux = f->inicio; 

    while(aux != NULL){
        prox = aux->prox;
        free(aux);
        aux = prox;
    }

    // fila vazia
    f->inicio = NULL;
    f->fim = NULL;
    f->total = 0;
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