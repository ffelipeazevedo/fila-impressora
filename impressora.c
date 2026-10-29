#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>         
#include <windows.h>  

// Constantes
#define TOTAL_COMPUTADORES 15         // qtd de IPs que sao variados
#define INTERVALO_MS 500              // 0.5 segundos 
#define QTD_POR_CICLO 5               // solicitações distribuídas por ciclo (substitui CICLO)
#define MIN_SOLICITACOES 50           // mínimo exigido pelo enunciado
#define ARQUIVO "solicitacoes.txt"    // nome do arquivo de entrada

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
void espera(int ms);                                    // delay 0.05 seg
int simular_distribuicao(Fila *entrada, Fila filas[]);  // simulacao distribuicao de acordo com ciclo
void imprime_situacao(Fila *entrada, Fila filas[]);     // imprime as 4 filas
void limpa_buffer(void);                                // descarta o que sobrou no teclado

// Menu provisório para testar as funções da fila
// Menu principal do simulador da fila de impressão
int main(){
    Fila entrada;           // Fila de Entrada: recebe as solicitações do arquivo
    Fila filas[3];          // filas[0] = máxima (1), filas[1] = normal (2), filas[2] = baixa (3)
    int opcao;              // opção escolhida no menu
    int qtd;                // quantidade de solicitações (a gerar / carregadas)

    srand(time(NULL));      // semente do rand: cada execução gera solicitações diferentes

    // todas as filas começam vazias
    inicializa_fila(&entrada);
    for (int i = 0; i < 3; i++)
        inicializa_fila(&filas[i]);

    for (;;) {             
        printf("\n===== SIMULADOR DA FILA DE IMPRESSAO =====\n");
        printf("1. Gerar arquivo de solicitacoes\n");
        printf("2. Carregar solicitacoes do arquivo\n");
        printf("3. Ver situacao das filas\n");
        printf("4. Iniciar distribuicao\n");
        printf("5. Limpar todas as filas\n");
        printf("0. Sair\n");
        printf("Opcao: ");

        // validacao para ver se o usuario digitou algo fora do esperado
        if (scanf("%d", &opcao) != 1)   
            opcao = -1;                
        limpa_buffer();                 

        switch (opcao) {
            case 1:     // gera um .txt com solicitações aleatórias
                do {    // repete até digitar uma quantidade válida
                    printf("Quantas solicitacoes (minimo %d)? ", MIN_SOLICITACOES);
                    if (scanf("%d", &qtd) != 1)
                        qtd = 0;
                    limpa_buffer();
                    if (qtd < MIN_SOLICITACOES)
                        printf("Quantidade invalida! O minimo e %d.\n", MIN_SOLICITACOES);
                } while (qtd < MIN_SOLICITACOES);

                if (gera_arquivo(ARQUIVO, qtd) == 1)
                    printf("Arquivo %s gerado com %d solicitacoes.\n", ARQUIVO, qtd);
                break;

            case 2:     
                // carrega a Fila de Entrada a partir do arquivo
                // esvazia tudo antes, para não somar com uma carga anterior
                liberar_fila(&entrada);
                for (int i = 0; i < 3; i++)
                    liberar_fila(&filas[i]);

                qtd = carrega_arquivo(ARQUIVO, &entrada);
                if (qtd == -1) {                        // arquivo não existe
                    printf("Gere o arquivo primeiro (opcao 1).\n");
                    break;
                }
                if (qtd < MIN_SOLICITACOES) {           // o enunciado exige no mínimo 50
                    printf("O arquivo tem %d solicitacoes validas, mas o minimo e %d.\n", qtd, MIN_SOLICITACOES);
                    liberar_fila(&entrada);             // não deixa a fila carregada pela metade
                    break;
                }
                printf("%d solicitacoes carregadas na Fila de Entrada.\n", qtd);
                imprime_situacao(&entrada, filas);
                break;

            case 3:     // mostra as 4 filas sem alterar nada
                imprime_situacao(&entrada, filas);
                break;

            case 4:     // distribui 5 a cada 0,5 s até a entrada esvaziar
                simular_distribuicao(&entrada, filas);  // já avisa se a entrada estiver vazia
                break;

            case 5:     // esvazia as 4 filas
                liberar_fila(&entrada);
                for (int i = 0; i < 3; i++)
                    liberar_fila(&filas[i]);
                printf("Todas as filas foram esvaziadas.\n");
                break;

            case 0:     // devolve toda a memória antes de sair
                liberar_fila(&entrada);
                for (int i = 0; i < 3; i++)
                    liberar_fila(&filas[i]);
                printf("Saindo...\n");
                return 0;                      

            default:
                printf("Opcao invalida!\n");
        }
    }
}

// Descarta tudo o que sobrou no buffer do teclado até o Enter
// Evita que uma letra digitada no lugar de um número trave o programa
void limpa_buffer(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);   // lê e joga fora caractere por caractere
}

// Imprime a situação das 4 filas: entrada + as 3 de prioridade
void imprime_situacao(Fila *entrada, Fila filas[]){
    char *nomes[3] = {"Fila 0 - maxima", "Fila 1 - normal", "Fila 2 - baixa"};

    imprime_fila(entrada, "Entrada");          // entrada já é ponteiro: sem &
    for (int i = 0; i < 3; i++)
        imprime_fila(&filas[i], nomes[i]);     // filas[i] é struct: com &
}

// Simulador de distribuicao
int simular_distribuicao(Fila *entrada, Fila filas[]){
    int ciclo = 1;
    int qtd;

    if (entrada ->inicio == NULL){
        printf("Fila de entrada vazia! Carregue o arquivo primeiro.\n");
        return 0;
    }

    while (entrada->inicio != NULL){
        qtd = distribui(entrada, filas, QTD_POR_CICLO);

        printf("\n========== CICLO %d ==========\n", ciclo);
        printf("%d solicitacoes distribuidas\n", qtd);

        imprime_fila(entrada, "Entrada");
        imprime_fila(&filas[0], "Fila 0");
        imprime_fila(&filas[1], "Fila 1");
        imprime_fila(&filas[2], "Fila 2");

        // pausar
        espera(INTERVALO_MS);
        ciclo++;
    }

    printf("\nDistribuicao concluida em %d ciclos!\n", ciclo - 1);
    return ciclo - 1;
}


// Pausa do programa de 0.5 segundos a cada distribuição
void espera(int ms){
    Sleep(ms);              // Windows: já recebe em milissegundos
}

// Executa um ciclo de distribuição: tira até 5 solicitações do início da
// Fila de Entrada e manda cada uma para a fila da sua prioridade.
int distribui(Fila *entrada, Fila filas[], int qtd){
    int prioridade; // vem da entrada
    char ip[16];
    int cont = 0;


    for(int i = 0; i < qtd; i++){
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