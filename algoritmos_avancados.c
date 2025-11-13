// Detective Quest – Árvore Binária de Salas (Mapa da Mansão)
// Nível: Novato (árvore fixa, exploração interativa)
// Funcionalidades: criar salas dinamicamente, montar o mapa, explorar com e/d/s, exibir salas visitadas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --------- Estruturas ---------
// Nó da árvore binária representando um cômodo da mansão
typedef struct Sala {
    char nome[64];
    struct Sala* esq; // caminho à esquerda
    struct Sala* dir; // caminho à direita
} Sala;

// --------- Utilidades de I/O ---------
static void limparBuffer(void) {
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
}

// --------- Criação e liberação ---------
// criaSala: aloca dinamicamente uma sala e copia seu nome
Sala* criarSala(const char* nome) {
    Sala* s = (Sala*)malloc(sizeof(Sala));
    if (!s) { perror("malloc"); exit(1); }
    strncpy(s->nome, nome, sizeof(s->nome) - 1);
    s->nome[sizeof(s->nome) - 1] = '\0';
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

// --------- Exploração ---------
// explorarSalas: navega a partir de 'atual' até um nó-folha ou saída do jogador
void explorarSalas(Sala* atual) {
    if (!atual) return;
    printf("Bem-vindo(a) ao Detective Quest!\n");
    Sala* sala = atual;
    while (sala) {
        int temEsq = sala->esq != NULL;
        int temDir = sala->dir != NULL;
        printf("\nVoce esta em: %s\n", sala->nome);
        if (!temEsq && !temDir) {
            printf("Este comodo nao possui mais caminhos. Exploracao encerrada.\n");
            break;
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
    Sala* hall = criarSala("Hall de Entrada");
    // Nível 1
    Sala* salaEstar = criarSala("Sala de Estar");
    Sala* jardim    = criarSala("Jardim");
    hall->esq = salaEstar;
    hall->dir = jardim;
    // Nível 2 - à esquerda
    salaEstar->esq = criarSala("Cozinha");
    salaEstar->dir = criarSala("Biblioteca");
    // Nível 2 - à direita
    jardim->esq = criarSala("Garagem");
    jardim->dir = criarSala("Escritorio");
    return hall;
}

// --------- Programa principal ---------
int main(void) {
    Sala* mapa = montarMapa();
    explorarSalas(mapa);
    liberarArvore(mapa);
    return 0;
}

