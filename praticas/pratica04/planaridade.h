#ifndef PLANARIDADE_H
#define PLANARIDADE_H

#include "conectividade.h"

/* Condicao necessaria de planaridade pela formula de Euler
 * (m <= 3n - 6 para n >= 3). Retorna 1 se a condicao E satisfeita
 * (o grafo PODE ser planar) e 0 se ela for violada (o grafo
 * certamente NAO e planar). Nao e suficiente sozinha: existem grafos
 * nao planares (como o K3,3) que satisfazem essa desigualdade. */
int eh_planar_euler(GrafoLista *g);

/* Heuristica de Kuratowski por forca bruta, valida para n <= 10:
 * 1) reduz topologicamente o grafo suprimindo vertices de grau 2
 *    (o que desfaz subdivisoes de arestas);
 * 2) verifica se o grafo reduzido contem K5 ou K3,3 como subgrafo.
 * Retorna 1 se encontrou uma subdivisao de K5 ou K3,3 (logo o grafo
 * NAO e planar). Retorna 0 se nao encontrou (nao GARANTE que o grafo
 * seja planar, apenas que essa heuristica nao achou evidencia
 * contraria) ou se g->n > 10 (fora do escopo da heuristica). */
int contem_subdivisao_kuratowski(GrafoLista *g);

/* Combina as duas checagens acima: primeiro tenta reprovar via Euler
 * (mais barato); se passar e n <= 10, tenta reprovar via Kuratowski.
 * Retorna 1 se nenhuma das duas encontrou evidencia de
 * nao-planaridade, 0 caso contrario. */
int eh_planar(GrafoLista *g);

#endif