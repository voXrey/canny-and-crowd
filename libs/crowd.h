#ifndef CROWD_H
#define CROWD_H


#include "image.h"
#include "image_usage.h"
#include "circular_list.h"

struct environment_s {
    int rows;
    int cols;
    int** agents;
    int max;
};
typedef struct environment_s environment_t;

struct movement_s {
    position_t start;
    position_t target;
    int agents;
};
typedef struct movement_s movement_t;


// Créer un environnement à partir d'une image
environment_t env_from_image(image_t image);

// Libérer la mémoire occupée par un environnement
void env_free(environment_t env);

// Faire parcourir un environnement par un agent
void env_move_agent(environment_t* env, position_t start, position_t target);

// Faire parcourir un environnement par plusieurs agents
void env_move(environment_t* env, circular_list_t movements);

// Modifier une image en fonction de l'environnement
void env_image_edit(image_t image, environment_t env, int n);

// Modifier une image colorée en fonction de l'environnement
void env_image_colored_edit(colored_image_t image, environment_t env, int n);

#endif