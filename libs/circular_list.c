#include <stdlib.h>
#include <stdbool.h>

#include "circular_list.h"


// Ajouter un élément à la liste circulaire (avant l'élément donné en argument)
void cl_add(circular_list_t cl, void* value) {
    circular_list_node_t* node = (circular_list_node_t*) malloc(sizeof(circular_list_node_t));
    node->next = cl;
    if (cl != NULL) {
        cl->prev = node;
        node->prev = cl->prev;
    }
    else node ->prev = node;
    node->value = value;
}

// Supprimer un élément de la liste circulaire
void cl_remove(circular_list_t* cl) {
    if (*cl == NULL) return;
    circular_list_node_t* node = *cl;
    
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    if (node->prev == node) *cl = NULL;
    else *cl = node->next;

    free(node);
}

// Libérer la mémoire occupée par la liste circulaire
void cl_free(circular_list_t* cl) {
    while (*cl != NULL) {
        cl_remove(cl);
    }
}

// Récupérer la valeur d'un élément de la liste circulaire
void* cl_get(circular_list_t cl) {
    if (cl == NULL) return NULL;
    return cl->value;
}

// Définir la valeur d'un élément de la liste circulaire
void cl_set(circular_list_t cl, void* value) {
    if (cl == NULL) return;
    cl->value = value;
}

// Récupérer le prochain élément de la liste circulaire
circular_list_t cl_next(circular_list_t cl) {
    if (cl == NULL) return NULL;
    return cl->next;
}

// Récupérer le précédent élément de la liste circulaire
circular_list_t cl_prev(circular_list_t cl) {
    if (cl == NULL) return NULL;
    return cl->prev;
}
