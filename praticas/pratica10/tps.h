#ifndef TPS_H
#define TPS_H

#include "hamiltoniano.h"

/* ---------- Instância do TSP: grafo completo com matriz de custos simétrica ---------- */
typedef struct {
    int n;
    int **c;   /* c[i][j] = custo de ir de i para j */
} TSP;

TSP *criar_tsp(int n);
void liberar_tsp(TSP *t);

/* Instância aleatória reprodutível: n pontos numa grade 100x100 e custo =
   distância de Manhattan (satisfaz a desigualdade triangular). */
TSP *gerar_tsp_aleatorio(int n, unsigned int semente);

/* Redução Hamiltoniano -> TSP: custo 1 para arestas existentes e 2 para as
   inexistentes. O grafo tem ciclo hamiltoniano  <=>  o TSP ótimo custa n. */
TSP *grafo_para_tsp(Grafo *g);

/* ---------- Utilidades ---------- */
/* Custo da rota que visita rota[0..n-1] em ordem e volta a rota[0]. */
int custo_rota(TSP *t, const int *rota);

/* 1 se 'rota' é uma permutação de 0..n-1. */
int rota_valida(TSP *t, const int *rota);

/* ---------- Algoritmos ----------
   Todos preenchem rota[0..n-1] (sempre começando em 0) e retornam o custo
   do ciclo, já incluindo a volta ao início. */

/* Força bruta: fixa a origem 0 e testa as (n-1)! permutações. */
int tsp_forca_bruta(TSP *t, int *rota);

/* Heurística construtiva: a partir de 0, vai sempre para o vizinho
   não visitado mais próximo. O(n^2). */
int tsp_vizinho_mais_proximo(TSP *t, int *rota);

/* Heurística de inserção do mais próximo: parte de um ciclo com 0 e seu
   vizinho mais próximo; repetidamente pega o vértice fora do ciclo mais
   próximo dele e o insere na posição de menor aumento de custo. O(n^3). */
int tsp_insercao(TSP *t, int *rota);

#endif