#include <stdlib.h>
#include <stdbool.h>

#include "circular_list.h"

// Créer une liste circulaire
circular_list_t* cl_create() {
    circular_list_t* cl = (circular_list_t*) malloc(sizeof(circular_list_t));
    cl->head = NULL;
    cl->size = 0;
    return cl;
}

// Ajouter un élément à la liste circulaire (placé en tête)
void cl_add(circular_list_t* cl, void* value) {
    circular_list_node_t* node = (circular_list_node_t*) malloc(sizeof(circular_list_node_t));
    node->value = value;

    if (cl_is_empty(cl)) {
        node->next = node;
        node->prev = node;
        cl->head = node;
    }
    else {
        node->next = cl->head;
        node->prev = cl->head->prev;
        cl->head->prev->next = node;
        cl->head->prev = node;
    }
    cl->head = node;
    cl->size++;
}

// Supprimer un élément de la liste circulaire (celui en tête), puis passage au suivant
void cl_remove(circular_list_t* cl) {
    if (cl_is_empty(cl)) return;

    circular_list_node_t* node = cl->head;
    if (cl->size == 1) {
        cl->head = NULL;
    }
    else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        cl->head = node->next;
    }
    free(node);
    cl->size--;
}

// Libérer la mémoire occupée par la liste circulaire
void cl_free(circular_list_t* cl) {
    circular_list_node_t* current = cl->head;
    while (current != NULL) {
        circular_list_node_t* next = current->next;
        free(current);
        current = next;
    }
    free(cl);
}

// Récupérer la valeur d'un élément de la liste circulaire (celui en tête)
void* cl_get(circular_list_t* cl) {
    if (cl_is_empty(cl)) return NULL;
    return cl->head->value;
}

// Définir la valeur d'un élément de la liste circulaire (celui en tête)
void cl_set(circular_list_t* cl, void* value) {
    if (cl_is_empty(cl)) return;
    cl->head->value = value;
}

// Passer à l'élément suivant de la liste circulaire
void cl_next(circular_list_t* cl) {
    if (cl_is_empty(cl)) return;
    cl->head = cl->head->next;
}

// Passer à l'élément précédent de la liste circulaire
void cl_prev(circular_list_t* cl) {
    if (cl_is_empty(cl)) return;
    cl->head = cl->head->prev;
}

// Vérifier si la liste circulaire est vide
bool cl_is_empty(circular_list_t* cl) {
    return cl->size == 0;
}