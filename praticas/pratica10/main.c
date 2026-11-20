#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hamiltoniano.h"
#include "tps.h"

static void imprimir_rota(const int *r, int n, int fechar) {
    for (int i = 0; i < n; i++)
        printf(i == 0 ? "%d" : " -> %d", r[i]);
    if (fechar && n > 0) printf(" -> %d", r[0]);
}

/* ---------- Parte 1: Hamiltoniano e redução para TSP ---------- */

static void analisar_grafo(const char *nome, Grafo *g) {
    int n = g->n;
    int *caminho = malloc(n * sizeof(int));
    int *ciclo = malloc(n * sizeof(int));

    printf("--- %s (n=%d, %d arestas) ---\n", nome, n, g->m);

    if (encontrar_caminho_hamiltoniano_bt(g, caminho)) {
        printf("Caminho hamiltoniano: sim | ");
        imprimir_rota(caminho, n, 0);
        printf("\n");
    } else {
        printf("Caminho hamiltoniano: nao\n");
    }

    int tem_ciclo = encontrar_ciclo_hamiltoniano_bt(g, ciclo);
    if (tem_ciclo) {
        printf("Ciclo hamiltoniano:   sim | ");
        imprimir_rota(ciclo, n, 1);
        printf("\n");
    } else {
        printf("Ciclo hamiltoniano:   nao\n");
    }

    /* Redução: ciclo hamiltoniano  <=>  TSP ótimo com custo exatamente n */
    TSP *t = grafo_para_tsp(g);
    int *rota = malloc(n * sizeof(int));
    int custo = tsp_forca_bruta(t, rota);
    int coincide = (custo == n) == (tem_ciclo != 0);
    printf("Reducao para TSP:     custo otimo = %d (n = %d) -> %s\n\n",
           custo, n, coincide ? "consistente" : "INCONSISTENTE");

    free(caminho);
    free(ciclo);
    free(rota);
    liberar_tsp(t);
}

static void parte_hamiltoniano(void) {
    printf("=== Parte 1: Caminho e Ciclo Hamiltoniano ===\n\n");

    /* A: ciclo 0-1-2-3-4-5-0 com a corda 0-3 (tem ciclo) */
    Grafo *a = criar_grafo(6);
    for (int i = 0; i < 6; i++) adicionar_aresta(a, i, (i + 1) % 6);
    adicionar_aresta(a, 0, 3);
    analisar_grafo("Grafo A: ciclo com corda", a);
    liberar_grafo(a);

    /* B: caminho 0-1-2-3-4 (tem caminho, mas nao tem ciclo) */
    Grafo *b = criar_grafo(5);
    for (int i = 0; i < 4; i++) adicionar_aresta(b, i, i + 1);
    analisar_grafo("Grafo B: apenas um caminho", b);
    liberar_grafo(b);

    /* C: estrela com centro 0 (nao tem nem caminho, nem ciclo) */
    Grafo *c = criar_grafo(5);
    for (int i = 1; i < 5; i++) adicionar_aresta(c, 0, i);
    analisar_grafo("Grafo C: estrela", c);
    liberar_grafo(c);
}

/* ---------- Parte 2: experimento do TSP ---------- */

typedef int (*AlgoTSP)(TSP *, int *);

/* Repete até acumular ~20 ms (para o tempo ser mensurável em instâncias
   pequenas). Devolve microssegundos por execução. */
static double medir(AlgoTSP f, TSP *t, int *rota, int *custo) {
    long reps = 0;
    clock_t ini = clock();
    double dec;
    do {
        *custo = f(t, rota);
        reps++;
        dec = (double)(clock() - ini) / CLOCKS_PER_SEC;
    } while (dec < 0.02);
    return dec * 1e6 / reps;
}

static double gap(int custo, int otimo) {
    return otimo > 0 ? (custo - otimo) * 100.0 / otimo : 0.0;
}

static void parte_tsp(void) {
    int tamanhos[] = {5, 8, 10, 12};
    int qtd = (int)(sizeof(tamanhos) / sizeof(tamanhos[0]));

    printf("=== Parte 2: TSP - forca bruta x heuristicas ===\n\n");
    printf("%3s | %7s %13s | %7s %10s %7s | %7s %10s %7s | %s\n",
           "n", "FB", "tempo(us)", "VMP", "tempo(us)", "gap", "INS", "tempo(us)", "gap", "rotas");
    printf("----+-----------------------+-------------------------------+"
           "-------------------------------+------\n");

    for (int k = 0; k < qtd; k++) {
        int n = tamanhos[k];
        TSP *t = gerar_tsp_aleatorio(n, 777u + (unsigned)n);
        int *r_fb = malloc(n * sizeof(int));
        int *r_vm = malloc(n * sizeof(int));
        int *r_in = malloc(n * sizeof(int));
        int c_fb, c_vm, c_in;

        double t_fb = medir(tsp_forca_bruta, t, r_fb, &c_fb);
        double t_vm = medir(tsp_vizinho_mais_proximo, t, r_vm, &c_vm);
        double t_in = medir(tsp_insercao, t, r_in, &c_in);

        int ok = rota_valida(t, r_fb) && rota_valida(t, r_vm) && rota_valida(t, r_in)
              && custo_rota(t, r_fb) == c_fb && custo_rota(t, r_vm) == c_vm
              && custo_rota(t, r_in) == c_in;

        printf("%3d | %7d %13.2f | %7d %10.2f %6.1f%% | %7d %10.2f %6.1f%% | %s\n",
               n, c_fb, t_fb, c_vm, t_vm, gap(c_vm, c_fb),
               c_in, t_in, gap(c_in, c_fb), ok ? "ok" : "ERRO");

        free(r_fb);
        free(r_vm);
        free(r_in);
        liberar_tsp(t);
    }

    printf("\nFB  = forca bruta (otimo, (n-1)! permutacoes)\n");
    printf("VMP = vizinho mais proximo | INS = insercao do mais proximo\n");
    printf("gap = quanto o custo da heuristica excede o otimo\n");
}

int main(void) {
    parte_hamiltoniano();
    parte_tsp();
    return 0;
}