#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cobertura.h"

static void imprimir_conjunto(const int *cob, int n) {
    int primeiro = 1;
    printf("{");
    for (int i = 0; i < n; i++) {
        if (cob[i]) {
            printf(primeiro ? "%d" : ", %d", i);
            primeiro = 0;
        }
    }
    printf("}");
}

/* Repete a execução até acumular ~20 ms, para o tempo ser mensurável
   mesmo em grafos pequenos. Devolve microssegundos por execução. */
static double medir_bt(Grafo *g, int *sol, int *tam, long *nos) {
    long reps = 0;
    clock_t ini = clock();
    double dec;
    do {
        *tam = cobertura_minima_bt(g, sol, nos);
        reps++;
        dec = (double)(clock() - ini) / CLOCKS_PER_SEC;
    } while (dec < 0.02);
    return dec * 1e6 / reps;
}

static double medir_gulosa(Grafo *g, int *sol, int *tam) {
    long reps = 0;
    clock_t ini = clock();
    double dec;
    do {
        *tam = cobertura_gulosa(g, sol);
        reps++;
        dec = (double)(clock() - ini) / CLOCKS_PER_SEC;
    } while (dec < 0.02);
    return dec * 1e6 / reps;
}

static void demonstracao(void) {
    /* Grafo de exemplo: ciclo 0-1-2-3-4-0 com a corda 0-2 e o pendente 4-5 */
    Grafo *g = criar_grafo(6);
    adicionar_aresta(g, 0, 1);
    adicionar_aresta(g, 1, 2);
    adicionar_aresta(g, 2, 3);
    adicionar_aresta(g, 3, 4);
    adicionar_aresta(g, 4, 0);
    adicionar_aresta(g, 0, 2);
    adicionar_aresta(g, 4, 5);

    int *bt = malloc(g->n * sizeof(int));
    int *gu = malloc(g->n * sizeof(int));
    long nos;

    int tam_bt = cobertura_minima_bt(g, bt, &nos);
    int tam_gu = cobertura_gulosa(g, gu);

    printf("=== Demonstracao (6 vertices, %d arestas) ===\n", g->m);
    printf("Backtracking: tamanho %d, cobertura ", tam_bt);
    imprimir_conjunto(bt, g->n);
    printf(" | eh_cobertura: %s\n", eh_cobertura(g, bt) ? "sim" : "NAO");
    printf("Gulosa:       tamanho %d, cobertura ", tam_gu);
    imprimir_conjunto(gu, g->n);
    printf(" | eh_cobertura: %s\n\n", eh_cobertura(g, gu) ? "sim" : "NAO");

    free(bt);
    free(gu);
    liberar_grafo(g);
}

static void experimento(void) {
    int tamanhos[] = {5, 8, 10, 12, 15};
    int qtd = (int)(sizeof(tamanhos) / sizeof(tamanhos[0]));
    int densidade = 40;   /* % de arestas */

    printf("=== Experimento: grafos aleatorios (densidade %d%%) ===\n\n", densidade);
    printf("%3s %7s | %5s %12s %8s | %5s %12s | %s\n",
           "n", "arestas", "BT", "tempo(us)", "nos", "Gul", "tempo(us)", "valido");
    printf("----------------+----------------------------+-------------------+-------\n");

    for (int k = 0; k < qtd; k++) {
        int n = tamanhos[k];
        Grafo *g = gerar_grafo_aleatorio(n, densidade, 2024u + (unsigned)n);
        int *sol_bt = malloc(n * sizeof(int));
        int *sol_gu = malloc(n * sizeof(int));
        int tam_bt, tam_gu;
        long nos = 0;

        double t_bt = medir_bt(g, sol_bt, &tam_bt, &nos);
        double t_gu = medir_gulosa(g, sol_gu, &tam_gu);
        int valido = eh_cobertura(g, sol_bt) && eh_cobertura(g, sol_gu);

        printf("%3d %7d | %5d %12.2f %8ld | %5d %12.2f | %s\n",
               n, g->m, tam_bt, t_bt, nos, tam_gu, t_gu, valido ? "ok" : "ERRO");

        free(sol_bt);
        free(sol_gu);
        liberar_grafo(g);
    }

    printf("\nBT = backtracking (otimo) | Gul = heuristica gulosa (maior grau)\n");
}

int main(void) {
    demonstracao();
    experimento();
    return 0;
}