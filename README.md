# Fila de Impressão com Prioridades

Simulador do gerenciamento de uma impressora compartilhada em rede, feito em **C** com **filas de alocação encadeada**.

Trabalho prático da disciplina de **Estrutura de Dados**, do curso de Tecnologia em Análise e Desenvolvimento de Sistemas (UFPR). Corresponde à parte **Aplicação de Fila** do 2º Trabalho Prático (Pilhas e Filas).

---

## O problema

Em uma rede, vários computadores enviam documentos para a mesma impressora. Para organizar esses pedidos, o sistema operacional usa **4 filas**:

| Fila | Recebe |
|---|---|
| **Fila de Entrada** | todas as solicitações, na ordem em que chegaram |
| **Fila 0** | solicitações de prioridade **máxima** (1) |
| **Fila 1** | solicitações de prioridade **normal** (2) |
| **Fila 2** | solicitações de prioridade **baixa** (3) |

A cada **0,5 segundo**, o programa retira **5 solicitações** do início da Fila de Entrada e distribui cada uma para a fila da sua prioridade, mostrando na tela a situação das 4 filas a cada ciclo.

```
 solicitacoes.txt
        │  (carregado no início, mínimo de 50)
        ▼
 ┌─────────────────┐
 │ Fila de Entrada │
 └────────┬────────┘
          │  a cada 0,5 s, retira 5 do início
          ▼
   qual a prioridade?
   ├── 1 (máxima) ──► Fila 0
   ├── 2 (normal) ──► Fila 1
   └── 3 (baixa)  ──► Fila 2
          │
          ▼
 imprime as 4 filas → repete até a entrada esvaziar
```

---

## Como compilar e executar

> ⚠️ **O programa foi desenvolvido para Windows.** A pausa de 0,5 s usa a função `Sleep`, da biblioteca `windows.h`, que não existe no Linux nem no macOS.

Com o GCC (MinGW) instalado, na pasta do projeto:

```bash
gcc -Wall impressora.c -o impressora
impressora.exe
```

O arquivo `solicitacoes.txt` precisa estar na **mesma pasta de onde o programa é executado**.

---

## Como usar

```
===== SIMULADOR DA FILA DE IMPRESSAO =====
1. Gerar arquivo de solicitacoes
2. Carregar solicitacoes do arquivo
3. Ver situacao das filas
4. Iniciar distribuicao
5. Limpar todas as filas
0. Sair
```

| Opção | O que faz |
|---|---|
| **1** | Cria um `solicitacoes.txt` novo, com solicitações aleatórias (mínimo de 50). **Substitui** o arquivo existente |
| **2** | Esvazia todas as filas e carrega a Fila de Entrada a partir do `solicitacoes.txt` |
| **3** | Mostra a situação atual das 4 filas |
| **4** | Inicia a distribuição: 5 solicitações a cada 0,5 s, até a entrada esvaziar |
| **5** | Esvazia as 4 filas, liberando a memória |
| **0** | Libera toda a memória e encerra o programa |

**Fluxo típico:** `1` (gerar) → `2` (carregar) → `4` (distribuir).
Para usar um arquivo próprio, coloque-o na pasta com o nome `solicitacoes.txt` e comece pela opção `2`.

---

## Formato do arquivo de entrada

Cada solicitação é formada por **prioridade** e **identificação** (o IP do computador), separadas por vírgula. As solicitações são separadas por ponto e vírgula:

```
2, 192.168.0.6; 1, 192.168.0.8; 3, 192.168.0.2; 2, 192.168.0.14; 1, 192.168.0.9;
3, 192.168.0.11; 1, 192.168.0.2; 2, 192.168.0.14; 3, 192.168.0.1; 2, 192.168.0.5;
```

- Espaços extras e quebras de linha entre as solicitações são aceitos
- A última solicitação pode terminar com `;` ou com `.`
- Prioridades diferentes de 1, 2 ou 3 são ignoradas, com um aviso na tela
- O arquivo precisa ter **no mínimo 50 solicitações válidas**; caso contrário, a carga é cancelada

### Gerador de solicitações (opção 1)

Para simular pessoas reais usando a impressora, o gerador considera uma rede com **15 computadores** (`192.168.0.1` a `192.168.0.15`). A cada solicitação, ele sorteia um computador e uma prioridade. Assim, o mesmo computador pode aparecer várias vezes, como alguém que manda vários documentos.

O endereço `192.168.0.0` não é usado porque, em uma rede `/24`, ele identifica a própria rede, e não um computador.

---

## Estruturas de dados

### O nodo: `Solicitacao`

```c
typedef struct solicitacao {
    char ip[16];                 // identificação: IP de quem pediu
    int prioridade;              // 1 = máxima, 2 = normal, 3 = baixa
    struct solicitacao *prox;    // endereço do próximo nodo da fila
} Solicitacao;
```

O IP usa 16 posições porque o maior endereço possível (`255.255.255.255`) tem 15 caracteres, mais o `\0` que termina a string.

### O cabeçalho: `Fila`

```c
typedef struct fila {
    Solicitacao *inicio;         // primeiro da fila (próximo a sair)
    Solicitacao *fim;            // último da fila (onde o novo entra)
    int total;                   // quantidade de solicitações na fila
} Fila;
```

```
 Fila
 ┌─────────────┐
 │ inicio ─────┼──► [1 | 192.168.0.10]──► [2 | 192.168.0.11]──► [3 | 192.168.0.12]──► NULL
 │ fim ────────┼──────────────────────────────────────────────────▲
 │ total = 3   │
 └─────────────┘
```

**Por que guardar o `fim`?** Uma fila insere pelo fim e remove pelo início (**FIFO**: *First In, First Out*). Sem o ponteiro `fim`, cada inserção teria que percorrer a fila inteira para achar o último nodo. Com ele, inserir e remover são operações imediatas, que não dependem do tamanho da fila.

**Por que ponteiros, e não índices?** Na alocação encadeada, cada nodo é criado com `malloc` e fica em uma posição qualquer da memória. Não existe "posição 3" como em um vetor: a única forma de chegar a um nodo é pelo seu endereço.

### As filas de prioridade

As 3 filas de prioridade são guardadas em um vetor: `Fila filas[3]`. Como as prioridades vão de 1 a 3 e os índices de 0 a 2, a fila de destino de uma solicitação é simplesmente:

```c
filas[prioridade - 1]
```

Isso dispensa uma sequência de `if`s e permite inicializar, imprimir e liberar as 3 filas com um único `for`.

---

## Funções

### Operações da fila

| Função | O que faz |
|---|---|
| `inicializa_fila(Fila *f)` | Deixa a fila vazia: `inicio` e `fim` em `NULL`, `total` em 0 |
| `cria_nodo(int prioridade, char *ip)` | Aloca um nodo com `malloc`, preenche seus campos e retorna o endereço. Encerra o programa se a alocação falhar |
| `enfileira(Fila *f, int prioridade, char *ip)` | Insere uma solicitação no **fim** da fila |
| `desenfileira(Fila *f, int *prioridade, char *ip)` | Remove a solicitação do **início** e devolve seus dados pelos parâmetros. Retorna 0 se a fila estiver vazia |
| `imprime_fila(Fila *f, char *nome)` | Mostra o nome, a quantidade e as solicitações da fila, do início ao fim |
| `liberar_fila(Fila *f)` | Libera todos os nodos e deixa a fila vazia |

### Funções da aplicação

| Função | O que faz |
|---|---|
| `gera_arquivo(char *nome_arquivo, int quantidade)` | Cria o arquivo de entrada com solicitações aleatórias |
| `carrega_arquivo(char *nome_arquivo, Fila *entrada)` | Lê o arquivo e enfileira cada solicitação válida na Fila de Entrada. Retorna a quantidade lida, ou -1 se o arquivo não existir |
| `distribui(Fila *entrada, Fila filas[], int qtd)` | Executa **um ciclo**: retira até `qtd` solicitações da entrada e manda cada uma para a fila da sua prioridade |
| `simular_distribuicao(Fila *entrada, Fila filas[])` | Repete os ciclos a cada 0,5 s até a entrada esvaziar, mostrando a situação das filas. Retorna o número de ciclos |
| `imprime_situacao(Fila *entrada, Fila filas[])` | Imprime as 4 filas |
| `espera(int ms)` | Pausa o programa por `ms` milissegundos |
| `limpa_buffer()` | Descarta o que sobrou no teclado, evitando que uma letra digitada no menu trave o programa |

---

## Como o algoritmo funciona

### Inserir no fim: `enfileira`

```
 Fila vazia:                          Fila com elementos:
 o novo é o primeiro E o último       1. o último atual aponta para o novo
                                      2. o novo passa a ser o último
 inicio ──► [novo] ──► NULL
 fim ───────┘                         inicio → [A] → [B] → [novo] → NULL
                                                             ▲
                                      fim ───────────────────┘
```

A ordem dos dois passos importa: se o `fim` fosse atualizado primeiro, o acesso ao nodo `B` seria perdido e ele não poderia mais ser ligado ao novo.

### Remover do início: `desenfileira`

1. Se a fila estiver vazia, retorna 0
2. Guarda o endereço do primeiro nodo em um auxiliar
3. Copia a prioridade e o IP para os parâmetros de saída (**antes** do `free`, senão os dados se perdem)
4. Avança o `inicio` para o próximo nodo
5. Se a fila ficou vazia, o `fim` também passa a ser `NULL`, para não apontar para memória já liberada
6. Libera o nodo, diminui o `total` e retorna 1

### Leitura do arquivo: `carrega_arquivo`

A leitura usa `fscanf` com o formato `" %d, %15[^;];"`:

| Parte | Significado |
|---|---|
| ` ` | pula espaços e quebras de linha |
| `%d` | lê a prioridade |
| `,` | consome a vírgula |
| `%15[^;]` | lê até 15 caracteres que não sejam `;` (o IP), sem estourar o vetor |
| `;` | consome o ponto e vírgula |

O `fscanf` retorna quantos campos conseguiu preencher. O laço continua enquanto ele retornar 2 (prioridade e IP lidos), e para quando o arquivo acaba.

### Distribuição: `distribui` e `simular_distribuicao`

```
enquanto a entrada não estiver vazia:          ← simular_distribuicao
    repete até 5 vezes:                        ← distribui
        desenfileira da entrada
            (se a entrada esvaziou, para)
        enfileira em filas[prioridade - 1]
    imprime as 4 filas
    espera 0,5 s
```

Como as duas operações são FIFO, a ordem de chegada é preservada dentro de cada fila de prioridade.

Com 50 solicitações, a distribuição leva **10 ciclos**. Se a quantidade não for múltipla de 5 (por exemplo, 52), o último ciclo distribui apenas o que restou (11 ciclos, sendo o último com 2).

### Pausa de 0,5 s: `espera`

O C padrão não tem uma função de `delay`, porque pausar o programa depende do sistema operacional. No Windows, a função é `Sleep(ms)`, da `windows.h`, que recebe o tempo em **milissegundos**: `Sleep(500)` equivale a 0,5 s.

No Linux, o equivalente seria `usleep(500000)`, da `unistd.h`, que recebe o tempo em **microssegundos**.

### Gerenciamento de memória

Todo nodo criado com `malloc` é liberado com `free`:
- ao remover da fila (`desenfileira`)
- ao limpar as filas (opção 5) ou recarregar o arquivo (opção 2)
- ao sair do programa (opção 0)

Em `liberar_fila`, o endereço do próximo nodo é guardado **antes** de liberar o atual, porque depois do `free` o campo `prox` não pode mais ser lido.

---

## Constantes

| Constante | Valor | Significado |
|---|---|---|
| `QTD_POR_CICLO` | 5 | solicitações distribuídas a cada ciclo |
| `INTERVALO_MS` | 500 | intervalo entre os ciclos, em milissegundos |
| `MIN_SOLICITACOES` | 50 | mínimo de solicitações exigido |
| `TOTAL_COMPUTADORES` | 15 | computadores da rede simulada pelo gerador |
| `ARQUIVO` | `"solicitacoes.txt"` | nome do arquivo de entrada |

---

## Estrutura do repositório

```
├── impressora.c        código-fonte
├── solicitacoes.txt    arquivo de entrada
├── README.md
├── .gitignore
└── .gitattributes
```

---

## Autor

**Felipe Fernandes de Azevedo**: Aplicação de Fila (fila de impressão)<br>
Professora: Dra. Andreia de Jesus
