#ifndef DAG_H
#define DAG_H

/* ---------- Digrafo (lista de adjacencia) ---------- */
typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;      /* numero de vertices */
    No **adj;   /* vetor de listas encadeadas */
} GrafoLista;

GrafoLista *criar_grafo_lista(int n);

/* Insere um arco dirigido u -> v (apenas um sentido, diferente da
 * aresta nao dirigida das praticas anteriores). */
void inserir_arco(GrafoLista *g, int u, int v);

void liberar_grafo_lista(GrafoLista *g);

/* ---------- DAG / ordenacao topologica ---------- */

/* Retorna 1 se o digrafo e acíclico (DAG), 0 se houver pelo menos um
 * ciclo. Usa DFS com 3 cores (branco/cinza/preto). */
int eh_dag(GrafoLista *g);

/* Algoritmo de Kahn (BFS com grau de entrada).
 * Retorna um vetor alocado dinamicamente com a ordenacao topologica e
 * escreve o numero de vertices em *tamanho.
 * Se o grafo tiver ciclo, retorna NULL e *tamanho = 0. */
int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho);

/* Ordenacao topologica via DFS: cada vertice e "empilhado" no momento
 * em que sua exploracao termina; a ordem topologica e o inverso dessa
 * pilha. Retorna vetor alocado dinamicamente e escreve o numero de
 * vertices em *tamanho. Se o grafo tiver ciclo, retorna NULL e
 * *tamanho = 0. */
int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho);

#endif