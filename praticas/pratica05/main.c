#include <stdio.h>
#include <stdlib.h>
#include "coloracao.h"

static void imprimir_coloracao(const char *rotulo, int *cor, int n, int num_cores) {
    printf("%s (usou %d cores):\n", rotulo, num_cores);
    for (int i = 0; i < n; i++) {
        printf("  vertice %d -> cor %d\n", i, cor[i]);
    }
}

int main(void) {
    /* ---- Grafo 1: hexagono "fora de ordem" + um vertice pendente ----
     * O ciclo de verdade e 0-2-4-1-3-5-0 (uma aresta liga cada par),
     * mas a numeracao dos vertices NAO segue essa ordem. Isso faz o
     * guloso simples (que processa 0,1,2,...,n-1) tropecar e usar 3
     * cores, mesmo o grafo sendo bipartido (otimo = 2). O vertice 6 e
     * pendurado no vertice 4, dando a ele o maior grau do grafo — e
     * exatamente o vertice que a heuristica Welsh-Powell processa
     * primeiro, o que basta para fechar a coloracao em apenas 2 cores. */
    GrafoLista *g1 = criar_grafo_lista(7);
    inserir_aresta_lista(g1, 0, 2);
    inserir_aresta_lista(g1, 2, 4);
    inserir_aresta_lista(g1, 4, 1);
    inserir_aresta_lista(g1, 1, 3);
    inserir_aresta_lista(g1, 3, 5);
    inserir_aresta_lista(g1, 5, 0);
    inserir_aresta_lista(g1, 4, 6);

    printf("===== Grafo 1 (bipartido, mas com numeracao 'ruim') =====\n");
    printf("E bipartido? %s (numero cromatico otimo = 2)\n\n", eh_bipartido(g1) ? "sim" : "nao");

    int num_cores_guloso = 0;
    int *cor_guloso = coloracao_gulosa(g1, &num_cores_guloso);
    imprimir_coloracao("Coloracao gulosa (ordem natural)", cor_guloso, g1->n, num_cores_guloso);
    free(cor_guloso);

    printf("\n");

    int num_cores_wp = 0;
    int *cor_wp = coloracao_welsh_powell(g1, &num_cores_wp);
    imprimir_coloracao("Coloracao Welsh-Powell (ordem por grau)", cor_wp, g1->n, num_cores_wp);
    free(cor_wp);

    liberar_grafo_lista(g1);

    /* ---- Grafo 2: triangulo (nao bipartido, precisa de 3 cores) ---- */
    GrafoLista *g2 = criar_grafo_lista(3);
    inserir_aresta_lista(g2, 0, 1);
    inserir_aresta_lista(g2, 1, 2);
    inserir_aresta_lista(g2, 2, 0);

    printf("\n===== Grafo 2 (triangulo) =====\n");
    printf("E bipartido? %s\n\n", eh_bipartido(g2) ? "sim" : "nao");

    int num_cores_g2 = 0;
    int *cor_g2 = coloracao_gulosa(g2, &num_cores_g2);
    imprimir_coloracao("Coloracao gulosa", cor_g2, g2->n, num_cores_g2);
    free(cor_g2);

    liberar_grafo_lista(g2);

    return 0;
}