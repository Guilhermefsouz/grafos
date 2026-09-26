#include <stdio.h>
#include <stdlib.h>
#include "conectividade.h"
#include "planaridade.h"

static void testar_conectividade(GrafoLista *g, const char *nome) {
    printf("----- %s -----\n", nome);

    int *eh_articulacao = calloc((size_t) g->n, sizeof(int));
    int total_articulacoes = contar_articulacoes(g, eh_articulacao);

    printf("Vertices de corte (%d): ", total_articulacoes);
    if (total_articulacoes == 0) {
        printf("nenhum");
    } else {
        int primeiro = 1;
        for (int i = 0; i < g->n; i++) {
            if (eh_articulacao[i]) {
                printf("%s%d", primeiro ? "" : ", ", i);
                primeiro = 0;
            }
        }
    }
    printf("\n");
    free(eh_articulacao);

    int total_pontes = 0;
    Ponte *pontes = detectar_pontes(g, &total_pontes);

    printf("Pontes (%d): ", total_pontes);
    if (total_pontes == 0) {
        printf("nenhuma");
    } else {
        for (int i = 0; i < total_pontes; i++) {
            printf("%s(%d-%d)", i == 0 ? "" : ", ", pontes[i].u, pontes[i].v);
        }
    }
    printf("\n");
    free(pontes);
}

static void testar_planaridade(GrafoLista *g, const char *nome) {
    printf("----- %s -----\n", nome);
    printf("Satisfaz Euler (m <= 3n-6)? %s\n", eh_planar_euler(g) ? "sim" : "nao");
    printf("Contem subdivisao de K5/K3,3? %s\n", contem_subdivisao_kuratowski(g) ? "sim" : "nao");
    printf("Conclusao (eh_planar): %s\n", eh_planar(g) ? "planar" : "nao planar");
}

int main(void) {
    /* ---- Grafo 1: "borboleta" -- dois triangulos ligados por uma ponte ----
     * triangulo {0,1,2} -- ponte (2,3) -- triangulo {3,4,5}
     * vertices 2 e 3 sao articulacoes; a aresta (2,3) e uma ponte. */
    GrafoLista *borboleta = criar_grafo_lista(6);
    inserir_aresta_lista(borboleta, 0, 1);
    inserir_aresta_lista(borboleta, 1, 2);
    inserir_aresta_lista(borboleta, 2, 0);
    inserir_aresta_lista(borboleta, 2, 3);
    inserir_aresta_lista(borboleta, 3, 4);
    inserir_aresta_lista(borboleta, 4, 5);
    inserir_aresta_lista(borboleta, 5, 3);

    printf("===== Conectividade =====\n");
    testar_conectividade(borboleta, "Grafo borboleta (2 triangulos + ponte)");

    printf("\n===== Planaridade =====\n");
    testar_planaridade(borboleta, "Grafo borboleta (planar, poucas arestas)");

    /* ---- Grafo 2: K5 (5 vertices, todos ligados entre si) ----
     * viola Euler diretamente (m=10 > 3*5-6=9). */
    GrafoLista *k5 = criar_grafo_lista(5);
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            inserir_aresta_lista(k5, i, j);
        }
    }
    printf("\n");
    testar_planaridade(k5, "K5 (nao planar - viola Euler)");

    /* ---- Grafo 3: K3,3 (bipartido completo 3+3) ----
     * m=9, n=6, 3n-6=12: satisfaz Euler, mas NAO e planar. E exatamente
     * o caso em que so a formula de Euler nao e suficiente, sendo
     * necessaria a heuristica de Kuratowski. */
    GrafoLista *k33 = criar_grafo_lista(6);
    for (int i = 0; i < 3; i++) {
        for (int j = 3; j < 6; j++) {
            inserir_aresta_lista(k33, i, j);
        }
    }
    printf("\n");
    testar_planaridade(k33, "K3,3 (nao planar - Euler nao detecta, Kuratowski sim)");

    /* ---- Grafo 4: ciclo simples C5 (planar de verdade) ---- */
    GrafoLista *c5 = criar_grafo_lista(5);
    for (int i = 0; i < 5; i++) {
        inserir_aresta_lista(c5, i, (i + 1) % 5);
    }
    printf("\n");
    testar_planaridade(c5, "Ciclo C5 (planar)");

    liberar_grafo_lista(borboleta);
    liberar_grafo_lista(k5);
    liberar_grafo_lista(k33);
    liberar_grafo_lista(c5);

    return 0;
}