#include <stdio.h>
#include <stdlib.h>

#include "csv.h"
#include "common.h"
#include "circular_list.h"

circular_list_t* load_movements(const char* filename) {
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
            movement_t* m = malloc(sizeof(movement_t));
            cl_add(cl, (void*) m);
        }
    }

    fclose(file);
    return cl;
}

void free_movements(circular_list_t* cl) {
    if (circular_list_is_empty(cl)) {
        cl_free(cl);
        return;
    }
    movement_t* m = (movement_t*) circular_list_get(cl);
    free(m);
    circular_list_remove(cl);
    free_movements(cl);
}