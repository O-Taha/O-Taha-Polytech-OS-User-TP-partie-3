#ifndef LIST_H
#define LIST_H

#include <pthread.h>

#define LPSEUDO 23

struct elt {
    char nom[LPSEUDO + 1];
    char adip[16];
    struct elt *next;
};

void ajouteElt(char *pseudo, char *adip);
void supprimeElt(char *adip);
void listeElts(void);
void viderListe(void);

extern struct elt *ma_liste;
extern pthread_mutex_t mutex_liste;

#endif