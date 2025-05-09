#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "priority_queue.h"


// Créer une file de priorité
priority_queue_t* pq_create(int capacity) {
    priority_queue_t* pq = (priority_queue_t*) malloc(sizeof(priority_queue_t));
    pq->nodes = (heap_node_t*) malloc(sizeof(heap_node_t) * capacity);
    pq->len = 0;
    pq->capacity = capacity;
    return pq;
}

// Libérer une file de priorité
void pq_free(priority_queue_t* pq) {
    free(pq->nodes);
    free(pq);
}

// Échanger deux noeuds dans le tas
void swap(heap_node_t* a, heap_node_t* b) {
    heap_node_t temp = *a;
    *a = *b;
    *b = temp;
}

// Maintenir la propriété du tas (min-heap) en descendant
void percolate_down(priority_queue_t* pq, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < pq->len && pq->nodes[left].priority < pq->nodes[smallest].priority) {
        smallest = left;
    }
    if (right < pq->len && pq->nodes[right].priority < pq->nodes[smallest].priority) {
        smallest = right;
    }
    if (smallest != index) {
        swap(&pq->nodes[index], &pq->nodes[smallest]);
        percolate_down(pq, smallest);
    }
}

// Maintenir la propriété du tas (min-heap) en remontant
void percolate_up(priority_queue_t* pq, int index) {
    int parent = (index - 1) / 2;

    if (index > 0 && pq->nodes[index].priority < pq->nodes[parent].priority) {
        swap(&pq->nodes[index], &pq->nodes[parent]);
        percolate_up(pq, parent);
    }
}

// Ajouter un élément à la file de priorité
void pq_push(priority_queue_t* pq, double priority, void* value) {
    if (pq->len == pq->capacity) {
        printf("Erreur : capacité maximale atteinte dans la file de priorité.\n");
        exit(-1);
    }

    pq->nodes[pq->len].priority = priority;
    pq->nodes[pq->len].value = value;
    pq->len++;
    percolate_up(pq, pq->len - 1);
}

// Extraire l'élément avec la plus petite priorité
void* pq_pop(priority_queue_t* pq) {
    if (pq->len == 0) {
        fprintf(stderr, "Erreur : la file de priorité est vide.\n");
        exit(-1);
    }

    void* min_position = pq->nodes[0].value;
    pq->nodes[0] = pq->nodes[pq->len - 1];
    pq->len--;
    percolate_down(pq, 0);

    return min_position;
}

// Vérifier si la file de priorité est vide
bool pq_is_empty(priority_queue_t* pq) {
    return pq->len == 0;
}