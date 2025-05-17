#ifndef CIRCULAR_LIST_H
#define CIRCULAR_LIST_H

#include <stdbool.h>

struct circular_list_node_s {
    struct circular_list_node_s* next;
    struct circular_list_node_s* prev;
    void* value;
};
typedef struct circular_list_node_s circular_list_node_t;

struct circular_list_s {
    circular_list_node_t* head;
    int size;
};
typedef struct circular_list_s circular_list_t;

// Créer une liste circulaire vide
circular_list_t* cl_create();

// Ajouter un élément à la liste circulaire (placé en tête)
void cl_add(circular_list_t* cl, void* value);

// Supprimer un élément de la liste circulaire (celui en tête)
void cl_remove(circular_list_t* cl);

// Libérer la mémoire occupée par la liste circulaire
void cl_free(circular_list_t* cl);

// Récupérer la valeur d'un élément de la liste circulaire (celui en tête)
void* cl_get(circular_list_t* cl);

// Définir la valeur d'un élément de la liste circulaire (celui en tête)
void cl_set(circular_list_t* cl, void* value);

// Passer à l'élément suivant de la liste circulaire
void cl_next(circular_list_t* cl);

// Passer à l'élément précédent de la liste circulaire
void cl_prev(circular_list_t* cl);

// Vérifier si la liste circulaire est vide
bool cl_is_empty(circular_list_t* cl);

#endif