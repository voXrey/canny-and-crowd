#ifndef CSV_H
#define CSV_H

#include "common.h"
#include "circular_list.h"

// Charge les mouvements depuis un fichier CSV et retourne la tête de la liste circulaire
circular_list_t* load_movements(const char* filename);

// Libère la mémoire de la liste circulaire
void free_movements(circular_list_t* cl);

#endif // CSV_H