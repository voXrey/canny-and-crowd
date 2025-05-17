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
                double new_dist = dis[p->i][p->j] + env->agents[ni][nj] + 20;
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
void env_move(environment_t* env, circular_list_t* movements) {
    log_debug("Déplacement des agents dans un environnement");

    while (!cl_is_empty(movements)) {
        movement_t* m = (movement_t*) cl_get(movements);
        if (m->agents == 0) {
            cl_remove(movements);
            continue;
        }
        env_move_agent(env, m->start, m->target);
        m->agents--;
        cl_next(movements);
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

// Distance de Manhattan
int manhattan_distance(position_t a, position_t b) {
    return abs(a.i - b.i) + abs(a.j - b.j);
}

// Parcourir un environnement avec un A* itératif
void move_env_a_star(movement_t movement, environment_t* env, int weight) {
    log_debug("Déplacement d'agents dans un environnement avec A* itératif");
    position_t start = movement.start;
    position_t target = movement.target;
    int agents = movement.agents;

    // Initialiser les tableaux
    position_t** pred = (position_t**) malloc(sizeof(position_t*) * env->rows);
    double** dis = (double**) malloc(sizeof(double*) * env->rows);
    double** heuristique = (double**) malloc(sizeof(double*) * env->rows);
    int** visited = (int**) malloc(sizeof(int*) * env->rows);
    position_t*** ptrs = (position_t***) malloc(sizeof(position_t**) * env->rows);
    for (int i = 0; i < env->rows; i++) {
        pred[i] = (position_t*) malloc(sizeof(position_t) * env->cols);
        heuristique[i] = (double*) malloc(sizeof(double) * env->cols);
        dis[i] = (double*) malloc(sizeof(double) * env->cols);
        visited[i] = (int*) malloc(sizeof(int*) * env->cols);
        ptrs[i] = (position_t**) malloc(sizeof(position_t*) * env->cols);
        for (int j = 0; j < env->cols; j++) {
            position_t pos = {.i = i, .j = j};
            pred[i][j] = (position_t) {.i = -1, .j = -1};
            heuristique[i][j] = manhattan_distance(pos, target);
            visited[i][j] = -1;
            ptrs[i][j] = (position_t*) malloc(sizeof(position_t));
            ptrs[i][j]->i = i;
            ptrs[i][j]->j = j;
        }
    }
    dis[start.i][start.j] = 0;
    visited[start.i][start.j] = 0;

    // Créer une file de priorité
    priority_queue_t* pq = pq_create(env->rows * env->cols);
    
    
    // Boucle principale
    int iteration = 0;
    while (agents > 0) {
        position_t* s = ptrs[start.i][start.j];
        pq_push(pq, 0, (void*) s);

        // Directions possibles
        int directions[4][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}, // Haut, Bas, Gauche, Droite
        };

        while (!pq_is_empty(pq)) {
            position_t* u = (position_t*) pq_pop(pq);
            if (u->i == target.i && u->j == target.j) break;

            for (int d = 0; d < 4; d++) {
                int ni = u->i + directions[d][0];
                int nj = u->j + directions[d][1];

                if (ni >= 0 && ni < env->rows && nj >= 0 && nj < env->cols
                        && visited[ni][nj] < iteration && env->agents[ni][nj] != -1) {
                    
                    double dis_n = (visited[ni][nj] == iteration) ? dis[ni][nj] : INFINITY;
                    double new_dist = dis[u->i][u->j] + env->agents[ni][nj] + weight;

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
            position_t* p = (position_t*) pq_pop(pq);
        }

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

        iteration++;
        agents--;
    }

    // Libérer les ressources
    for (int i = 0; i < env->rows; i++) {
        free(pred[i]);
        free(dis[i]);
        free(heuristique[i]);
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
    free(visited);

    pq_free(pq);

    log_debug("Déplacement d'agents dans un environnement avec A* itératif terminé");
}

// Appliquer plusieurs mouvements à un environnement avec A* itératif
void multiple_move_env_a_star(circular_list_t* movements, environment_t* env, int weight) {
    log_debug("Déplacement d'agents dans un environnement avec plusieurs mouvements avec A* itératif");
    int i = 0;
    int n = movements->size;
    while (!cl_is_empty(movements)) {
        movement_t* m = (movement_t*) cl_get(movements);
        log_debug("(%d/%d) Déplacement de %d agents de (%d, %d) vers (%d, %d)", i, n, m->agents, m->start.i, m->start.j, m->target.i, m->target.j);
        move_env_a_star(*m, env, weight);
        i++;
        log_debug("(%d/%d) Déplacement terminé", i, n);
        free(m);
        cl_remove(movements);
    }

    log_debug("Déplacement d'agents dans un environnement avec plusieurs mouvements avec A* itératif terminé");
}