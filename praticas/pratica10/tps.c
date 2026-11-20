#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "tps.h"

/* ====================== Instância ====================== */

TSP *criar_tsp(int n) {
    TSP *t = malloc(sizeof(TSP));
    if (!t) return NULL;
    t->n = n;
    t->c = malloc(n * sizeof(int *));
    if (!t->c) {
        free(t);
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        t->c[i] = calloc(n, sizeof(int));
        if (!t->c[i]) {
            for (int j = 0; j < i; j++) free(t->c[j]);
            free(t->c);
            free(t);
            return NULL;
        }
    }
    return t;
}

void liberar_tsp(TSP *t) {
    if (!t) return;
    for (int i = 0; i < t->n; i++) free(t->c[i]);
    free(t->c);
    free(t);
}

static unsigned int proximo(unsigned int *estado) {
    *estado = *estado * 1103515245u + 12345u;
    return (*estado >> 16) & 0x7fff;
}

TSP *gerar_tsp_aleatorio(int n, unsigned int semente) {
    TSP *t = criar_tsp(n);
    if (!t) return NULL;

    int *x = malloc(n * sizeof(int));
    int *y = malloc(n * sizeof(int));
    if (!x || !y) {
        free(x);
        free(y);
        liberar_tsp(t);
        return NULL;
    }

    unsigned int estado = semente;
    for (int i = 0; i < n; i++) {
        x[i] = (int)(proximo(&estado) % 100);
        y[i] = (int)(proximo(&estado) % 100);
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            t->c[i][j] = abs(x[i] - x[j]) + abs(y[i] - y[j]);

    free(x);
    free(y);
    return t;
}

TSP *grafo_para_tsp(Grafo *g) {
    TSP *t = criar_tsp(g->n);
    if (!t) return NULL;
    for (int i = 0; i < g->n; i++)
        for (int j = 0; j < g->n; j++)
            if (i != j)
                t->c[i][j] = g->adj[i][j] ? 1 : 2;
    return t;
}

/* ====================== Utilidades ====================== */

int custo_rota(TSP *t, const int *rota) {
    int n = t->n;
    if (n <= 1) return 0;
    int total = 0;
    for (int i = 0; i < n - 1; i++)
        total += t->c[rota[i]][rota[i + 1]];
    total += t->c[rota[n - 1]][rota[0]];   /* volta ao início */
    return total;
}

int rota_valida(TSP *t, const int *rota) {
    int n = t->n;
    int *visto = calloc(n > 0 ? n : 1, sizeof(int));
    if (!visto) return 0;
    int ok = 1;
    for (int i = 0; i < n; i++) {
        if (rota[i] < 0 || rota[i] >= n || visto[rota[i]]) {
            ok = 0;
            break;
        }
        visto[rota[i]] = 1;
    }
    free(visto);
    return ok;
}

/* ====================== Força bruta ====================== */

typedef struct {
    TSP *t;
    int *atual;
    int *melhor;
    int *usado;
    int melhor_custo;
} Contexto;

/* Gera todas as permutações das posições 1..n-1 (posição 0 fixa em 0),
   acumulando o custo do trecho já montado. */
static void permutar(Contexto *c, int pos, int custo) {
    int n = c->t->n;

    if (pos == n) {
        int total = custo + c->t->c[c->atual[n - 1]][0];   /* fecha o ciclo */
        if (total < c->melhor_custo) {
            c->melhor_custo = total;
            memcpy(c->melhor, c->atual, n * sizeof(int));
        }
        return;
    }

    for (int v = 1; v < n; v++) {
        if (!c->usado[v]) {
            c->usado[v] = 1;
            c->atual[pos] = v;
            permutar(c, pos + 1, custo + c->t->c[c->atual[pos - 1]][v]);
            c->usado[v] = 0;
        }
    }
}

int tsp_forca_bruta(TSP *t, int *rota) {
    int n = t->n;
    if (n <= 0) return 0;
    if (n == 1) {
        rota[0] = 0;
        return 0;
    }

    Contexto c;
    c.t = t;
    c.atual = calloc(n, sizeof(int));
    c.usado = calloc(n, sizeof(int));
    c.melhor = rota;
    c.melhor_custo = INT_MAX;
    if (!c.atual || !c.usado) {
        free(c.atual);
        free(c.usado);
        return 0;
    }

    c.atual[0] = 0;      /* origem fixa: (n-1)! permutações */
    c.usado[0] = 1;
    permutar(&c, 1, 0);

    free(c.atual);
    free(c.usado);
    return c.melhor_custo;
}

/* ====================== Vizinho mais próximo ====================== */

int tsp_vizinho_mais_proximo(TSP *t, int *rota) {
    int n = t->n;
    if (n <= 0) return 0;

    int *visitado = calloc(n, sizeof(int));
    if (!visitado) return 0;

    rota[0] = 0;
    visitado[0] = 1;

    for (int pos = 1; pos < n; pos++) {
        int atual = rota[pos - 1];
        int melhor = -1;
        for (int v = 0; v < n; v++) {
            if (!visitado[v] && (melhor == -1 || t->c[atual][v] < t->c[atual][melhor]))
                melhor = v;
        }
        rota[pos] = melhor;
        visitado[melhor] = 1;
    }

    free(visitado);
    return custo_rota(t, rota);
}

/* ====================== Inserção do mais próximo ====================== */

int tsp_insercao(TSP *t, int *rota) {
    int n = t->n;
    if (n <= 0) return 0;
    if (n == 1) {
        rota[0] = 0;
        return 0;
    }

    int *na_rota = calloc(n, sizeof(int));
    if (!na_rota) return 0;

    /* ciclo inicial: 0 e o vértice mais próximo dele */
    int tam = 1;
    rota[0] = 0;
    na_rota[0] = 1;

    int primeiro = -1;
    for (int v = 1; v < n; v++)
        if (primeiro == -1 || t->c[0][v] < t->c[0][primeiro])
            primeiro = v;
    rota[tam++] = primeiro;
    na_rota[primeiro] = 1;

    while (tam < n) {
        /* 1) vértice fora do ciclo mais próximo de algum vértice do ciclo */
        int k = -1, dist_k = INT_MAX;
        for (int v = 0; v < n; v++) {
            if (na_rota[v]) continue;
            for (int i = 0; i < tam; i++) {
                if (t->c[rota[i]][v] < dist_k) {
                    dist_k = t->c[rota[i]][v];
                    k = v;
                }
            }
        }

        /* 2) posição de menor aumento de custo: entre rota[p] e rota[p+1] */
        int melhor_p = 0, menor_aumento = INT_MAX;
        for (int p = 0; p < tam; p++) {
            int a = rota[p];
            int b = rota[(p + 1) % tam];
            int aumento = t->c[a][k] + t->c[k][b] - t->c[a][b];
            if (aumento < menor_aumento) {
                menor_aumento = aumento;
                melhor_p = p;
            }
        }

        /* 3) insere k logo após rota[melhor_p] */
        for (int j = tam; j > melhor_p + 1; j--)
            rota[j] = rota[j - 1];
        rota[melhor_p + 1] = k;
        na_rota[k] = 1;
        tam++;
    }

    free(na_rota);
    return custo_rota(t, rota);
}