#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

int main(void) {
    const int N = 7;
    const int origem = 0;

    GrafoListaPonderado *g = criar_grafo(N);
    if (!g) {
        fprintf(stderr, "Erro ao criar o grafo.\n");
        return 1;
    }

    /* Grafo nao-direcionado (vertice 6 fica isolado de proposito) */
    adicionar_aresta_bidirecional(g, 0, 1, 7);
    adicionar_aresta_bidirecional(g, 0, 2, 9);
    adicionar_aresta_bidirecional(g, 0, 5, 14);
    adicionar_aresta_bidirecional(g, 1, 2, 10);
    adicionar_aresta_bidirecional(g, 1, 3, 15);
    adicionar_aresta_bidirecional(g, 2, 3, 11);
    adicionar_aresta_bidirecional(g, 2, 5, 2);
    adicionar_aresta_bidirecional(g, 3, 4, 6);
    adicionar_aresta_bidirecional(g, 4, 5, 9);

    int *dist = malloc((size_t)N * sizeof(int));
    int *pred = malloc((size_t)N * sizeof(int));
    if (!dist || !pred) {
        fprintf(stderr, "Erro de alocacao.\n");
        free(dist);
        free(pred);
        liberar_grafo(g);
        return 1;
    }

    if (dijkstra(g, origem, dist, pred) != 0) {
        fprintf(stderr, "Erro ao executar Dijkstra (parametros invalidos ou peso negativo).\n");
        free(dist);
        free(pred);
        liberar_grafo(g);
        return 1;
    }

    printf("Dijkstra a partir do vertice %d\n\n", origem);
    printf("Vertice | Distancia | Predecessor\n");
    printf("--------+-----------+------------\n");
    for (int v = 0; v < N; v++) {
        if (dist[v] == INF)
            printf("%7d | %9s | %11s\n", v, "INF", "-");
        else if (pred[v] == -1)
            printf("%7d | %9d | %11s\n", v, dist[v], "-");
        else
            printf("%7d | %9d | %11d\n", v, dist[v], pred[v]);
    }

    printf("\nCaminhos minimos:\n");
    for (int v = 0; v < N; v++) {
        printf("  %d -> %d: ", origem, v);
        reconstruir_caminho(origem, v, pred);
    }

    free(dist);
    free(pred);
    liberar_grafo(g);
    return 0;
}