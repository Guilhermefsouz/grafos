#ifndef HAMILTONIANO_H
#define HAMILTONIANO_H

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
   'percentual'% (0 a 100). */
Grafo *gerar_grafo_aleatorio(int n, int percentual, unsigned int semente);

/* ---------- Caminho / Ciclo Hamiltoniano (backtracking) ---------- */

/* Procura um caminho que visita todos os vértices exatamente uma vez.
   Retorna 1 e preenche caminho[0..n-1] se existir; retorna 0 caso contrário. */
int encontrar_caminho_hamiltoniano_bt(Grafo *g, int *caminho);

/* Procura um ciclo hamiltoniano (n >= 3): caminho hamiltoniano cujo último
   vértice é adjacente ao primeiro. Retorna 1 e preenche ciclo[0..n-1]
   (o ciclo se fecha de ciclo[n-1] de volta a ciclo[0]); 0 se não existir. */
int encontrar_ciclo_hamiltoniano_bt(Grafo *g, int *ciclo);

#endif