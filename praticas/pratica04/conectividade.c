#include <stdio.h>
#include <stdlib.h>
#include "conectividade.h"

/* ==================== Grafo (lista de adjacencia) ==================== */

GrafoLista *criar_grafo_lista(int n) {
    GrafoLista *g = malloc(sizeof(GrafoLista));
    g->n = n;
    g->adj = calloc((size_t) n, sizeof(No *));
    return g;
}

static void inserir_no(No **lista, int destino) {
    No *novo = malloc(sizeof(No));
    novo->destino = destino;
    novo->prox = *lista;
    *lista = novo;
}

void inserir_aresta_lista(GrafoLista *g, int u, int v) {
    inserir_no(&g->adj[u], v);
    inserir_no(&g->adj[v], u);
}

void liberar_grafo_lista(GrafoLista *g) {
    if (g == NULL) {
        return;
    }
    for (int i = 0; i < g->n; i++) {
        No *atual = g->adj[i];
        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(g->adj);
    free(g);
}

/* ==================== Vertices de corte (Tarjan) ==================== */

void dfs_articulacoes(GrafoLista *g, int u, int pai, int *visitado,
                       int *descoberta, int *low, int *tempo,
                       int *eh_articulacao) {
    visitado[u] = 1;
    (*tempo)++;
    descoberta[u] = *tempo;
    low[u] = *tempo;

    int filhos = 0;

    for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
        int v = aux->destino;

        if (!visitado[v]) {
            filhos++;
            dfs_articulacoes(g, v, u, visitado, descoberta, low, tempo, eh_articulacao);

            if (low[v] < low[u]) {
                low[u] = low[v];
            }

            /* u nao e raiz: e articulacao se algum filho nao consegue
             * "escalar" a arvore acima de u sem passar por u */
            if (pai != -1 && low[v] >= descoberta[u]) {
                eh_articulacao[u] = 1;
            }
        } else if (v != pai) {
            /* aresta de retorno */
            if (descoberta[v] < low[u]) {
                low[u] = descoberta[v];
            }
        }
    }

    /* u e raiz da arvore de busca: e articulacao se tiver mais de um
     * filho na arvore (desconectaria a busca em duas partes) */
    if (pai == -1 && filhos > 1) {
        eh_articulacao[u] = 1;
    }
}

int contar_articulacoes(GrafoLista *g, int *eh_articulacao) {
    int n = g->n;
    int *visitado = calloc((size_t) n, sizeof(int));
    int *descoberta = malloc((size_t) n * sizeof(int));
    int *low = malloc((size_t) n * sizeof(int));
    int tempo = 0;

    for (int i = 0; i < n; i++) {
        eh_articulacao[i] = 0;
    }

    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            dfs_articulacoes(g, i, -1, visitado, descoberta, low, &tempo, eh_articulacao);
        }
    }

    int total = 0;
    for (int i = 0; i < n; i++) {
        if (eh_articulacao[i]) {
            total++;
        }
    }

    free(visitado);
    free(descoberta);
    free(low);
    return total;
}

/* ==================== Pontes (arestas de corte) ==================== */

static void dfs_pontes(GrafoLista *g, int u, int pai, int *visitado,
                        int *descoberta, int *low, int *tempo,
                        Ponte *pontes, int *quantidade) {
    visitado[u] = 1;
    (*tempo)++;
    descoberta[u] = *tempo;
    low[u] = *tempo;

    for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
        int v = aux->destino;

        if (!visitado[v]) {
            dfs_pontes(g, v, u, visitado, descoberta, low, tempo, pontes, quantidade);

            if (low[v] < low[u]) {
                low[u] = low[v];
            }

            if (low[v] > descoberta[u]) {
                pontes[*quantidade].u = u;
                pontes[*quantidade].v = v;
                (*quantidade)++;
            }
        } else if (v != pai) {
            if (descoberta[v] < low[u]) {
                low[u] = descoberta[v];
            }
        }
    }
}

Ponte *detectar_pontes(GrafoLista *g, int *quantidade) {
    int n = g->n;
    int *visitado = calloc((size_t) n, sizeof(int));
    int *descoberta = malloc((size_t) n * sizeof(int));
    int *low = malloc((size_t) n * sizeof(int));
    int tempo = 0;

    /* o numero de pontes nunca ultrapassa n (elas formam uma floresta
     * ao contrair as componentes 2-aresta-conexas), entao um vetor de
     * tamanho n e sempre suficiente. */
    Ponte *pontes = malloc((size_t) n * sizeof(Ponte));
    *quantidade = 0;

    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            dfs_pontes(g, i, -1, visitado, descoberta, low, &tempo, pontes, quantidade);
        }
    }

    free(visitado);
    free(descoberta);
    free(low);
    return pontes;
}