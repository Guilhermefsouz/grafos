#ifndef COBERTURA_H
#define COBERTURA_H

/* ---------- Grafo não direcionado e sem peso (matriz de adjacência) ---------- */
typedef struct {
    int n;        /* número de vértices */
    int m;        /* número de arestas */
    int **adj;    /* adj[u][v] = 1 se existe aresta u-v */
} Grafo;

Grafo *criar_grafo(int n);
void adicionar_aresta(Grafo *g, int u, int v);
void liberar_grafo(Grafo *g);

/* Grafo aleatório reprodutível: cada aresta existe com probabilidade
   'percentual'% (0 a 100). Mesma semente => mesmo grafo em qualquer sistema. */
Grafo *gerar_grafo_aleatorio(int n, int percentual, unsigned int semente);

/* ---------- Cobertura de vértices ---------- */
/* Os vetores 'cobertura' têm tamanho n: cobertura[v] = 1 se v foi escolhido. */

/* Retorna 1 se TODA aresta tem pelo menos uma extremidade na cobertura. */
int eh_cobertura(Grafo *g, const int *cobertura);

/* Força bruta com backtracking e poda por limite. Retorna o tamanho da
   cobertura MÍNIMA e a preenche em 'melhor'. Se 'nos' != NULL, devolve a
   quantidade de nós visitados na árvore de busca. */
int cobertura_minima_bt(Grafo *g, int *melhor, long *nos);

/* Heurística gulosa: escolhe sempre o vértice de maior grau restante.
   Retorna o tamanho da cobertura encontrada (não necessariamente mínima). */
int cobertura_gulosa(Grafo *g, int *cobertura);

#endif