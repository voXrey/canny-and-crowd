#ifndef COMMON_H
#define COMMON_H


typedef struct position_s {
    int i;
    int j;
} position_t;

struct movement_s {
    position_t start;
    position_t target;
    int agents;
};
typedef struct movement_s movement_t;

// Directions possibles
const int directions[4][2] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}, // Haut, Bas, Gauche, Droite
};

// Tableaux
extern position_t** pred;
extern double** dis;
extern double** heuristique;
extern int** heuristique_propagation;
extern int** heuristique_in_queue;
extern int** visited;
extern position_t*** ptrs;


#endif