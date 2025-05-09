#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

// Définition des structures
typedef struct queue_s queue_t;

// Fonctions pour manipuler la queue
queue_t* queue_create();                     // Créer une nouvelle queue
void queue_enqueue(queue_t* q, void* value); // Ajouter un élément à la queue
void* queue_dequeue(queue_t* q);             // Retirer un élément de la queue
bool queue_is_empty(queue_t* q);             // Vérifier si la queue est vide
void queue_free(queue_t* q);                 // Libérer la mémoire de la queue

#endif // QUEUE_H