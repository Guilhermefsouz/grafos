#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

static void executar(const char *titulo, GrafoListaPonderado *g, int origem) {
    int V = g->V;
    int *dist = malloc(V * sizeof(int));
    int *pred = malloc(V * sizeof(int));

    printf("=== %s ===\n", titulo);
    printf("Origem: %d\n\n", origem);

    if (bellman_ford(g, origem, dist, pred)) {
        printf("Sem ciclo negativo. Menores caminhos:\n");
        for (int v = 0; v < V; v++) {
            printf("Destino %d | distancia: ", v);
            if (dist[v] == INF)
                printf("INF");
            else
                printf("%d", dist[v]);
            printf(" | caminho: ");
            reconstruir_caminho(origem, v, pred);
            printf("\n");
        }
    } else {
        printf("Ciclo de custo negativo detectado!\n");
        reconstruir_ciclo_negativo(V, pred, bf_vertice_ciclo);
    }
    printf("\n");

    free(dist);
    free(pred);
}

int main(void) {
    /* Teste 1: pesos negativos, mas SEM ciclo negativo */
    GrafoListaPonderado *g1 = criar_grafo_ponderado(5);
    adicionar_aresta(g1, 0, 1, 6);
    adicionar_aresta(g1, 0, 2, 7);
    adicionar_aresta(g1, 1, 2, 8);
    adicionar_aresta(g1, 1, 3, 5);
    adicionar_aresta(g1, 1, 4, -4);
    adicionar_aresta(g1, 2, 3, -3);
    adicionar_aresta(g1, 2, 4, 9);
    adicionar_aresta(g1, 3, 1, -2);
    adicionar_aresta(g1, 4, 3, 7);
    adicionar_aresta(g1, 4, 0, 2);
    executar("Teste 1: sem ciclo negativo", g1, 0);
    liberar_grafo(g1);

    /* Teste 2: ciclo 1 -> 2 -> 3 -> 1 com custo total -3 */
    GrafoListaPonderado *g2 = criar_grafo_ponderado(5);
    adicionar_aresta(g2, 0, 1, 1);
    adicionar_aresta(g2, 1, 2, -1);
    adicionar_aresta(g2, 2, 3, -1);
    adicionar_aresta(g2, 3, 1, -1);
    adicionar_aresta(g2, 3, 4, 2);
    executar("Teste 2: com ciclo negativo", g2, 0);
    liberar_grafo(g2);

    return 0;
}