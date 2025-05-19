#include <stdio.h>
#include <stdlib.h>

#include "csv.h"
#include "common.h"
#include "circular_list.h"

circular_list_t* load_movements(const char* filename, int n) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;

    char line[4096];
    // Sauter l'en-tête
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return NULL;
    }

    circular_list_t* cl = cl_create();

    while (fgets(line, sizeof(line), file)) {
        position_t start, target;
        int agents;

        if (sscanf(line, "%d:%d,%d:%d,%d", &start.i, &start.j, &target.i, &target.j, &agents) == 5) {
            start.i /= n;
            start.j /= n;
            target.i /= n;
            target.j /= n;
            
            movement_t* m = (movement_t*) malloc(sizeof(movement_t));
            m->start = start;
            m->target = target;
            m->agents = agents;
            cl_add(cl, (void*) m);
        }
    }

    fclose(file);
    return cl;
}

void free_movements(circular_list_t* cl) {
    if (cl_is_empty(cl)) {
        cl_free(cl);
        return;
    }
    movement_t* m = (movement_t*) cl_get(cl);
    free(m);
    cl_remove(cl);
    free_movements(cl);
}

// Écrire le résultat en performance d'une exécution de parcours dans un fichier CSV
void write_result(const char filename, int modulo, int clocks, double time) {
    fopen(filename, "a");
    FILE* file = fopen(filename, "a");
    if (!file) return;
    fprintf(file, "%d;%d;%ld\n", modulo, clocks, time);
    fclose(file);
}