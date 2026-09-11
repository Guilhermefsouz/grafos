#include <stdio.h>
#include <stdlib.h>
#include "busca_largura.h"
#include "busca_profundidade.h"

static void imprimir_bfs(GrafoLista *g, int origem) {
    int *dist = malloc((size_t) g->n * sizeof(int));
    int *pred = malloc((size_t) g->n * sizeof(int));

    bfs(g, origem, dist, pred);

    printf("BFS a partir do vertice %d:\n", origem);
    for (int i = 0; i < g->n; i++) {
        if (dist[i] == INFINITO) {
            printf("  vertice %d: inalcancavel\n", i);
        } else {
            printf("  vertice %d -> dist=%d, pred=%d\n", i, dist[i], pred[i]);
        }
    }

    free(dist);
    free(pred);
}

static void imprimir_dfs(GrafoLista *g) {
    int *visitado = calloc((size_t) g->n, sizeof(int));
    int *descoberta = malloc((size_t) g->n * sizeof(int));
    int *termino = malloc((size_t) g->n * sizeof(int));
    int *pred = malloc((size_t) g->n * sizeof(int));
    int tempo = 0;

    for (int i = 0; i < g->n; i++) {
        pred[i] = -1;
    }

    printf("DFS recursiva (tempos de descoberta/termino):\n");
    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            dfs_recursiva(g, i, visitado, &tempo, descoberta, termino, pred);
        }
    }
    for (int i = 0; i < g->n; i++) {
        printf("  vertice %d -> descoberta=%d, termino=%d, pred=%d\n",
               i, descoberta[i], termino[i], pred[i]);
    }

    free(visitado);
    free(descoberta);
    free(termino);
    free(pred);
}

int main(void) {
    /* ---- Grafo 1: arvore, conexa, bipartida e sem ciclos ---- */
    GrafoLista *g1 = criar_grafo_lista(6);
    inserir_aresta_lista(g1, 0, 1);
    inserir_aresta_lista(g1, 0, 2);
    inserir_aresta_lista(g1, 1, 3);
    inserir_aresta_lista(g1, 1, 4);
    inserir_aresta_lista(g1, 2, 5);

    printf("===== Grafo 1 (arvore) =====\n");
    imprimir_bfs(g1, 0);
    imprimir_dfs(g1);
    printf("Componentes conexos: %d\n", contar_componentes(g1));
    printf("Possui ciclo? %s\n", tem_ciclo(g1) ? "sim" : "nao");
    printf("E bipartido? %s\n\n", eh_bipartido(g1) ? "sim" : "nao");

    liberar_grafo_lista(g1);

    /* ---- Grafo 2: triangulo (0,1,2) + aresta (3,4) + vertice isolado 5 -----
     * 3 componentes, possui ciclo (triangulo) e nao e bipartido. */
    GrafoLista *g2 = criar_grafo_lista(6);
    inserir_aresta_lista(g2, 0, 1);
    inserir_aresta_lista(g2, 1, 2);
    inserir_aresta_lista(g2, 2, 0);
    inserir_aresta_lista(g2, 3, 4);

    printf("===== Grafo 2 (triangulo + aresta isolada + vertice isolado) =====\n");
    imprimir_bfs(g2, 0);
    imprimir_dfs(g2);
    printf("Componentes conexos: %d\n", contar_componentes(g2));
    printf("Possui ciclo? %s\n", tem_ciclo(g2) ? "sim" : "nao");
    printf("E bipartido? %s\n", eh_bipartido(g2) ? "sim" : "nao");

    liberar_grafo_lista(g2);

    return 0;
}