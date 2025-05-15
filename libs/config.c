#include <stdio.h>

#include "config.h"


// Charger une configuration à partir d'un fichier
void config_load(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier de configuration : %s\n", filename);
        return;
    }
    fscanf(file, "DEBUG==%d\n", &DEBUG_MODE);
}