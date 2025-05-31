#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "crowd.h"
#include "image.h"
#include "image_usage.h"
#include "circular_list.h"
#include "logging.h"
#include "common.h"

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
                double alpha = 1. - (double) env.agents[i][j] / (double) env.max;
                colored_pixel_t pix {
                    .r = 255. * alpha * alpha * alpha,
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

// Distance norme 1
int distance_norme1(position_t p1, position_t p2) {
    return abs(p1.i - p2.i) + abs(p1.j - p2.j);
}

// Parcourir un environnement avec un A* itératif
void move_env_iterative_a_star(movement_t movement, environment_t* env, int weight0, int alpha, int modulo) {
    log_debug("Déplacement de %d agents dans un environnement avec A* itératif", movement.agents);
    position_t start = movement.start;
    position_t target = movement.target;
    int agents = movement.agents;

    // Initialiser les tableaux
    for (int i = 0; i < env->rows; i++) {
        for (int j = 0; j < env->cols; j++) {
            visited[i][j] = -1;
        }
    }

    // Créer une file de priorité
    priority_queue_t* pq = pq_create(env->rows * env->cols);

    // Boucle principale
    position_t* s;
    position_t* t;
    int iteration = 0;
    while (agents > 0) {
        if (iteration % modulo != 0) {
            dis[start.i][start.j] = 0.;
            visited[start.i][start.j] = iteration;
            s = ptrs[start.i][start.j];
            t = ptrs[target.i][target.j];
        }
        else {
            dis[target.i][target.j] = 0.;
            visited[target.i][target.j] = iteration;
            s = ptrs[target.i][target.j];
            t = ptrs[start.i][start.j];
        }
        pq_push(pq, 0, (void*) s);

        while (!pq_is_empty(pq)) {
            position_t* u = (position_t*) pq_pop(pq);
            if ((iteration % modulo != 0 || agents == 1) && u->i == t->i && u->j == t->j) break;

            for (int d = 0; d < 4; d++) {
                int ni = u->i + directions[d][0];
                int nj = u->j + directions[d][1];

                if (ni >= 0 && ni < env->rows && nj >= 0 && nj < env->cols
                        && visited[ni][nj] < iteration && env->agents[ni][nj] != -1) {
                    
                    double dis_n = (visited[ni][nj] == iteration) ? dis[ni][nj] : INFINITY;
                    double new_dist = dis[u->i][u->j] + (env->agents[ni][nj]*alpha + weight0);

                    if (new_dist < dis_n) {
                        dis[ni][nj] = new_dist;
                        pred[ni][nj] = *u;

                        position_t* pos = ptrs[ni][nj];
                        
                        visited[ni][nj] = iteration;

                        int total_cost = new_dist + heuristique[ni][nj];

                        pq_push(pq, total_cost, (void*) pos);
                    }
                }
            }
            visited[u->i][u->j] = iteration;
        }
        while (!pq_is_empty(pq)) {
            s = (position_t*) pq_pop(pq);
        }
        // Agir sur l'environnement

        if (iteration % modulo == 0) {
            double** temps = heuristique;
            heuristique = dis;
            dis = temps;
        }
        else {
            position_t current = target;
            while ((current.i != start.i || current.j != start.j)
                    && visited[current.i][current.j] == iteration) {
                env->agents[current.i][current.j]++;
                heuristique[current.i][current.j] = dis[target.i][target.j]-dis[current.i][current.j];
                if (env->agents[current.i][current.j] > env->max) {
                    env->max = env->agents[current.i][current.j];
                }
                current = pred[current.i][current.j];
            }
            env->agents[start.i][start.j]++;
            if (env->agents[start.i][start.j] > env->max) {
                env->max = env->agents[start.i][start.j];
            }
        }
        
        iteration++;
        agents--;
    }
    log_debug("Tous les agents ont été déplacés");
    // Libérer les ressources
    pq_free(pq);

    log_debug("Déplacement des %d agents dans un environnement avec A* itératif terminé", movement.agents);
}

// Appliquer plusieurs mouvements à un environnement avec A* itératif
void multiple_move_env_iterative_a_star(circular_list_t* movements, environment_t* env,
                                        int weight0, int alpha, int modulo) {
    log_debug("Déplacement d'agents dans un environnement avec A* itératif");
    int n = movements->size;
    while (!cl_is_empty(movements)) {
        movement_t* m = (movement_t*) cl_get(movements);
        move_env_iterative_a_star(*m, env, weight0, alpha, modulo);
        free(m);
        cl_remove(movements);
    }
    log_debug("Déplacement d'agents dans un environnement avec A* itératif terminé");
}

// Initialiser les tableaux nécessaires pour les déplacements dans un environnement
void env_initialiser_tableaux(environment_t* env) {
    pred = (position_t**) malloc(sizeof(position_t*) * env->rows);
    dis = (double**) malloc(sizeof(double*) * env->rows);
    heuristique = (double**) malloc(sizeof(double*) * env->rows);
    heuristique_propagation = (int**) malloc(sizeof(int*) * env->rows);
    heuristique_in_queue = (int**) malloc(sizeof(int*) * env->rows);
    visited = (int**) malloc(sizeof(int*) * env->rows);
    ptrs = (position_t***) malloc(sizeof(position_t**) * env->rows);
    for (int i = 0; i < env->rows; i++) {
        pred[i] = (position_t*) malloc(sizeof(position_t) * env->cols);
        heuristique[i] = (double*) malloc(sizeof(double) * env->cols);
        heuristique_propagation[i] = (int*) malloc(sizeof(int) * env->cols);
        heuristique_in_queue[i] = (int*) malloc(sizeof(int) * env->cols);
        dis[i] = (double*) malloc(sizeof(double) * env->cols);
        visited[i] = (int*) malloc(sizeof(int*) * env->cols);
        ptrs[i] = (position_t**) malloc(sizeof(position_t*) * env->cols);
        for (int j = 0; j < env->cols; j++) {
            position_t pos = {.i = i, .j = j};
            pred[i][j] = (position_t) {.i = -1, .j = -1};
            heuristique[i][j] = 0;
            heuristique_propagation[i][j] = 0;
            heuristique_in_queue[i][j] = false;
            visited[i][j] = -1;
            ptrs[i][j] = (position_t*) malloc(sizeof(position_t));
            ptrs[i][j]->i = i;
            ptrs[i][j]->j = j;
        }
    }
}

// Libérer les ressources allouées pour les tableaux
void env_liberer_tableaux(environment_t* env) {
    for (int i = 0; i < env->rows; i++) {
        free(pred[i]);
        free(dis[i]);
        free(heuristique[i]);
        free(heuristique_propagation[i]);
        free(heuristique_in_queue[i]);
        free(visited[i]);
        for (int j = 0; j < env->cols; j++) {
            free(ptrs[i][j]);
        }
        free(ptrs[i]);
    }
    free(ptrs);
    free(pred);
    free(dis);
    free(heuristique);
    free(heuristique_propagation);
    free(heuristique_in_queue);
    free(visited);
}
