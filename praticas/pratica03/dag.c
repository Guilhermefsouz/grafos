#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

/* ==================== Digrafo (lista de adjacencia) ==================== */

GrafoLista *criar_grafo_lista(int n) {
    GrafoLista *g = malloc(sizeof(GrafoLista));
    g->n = n;
    g->adj = calloc((size_t) n, sizeof(No *));
    return g;
}

void inserir_arco(GrafoLista *g, int u, int v) {
    No *novo = malloc(sizeof(No));
    novo->destino = v;
    novo->prox = g->adj[u];
    g->adj[u] = novo;
}

void liberar_grafo_lista(GrafoLista *g) {
    if (g == NULL) {
        return;
    }
    for (int i = 0; i < g->n; i++) {
        No *atual = g->adj[i];
        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(g->adj);
    free(g);
}

/* ==================== Deteccao de ciclo (DAG?) ==================== */

#define BRANCO 0 /* ainda nao visitado */
#define CINZA  1 /* em processamento (na pilha de recursao) */
#define PRETO  2 /* processamento concluido */

static int tem_ciclo_dirigido(GrafoLista *g, int u, int *cor) {
    cor[u] = CINZA;

    for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
        int v = aux->destino;
        if (cor[v] == CINZA) {
            /* arco para um vertice em processamento: aresta de retorno */
            return 1;
        }
        if (cor[v] == BRANCO && tem_ciclo_dirigido(g, v, cor)) {
            return 1;
        }
    }

    cor[u] = PRETO;
    return 0;
}

int eh_dag(GrafoLista *g) {
    int *cor = calloc((size_t) g->n, sizeof(int));
    int ciclo = 0;

    for (int i = 0; i < g->n && !ciclo; i++) {
        if (cor[i] == BRANCO) {
            if (tem_ciclo_dirigido(g, i, cor)) {
                ciclo = 1;
            }
        }
    }

    free(cor);
    return !ciclo;
}

/* ==================== Kahn (BFS com grau de entrada) ==================== */

int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho) {
    int n = g->n;
    int *grau_entrada = calloc((size_t) n, sizeof(int));

    for (int u = 0; u < n; u++) {
        for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
            grau_entrada[aux->destino]++;
        }
    }

    /* fila (FIFO) simples baseada em array: cada vertice entra no
     * maximo uma vez, entao um array de tamanho n e suficiente. */
    int *fila = malloc((size_t) n * sizeof(int));
    int inicio = 0, fim = 0;

    for (int v = 0; v < n; v++) {
        if (grau_entrada[v] == 0) {
            fila[fim++] = v;
        }
    }

    int *ordem = malloc((size_t) n * sizeof(int));
    int contagem = 0;

    while (inicio < fim) {
        int u = fila[inicio++];
        ordem[contagem++] = u;

        for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
            int v = aux->destino;
            grau_entrada[v]--;
            if (grau_entrada[v] == 0) {
                fila[fim++] = v;
            }
        }
    }

    free(grau_entrada);
    free(fila);

    if (contagem != n) {
        /* nem todos os vertices foram processados: ha ciclo */
        free(ordem);
        *tamanho = 0;
        return NULL;
    }

    *tamanho = contagem;
    return ordem;
}

/* ==================== Ordenacao topologica via DFS ==================== */

static void dfs_topologica(GrafoLista *g, int u, int *visitado, int *pilha, int *topo) {
    visitado[u] = 1;

    for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
        int v = aux->destino;
        if (!visitado[v]) {
            dfs_topologica(g, v, visitado, pilha, topo);
        }
    }

    /* empilha o vertice no momento em que sua exploracao termina */
    pilha[(*topo)++] = u;
}

int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho) {
    if (!eh_dag(g)) {
        *tamanho = 0;
        return NULL;
    }

    int n = g->n;
    int *visitado = calloc((size_t) n, sizeof(int));
    int *pilha = malloc((size_t) n * sizeof(int));
    int topo = 0;

    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            dfs_topologica(g, i, visitado, pilha, &topo);
        }
    }

    /* a pilha contem os vertices em ordem crescente de termino; a
     * ordenacao topologica e o inverso dessa ordem. */
    int *ordem = malloc((size_t) n * sizeof(int));
    for (int i = 0; i < n; i++) {
        ordem[i] = pilha[n - 1 - i];
    }

    free(visitado);
    free(pilha);

    *tamanho = n;
    return ordem;
}