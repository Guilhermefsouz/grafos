#include <stdio.h>
#include <stdlib.h>
#include "busca_profundidade.h"

/* ==================== Pilha (LIFO) para DFS iterativa ==================== */

Pilha *pilha_criar(int capacidade) {
    Pilha *p = malloc(sizeof(Pilha));
    p->dados = malloc((size_t) capacidade * sizeof(int));
    p->capacidade = capacidade;
    p->topo = -1;
    return p;
}

int pilha_vazia(Pilha *p) {
    return p->topo == -1;
}

int pilha_cheia(Pilha *p) {
    return p->topo == p->capacidade - 1;
}

void pilha_empilhar(Pilha *p, int valor) {
    if (pilha_cheia(p)) {
        return;
    }
    p->topo++;
    p->dados[p->topo] = valor;
}

int pilha_desempilhar(Pilha *p) {
    if (pilha_vazia(p)) {
        return -1;
    }
    int valor = p->dados[p->topo];
    p->topo--;
    return valor;
}

void pilha_liberar(Pilha *p) {
    free(p->dados);
    free(p);
}

/* ==================== DFS ==================== */

void dfs_recursiva(GrafoLista *g, int u, int *visitado, int *tempo,
                    int *descoberta, int *termino, int *pred) {
    visitado[u] = 1;
    (*tempo)++;
    descoberta[u] = *tempo;

    for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
        int v = aux->destino;
        if (!visitado[v]) {
            pred[v] = u;
            dfs_recursiva(g, v, visitado, tempo, descoberta, termino, pred);
        }
    }

    (*tempo)++;
    termino[u] = *tempo;
}

void dfs_iterativa(GrafoLista *g, int origem, int *visitado, int *pred) {
    Pilha *pilha = pilha_criar(g->n);
    pilha_empilhar(pilha, origem);

    while (!pilha_vazia(pilha)) {
        int u = pilha_desempilhar(pilha);

        if (visitado[u]) {
            continue;
        }
        visitado[u] = 1;

        for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
            int v = aux->destino;
            if (!visitado[v]) {
                pred[v] = u;
                pilha_empilhar(pilha, v);
            }
        }
    }

    pilha_liberar(pilha);
}

static int tem_ciclo_dfs(GrafoLista *g, int u, int pai, int *visitado) {
    visitado[u] = 1;

    for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
        int v = aux->destino;
        if (!visitado[v]) {
            if (tem_ciclo_dfs(g, v, u, visitado)) {
                return 1;
            }
        } else if (v != pai) {
            /* vizinho ja visitado que nao e o pai direto: ciclo */
            return 1;
        }
    }

    return 0;
}

int tem_ciclo(GrafoLista *g) {
    int *visitado = calloc((size_t) g->n, sizeof(int));
    int encontrado = 0;

    for (int i = 0; i < g->n && !encontrado; i++) {
        if (!visitado[i]) {
            if (tem_ciclo_dfs(g, i, -1, visitado)) {
                encontrado = 1;
            }
        }
    }

    free(visitado);
    return encontrado;
}