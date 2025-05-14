#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "crowd.h"
#include "image.h"
#include "image_usage.h"
#include "circular_list.h"


// Créer un environnement à partir d'une image
environment_t env_from_image(image_t image) {
    environment_t env {
        .rows = image.rows,
        .cols = image.cols,
        .agents = (int**) malloc(sizeof(int*) * env.rows),
        .max = 0
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
void env_free(environment_t env) {
    for (int i = 0; i < env.rows; i++) {
        free(env.agents[i]);
    }
    free(env.agents);
}

// Faire parcourir un environnement par un agent
void env_move_agent(environment_t env, position_t start, position_t target) {
    
    // Initialiser les tableaux
    position_t** pred = (position_t**) malloc(sizeof(position_t*) * env.rows);
    double** dis = (double**) malloc(sizeof(double*) * env.rows);
    bool** visited = (bool**) malloc(sizeof(bool*) * env.rows);
    for (int i = 0; i < env.rows; i++) {
        pred[i] = (position_t*) malloc(sizeof(position_t) * env.cols);
        dis[i] = (double*) malloc(sizeof(double) * env.cols);
        visited[i] = (bool*) malloc(sizeof(bool*) * env.cols);
        for (int j = 0; j < env.cols; j++) {
            pred[i][j] = (position_t){.i = -1, .j = -1};
            dis[i][j] = INFINITY;
            visited[i][j] = env.agents[i][j] == -1;
        }
    }
    dis[start.i][start.j] = 0;

    // Créer une file de priorité
    priority_queue_t* pq = pq_create(env.rows * env.cols);
    position_t* s = (position_t*) malloc(sizeof(position_t));
    s->i = s.i;
    s->j = s.j;
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
        if (p->i == target.i && p->j == target.j) break;

        // Mettre à jour les voisins
        for (int d = 0; d < 4; d++) {
            int ni = p->i + directions[d][0];
            int nj = p->j + directions[d][1];

            if (ni >= 0 && ni < env.rows && nj >= 0 && nj < env.cols && !visited[ni][nj]) {
                double new_dist = dis[p->i][p->j] + env.agents[ni][nj];
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
    while (current.i != start.i && current.j != start.j) {
        env.agents[current.i][current.j]++;
        if (env.agents[current.i][current.j] > env.max) {
            env.max = env.agents[current.i][current.j];
        }
        current = pred[current.i][current.j];
    }
    env.agents[start.i][start.j]++;

    // Libérer les ressources
    for (int i = 0; i < image.rows; i++) {
        free(pred[i]);
        free(dis[i]);
        free(visited[i]);
    }

    free(pred);
    free(dis);
    free(visited);
    free(s);
}

// Faire parcourir un environnement par plusieurs agents
void env_move(environment_t env, circular_list_t* movements) {
    while (!circular_list_is_empty(*movements)) {
        movement_t* m = (movement_t*) cl_get(*movements);
        if (m->agents == 0) {
            cl_remove(movements);
            continue;
        }
        env_move_agent(env, m->start, m->target);
        m->agents--;
        movements = cl_next(*movements);
    }
}

// Modifier une image en fonction de l'environnement
void env_image_edit(image_t* image, environment_t env) {
    if (env.max == 0) return;
    for (int i = 0; i < env.rows; i++) {
        for (int j = 0; j < env.cols; j++) {
            if (env.agents[i][j] > 0) {
                image->pixels[i][j] = (double) env.agents[i][j] / (double) env.max;
            }
        }
    }
}

// Modifier une image colorée en fonction de l'environnement
void env_image_color_edit(image_t* image, environment_t env) {
    if (env.max == 0) return;
    for (int i = 0; i < env.rows; i++) {
        for (int j = 0; j < env.cols; j++) {
            if (env.agents[i][j] > 0) {
                image->pixels[i][j].r = 255. * (1. - (double) env.agents[i][j] / (double) env.max);
                image->pixels[i][j].g = 0;
                image->pixels[i][j].b = 0;
            }
        }
    }
}