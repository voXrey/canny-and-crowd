#ifndef CROWD_H
#define CROWD_H


#include "image.h"
#include "image_usage.h"
#include "circular_list.h"
#include "common.h"

struct environment_s {
    int rows;
    int cols;
    int** agents;
    int max;
};
typedef struct environment_s environment_t;

// Créer un environnement à partir d'une image
environment_t env_from_image(image_t image);

// Libérer la mémoire occupée par un environnement
void env_free(environment_t env);

// Modifier une image en fonction de l'environnement
void env_image_edit(image_t image, environment_t env, int n);

// Modifier une image colorée en fonction de l'environnement
void env_image_colored_edit(colored_image_t image, environment_t env, int n);

// Parcourir un environnement avec un A* itératif
void move_env_iterative_a_star(movement_t movement, environment_t* env, int weight, int);

// Appliquer plusieurs mouvements à un environnement avec A* itératif
void multiple_move_env_iterative_a_star(circular_list_t* movements, environment_t* env, int weight, int modulo);

// Initialiser les tableaux nécessaires pour les déplacements dans un environnement
void env_initialiser_tableaux(environment_t* env);

// Libérer les ressources allouées pour les tableaux
void env_liberer_tableaux(environment_t* env);

#endif