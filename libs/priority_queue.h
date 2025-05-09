#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

// Définition des structures
typedef struct heap_node_s {
    double priority; // La priorité (plus petite est meilleure)
    void* value;     // La valeur associée
} heap_node_t;

typedef struct priority_queue_s {
    heap_node_t* nodes; // Tableau de nœuds
    int len;            // Nombre d'éléments dans la file
    int capacity;       // Capacité maximale de la file
} priority_queue_t;

// Fonctions pour manipuler la file de priorité
priority_queue_t* pq_create(int capacity);               // Créer une file de priorité
void pq_free(priority_queue_t* pq);                      // Libérer une file de priorité
void pq_push(priority_queue_t* pq, double priority, void* value); // Ajouter un élément
void* pq_pop(priority_queue_t* pq);                      // Extraire l'élément avec la plus petite priorité
bool pq_is_empty(priority_queue_t* pq);                  // Vérifier si la file est vide

#endif // PRIORITY_QUEUE_H