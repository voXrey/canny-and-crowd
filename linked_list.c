#include <stdlib.h>

#include "linked_list.h"

// Initialiser une nouvelle liste chaînée
LinkedList* create_list() {
    LinkedList* list = (LinkedList*) malloc(sizeof(LinkedList));
    list->head = NULL;
    return list;
}

// Ajouter un élément en tête de la liste
void list_add_front(LinkedList* list, void* data) {
    Mail* new_mail = malloc(sizeof(Mail));
    new_mail->data = data;
    new_mail->next = list->head;
    new_mail->previous = NULL;
    list->head = new_mail;
}

// Insérer un élément après un maillon
void list_insert_after_mail(LinkedList* list, Mail* mail, void* data) {
    Mail* new_mail = malloc(sizeof(Mail));
    new_mail->data = data;
    new_mail->next = mail->next;
    new_mail ->previous = mail;
    mail->next = new_mail;
}

// Supprimer un maillon (mais pas sa valeur)
void list_delete_mail(Mail* mail) {
    if (mail->previous != NULL) {
        mail->previous->next = mail->next;
    }
    if (mail->next != NULL) {
        mail->next->previous = mail->previous;
    }
    free(mail);
}

// Supprimer l'élément en tête de la liste
void list_remove_front(LinkedList* list) {
    if (list->head == NULL) {
        return;
    }
    Mail* old_head = list->head;
    void* data = old_head->data;
    list->head = old_head->next;
    list->head->previous = NULL;
    free(old_head);
}

// Obtenir l'élément en tête de la liste
void* list_peek_front(LinkedList* list) {
    if (list->head == NULL) {
        return NULL;
    }
    return list->head->data;
}

// Libérer la mémoire allouée pour la liste
void free_list(LinkedList* list) {
    while (list->head != NULL) {
        Mail* old_head = list->head;
        list->head = old_head->next;
        free(old_head);
    }
    free(list);
}