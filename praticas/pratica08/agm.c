#include <stdio.h>
#include <stdlib.h>
#include "agm.h"

/* ====================== Grafo ====================== */

GrafoListaPonderado *criar_grafo_ponderado(int V) {
    GrafoListaPonderado *g = malloc(sizeof(GrafoListaPonderado));
    if (!g) return NULL;
    g->V = V;
    g->adj = calloc(V, sizeof(NoAdj *));
    if (!g->adj) {
        free(g);
        return NULL;
    }
    return g;
}

static void inserir_adj(GrafoListaPonderado *g, int origem, int destino, int peso) {
    NoAdj *novo = malloc(sizeof(NoAdj));
    if (!novo) return;
    novo->destino = destino;
    novo->peso = peso;
    novo->prox = g->adj[origem];
    g->adj[origem] = novo;
}

void adicionar_aresta(GrafoListaPonderado *g, int u, int v, int peso) {
    inserir_adj(g, u, v, peso);
    inserir_adj(g, v, u, peso);
}

void liberar_grafo(GrafoListaPonderado *g) {
    if (!g) return;
    for (int i = 0; i < g->V; i++) {
        NoAdj *p = g->adj[i];
        while (p) {
            NoAdj *prox = p->prox;
            free(p);
            p = prox;
        }
    }
    free(g->adj);
    free(g);
}

/* ====================== Union-Find ====================== */

DSU *criar_dsu(int n) {
    DSU *d = malloc(sizeof(DSU));
    if (!d) return NULL;
    d->pai = malloc(n * sizeof(int));
    d->rank = calloc(n, sizeof(int));
    if (!d->pai || !d->rank) {
        free(d->pai);
        free(d->rank);
        free(d);
        return NULL;
    }
    d->n = n;
    for (int i = 0; i < n; i++) d->pai[i] = i;
    return d;
}

void liberar_dsu(DSU *d) {
    if (!d) return;
    free(d->pai);
    free(d->rank);
    free(d);
}

/* Compressão de caminho: todo nó visitado passa a apontar direto para a raiz */
int find(DSU *d, int x) {
    if (d->pai[x] != x)
        d->pai[x] = find(d, d->pai[x]);
    return d->pai[x];
}

/* União por rank: a árvore menor entra embaixo da maior */
int unir(DSU *d, int x, int y) {
    int rx = find(d, x);
    int ry = find(d, y);
    if (rx == ry) return 0;

    if (d->rank[rx] < d->rank[ry]) {
        d->pai[rx] = ry;
    } else if (d->rank[rx] > d->rank[ry]) {
        d->pai[ry] = rx;
    } else {
        d->pai[ry] = rx;
        d->rank[rx]++;
    }
    return 1;
}

/* ====================== Kruskal ====================== */

Aresta *extrair_arestas(GrafoListaPonderado *g, int *m) {
    int total = 0;
    for (int u = 0; u < g->V; u++)
        for (NoAdj *p = g->adj[u]; p != NULL; p = p->prox)
            if (u < p->destino) total++;   /* cada aresta conta uma vez */

    Aresta *vet = malloc((total > 0 ? total : 1) * sizeof(Aresta));
    if (!vet) {
        *m = 0;
        return NULL;
    }

    int k = 0;
    for (int u = 0; u < g->V; u++) {
        for (NoAdj *p = g->adj[u]; p != NULL; p = p->prox) {
            if (u < p->destino) {
                vet[k].u = u;
                vet[k].v = p->destino;
                vet[k].peso = p->peso;
                k++;
            }
        }
    }
    *m = k;
    return vet;
}

static int comparar_arestas(const void *a, const void *b) {
    const Aresta *x = (const Aresta *)a;
    const Aresta *y = (const Aresta *)b;
    return (x->peso > y->peso) - (x->peso < y->peso);
}

int kruskal(int n, Aresta *arestas, int m, Aresta *mst, int *qtd) {
    int total = 0;
    *qtd = 0;

    DSU *d = criar_dsu(n);
    if (!d) return 0;

    qsort(arestas, m, sizeof(Aresta), comparar_arestas);   /* O(E log E) */

    for (int i = 0; i < m && *qtd < n - 1; i++) {
        if (unir(d, arestas[i].u, arestas[i].v)) {   /* não forma ciclo */
            mst[*qtd] = arestas[i];
            (*qtd)++;
            total += arestas[i].peso;
        }
    }

    liberar_dsu(d);
    return total;
}

/* ====================== Min-Heap ====================== */

static void trocar(MinHeap *h, int i, int j) {
    NoHeap tmp = h->dados[i];
    h->dados[i] = h->dados[j];
    h->dados[j] = tmp;
    h->posicao[h->dados[i].vertice] = i;
    h->posicao[h->dados[j].vertice] = j;
}

static void subir(MinHeap *h, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->dados[pai].distancia <= h->dados[i].distancia) break;
        trocar(h, pai, i);
        i = pai;
    }
}

static void descer(MinHeap *h, int i) {
    for (;;) {
        int esq = 2 * i + 1, dir = 2 * i + 2, menor = i;
        if (esq < h->tamanho && h->dados[esq].distancia < h->dados[menor].distancia)
            menor = esq;
        if (dir < h->tamanho && h->dados[dir].distancia < h->dados[menor].distancia)
            menor = dir;
        if (menor == i) break;
        trocar(h, i, menor);
        i = menor;
    }
}

MinHeap *criar_min_heap(int capacidade) {
    MinHeap *h = malloc(sizeof(MinHeap));
    if (!h) return NULL;
    h->dados = malloc(capacidade * sizeof(NoHeap));
    h->posicao = malloc(capacidade * sizeof(int));
    if (!h->dados || !h->posicao) {
        free(h->dados);
        free(h->posicao);
        free(h);
        return NULL;
    }
    h->capacidade = capacidade;
    h->tamanho = 0;
    for (int i = 0; i < capacidade; i++) h->posicao[i] = -1;
    return h;
}

void inserir(MinHeap *h, int vertice, int distancia) {
    if (h->tamanho >= h->capacidade) return;
    int i = h->tamanho++;
    h->dados[i].vertice = vertice;
    h->dados[i].distancia = distancia;
    h->posicao[vertice] = i;
    subir(h, i);
}

NoHeap extrair_minimo(MinHeap *h) {
    NoHeap raiz = h->dados[0];
    NoHeap ultimo = h->dados[h->tamanho - 1];

    h->posicao[raiz.vertice] = -1;
    h->tamanho--;

    if (h->tamanho > 0) {
        h->dados[0] = ultimo;
        h->posicao[ultimo.vertice] = 0;
        descer(h, 0);
    }
    return raiz;
}

void diminuir_chave(MinHeap *h, int vertice, int nova_chave) {
    int i = h->posicao[vertice];
    if (i < 0) return;
    if (nova_chave >= h->dados[i].distancia) return;
    h->dados[i].distancia = nova_chave;
    subir(h, i);
}

int heap_vazio(MinHeap *h) {
    return h->tamanho == 0;
}

void liberar_min_heap(MinHeap *h) {
    if (!h) return;
    free(h->dados);
    free(h->posicao);
    free(h);
}

/* ====================== Prim ====================== */

int prim(GrafoListaPonderado *g, int origem, int *pai, int *chave) {
    int V = g->V;
    int total = 0;

    MinHeap *h = criar_min_heap(V);
    if (!h) return 0;

    for (int v = 0; v < V; v++) {
        chave[v] = (v == origem) ? 0 : INF;
        pai[v] = -1;
        inserir(h, v, chave[v]);
    }

    while (!heap_vazio(h)) {
        NoHeap u = extrair_minimo(h);

        if (u.distancia == INF) break;   /* resto está em outra componente */
        total += u.distancia;

        for (NoAdj *p = g->adj[u.vertice]; p != NULL; p = p->prox) {
            int v = p->destino;
            /* só atualiza se v ainda está no heap e a aresta é mais leve */
            if (h->posicao[v] != -1 && p->peso < chave[v]) {
                chave[v] = p->peso;
                pai[v] = u.vertice;
                diminuir_chave(h, v, p->peso);
            }
        }
    }

    liberar_min_heap(h);
    return total;
}