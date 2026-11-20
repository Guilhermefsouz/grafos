#include <stdio.h>
#include <stdlib.h>
#include "hamiltoniano.h"

/* ====================== Grafo ====================== */

Grafo *criar_grafo(int n) {
    Grafo *g = malloc(sizeof(Grafo));
    if (!g) return NULL;
    g->n = n;
    g->m = 0;
    g->adj = malloc(n * sizeof(int *));
    if (!g->adj) {
        free(g);
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        g->adj[i] = calloc(n, sizeof(int));
        if (!g->adj[i]) {
            for (int j = 0; j < i; j++) free(g->adj[j]);
            free(g->adj);
            free(g);
            return NULL;
        }
    }
    return g;
}

void adicionar_aresta(Grafo *g, int u, int v) {
    if (u == v || g->adj[u][v]) return;   /* ignora laço e aresta repetida */
    g->adj[u][v] = 1;
    g->adj[v][u] = 1;
    g->m++;
}

void liberar_grafo(Grafo *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj);
    free(g);
}

/* Gerador congruencial próprio: dá o mesmo grafo em qualquer sistema. */
static unsigned int proximo(unsigned int *estado) {
    *estado = *estado * 1103515245u + 12345u;
    return (*estado >> 16) & 0x7fff;
}

Grafo *gerar_grafo_aleatorio(int n, int percentual, unsigned int semente) {
    Grafo *g = criar_grafo(n);
    if (!g) return NULL;
    unsigned int estado = semente;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if ((int)(proximo(&estado) % 100) < percentual)
                adicionar_aresta(g, i, j);
    return g;
}

/* ====================== Caminho Hamiltoniano ====================== */

/* Estende o caminho parcial caminho[0..pos-1]. Cada chamada tenta, em ordem,
   os vértices ainda não usados e adjacentes ao último (gera as permutações
   válidas e poda os ramos sem aresta). */
static int estender_caminho(Grafo *g, int *caminho, int *visitado, int pos) {
    if (pos == g->n) return 1;   /* todos os vértices usados */

    int ultimo = caminho[pos - 1];
    for (int v = 0; v < g->n; v++) {
        if (!visitado[v] && g->adj[ultimo][v]) {
            visitado[v] = 1;
            caminho[pos] = v;
            if (estender_caminho(g, caminho, visitado, pos + 1)) return 1;
            visitado[v] = 0;   /* desfaz (backtrack) */
        }
    }
    return 0;
}

int encontrar_caminho_hamiltoniano_bt(Grafo *g, int *caminho) {
    int n = g->n;
    if (n <= 0) return 0;

    int *visitado = calloc(n, sizeof(int));
    if (!visitado) return 0;

    int achou = 0;
    for (int inicio = 0; inicio < n && !achou; inicio++) {
        visitado[inicio] = 1;
        caminho[0] = inicio;
        achou = estender_caminho(g, caminho, visitado, 1);
        visitado[inicio] = 0;
    }

    free(visitado);
    return achou;
}

/* ====================== Ciclo Hamiltoniano ====================== */

static int estender_ciclo(Grafo *g, int *ciclo, int *visitado, int pos) {
    if (pos == g->n)
        return g->adj[ciclo[pos - 1]][ciclo[0]];   /* precisa fechar o ciclo */

    int ultimo = ciclo[pos - 1];
    for (int v = 1; v < g->n; v++) {
        if (!visitado[v] && g->adj[ultimo][v]) {
            visitado[v] = 1;
            ciclo[pos] = v;
            if (estender_ciclo(g, ciclo, visitado, pos + 1)) return 1;
            visitado[v] = 0;
        }
    }
    return 0;
}

int encontrar_ciclo_hamiltoniano_bt(Grafo *g, int *ciclo) {
    int n = g->n;
    if (n < 3) return 0;   /* ciclo simples exige ao menos 3 vértices */

    int *visitado = calloc(n, sizeof(int));
    if (!visitado) return 0;

    /* Todo ciclo passa por todos os vértices: basta começar em 0 */
    visitado[0] = 1;
    ciclo[0] = 0;
    int achou = estender_ciclo(g, ciclo, visitado, 1);

    free(visitado);
    return achou;
}