#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cobertura.h"

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

/* ====================== Verificação ====================== */

int eh_cobertura(Grafo *g, const int *cobertura) {
    for (int i = 0; i < g->n; i++)
        for (int j = i + 1; j < g->n; j++)
            if (g->adj[i][j] && !cobertura[i] && !cobertura[j])
                return 0;   /* aresta i-j sem cobertura */
    return 1;
}

/* ====================== Backtracking ====================== */

typedef struct {
    Grafo *g;
    int *atual;       /* cobertura parcial em construção */
    int *melhor;      /* melhor cobertura completa encontrada */
    int *usado;       /* área de trabalho do limite inferior */
    int melhor_tam;
    long nos;
} Contexto;

/* Acha a primeira aresta ainda não coberta. Retorna 0 se não há nenhuma. */
static int primeira_descoberta(Contexto *c, int *u, int *v) {
    Grafo *g = c->g;
    for (int i = 0; i < g->n; i++) {
        if (c->atual[i]) continue;
        for (int j = i + 1; j < g->n; j++) {
            if (g->adj[i][j] && !c->atual[j]) {
                *u = i;
                *v = j;
                return 1;
            }
        }
    }
    return 0;
}

/* Limite inferior: tamanho de um emparelhamento maximal (guloso) entre as
   arestas descobertas. Arestas disjuntas exigem vértices diferentes na
   cobertura, então qualquer solução precisa de pelo menos esse número. */
static int limite_inferior(Contexto *c) {
    Grafo *g = c->g;
    int total = 0;
    memset(c->usado, 0, g->n * sizeof(int));
    for (int i = 0; i < g->n; i++) {
        if (c->atual[i] || c->usado[i]) continue;
        for (int j = i + 1; j < g->n; j++) {
            if (g->adj[i][j] && !c->atual[j] && !c->usado[j]) {
                c->usado[i] = 1;
                c->usado[j] = 1;
                total++;
                break;
            }
        }
    }
    return total;
}

static void busca(Contexto *c, int tam) {
    c->nos++;

    /* Poda 1: já não é possível melhorar a melhor solução */
    if (tam >= c->melhor_tam) return;

    int u, v;
    if (!primeira_descoberta(c, &u, &v)) {
        /* Todas as arestas cobertas: nova melhor solução */
        c->melhor_tam = tam;
        memcpy(c->melhor, c->atual, c->g->n * sizeof(int));
        return;
    }

    /* Poda 2: limite inferior */
    if (tam + limite_inferior(c) >= c->melhor_tam) return;

    /* A aresta u-v precisa de u ou v na cobertura: ramifica nas duas opções */
    c->atual[u] = 1;
    busca(c, tam + 1);
    c->atual[u] = 0;

    c->atual[v] = 1;
    busca(c, tam + 1);
    c->atual[v] = 0;
}

int cobertura_minima_bt(Grafo *g, int *melhor, long *nos) {
    int n = g->n;
    Contexto c;
    c.g = g;
    c.atual = calloc(n > 0 ? n : 1, sizeof(int));
    c.usado = calloc(n > 0 ? n : 1, sizeof(int));
    c.melhor = melhor;
    c.nos = 0;

    /* Solução inicial válida: todos os vértices */
    for (int i = 0; i < n; i++) melhor[i] = 1;
    c.melhor_tam = n;

    if (c.atual && c.usado)
        busca(&c, 0);

    if (nos) *nos = c.nos;
    free(c.atual);
    free(c.usado);
    return c.melhor_tam;
}

/* ====================== Heurística gulosa ====================== */

int cobertura_gulosa(Grafo *g, int *cobertura) {
    int n = g->n;
    int tam = 0;

    for (int i = 0; i < n; i++) cobertura[i] = 0;

    for (;;) {
        int escolhido = -1, maior_grau = 0;

        /* grau restante = arestas ainda não cobertas incidentes em i */
        for (int i = 0; i < n; i++) {
            if (cobertura[i]) continue;
            int grau = 0;
            for (int j = 0; j < n; j++)
                if (g->adj[i][j] && !cobertura[j]) grau++;
            if (grau > maior_grau) {
                maior_grau = grau;
                escolhido = i;
            }
        }

        if (escolhido == -1) break;   /* nenhuma aresta restante */
        cobertura[escolhido] = 1;
        tam++;
    }
    return tam;
}