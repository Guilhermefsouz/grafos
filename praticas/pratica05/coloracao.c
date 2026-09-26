#include <stdio.h>
#include <stdlib.h>
#include "coloracao.h"

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

static int grau(GrafoLista *g, int v) {
    int contador = 0;
    for (No *aux = g->adj[v]; aux != NULL; aux = aux->prox) {
        contador++;
    }
    return contador;
}

/* ==================== Algoritmo guloso ==================== */

/* Colore os vertices na ordem dada por "ordem" (um vetor com uma
 * permutacao de 0..n-1). Reutilizada tanto pela coloracao guloga
 * simples (ordem natural) quanto pela Welsh-Powell (ordem por grau
 * decrescente). */
static int *colorir_na_ordem(GrafoLista *g, const int *ordem, int *num_cores) {
    int n = g->n;
    int *cor = malloc((size_t) n * sizeof(int));
    int *usada = malloc((size_t) n * sizeof(int));

    for (int i = 0; i < n; i++) {
        cor[i] = -1;
    }

    int maior_cor_usada = -1;

    for (int i = 0; i < n; i++) {
        int v = ordem[i];

        for (int c = 0; c < n; c++) {
            usada[c] = 0;
        }

        for (No *aux = g->adj[v]; aux != NULL; aux = aux->prox) {
            int vizinho = aux->destino;
            if (cor[vizinho] != -1) {
                usada[cor[vizinho]] = 1;
            }
        }

        int c = 0;
        while (usada[c]) {
            c++;
        }

        cor[v] = c;
        if (c > maior_cor_usada) {
            maior_cor_usada = c;
        }
    }

    free(usada);
    *num_cores = maior_cor_usada + 1;
    return cor;
}

int *coloracao_gulosa(GrafoLista *g, int *num_cores) {
    int n = g->n;
    int *ordem = malloc((size_t) n * sizeof(int));

    for (int i = 0; i < n; i++) {
        ordem[i] = i;
    }

    int *cor = colorir_na_ordem(g, ordem, num_cores);

    free(ordem);
    return cor;
}

int *coloracao_welsh_powell(GrafoLista *g, int *num_cores) {
    int n = g->n;
    int *ordem = malloc((size_t) n * sizeof(int));
    int *graus = malloc((size_t) n * sizeof(int));

    for (int i = 0; i < n; i++) {
        ordem[i] = i;
        graus[i] = grau(g, i);
    }

    /* insertion sort estavel por grau decrescente: o numero de
     * vertices e pequeno nestas praticas, entao O(n^2) e suficiente */
    for (int i = 1; i < n; i++) {
        int v = ordem[i];
        int grau_v = graus[v];
        int j = i - 1;
        while (j >= 0 && graus[ordem[j]] < grau_v) {
            ordem[j + 1] = ordem[j];
            j--;
        }
        ordem[j + 1] = v;
    }

    int *cor = colorir_na_ordem(g, ordem, num_cores);

    free(ordem);
    free(graus);
    return cor;
}

/* ==================== Bipartição (BFS, 2-coloracao) ==================== */

int eh_bipartido(GrafoLista *g) {
    int n = g->n;
    int *cor = malloc((size_t) n * sizeof(int));
    int *fila = malloc((size_t) n * sizeof(int));

    for (int i = 0; i < n; i++) {
        cor[i] = -1;
    }

    int bipartido = 1;

    for (int s = 0; s < n && bipartido; s++) {
        if (cor[s] != -1) {
            continue;
        }

        int inicio = 0, fim = 0;
        cor[s] = 0;
        fila[fim++] = s;

        while (inicio < fim && bipartido) {
            int u = fila[inicio++];
            for (No *aux = g->adj[u]; aux != NULL && bipartido; aux = aux->prox) {
                int v = aux->destino;
                if (cor[v] == -1) {
                    cor[v] = 1 - cor[u];
                    fila[fim++] = v;
                } else if (cor[v] == cor[u]) {
                    bipartido = 0;
                }
            }
        }
    }

    free(cor);
    free(fila);
    return bipartido;
}