// Detective Quest – Árvore de Salas + Coleta de Pistas (BST)
// Nível: Aventureiro (mapa fixo + árvore BST de pistas)
// Funcionalidades: criar salas com pistas, explorar com e/d/s, coletar pistas automaticamente,
// armazenar em BST e exibir pistas coletadas em ordem alfabética ao final.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// --------- Estruturas ---------
// Nó da árvore binária representando um cômodo da mansão
typedef struct Sala {
    char nome[64];
    char pista[128]; // pista opcional deste cômodo (string vazia = sem pista)
    struct Sala* esq; // caminho à esquerda
    struct Sala* dir; // caminho à direita
} Sala;

// Nó da BST de pistas coletadas
typedef struct PistaNode {
    char texto[128];
    struct PistaNode* esq;
    struct PistaNode* dir;
} PistaNode;

// --------- Tabela Hash para pistas->suspeitos ---------
typedef struct HashEntry {
    char chave[128];
    char suspeito[64];
    struct HashEntry* prox;
} HashEntry;

#define TAM_HASH 101
typedef struct HashTable {
    HashEntry* buckets[TAM_HASH];
} HashTable;

// --------- Utilidades de I/O ---------
static void limparBuffer(void) {
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
}

static void removerNovaLinha(char* s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n && s[n-1] == '\n') s[n-1] = '\0';
}

// --------- Criação e liberação ---------
// criarSala() – cria dinamicamente um cômodo com nome e pista opcional.
// Responsabilidade: encapsula malloc e a inicialização de campos, garantindo strings terminadas em '\0'.
Sala* criarSala(const char* nome, const char* pista) {
    Sala* s = (Sala*)malloc(sizeof(Sala));
    if (!s) { perror("malloc"); exit(1); }
    strncpy(s->nome, nome, sizeof(s->nome) - 1);
    s->nome[sizeof(s->nome) - 1] = '\0';
    if (pista && *pista) {
        strncpy(s->pista, pista, sizeof(s->pista) - 1);
        s->pista[sizeof(s->pista) - 1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esq = s->dir = NULL;
    return s;
}

// liberarArvore: libera recursivamente todos os nós da árvore
void liberarArvore(Sala* raiz) {
    if (!raiz) return;
    liberarArvore(raiz->esq);
    liberarArvore(raiz->dir);
    free(raiz);
}

// libera BST de pistas
void liberarPistas(PistaNode* r) {
    if (!r) return;
    liberarPistas(r->esq);
    liberarPistas(r->dir);
    free(r);
}

// --------- BST de Pistas ---------
// inserirPista() – insere a pista coletada na árvore de pistas (BST).
// Regras: ignora duplicatas; mantém a ordenação lexicográfica via strcmp.
// Retorna 1 se inseriu, 0 se duplicata ou string vazia.
int inserirPista(PistaNode** r, const char* texto) {
    if (!texto || !*texto) return 0;
    if (!*r) {
        PistaNode* n = (PistaNode*)malloc(sizeof(PistaNode));
        if (!n) { perror("malloc"); exit(1); }
        strncpy(n->texto, texto, sizeof(n->texto) - 1);
        n->texto[sizeof(n->texto) - 1] = '\0';
        n->esq = n->dir = NULL;
        *r = n;
        return 1;
    }
    int cmp = strcmp(texto, (*r)->texto);
    if (cmp == 0) return 0;           // duplicata
    if (cmp < 0) return inserirPista(&(*r)->esq, texto);
    return inserirPista(&(*r)->dir, texto);
}

// adicionarPista() – alias para inserirPista(), mantido para aderir à especificação.
int adicionarPista(PistaNode** r, const char* texto) {
    return inserirPista(r, texto);
}

// Exibe pistas em-ordem (alfabética)
void exibirPistas(const PistaNode* r) {
    if (!r) return;
    exibirPistas(r->esq);
    printf("- %s\n", r->texto);
    exibirPistas(r->dir);
}

// --------- Hash: pistas -> suspeitos ---------
// Funções: inserirNaHash(), encontrarSuspeito()

static unsigned long hash_str(const char* s) {
    // djb2
    unsigned long h = 5381; int c;
    while ((c = (unsigned char)*s++)) h = ((h << 5) + h) + c;
    return h;
}

// inicializa a tabela hash
static void inicializarHash(HashTable* h) {
    for (int i = 0; i < TAM_HASH; ++i) h->buckets[i] = NULL;
}

// inserirNaHash() – insere associação pista/suspeito (chave/valor) na tabela.
// Em caso de chave já existente, substitui o suspeito.
void inserirNaHash(HashTable* h, const char* chave, const char* suspeito) {
    if (!h || !chave || !*chave || !suspeito || !*suspeito) return;
    unsigned long idx = hash_str(chave) % TAM_HASH;
    HashEntry* e = h->buckets[idx];
    while (e) {
        if (strcmp(e->chave, chave) == 0) {
            strncpy(e->suspeito, suspeito, sizeof(e->suspeito) - 1);
            e->suspeito[sizeof(e->suspeito) - 1] = '\0';
            return;
        }
        e = e->prox;
    }
    HashEntry* novo = (HashEntry*)malloc(sizeof(HashEntry));
    if (!novo) { perror("malloc"); exit(1); }
    strncpy(novo->chave, chave, sizeof(novo->chave) - 1);
    novo->chave[sizeof(novo->chave) - 1] = '\0';
    strncpy(novo->suspeito, suspeito, sizeof(novo->suspeito) - 1);
    novo->suspeito[sizeof(novo->suspeito) - 1] = '\0';
    novo->prox = h->buckets[idx];
    h->buckets[idx] = novo;
}

// encontrarSuspeito() – consulta o suspeito correspondente à pista (chave).
// Retorna ponteiro para string armazenada na hash ou NULL se não encontrado.
const char* encontrarSuspeito(const HashTable* h, const char* chave) {
    if (!h || !chave || !*chave) return NULL;
    unsigned long idx = hash_str(chave) % TAM_HASH;
    HashEntry* e = h->buckets[idx];
    while (e) {
        if (strcmp(e->chave, chave) == 0) return e->suspeito;
        e = e->prox;
    }
    return NULL;
}

static void liberarHash(HashTable* h) {
    if (!h) return;
    for (int i = 0; i < TAM_HASH; ++i) {
        HashEntry* e = h->buckets[i];
        while (e) {
            HashEntry* nx = e->prox;
            free(e);
            e = nx;
        }
        h->buckets[i] = NULL;
    }
}

// --------- Exploração ---------
// explorarSalas() – navega pela árvore e ativa o sistema de pistas.
// Responsabilidade: imprimir localização, coletar pista (se existir) e decidir próximo passo (e/d/s).
void explorarSalas(Sala* atual, PistaNode** bstPistas) {
    if (!atual) return;
    printf("Bem-vindo(a) ao Detective Quest!\n");
    Sala* sala = atual;
    while (sala) {
        int temEsq = sala->esq != NULL;
        int temDir = sala->dir != NULL;
        printf("\nVoce esta em: %s\n", sala->nome);

        // Coleta automática da pista (se existir)
        if (sala->pista[0]) {
            int novo = adicionarPista(bstPistas, sala->pista);
            printf("Pista encontrada: \"%s\"%s\n", sala->pista, novo ? " (adicionada)" : " (ja coletada)");
        } else {
            printf("Sem pista neste comodo.\n");
        }

        printf("Escolha o caminho: (e) esquerda%s | (d) direita%s | (s) sair\n",
               temEsq ? "" : " (indisponivel)",
               temDir ? "" : " (indisponivel)");
        printf("Opcao: ");
        int ch = getchar();
        limparBuffer();
        if (ch == 's' || ch == 'S') {
            printf("Saindo da exploracao.\n");
            break;
        } else if ((ch == 'e' || ch == 'E')) {
            if (temEsq) sala = sala->esq; else printf("Nao ha caminho a esquerda.\n");
        } else if ((ch == 'd' || ch == 'D')) {
            if (temDir) sala = sala->dir; else printf("Nao ha caminho a direita.\n");
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }
}

// --------- Julgamento Final ---------
// Conta, de forma recursiva, quantas pistas na BST implicam o suspeito acusado.
static int contarPistasDoSuspeito(const PistaNode* r, const HashTable* h, const char* suspeito) {
    if (!r) return 0;
    int soma = contarPistasDoSuspeito(r->esq, h, suspeito);
    const char* s = encontrarSuspeito(h, r->texto);
    if (s && strcasecmp(s, suspeito) == 0) soma += 1;
    soma += contarPistasDoSuspeito(r->dir, h, suspeito);
    return soma;
}

// verificarSuspeitoFinal() – conduz à fase de julgamento final.
// Mostra pistas coletadas, solicita acusação e verifica se >= 2 pistas sustentam a escolha.
void verificarSuspeitoFinal(const PistaNode* pistas, const HashTable* hash) {
    printf("\n=== Fase de Julgamento ===\n");
    printf("Pistas coletadas (ordem alfabetica):\n");
    if (!pistas) {
        printf("(nenhuma)\n");
        printf("Sem pistas, nenhuma acusacao pode ser sustentada.\n");
        return;
    }
    exibirPistas(pistas);
    printf("\nDigite o nome do suspeito a acusar: ");
    char acusado[64];
    if (!fgets(acusado, sizeof(acusado), stdin)) {
        printf("Entrada invalida.\n");
        return;
    }
    removerNovaLinha(acusado);
    int qt = contarPistasDoSuspeito(pistas, hash, acusado);
    if (qt >= 2) {
        printf("Acusacao de '%s' SUSTENTADA por %d pistas. Caso encerrado!\n", acusado, qt);
    } else {
        printf("Acusacao de '%s' NAO sustentada (apenas %d pista(s)). Continue investigando!\n", acusado, qt);
    }
}

// --------- Montagem do mapa ---------
// Monta manualmente a árvore da mansão (fixa) e retorna a raiz (Hall)
Sala* montarMapa(void) {
    // Nível 0
    Sala* hall = criarSala("Hall de Entrada", "Pegadas recentes no tapete.");
    // Nível 1
    Sala* salaEstar = criarSala("Sala de Estar", "Retrato torto na parede.");
    Sala* jardim    = criarSala("Jardim", "Terra revirada proxima ao canteiro.");
    hall->esq = salaEstar;
    hall->dir = jardim;
    // Nível 2 - à esquerda
    salaEstar->esq = criarSala("Cozinha", "Faca molhada na pia.");
    salaEstar->dir = criarSala("Biblioteca", "Livro fora de lugar.");
    // Nível 2 - à direita
    jardim->esq = criarSala("Garagem", "Chave inglesa sobre o banco.");
    jardim->dir = criarSala("Escritorio", "Janela aberta com cortina rasgada.");
    return hall;
}

// Popular associações pista->suspeito (regras codificadas)
static void popularHashSuspeitos(HashTable* h) {
    inserirNaHash(h, "Pegadas recentes no tapete.", "Intruso");
    inserirNaHash(h, "Retrato torto na parede.", "Morador");
    inserirNaHash(h, "Livro fora de lugar.", "Morador");
    inserirNaHash(h, "Faca molhada na pia.", "Cozinheiro");
    inserirNaHash(h, "Terra revirada proxima ao canteiro.", "Jardineiro");
    inserirNaHash(h, "Chave inglesa sobre o banco.", "Mecanico");
    inserirNaHash(h, "Janela aberta com cortina rasgada.", "Intruso");
}

// --------- Programa principal ---------
int main(void) {
    Sala* mapa = montarMapa();
    PistaNode* pistas = NULL;

    HashTable hash;
    inicializarHash(&hash);
    popularHashSuspeitos(&hash);

    explorarSalas(mapa, &pistas);

    verificarSuspeitoFinal(pistas, &hash);

    liberarArvore(mapa);
    liberarPistas(pistas);
    liberarHash(&hash);
    return 0;
}

