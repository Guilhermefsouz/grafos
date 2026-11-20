#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

int bf_vertice_ciclo = -1;

/* ====================== Grafo ====================== */

GrafoListaPonderado *criar_grafo_ponderado(int V) {
    GrafoListaPonderado *g = malloc(sizeof(GrafoListaPonderado));
    if (!g) return NULL;
    g->V = V;
    g->adj = calloc(V, sizeof(NoAdj *));
    if (!g->adj) {
        free(g);
        return NULL;
    }
    return g;
}

/* Aresta direcionada origem -> destino (peso pode ser negativo). */
void adicionar_aresta(GrafoListaPonderado *g, int origem, int destino, int peso) {
    NoAdj *novo = malloc(sizeof(NoAdj));
    if (!novo) return;
    novo->destino = destino;
    novo->peso = peso;
    novo->prox = g->adj[origem];
    g->adj[origem] = novo;
}

void liberar_grafo(GrafoListaPonderado *g) {
    if (!g) return;
    for (int i = 0; i < g->V; i++) {
        NoAdj *p = g->adj[i];
        while (p) {
            NoAdj *prox = p->prox;
            free(p);
            p = prox;
        }
    }
    free(g->adj);
    free(g);
}

/* ====================== Bellman-Ford ====================== */

int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    int n = g->V;
    bf_vertice_ciclo = -1;

    for (int v = 0; v < n; v++) {
        dist[v] = INF;
        pred[v] = -1;
    }
    dist[origem] = 0;

    /* n-1 iterações de relaxação de todas as arestas */
    for (int i = 1; i <= n - 1; i++) {
        int mudou = 0;
        for (int u = 0; u < n; u++) {
            if (dist[u] == INF) continue;   /* ainda não alcançado */
            for (NoAdj *p = g->adj[u]; p != NULL; p = p->prox) {
                if (dist[u] + p->peso < dist[p->destino]) {
                    dist[p->destino] = dist[u] + p->peso;
                    pred[p->destino] = u;
                    mudou = 1;
                }
            }
        }
        if (!mudou) return 1;   /* convergiu antes: sem ciclo negativo */
    }

    /* n-ésima iteração: se ainda relaxa, existe ciclo negativo */
    for (int u = 0; u < n; u++) {
        if (dist[u] == INF) continue;
        for (NoAdj *p = g->adj[u]; p != NULL; p = p->prox) {
            if (dist[u] + p->peso < dist[p->destino]) {
                dist[p->destino] = dist[u] + p->peso;
                pred[p->destino] = u;
                bf_vertice_ciclo = p->destino;
                return 0;
            }
        }
    }

    return 1;
}

/* Imprime o caminho mínimo origem -> destino */
void reconstruir_caminho(int origem, int destino, int *pred) {
    if (destino == origem) {
        printf("%d", origem);
        return;
    }
    if (pred[destino] == -1) {
        printf("(sem caminho)");
        return;
    }
    reconstruir_caminho(origem, pred[destino], pred);
    printf(" -> %d", destino);
}

/* Retrocede n vezes por pred[] a partir de v: com isso o vértice x
   com certeza está DENTRO do ciclo. Depois percorre pred[] até voltar
   em x e imprime o ciclo na ordem correta das arestas. */
void reconstruir_ciclo_negativo(int n, int *pred, int v) {
    if (v < 0) {
        printf("Nenhum ciclo negativo.\n");
        return;
    }

    int x = v;
    for (int i = 0; i < n; i++) {
        x = pred[x];
        if (x == -1) {
            printf("Nao foi possivel reconstruir o ciclo.\n");
            return;
        }
    }

    int *ciclo = malloc(n * sizeof(int));
    if (!ciclo) return;

    int tam = 0;
    int y = x;
    do {
        ciclo[tam++] = y;
        y = pred[y];
    } while (y != x && y != -1 && tam < n);

    /* pred[] anda "para trás", então imprimimos invertido */
    printf("Ciclo negativo: ");
    for (int i = tam - 1; i >= 0; i--) {
        printf("%d -> ", ciclo[i]);
    }
    printf("%d\n", ciclo[tam - 1]);

    free(ciclo);
}