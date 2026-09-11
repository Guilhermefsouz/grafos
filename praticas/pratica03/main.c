#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

static void imprimir_ordem(const char *rotulo, int *ordem, int tamanho) {
    if (ordem == NULL) {
        printf("%s: impossivel (grafo possui ciclo)\n", rotulo);
        return;
    }

    printf("%s: ", rotulo);
    for (int i = 0; i < tamanho; i++) {
        printf("%d", ordem[i]);
        if (i < tamanho - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

int main(void) {
    /* ---- Grafo 1: DAG (dependencias de "disciplinas") ----
     * 0: Fundamentos     -> 1: Estruturas de Dados, 2: Logica
     * 1: Estruturas       -> 3: Algoritmos
     * 2: Logica            -> 3: Algoritmos
     * 3: Algoritmos        -> 4: Grafos
     * 5: Optativa (sem dependencias, mas depende de 4)
     * 4: Grafos            -> 5: Optativa
     */
    GrafoLista *dag = criar_grafo_lista(6);
    inserir_arco(dag, 0, 1);
    inserir_arco(dag, 0, 2);
    inserir_arco(dag, 1, 3);
    inserir_arco(dag, 2, 3);
    inserir_arco(dag, 3, 4);
    inserir_arco(dag, 4, 5);

    printf("===== Digrafo 1 (dependencias, sem ciclos) =====\n");
    printf("E DAG? %s\n", eh_dag(dag) ? "sim" : "nao");

    int tamanho_kahn = 0;
    int *ordem_kahn = ordenacao_topologica_kahn(dag, &tamanho_kahn);
    imprimir_ordem("Ordenacao (Kahn)", ordem_kahn, tamanho_kahn);
    free(ordem_kahn);

    int tamanho_dfs = 0;
    int *ordem_dfs = ordenacao_topologica_dfs(dag, &tamanho_dfs);
    imprimir_ordem("Ordenacao (DFS)", ordem_dfs, tamanho_dfs);
    free(ordem_dfs);

    liberar_grafo_lista(dag);

    /* ---- Grafo 2: digrafo com ciclo (0 -> 1 -> 2 -> 0) ---- */
    GrafoLista *ciclico = criar_grafo_lista(4);
    inserir_arco(ciclico, 0, 1);
    inserir_arco(ciclico, 1, 2);
    inserir_arco(ciclico, 2, 0);
    inserir_arco(ciclico, 2, 3);

    printf("\n===== Digrafo 2 (com ciclo) =====\n");
    printf("E DAG? %s\n", eh_dag(ciclico) ? "sim" : "nao");

    int tamanho_kahn2 = 0;
    int *ordem_kahn2 = ordenacao_topologica_kahn(ciclico, &tamanho_kahn2);
    imprimir_ordem("Ordenacao (Kahn)", ordem_kahn2, tamanho_kahn2);
    free(ordem_kahn2);

    int tamanho_dfs2 = 0;
    int *ordem_dfs2 = ordenacao_topologica_dfs(ciclico, &tamanho_dfs2);
    imprimir_ordem("Ordenacao (DFS)", ordem_dfs2, tamanho_dfs2);
    free(ordem_dfs2);

    liberar_grafo_lista(ciclico);

    return 0;
}