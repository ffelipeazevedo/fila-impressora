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
void inicializa_fila(Fila *f);                          // iniciliza a fila
void imprime_fila(Fila *f, char *nome);                 // imprimi fila
Solicitacao *cria_nodo(int prioridade, char *ip);       // cria um no 
void enfileira(Fila *f, int prioridade, char *ip);      // adiciona no fim
int desenfileira(Fila *f, int *prioridade, char *ip);   // retira do inicio
void liberar_fila(Fila *f);                             // libera memoria
int gera_arquivo(char *nome_arquivo, int quantidade);   // gera solicitacoes
int carrega_arquivo(char *nome_arquivo, Fila *entrada); // carrega solicitacoes
int distribui(Fila *entrada, Fila filas[], int qtd);    // distribui as solicitacoes nas filas

// Menu provisório para testar as funções da fila
int main(){
    Fila entrada;   
    Fila filas[3];  
    int opcao;              // opção escolhida no menu
    int prio;               // prioridade digitada ou removida
    int qtd;                // quantidade de solicitações a gerar
    char ip[16];            // IP digitado ou removido
    
    srand(time(NULL));

    inicializa_fila(&entrada);      // começa com a fila vazia
    
    for (int i = 0; i < 3; i++){
        inicializa_fila(&filas[i]);
    }

    for (;;) {                   
        printf("\n===== MENU DE TESTE =====\n");
        printf("1. Gerar arquivo de solicitacoes\n");
        printf("2. Enfileirar manualmente\n");
        printf("3. Desenfileirar\n");
        printf("4. Imprimir fila de entrada\n");
        printf("5. Liberar fila\n");
        printf("6. Carregar solicitacoes\n");
        printf("7. Distibuir solicitacoes\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Quantas solicitacoes (minimo 50)? ");
                scanf("%d", &qtd);
                if (qtd < 50) {                        
                    printf("Quantidade invalida! Minimo de 50.\n");
                    break;
                }
                if (gera_arquivo("solicitacoes.txt", qtd) == 1)
                    printf("Arquivo gerado! Abra o solicitacoes.txt para conferir.\n");
                break;

            case 2:
                printf("Prioridade (1-3): ");
                scanf("%d", &prio);
                printf("IP: ");
                scanf("%15s", ip);                      
                enfileira(&entrada, prio, ip);
                printf("Solicitacao enfileirada!\n");
                break;

            case 3:
                if (desenfileira(&entrada, &prio, ip) == 0)
                    printf("Fila vazia! Nada a remover.\n");
                else
                    printf("Saiu da fila: [%d | %s]\n", prio, ip);
                break;

            case 4:
                imprime_fila(&entrada, "Entrada");
                break;

            case 5:
                for (int i = 0; i < 3; i++){
                    liberar_fila(&entrada);
                }
                    printf("Fila liberada!\n");
                break;

            case 6:
                qtd = carrega_arquivo("solicitacoes.txt", &entrada);
                if (qtd == -1){
                    break;
                }
                printf("%d solicitacoes carregadas.\n", qtd);
                if (qtd < 50){
                    printf("Atencao: o enunciado exige no minimo 50!\n");
                    imprime_fila(&entrada, "Entrada");
                    break;
                }
                break;
            
            case 7:
                qtd = distribui(&entrada, filas, 5);     
                printf("%d solicitacoes distribuidas.\n", qtd);
                imprime_fila(&entrada, "Entrada");
                imprime_fila(&filas[0], "Fila 0");
                imprime_fila(&filas[1], "Fila 1");
                imprime_fila(&filas[2], "Fila 2");
                  
                break;
            case 0:
                // devolve a memória antes de sair
                for (int i = 0; i < 3; i++){ 
                    liberar_fila(&entrada);     
                }            
                printf("Saindo...\n");
                return 0;                           

            default:
                printf("Opcao invalida!\n");
        }
    }
}

// Executa um ciclo de distribuição: tira até 5 solicitações do início da
// Fila de Entrada e manda cada uma para a fila da sua prioridade.
int distribui(Fila *entrada, Fila filas[], int qtd){
    int prioridade; // vem da entrada
    char ip[16];
    int cont = 0;


    for(int i = 0; i < qtd; i++){
        desenfileira(entrada, &prioridade, ip);
        if(desenfileira(entrada, &prioridade, ip) == 0){
            break;
        }
        enfileira(&filas[prioridade - 1], prioridade, ip);
        cont++;
    }

    return cont;
}


// Abre o solicitacoes.txt, lê cada par prioridade + IP e chama o enfileira 
// para colocar na Fila de Entrada. No final, retorna quantas solicitações leu.

int carrega_arquivo(char *nome_arquivo, Fila *entrada){
    FILE *arq;
    int prio_lida;
    char ip_lido[16];
    int total_lido = 0;

    arq = fopen(nome_arquivo, "r");
    if (arq == NULL) {                     
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return -1;
    }

    while (fscanf(arq, " %d, %15[^;];", &prio_lida, ip_lido) == 2) {
        if (prio_lida >= 1 && prio_lida <= 3) {                
            enfileira(entrada, prio_lida, ip_lido);                
            total_lido++;                   
        } else {
            printf("Prioridade invalida ignorada: %d\n", prio_lida);
        }
    }
    fclose(arq);
    return total_lido;
}

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

    printf("\n%s (%d): \n", nome, f->total);        // cabeçalho: nome e quantidade

    if (f->inicio == NULL) {                    // fila vazia
        printf("vazia\n");
        return;
    }

    // percorre do primeiro nodo até o fim (quando aux vira NULL)
    for (aux = f->inicio; aux != NULL; aux = aux->prox) {
        printf("[%d | %s] \n", aux->prioridade, aux->ip);
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
    if (f->inicio == NULL){
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