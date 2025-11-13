// Detective Quest – Árvore de Salas + Coleta de Pistas (BST)
// Nível: Aventureiro (mapa fixo + árvore BST de pistas)
// Funcionalidades: criar salas com pistas, explorar com e/d/s, coletar pistas automaticamente,
// armazenar em BST e exibir pistas coletadas em ordem alfabética ao final.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// --------- Utilidades de I/O ---------
static void limparBuffer(void) {
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
}

// --------- Criação e liberação ---------
// criaSala: aloca dinamicamente uma sala e copia seu nome e pista (opcional)
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
// Insere uma pista na BST (ignora duplicatas). Retorna 1 se inseriu, 0 se duplicata.
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

// Exibe pistas em-ordem (alfabética)
void exibirPistas(const PistaNode* r) {
    if (!r) return;
    exibirPistas(r->esq);
    printf("- %s\n", r->texto);
    exibirPistas(r->dir);
}

// --------- Exploração ---------
// explorarSalasComPistas: navega a partir de 'atual', coletando pistas, até o jogador optar por sair (s)
void explorarSalasComPistas(Sala* atual, PistaNode** bstPistas) {
    if (!atual) return;
    printf("Bem-vindo(a) ao Detective Quest!\n");
    Sala* sala = atual;
    while (sala) {
        int temEsq = sala->esq != NULL;
        int temDir = sala->dir != NULL;
        printf("\nVoce esta em: %s\n", sala->nome);

        // Coleta automática da pista (se existir)
        if (sala->pista[0]) {
            int novo = inserirPista(bstPistas, sala->pista);
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

// --------- Programa principal ---------
int main(void) {
    Sala* mapa = montarMapa();
    PistaNode* pistas = NULL;
    explorarSalasComPistas(mapa, &pistas);

    printf("\nPistas coletadas (ordem alfabetica):\n");
    if (!pistas) {
        printf("(nenhuma)\n");
    } else {
        exibirPistas(pistas);
    }

    liberarArvore(mapa);
    liberarPistas(pistas);
    return 0;
}

