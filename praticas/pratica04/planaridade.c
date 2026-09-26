#include <stdio.h>
#include <stdlib.h>
#include "planaridade.h"

/* ==================== Formula de Euler ==================== */

int eh_planar_euler(GrafoLista *g) {
    int n = g->n;
    long m = 0;

    for (int i = 0; i < n; i++) {
        for (No *aux = g->adj[i]; aux != NULL; aux = aux->prox) {
            m++;
        }
    }
    m /= 2; /* cada aresta foi contada duas vezes (grafo nao dirigido) */

    if (n < 3) {
        return 1; /* grafos com 0, 1 ou 2 vertices sao sempre planares */
    }

    return (m <= 3L * n - 6) ? 1 : 0;
}

/* ==================== Matriz de adjacencia auxiliar ==================== */

static int **criar_matriz(GrafoLista *g) {
    int n = g->n;
    int **mat = malloc((size_t) n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        mat[i] = calloc((size_t) n, sizeof(int));
    }
    for (int u = 0; u < n; u++) {
        for (No *aux = g->adj[u]; aux != NULL; aux = aux->prox) {
            mat[u][aux->destino] = 1;
        }
    }
    return mat;
}

static void liberar_matriz(int **mat, int n) {
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

/* ==================== Reducao topologica ====================
 * Suprime, um de cada vez, vertices de grau exatamente 2 ligando
 * diretamente seus dois vizinhos. Isso e exatamente o inverso de uma
 * subdivisao de aresta: se o grafo original continha uma subdivisao
 * de K5/K3,3, o grafo reduzido contera o proprio K5/K3,3 como
 * subgrafo (contanto que os vizinhos ja nao estejam ligados, caso em
 * que o vertice fica como esta, para nao criar arestas duplicadas). */
static void reduzir_topologicamente(int **mat, int n, int *removido) {
    for (int i = 0; i < n; i++) {
        removido[i] = 0;
    }

    int alterou = 1;
    while (alterou) {
        alterou = 0;

        for (int v = 0; v < n; v++) {
            if (removido[v]) {
                continue;
            }

            int grau = 0, a = -1, b = -1;
            for (int w = 0; w < n; w++) {
                if (!removido[w] && mat[v][w]) {
                    grau++;
                    if (a == -1) {
                        a = w;
                    } else {
                        b = w;
                    }
                }
            }

            if (grau == 2 && !mat[a][b]) {
                mat[a][b] = 1;
                mat[b][a] = 1;
                mat[v][a] = mat[a][v] = 0;
                mat[v][b] = mat[b][v] = 0;
                removido[v] = 1;
                alterou = 1;
            }
        }
    }
}

/* ==================== Busca por K5 (forca bruta) ==================== */

static int contem_k5(int **mat, int n, int *removido) {
    int vertices[10], total = 0;
    for (int i = 0; i < n && total < 10; i++) {
        if (!removido[i]) {
            vertices[total++] = i;
        }
    }

    if (total < 5) {
        return 0;
    }

    for (int a = 0; a < total; a++) {
        for (int b = a + 1; b < total; b++) {
            for (int c = b + 1; c < total; c++) {
                for (int d = c + 1; d < total; d++) {
                    for (int e = d + 1; e < total; e++) {
                        int v[5] = { vertices[a], vertices[b], vertices[c],
                                     vertices[d], vertices[e] };
                        int completo = 1;
                        for (int i = 0; i < 5 && completo; i++) {
                            for (int j = i + 1; j < 5 && completo; j++) {
                                if (!mat[v[i]][v[j]]) {
                                    completo = 0;
                                }
                            }
                        }
                        if (completo) {
                            return 1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

/* ==================== Busca por K3,3 (forca bruta) ==================== */

static int bipartido_completo(int **mat, const int *lado_a, const int *lado_b) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (!mat[lado_a[i]][lado_b[j]]) {
                return 0;
            }
        }
    }
    return 1;
}

static int contem_k33(int **mat, int n, int *removido) {
    int vertices[10], total = 0;
    for (int i = 0; i < n && total < 10; i++) {
        if (!removido[i]) {
            vertices[total++] = i;
        }
    }

    if (total < 6) {
        return 0;
    }

    /* todas as formas de escolher 2 indices (dentre 1..5) para
     * acompanhar o indice 0 no "lado A"; os 3 indices restantes
     * (tambem dentre 1..5) formam o "lado B". Cobre as 10 particoes
     * distintas de um conjunto de 6 elementos em dois grupos de 3. */
    static const int pares[10][2] = {
        {1, 2}, {1, 3}, {1, 4}, {1, 5},
        {2, 3}, {2, 4}, {2, 5},
        {3, 4}, {3, 5},
        {4, 5}
    };

    for (int a = 0; a < total; a++) {
    for (int b = a + 1; b < total; b++) {
    for (int c = b + 1; c < total; c++) {
    for (int d = c + 1; d < total; d++) {
    for (int e = d + 1; e < total; e++) {
    for (int f = e + 1; f < total; f++) {
        int seis[6] = { vertices[a], vertices[b], vertices[c],
                         vertices[d], vertices[e], vertices[f] };

        for (int p = 0; p < 10; p++) {
            int x = pares[p][0];
            int y = pares[p][1];
            int lado_a[3] = { seis[0], seis[x], seis[y] };
            int lado_b[3];
            int k = 0;

            for (int idx = 1; idx <= 5; idx++) {
                if (idx != x && idx != y) {
                    lado_b[k++] = seis[idx];
                }
            }

            if (bipartido_completo(mat, lado_a, lado_b)) {
                return 1;
            }
        }
    }}}}}}

    return 0;
}

/* ==================== Funcoes publicas ==================== */

int contem_subdivisao_kuratowski(GrafoLista *g) {
    int n = g->n;

    if (n > 10) {
        /* heuristica de forca bruta definida apenas para n <= 10 */
        return 0;
    }

    int **mat = criar_matriz(g);
    int *removido = malloc((size_t) n * sizeof(int));

    reduzir_topologicamente(mat, n, removido);

    int achou = contem_k5(mat, n, removido) || contem_k33(mat, n, removido);

    free(removido);
    liberar_matriz(mat, n);

    return achou;
}

int eh_planar(GrafoLista *g) {
    if (!eh_planar_euler(g)) {
        return 0;
    }
    if (g->n <= 10 && contem_subdivisao_kuratowski(g)) {
        return 0;
    }
    return 1;
}