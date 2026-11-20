#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <limits.h>

/* Valor usado para "distancia infinita" / vertice inalcancavel */
#define INF INT_MAX

/* ------------------------------------------------------------------ */
/* Grafo ponderado com lista de adjacencia                            */
/* (se a sua Pratica anterior ja define GrafoListaPonderado, adapte   */
/*  os campos usados em dijkstra.c: num_vertices, adj, destino, peso) */
/* ------------------------------------------------------------------ */
typedef struct NoAdj {
    int destino;
    int peso;
    struct NoAdj *prox;
} NoAdj;

typedef struct {
    int num_vertices;
    NoAdj **adj;            /* adj[u] = lista de arestas que saem de u */
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo(int num_vertices);
int  adicionar_aresta(GrafoListaPonderado *g, int u, int v, int peso);     /* dirigida */
int  adicionar_aresta_bidirecional(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo(GrafoListaPonderado *g);

/* ------------------------------------------------------------------ */
/* Min-Heap (fila de prioridade) - estruturas obrigatorias            */
/* ------------------------------------------------------------------ */
typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao;           /* mapeia vertice -> indice no heap (-1 = fora do heap) */
} MinHeap;

MinHeap *criar_min_heap(int capacidade);
int      inserir(MinHeap *h, int vertice, int distancia);               /* insert       */
NoHeap   extrair_minimo(MinHeap *h);                                    /* extract_min  */
void     diminuir_chave(MinHeap *h, int vertice, int nova_distancia);   /* decrease_key */
int      heap_vazio(const MinHeap *h);
void     liberar_min_heap(MinHeap *h);

/* ------------------------------------------------------------------ */
/* Dijkstra                                                           */
/* ------------------------------------------------------------------ */

/* Preenche dist[] e pred[] (ambos com g->num_vertices posicoes).
 * Retorna 0 em caso de sucesso e -1 em caso de erro
 * (parametros invalidos, falta de memoria ou aresta com peso negativo). */
int dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/* Imprime o caminho minimo origem -> destino usando pred[].
 * Retorna a quantidade de vertices no caminho, ou 0 se nao existe caminho. */
int reconstruir_caminho(int origem, int destino, int *pred);

#endif /* DIJKSTRA_H */