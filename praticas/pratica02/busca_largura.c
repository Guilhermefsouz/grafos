#include <stdio.h>
#include <stdlib.h>
#include "busca_largura.h"

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

/* Grafo nao orientado: a aresta e inserida nos dois sentidos. */
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

/* ==================== Fila (FIFO) para BFS ==================== */

Fila *fila_criar(int capacidade) {
    Fila *f = malloc(sizeof(Fila));
    f->dados = malloc((size_t) capacidade * sizeof(int));
    f->capacidade = capacidade;
    f->inicio = 0;
    f->fim = 0;
    f->tamanho = 0;
    return f;
}

int fila_vazia(Fila *f) {
    return f->tamanho == 0;
}

int fila_cheia(Fila *f) {
    return f->tamanho == f->capacidade;
}

void fila_inserir(Fila *f, int valor) {
    if (fila_cheia(f)) {
        return;
    }
    f->dados[f->fim] = valor;
    f->fim = (f->fim + 1) % f->capacidade;
    f->tamanho++;
}

int fila_remover(Fila *f) {
    if (fila_vazia(f)) {
        return -1;
    }
    int valor = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return valor;
}

void fila_liberar(Fila *f) {
    free(f->dados);
    free(f);
}

/* ==================== Algoritmos baseados em BFS ==================== */

void bfs(GrafoLista *g, int origem, int *dist, int *pred) {
    for (int i = 0; i < g->n; i++) {
        dist[i] = INFINITO;
        pred[i] = -1;
    }

    Fila *fila = fila_criar(g->n);
    dist[origem] = 0;
    fila_inserir(fila, origem);

    while (!fila_vazia(fila)) {
        int u = fila_remover(fila);
        for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
            int v = aux->destino;
            if (dist[v] == INFINITO) {
                dist[v] = dist[u] + 1;
                pred[v] = u;
                fila_inserir(fila, v);
            }
        }
    }

    fila_liberar(fila);
}

int eh_bipartido(GrafoLista *g) {
    int *cor = malloc((size_t) g->n * sizeof(int));
    for (int i = 0; i < g->n; i++) {
        cor[i] = -1;
    }

    Fila *fila = fila_criar(g->n);
    int bipartido = 1;

    for (int s = 0; s < g->n && bipartido; s++) {
        if (cor[s] != -1) {
            continue;
        }

        cor[s] = 0;
        fila_inserir(fila, s);

        while (!fila_vazia(fila) && bipartido) {
            int u = fila_remover(fila);
            for (No *aux = g->adj[u]; aux != NULL && bipartido; aux = aux->prox) {
                int v = aux->destino;
                if (cor[v] == -1) {
                    cor[v] = 1 - cor[u];
                    fila_inserir(fila, v);
                } else if (cor[v] == cor[u]) {
                    bipartido = 0;
                }
            }
        }
    }

    fila_liberar(fila);
    free(cor);
    return bipartido;
}

int contar_componentes(GrafoLista *g) {
    int *visitado = calloc((size_t) g->n, sizeof(int));
    int componentes = 0;

    Fila *fila = fila_criar(g->n);

    for (int s = 0; s < g->n; s++) {
        if (visitado[s]) {
            continue;
        }

        componentes++;
        visitado[s] = 1;
        fila_inserir(fila, s);

        while (!fila_vazia(fila)) {
            int u = fila_remover(fila);
            for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
                int v = aux->destino;
                if (!visitado[v]) {
                    visitado[v] = 1;
                    fila_inserir(fila, v);
                }
            }
        }
    }

    fila_liberar(fila);
    free(visitado);
    return componentes;
}