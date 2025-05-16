#include <stdio.h>

#include "config.h"

int DEBUG_MODE = 0; // Mode de débogage par défaut

// Charger une configuration à partir d'un fichier
void config_load(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de configuration : %s\n", filename);
        return;
    }
    fscanf(file, "DEBUG_MODE==%d\n", &DEBUG_MODE);
    fclose(file);
    fprintf(stderr, "debug mode : %d\n", DEBUG_MODE);
}