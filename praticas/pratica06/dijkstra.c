#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

/* ================================================================== */
/* Grafo                                                              */
/* ================================================================== */

GrafoListaPonderado *criar_grafo(int num_vertices) {
    if (num_vertices <= 0) return NULL;

    GrafoListaPonderado *g = malloc(sizeof *g);
    if (!g) return NULL;

    g->num_vertices = num_vertices;
    g->adj = calloc((size_t)num_vertices, sizeof(NoAdj *));
    if (!g->adj) {
        free(g);
        return NULL;
    }
    return g;
}

int adicionar_aresta(GrafoListaPonderado *g, int u, int v, int peso) {
    if (!g || u < 0 || u >= g->num_vertices || v < 0 || v >= g->num_vertices)
        return -1;

    NoAdj *no = malloc(sizeof *no);
    if (!no) return -1;

    no->destino = v;
    no->peso = peso;
    no->prox = g->adj[u];   /* insercao no inicio da lista: O(1) */
    g->adj[u] = no;
    return 0;
}

int adicionar_aresta_bidirecional(GrafoListaPonderado *g, int u, int v, int peso) {
    if (adicionar_aresta(g, u, v, peso) != 0) return -1;
    return adicionar_aresta(g, v, u, peso);
}

void liberar_grafo(GrafoListaPonderado *g) {
    if (!g) return;
    for (int i = 0; i < g->num_vertices; i++) {
        NoAdj *atual = g->adj[i];
        while (atual) {
            NoAdj *prox = atual->prox;
            free(atual);
            atual = prox;
        }
    }
    free(g->adj);
    free(g);
}

/* ================================================================== */
/* Min-Heap                                                           */
/* ================================================================== */

/* Troca dois nos do heap e mantem o vetor posicao[] consistente. */
static void trocar(MinHeap *h, int i, int j) {
    NoHeap tmp = h->dados[i];
    h->dados[i] = h->dados[j];
    h->dados[j] = tmp;

    h->posicao[h->dados[i].vertice] = i;
    h->posicao[h->dados[j].vertice] = j;
}

/* Sobe o no do indice i ate restaurar a propriedade de min-heap: O(log V). */
static void subir(MinHeap *h, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->dados[i].distancia >= h->dados[pai].distancia) break;
        trocar(h, i, pai);
        i = pai;
    }
}

/* Desce o no do indice i ate restaurar a propriedade de min-heap: O(log V). */
static void descer(MinHeap *h, int i) {
    for (;;) {
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        int menor = i;

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
    if (capacidade <= 0) return NULL;

    MinHeap *h = malloc(sizeof *h);
    if (!h) return NULL;

    h->dados = malloc((size_t)capacidade * sizeof(NoHeap));
    h->posicao = malloc((size_t)capacidade * sizeof(int));
    if (!h->dados || !h->posicao) {
        free(h->dados);
        free(h->posicao);
        free(h);
        return NULL;
    }

    h->capacidade = capacidade;
    h->tamanho = 0;
    for (int i = 0; i < capacidade; i++)
        h->posicao[i] = -1;     /* nenhum vertice esta no heap ainda */

    return h;
}

int heap_vazio(const MinHeap *h) {
    return h == NULL || h->tamanho == 0;
}

/* insert: O(log V) */
int inserir(MinHeap *h, int vertice, int distancia) {
    if (!h || h->tamanho >= h->capacidade) return -1;
    if (vertice < 0 || vertice >= h->capacidade) return -1;
    if (h->posicao[vertice] != -1) return -1;   /* ja esta no heap */

    int i = h->tamanho++;
    h->dados[i].vertice = vertice;
    h->dados[i].distancia = distancia;
    h->posicao[vertice] = i;

    subir(h, i);
    return 0;
}

/* extract_min: O(log V). Retorna {-1, INF} se o heap estiver vazio. */
NoHeap extrair_minimo(MinHeap *h) {
    NoHeap vazio = { -1, INF };
    if (heap_vazio(h)) return vazio;

    NoHeap minimo = h->dados[0];
    NoHeap ultimo = h->dados[h->tamanho - 1];

    h->tamanho--;
    h->posicao[minimo.vertice] = -1;    /* saiu do heap */

    if (h->tamanho > 0) {
        h->dados[0] = ultimo;
        h->posicao[ultimo.vertice] = 0;
        descer(h, 0);
    }
    return minimo;
}

/* decrease_key: O(log V) gracas ao vetor posicao[] (acesso O(1) ao indice). */
void diminuir_chave(MinHeap *h, int vertice, int nova_distancia) {
    if (!h || vertice < 0 || vertice >= h->capacidade) return;

    int i = h->posicao[vertice];
    if (i == -1) return;                              /* vertice nao esta no heap */
    if (nova_distancia >= h->dados[i].distancia) return;  /* nao diminui a chave */

    h->dados[i].distancia = nova_distancia;
    subir(h, i);
}

void liberar_min_heap(MinHeap *h) {
    if (!h) return;
    free(h->dados);
    free(h->posicao);
    free(h);
}

/* ================================================================== */
/* Dijkstra                                                           */
/* ================================================================== */

int dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    if (!g || !dist || !pred) return -1;
    if (origem < 0 || origem >= g->num_vertices) return -1;

    int n = g->num_vertices;

    /* Dijkstra so e correto para custos nao-negativos. */
    for (int u = 0; u < n; u++)
        for (NoAdj *e = g->adj[u]; e; e = e->prox)
            if (e->peso < 0) return -1;

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        pred[i] = -1;
    }

    MinHeap *h = criar_min_heap(n);
    if (!h) return -1;

    dist[origem] = 0;
    inserir(h, origem, 0);

    while (!heap_vazio(h)) {
        NoHeap atual = extrair_minimo(h);
        int u = atual.vertice;

        for (NoAdj *e = g->adj[u]; e; e = e->prox) {
            int v = e->destino;
            long long nova = (long long)dist[u] + e->peso;  /* evita overflow */

            if (nova < dist[v]) {               /* relaxamento da aresta (u, v) */
                dist[v] = (int)nova;
                pred[v] = u;

                if (h->posicao[v] == -1)        /* primeira vez que v e descoberto */
                    inserir(h, v, dist[v]);
                else                            /* v ja esta no heap: atualiza chave */
                    diminuir_chave(h, v, dist[v]);
            }
        }
    }

    liberar_min_heap(h);
    return 0;
}

/* Funcao auxiliar: imprime o caminho recursivamente (so imprime se existir). */
static int imprimir_caminho(int origem, int v, int *pred) {
    if (v == origem) {
        printf("%d", v);
        return 1;
    }
    if (pred[v] == -1) return 0;    /* inalcancavel */

    int qtd = imprimir_caminho(origem, pred[v], pred);
    if (qtd == 0) return 0;

    printf(" -> %d", v);
    return qtd + 1;
}

int reconstruir_caminho(int origem, int destino, int *pred) {
    if (!pred || origem < 0 || destino < 0) return 0;

    int qtd = imprimir_caminho(origem, destino, pred);
    if (qtd == 0)
        printf("Sem caminho de %d para %d", origem, destino);
    printf("\n");
    return qtd;
}
