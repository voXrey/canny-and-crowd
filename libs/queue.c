#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"


struct maillon_s {
    void* value;
    struct maillon_s* next;
};
typedef struct maillon_s maillon_t;

struct queue_s {
    maillon_t* head;
    maillon_t* tail;
    int len;
};
typedef struct queue_s queue_t;

// Créer un maillon
maillon_t* maillon_create(void* value) {
    maillon_t* m = (maillon_t*) malloc(sizeof(maillon_t));
    m->value = value;
    m->next = NULL;
    return m;
}

// Initialiser une queue
queue_t* queue_create() {
    queue_t* q = (queue_t*) malloc(sizeof(queue_t));
    q->head = NULL;
    q->tail = NULL;
    q->len = 0;
    return q;
}

// Ajouter un élément à la queue (enqueue)
void queue_enqueue(queue_t* q, void* value) {
    maillon_t* new_maillon = maillon_create(value);
    if (q->tail == NULL) {
        q->head = new_maillon;
        q->tail = new_maillon;
    } else {
        q->tail->next = new_maillon;
        q->tail = new_maillon;
    }
    q->len++;
}

// Retirer un élément de la queue (dequeue)
void* queue_dequeue(queue_t* q) {
    if (q->head == NULL) {
        fprintf(stderr, "Erreur : la queue est vide.\n");
        exit(-1);
    }
    maillon_t* temp = q->head;
    void* value = temp->value;
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    q->len--;
    free(temp);
    return value;
}

// Vérifier si la queue est vide
bool queue_is_empty(queue_t* q) {
    return q->head == NULL;
}

// Libérer une queue
void queue_free(queue_t* q) {
    while (!queue_is_empty(q)) {
        queue_dequeue(q);
    }
    free(q);
}