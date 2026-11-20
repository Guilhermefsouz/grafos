#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <limits.h>

#define INF INT_MAX

/* ---------- Grafo ponderado (lista de adjacência) ---------- */
typedef struct NoAdj {
    int destino;
    int peso;
    struct NoAdj *prox;
} NoAdj;

typedef struct {
    int V;          /* número de vértices */
    NoAdj **adj;    /* vetor de listas de adjacência */
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo_ponderado(int V);
void adicionar_aresta(GrafoListaPonderado *g, int origem, int destino, int peso);
void liberar_grafo(GrafoListaPonderado *g);

/* ---------- Bellman-Ford ---------- */

/* Vértice que foi relaxado na n-ésima iteração (-1 se não há ciclo negativo).
   É preenchido por bellman_ford() e usado em reconstruir_ciclo_negativo(). */
extern int bf_vertice_ciclo;

/* Retorna 1 (sucesso) ou 0 (ciclo de custo negativo alcançável da origem). */
int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/* Imprime o caminho origem -> destino usando o vetor de predecessores. */
void reconstruir_caminho(int origem, int destino, int *pred);

/* Retrocede n vezes pelo pred[] a partir de v (para cair dentro do ciclo)
   e imprime o ciclo negativo. */
void reconstruir_ciclo_negativo(int n, int *pred, int v);

#endif