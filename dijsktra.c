#include <linux/limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>

#include "dijsktra.h"
#include "linked_list.h"

struct graph_values_type_s {
    int a;
    int b;
    int q;
};
typedef struct graph_values_type_s graph_values_type;

struct graph_type_s {
    graph_values_type** arr;
    int n;
    int m;
};
typedef struct graph_type_s graph_type;

struct position_type_s {
    int i;
    int j;
};
typedef struct position_type_s position_type;

// Utils
int** make_int_matrix(int n, int m, int default_value) {
    int** matrix = malloc(sizeof(int*)*n);
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(sizeof(int)*m);
        for (int j = 0; j < m; j++) {
            matrix[i][j] = default_value;
        }
    }
    return matrix;
}

position_type** make_pos_matrix(int n, int m, position_type default_value) {
    position_type** matrix = malloc(sizeof(position_type*)*n);
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(sizeof(position_type)*m);
        for (int j = 0; j < m; j++) {
            matrix[i][j] = default_value;
        }
    }
    return matrix;
}

position_type none_position = {-1, -1};

// Calcule le poids à partir de la donnée d'un noeud
int calcul_weight(graph_values_type data) {
    return data.a * data.q + data.b;
}

// Teste si 2 positions sont égales
bool equals_positions(position_type p1, position_type p2) {
    return p1.i == p2.i && p1.j == p2.j;
}

// Renvoie le sommet de distance minimale parmi ceux de la liste et la supprime de celle-ci
position_type min_distance(int** distances, LinkedList* sommets) {
    int min = INT_MAX;
    Mail* min_mail = NULL;

    Mail* sommet = sommets->head;
    while (sommet != NULL) {
        position_type* pos = sommet->data;
        if (distances[pos->i][pos->j] < min) {
            min = distances[pos->i][pos->j];
            min_mail = sommet;
        }
    }
    position_type pos = *(position_type*) min_mail->data;

    free(min_mail->data);
    list_delete_mail(min_mail);

    return pos;
}

// Mise à jour des distances pour déterminer par quel sommet il faut mieux passer
void update_distances(graph_type graph, int** distances, position_type** pred, position_type p1, position_type p2) {
    int w = calcul_weight(graph.arr[p2.i][p2.j]);

    if (distances[p1.i][p1.j] < INT_MAX && distances[p2.i][p2.j] > distances[p1.i][p1.j] + w) {
        distances[p2.i][p2.j] = distances[p1.i][p1.j] + w;
        pred[p2.i][p2.j] = p1;
    }
}

// Recherche le plus court chemin entre 2 positions du graphe selon l'algorithme de Dijskra
LinkedList* dijsktra(graph_type graph, position_type start_position, position_type end_position) {
    // Initialisation
    int** distances = make_int_matrix(graph.n, graph.m, INT_MAX);
    position_type** pred = make_pos_matrix(graph.n, graph.m, none_position);
    distances[start_position.i][start_position.j] = 0;
    pred[start_position.i][start_position.j] = start_position;

    LinkedList* q = create_list();
    for (int i = 0; i < graph.n; i++) {
        for (int j = 0; j < graph.m; j++) {
            position_type* pos = malloc(sizeof(position_type));
            pos->i = i;
            pos->j = j;
            list_add_front(q, pos);
        }
    }

    while (q->head != NULL) {
        position_type p1 = min_distance(distances, q);
        
    }

}

