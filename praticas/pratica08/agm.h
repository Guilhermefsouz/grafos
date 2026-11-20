#ifndef AGM_H
#define AGM_H

#include <limits.h>

#define INF INT_MAX

/* ---------- Grafo ponderado NÃO direcionado (lista de adjacência) ---------- */
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
/* Insere a aresta nos dois sentidos (grafo não direcionado). */
void adicionar_aresta(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo(GrafoListaPonderado *g);

/* ---------- Estruturas obrigatórias ---------- */
typedef struct {
    int *pai, *rank;
    int n;
} DSU;

typedef struct {
    int u, v, peso;
} Aresta;

/* ---------- Union-Find ---------- */
DSU *criar_dsu(int n);
void liberar_dsu(DSU *d);
int find(DSU *d, int x);              /* com compressão de caminho */
int unir(DSU *d, int x, int y);       /* união por rank; 1 se uniu, 0 se já estavam juntos */

/* ---------- Kruskal ---------- */
/* Gera a lista de arestas (cada aresta uma única vez) a partir do grafo.
   Retorna vetor alocado (liberar com free) e o tamanho em *m. */
Aresta *extrair_arestas(GrafoListaPonderado *g, int *m);

/* Ordena 'arestas' (in-place) e seleciona as arestas da AGM em 'mst'
   (vetor com espaço para n-1 arestas). Retorna o peso total e a
   quantidade de arestas escolhidas em *qtd. Se o grafo for desconexo,
   o resultado é uma floresta geradora mínima (*qtd < n-1). O(E log E). */
int kruskal(int n, Aresta *arestas, int m, Aresta *mst, int *qtd);

/* ---------- Min-Heap (usado pelo Prim) ---------- */
typedef struct {
    int vertice;
    int distancia;   /* chave */
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao;    /* vértice -> índice no heap (-1 se fora do heap) */
} MinHeap;

MinHeap *criar_min_heap(int capacidade);
void inserir(MinHeap *h, int vertice, int distancia);
NoHeap extrair_minimo(MinHeap *h);
void diminuir_chave(MinHeap *h, int vertice, int nova_chave);
int heap_vazio(MinHeap *h);
void liberar_min_heap(MinHeap *h);

/* ---------- Prim ---------- */
/* Preenche pai[] (-1 para a raiz/inalcançados) e chave[] (peso da aresta
   que liga o vértice ao pai). Retorna o peso total da árvore da
   componente de 'origem'. O((V+E) log V). */
int prim(GrafoListaPonderado *g, int origem, int *pai, int *chave);

#endif