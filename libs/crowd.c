#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "crowd.h"
#include "image.h"
#include "image_usage.h"
#include "circular_list.h"
#include "logging.h"

// Créer un environnement à partir d'une image
environment_t env_from_image(image_t image) {
    log_debug("Création d'un environnement à partir de l'image : %s", image.name);

    environment_t env {
        .rows = image.rows,
        .cols = image.cols,
        .agents = (int**) malloc(sizeof(int*) * env.rows),
        .max = 0
    };
    for (int i = 0; i < env.rows; i++) {
        env.agents[i] = (int*) malloc(sizeof(int) * env.cols);
        for (int j = 0; j < env.cols; j++) {
            if (image.pixels[i][j] == 1.) env.agents[i][j] = -1;
            else env.agents[i][j] = 0;
        }
    }
    log_debug("Environnement créé à partir de l'image : %s", image.name);

    return env;
}

// Libérer la mémoire occupée par un environnement
void env_free(environment_t env) {
    log_debug("Libération de la mémoire d'un environnement");

    for (int i = 0; i < env.rows; i++) {
        free(env.agents[i]);
    }
    free(env.agents);

    log_debug("Mémoire de l'environnement libérée");
}

// Faire parcourir un environnement par un agent
void env_move_agent(environment_t* env, position_t start, position_t target) {
    log_debug("Déplacement d'un agent dans un environnement : %d,%d -> %d,%d", start.i, start.j, target.i, target.j);

    // Initialiser les tableaux
    position_t** pred = (position_t**) malloc(sizeof(position_t*) * env->rows);
    double** dis = (double**) malloc(sizeof(double*) * env->rows);
    bool** visited = (bool**) malloc(sizeof(bool*) * env->rows);
    for (int i = 0; i < env->rows; i++) {
        pred[i] = (position_t*) malloc(sizeof(position_t) * env->cols);
        dis[i] = (double*) malloc(sizeof(double) * env->cols);
        visited[i] = (bool*) malloc(sizeof(bool*) * env->cols);
        for (int j = 0; j < env->cols; j++) {
            pred[i][j] = (position_t) {.i = -1, .j = -1};
            dis[i][j] = INFINITY;
            visited[i][j] = false;
        }
    }
    dis[start.i][start.j] = 0;

    // Créer une file de priorité
    priority_queue_t* pq = pq_create(env->rows * env->cols);
    position_t* s = (position_t*) malloc(sizeof(position_t));
    s->i = start.i;
    s->j = start.j;
    pq_push(pq, 0, (void*) s);

    // Directions possibles
    int directions[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}, // Haut, Bas, Gauche, Droite
    };


    // Algorithme principal
    while (!pq_is_empty(pq)) {
        position_t* p = (position_t*) pq_pop(pq);

        if (visited[p->i][p->j]) continue;
        visited[p->i][p->j] = true;

        // Arrêter si on atteint la cible
        if (p->i == target.i && p->j == target.j) {
            free(p);
            break;
        };

        // Mettre à jour les voisins
        for (int d = 0; d < 4; d++) {
            int ni = p->i + directions[d][0];
            int nj = p->j + directions[d][1];

            if (ni >= 0 && ni < env->rows && nj >= 0 && nj < env->cols
                    && !visited[ni][nj] && env->agents[ni][nj] != -1) {
                double new_dist = dis[p->i][p->j] + env->agents[ni][nj] + 1;
                if (new_dist < dis[ni][nj]) {
                    dis[ni][nj] = new_dist;
                    pred[ni][nj] = *p;

                    position_t* pos = (position_t*) malloc(sizeof(position_t));
                    pos->i = ni;
                    pos->j = nj;

                    pq_push(pq, new_dist, (void*) pos);
                }
            }
        }
        free(p);
    }
    while (!pq_is_empty(pq)) {
        position_t* p = (position_t*) pq_pop(pq);
        free(p);
    }
    pq_free(pq);
    
    // Agir sur l'environnement
    position_t current = target;
    while (pred[current.i][current.j].i != -1) {
        env->agents[current.i][current.j]++;
        if (env->agents[current.i][current.j] > env->max) {
            env->max = env->agents[current.i][current.j];
        }
        current = pred[current.i][current.j];
    }
    env->agents[start.i][start.j]++;

    // Libérer les ressources
    for (int i = 0; i < env->rows; i++) {
        free(pred[i]);
        free(dis[i]);
        free(visited[i]);
    }

    free(pred);
    free(dis);
    free(visited);
}

// Faire parcourir un environnement par plusieurs agents
void env_move(environment_t* env, circular_list_t movements) {
    log_debug("Déplacement des agents dans un environnement");

    while (!circular_list_is_empty(movements)) {
        movement_t* m = (movement_t*) cl_get(movements);
        if (m->agents == 0) {
            cl_remove(&movements);
            continue;
        }
        env_move_agent(env, m->start, m->target);
        m->agents--;
        movements = cl_next(movements);
    }
    log_debug("Déplacement des agents terminé");
}

// Modifier une image en fonction de l'environnement
void env_image_edit(image_t image, environment_t env, int n) {
    log_debug("Modification de l'image en fonction de l'environnement : %s", image.name);

    if (env.max == 0) return;
    for (int i = 0; i < env.rows; i++) {
        for (int j = 0; j < env.cols; j++) {

            if (env.agents[i][j] > -1) {
                pixel_t pix = (double) env.agents[i][j] / (double) env.max;

                for (int k = 0; k < n; k++) {
                    if (i * n + k >= image.rows) break;
                    for (int l = 0; l < n; l++) {
                        if (j * n + l >= image.cols) break;
                        image.pixels[i * n + k][j * n + l] = pix;
                    }
                }
            }
        }
    }
    log_debug("Image modifiée en fonction de l'environnement : %s", image.name);
}

// Modifier une image colorée en fonction de l'environnement
void env_image_colored_edit(colored_image_t image, environment_t env, int n) {
    log_debug("Modification de l'image colorée en fonction de l'environnement : %s", image.name);

    if (env.max == 0) return;
    for (int i = 0; i < env.rows; i++) {
        for (int j = 0; j < env.cols; j++) {

            if (env.agents[i][j] > 0) {
                colored_pixel_t pix {
                    .r = 255. * (1. - (double) env.agents[i][j] / (double) env.max),
                    .g = 0,
                    .b = 0
                };

                for (int k = 0; k < n; k++) {
                    if (i * n + k >= image.rows) break;
                    for (int l = 0; l < n; l++) {
                        if (j * n + l >= image.cols) break;
                        image.pixels[i * n + k][j * n + l] = pix;
                    }
                }
            }
        }
    }
    log_debug("Image colorée modifiée en fonction de l'environnement : %s", image.name);
}