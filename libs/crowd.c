#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "crowd.h"
#include "image.h"
#include "image_usage.h"
#include "circular_list.h"


// Créer un environnement à partir d'une image
environment_t environment_from_image(image_t image) {
    environment_t env {
        .rows = image.rows,
        .cols = image.cols,
        .agents = (int**) malloc(sizeof(int*) * env.rows)
    }
    for (int i = 0; i < env.rows; i++) {
        env.agents[i] = (int*) malloc(sizeof(int) * env.cols);
        for (int j = 0; j < env.cols; j++) {
            if (image.pixels[i][j] == 1.) env.agents[i][j] = -1;
            else env.agents[i][j] = 0;
        }
    }
    return env;
}

// Libérer la mémoire occupée par un environnement
void environment_free(environment_t env) {
    for (int i = 0; i < env.rows; i++) {
        free(env.agents[i]);
    }
    free(env.agents);
}


