#ifndef BUSCA_PROFUNDIDADE_H
#define BUSCA_PROFUNDIDADE_H

#include "busca_largura.h"

/* ---------- Pilha (LIFO) para DFS iterativa ---------- */
typedef struct {
    int *dados;
    int topo, capacidade;
} Pilha;

Pilha *pilha_criar(int capacidade);
int pilha_vazia(Pilha *p);
int pilha_cheia(Pilha *p);
void pilha_empilhar(Pilha *p, int valor);
int pilha_desempilhar(Pilha *p);
void pilha_liberar(Pilha *p);

/* ---------- DFS ---------- */

/* DFS recursiva classica, com tempos de descoberta/termino (estilo
 * Cormen). "tempo" e um contador global (passado por ponteiro) que e
 * incrementado a cada evento. visitado, descoberta, termino e pred
 * devem ser vetores de tamanho g->n previamente alocados/inicializados
 * pelo chamador (visitado com 0 e pred com -1). */
void dfs_recursiva(GrafoLista *g, int u, int *visitado, int *tempo,
                    int *descoberta, int *termino, int *pred);

/* Versao iterativa da DFS, usando a Pilha declarada acima. */
void dfs_iterativa(GrafoLista *g, int origem, int *visitado, int *pred);

/* Retorna 1 se o grafo (nao orientado) possui pelo menos um ciclo,
 * 0 caso contrario. Considera todos os componentes. */
int tem_ciclo(GrafoLista *g);

#endif