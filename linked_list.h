#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

// Définition d'un nœud de la liste chaînée
typedef struct Mail {
    void* data;
    struct Mail* next;
    struct Mail* previous;
} Mail;

// Définition de la liste chaînée
typedef struct LinkedList {
    Mail* head;
} LinkedList;

// Initialiser une nouvelle liste chaînée
LinkedList* create_list();

// Ajouter un élément en tête de la liste
void list_add_front(LinkedList* list, void* data);

// Insérer un élément après un maillon
void list_insert_after_mail(LinkedList* list, Mail* mail, void* data);

// Supprimer un maillon
void list_delete_mail(Mail* mail);

// Supprimer l'élément en tête de la liste
void list_remove_front(LinkedList* list);

// Obtenir l'élément en tête de la liste
void* list_peek_front(LinkedList* list);

// Libérer la mémoire allouée pour la liste
void free_list(LinkedList* list);

#endif // LINKED_LIST_H