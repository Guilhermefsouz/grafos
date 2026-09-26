#ifndef CONECTIVIDADE_H
#define CONECTIVIDADE_H

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

/* ---------- Vertices de corte (articulacoes) - algoritmo de Tarjan ---------- */

/* Uma chamada recursiva do DFS de Tarjan. u e o vertice atual, pai e
 * seu pai na arvore de busca (-1 se u for raiz). visitado, descoberta,
 * low e eh_articulacao devem ser vetores de tamanho g->n alocados e
 * inicializados (visitado/eh_articulacao com 0) pelo chamador. tempo e
 * um contador global de descoberta, passado por ponteiro. Ao final,
 * eh_articulacao[i] == 1 para todo vertice de corte encontrado. */
void dfs_articulacoes(GrafoLista *g, int u, int pai, int *visitado,
                       int *descoberta, int *low, int *tempo,
                       int *eh_articulacao);

/* Executa dfs_articulacoes a partir de todo vertice ainda nao
 * visitado (cobre grafos desconexos). eh_articulacao deve ser um
 * vetor de tamanho g->n, sera zerado e preenchido por esta funcao.
 * Retorna a quantidade de vertices de corte encontrados. */
int contar_articulacoes(GrafoLista *g, int *eh_articulacao);

/* ---------- Pontes (arestas de corte) ---------- */

typedef struct {
    int u, v;
} Ponte;

/* Detecta as pontes do grafo (condicao low[v] > descoberta[u]).
 * Retorna um vetor alocado dinamicamente (o chamador deve dar free)
 * com as pontes encontradas, e escreve a quantidade em *quantidade. */
Ponte *detectar_pontes(GrafoLista *g, int *quantidade);

#endif