#include <stdlib.h>
#include <stdbool.h>

#include "circular_list.h"

// Créer une liste circulaire
circular_list_t cl_create() {
    return NULL;
}

// Ajouter un élément à la liste circulaire (avant l'élément courant)
void cl_add(circular_list_t* cl, void* value) {
    circular_list_node_t* node = (circular_list_node_t*) malloc(sizeof(circular_list_node_t));
    node->value = value;

    if (*cl == NULL) {
        node->next = node;
        node->prev = node;
    } else {
        node->next = *cl;
        node->prev = (*cl)->prev;
        (*cl)->prev->next = node;
        (*cl)->prev = node;
    }
    *cl = node;
}

// Supprimer un élément de la liste circulaire
void cl_remove(circular_list_t* cl) {
    if (*cl == NULL) return;
    circular_list_node_t* node = *cl;
    
    if (node->next == node) *cl = NULL;
    else *cl = node->next;

    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    free(node);
}

// Libérer la mémoire occupée par la liste circulaire
void cl_free(circular_list_t cl) {
    while (cl != NULL) {
        cl_remove(&cl);
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

// Vérifier si la liste circulaire est vide
bool circular_list_is_empty(circular_list_t cl) {
    return cl == NULL;
}