#ifndef BUSCA_LARGURA_H
#define BUSCA_LARGURA_H

#define INFINITO -1

/* ---------- Grafo (lista de adjacência) ---------- */
typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;      /* numero de vertices */
    No **adj;   /* vetor de listas encadeadas */
} GrafoLista;

GrafoLista *criar_grafo_lista(int n);
void inserir_aresta_lista(GrafoLista *g, int u, int v);
void liberar_grafo_lista(GrafoLista *g);

/* ---------- Fila (FIFO) para BFS ---------- */
typedef struct {
    int *dados;
    int capacidade, inicio, fim, tamanho;
} Fila;

Fila *fila_criar(int capacidade);
int fila_vazia(Fila *f);
int fila_cheia(Fila *f);
void fila_inserir(Fila *f, int valor);
int fila_remover(Fila *f);
void fila_liberar(Fila *f);

/* ---------- Algoritmos baseados em BFS ---------- */

/* Calcula, a partir de origem, a distancia (numero de arestas) e o
 * predecessor de cada vertice. dist e pred devem ser vetores de
 * tamanho g->n previamente alocados pelo chamador.
 * Vertices nao alcancados ficam com dist[i] == INFINITO e pred[i] == -1. */
void bfs(GrafoLista *g, int origem, int *dist, int *pred);

/* Retorna 1 se o grafo (considerando todos os componentes) e
 * bipartido, 0 caso contrario. Usa 2-coloracao via BFS. */
int eh_bipartido(GrafoLista *g);

/* Retorna a quantidade de componentes conexos do grafo. */
int contar_componentes(GrafoLista *g);

#endif