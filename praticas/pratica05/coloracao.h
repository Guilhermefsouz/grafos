#ifndef COLORACAO_H
#define COLORACAO_H

/* ---------- Grafo (lista de adjacencia, nao dirigido) ---------- */
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

/* ---------- Coloracao de vertices ---------- */

/* Algoritmo guloso simples: percorre os vertices na ordem 0..n-1 e
 * atribui a cada um a menor cor (0-indexada) ainda nao usada por seus
 * vizinhos ja coloridos. Retorna um vetor alocado dinamicamente com a
 * cor de cada vertice e escreve em *num_cores a quantidade de cores
 * distintas utilizadas. A ordem arbitraria (a propria ordem dos
 * indices) pode levar a mais cores do que o necessario. */
int *coloracao_gulosa(GrafoLista *g, int *num_cores);

/* Heuristica de Welsh-Powell: ordena os vertices por grau decrescente
 * e so entao aplica o mesmo algoritmo guloso. Costuma usar menos (ou
 * o mesmo numero de) cores que a ordem arbitraria. Mesma
 * assinatura/retorno de coloracao_gulosa. */
int *coloracao_welsh_powell(GrafoLista *g, int *num_cores);

/* Retorna 1 se o grafo e bipartido (numero cromatico <= 2), 0 caso
 * contrario. Usa 2-coloracao via BFS, cobrindo grafos desconexos. */
int eh_bipartido(GrafoLista *g);

#endif