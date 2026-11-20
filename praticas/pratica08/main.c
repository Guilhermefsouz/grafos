#include <stdio.h>
#include <stdlib.h>
#include "agm.h"

static void testar(const char *titulo, GrafoListaPonderado *g) {
    int V = g->V;

    printf("=== %s ===\n\n", titulo);

    /* ---------- Kruskal ---------- */
    int m;
    Aresta *arestas = extrair_arestas(g, &m);
    Aresta *mst = malloc((V > 1 ? V - 1 : 1) * sizeof(Aresta));
    int qtd;
    int peso_k = kruskal(V, arestas, m, mst, &qtd);

    printf("Kruskal (arestas escolhidas):\n");
    for (int i = 0; i < qtd; i++)
        printf("  %d - %d (peso %d)\n", mst[i].u, mst[i].v, mst[i].peso);
    printf("Peso total: %d\n", peso_k);
    if (qtd < V - 1)
        printf("Grafo desconexo: resultado e uma floresta geradora minima.\n");

    /* ---------- Prim ---------- */
    int *pai = malloc(V * sizeof(int));
    int *chave = malloc(V * sizeof(int));
    int peso_p = prim(g, 0, pai, chave);

    printf("\nPrim a partir do vertice 0 (pai - vertice):\n");
    for (int v = 0; v < V; v++)
        if (pai[v] != -1)
            printf("  %d - %d (peso %d)\n", pai[v], v, chave[v]);
    printf("Peso total: %d\n\n", peso_p);

    free(arestas);
    free(mst);
    free(pai);
    free(chave);
}

int main(void) {
    /* Teste 1: grafo conexo com 9 vertices (AGM de peso 37) */
    GrafoListaPonderado *g1 = criar_grafo_ponderado(9);
    adicionar_aresta(g1, 0, 1, 4);
    adicionar_aresta(g1, 0, 7, 8);
    adicionar_aresta(g1, 1, 2, 8);
    adicionar_aresta(g1, 1, 7, 11);
    adicionar_aresta(g1, 2, 3, 7);
    adicionar_aresta(g1, 2, 8, 2);
    adicionar_aresta(g1, 2, 5, 4);
    adicionar_aresta(g1, 3, 4, 9);
    adicionar_aresta(g1, 3, 5, 14);
    adicionar_aresta(g1, 4, 5, 10);
    adicionar_aresta(g1, 5, 6, 2);
    adicionar_aresta(g1, 6, 7, 1);
    adicionar_aresta(g1, 6, 8, 6);
    adicionar_aresta(g1, 7, 8, 7);
    testar("Teste 1: grafo conexo", g1);
    liberar_grafo(g1);

    /* Teste 2: grafo desconexo (componentes {0,1,2} e {3,4}) */
    GrafoListaPonderado *g2 = criar_grafo_ponderado(5);
    adicionar_aresta(g2, 0, 1, 3);
    adicionar_aresta(g2, 1, 2, 1);
    adicionar_aresta(g2, 0, 2, 5);
    adicionar_aresta(g2, 3, 4, 2);
    testar("Teste 2: grafo desconexo", g2);
    liberar_grafo(g2);

    return 0;
}