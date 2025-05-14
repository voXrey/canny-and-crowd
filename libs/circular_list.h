#ifndef CIRCULAR_LIST_H
#define CIRCULAR_LIST_H

struct circular_list_node_s {
    struct circular_list_node_s* next;
    struct circular_list_node_s* prev;
    void* value;
};
typedef struct circular_list_node_s circular_list_node_t;

typedef circular_list circular_list_t;


// Ajouter un élément à la liste circulaire (avant l'élément donné en argument)
void cl_add(circular_list_t cl, void* value);

// Supprimer un élément de la liste circulaire
void cl_remove(circular_list_t* cl);

// Libérer la mémoire occupée par la liste circulaire
void cl_free(circular_list_t* cl);

// Récupérer la valeur d'un élément de la liste circulaire
void* cl_get(circular_list_t cl);

// Définir la valeur d'un élément de la liste circulaire
void cl_set(circular_list_t cl, void* value);

// Récupérer le prochain élément de la liste circulaire
circular_list_t cl_next(circular_list_t cl);

// Récupérer le précédent élément de la liste circulaire
circular_list_t cl_prev(circular_list_t cl);

// Vérifier si la liste circulaire est vide
bool circular_list_is_empty(circular_list_t cl);

#endif