#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct elt *ma_liste = NULL;
pthread_mutex_t mutex_liste = PTHREAD_MUTEX_INITIALIZER;

void ajouteElt(char *pseudo, char *adip) {
    pthread_mutex_lock(&mutex_liste);
    struct elt **curr = &ma_liste;
    while (*curr && strcmp((*curr)->nom, pseudo) < 0) curr = &(*curr)->next;
    
    if (*curr && strcmp((*curr)->adip, adip) == 0) {
        pthread_mutex_unlock(&mutex_liste);
        return; 
    }

    struct elt *nouveau = malloc(sizeof(struct elt));
    strncpy(nouveau->nom, pseudo, LPSEUDO);
	nouveau->nom[LPSEUDO] = '\0';
    strcpy(nouveau->adip, adip);
    nouveau->next = *curr;
    *curr = nouveau;
    pthread_mutex_unlock(&mutex_liste);
}

void supprimeElt(char *adip) {
    pthread_mutex_lock(&mutex_liste);
    struct elt **curr = &ma_liste, *temp;
    while (*curr && strcmp((*curr)->adip, adip) != 0) curr = &(*curr)->next;
    if (*curr) {
        temp = *curr;
        *curr = (*curr)->next;
        free(temp);
    }
    pthread_mutex_unlock(&mutex_liste);
}

void listeElts(void) {
    pthread_mutex_lock(&mutex_liste);
    struct elt *curr = ma_liste;
    while (curr) {
        printf("%s : %s\n", curr->adip, curr->nom);
        curr = curr->next;
    }
    pthread_mutex_unlock(&mutex_liste);
}

void viderListe(void) {
    pthread_mutex_lock(&mutex_liste);
    while (ma_liste) {
        struct elt *temp = ma_liste;
        ma_liste = ma_liste->next;
        free(temp);
    }
    pthread_mutex_unlock(&mutex_liste);
}