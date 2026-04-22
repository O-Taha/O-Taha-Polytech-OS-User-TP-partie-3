#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "creme.h"
#include "list.h"

pthread_t srv_thread;
int srv_active = 0;
char pseudo_global[LPSEUDO + 1];
extern int running;

void cmd_message(char *target, char *msg) {
    if (strcmp(target, "all") == 0) {
        pthread_mutex_lock(&mutex_liste);
        struct elt *curr = ma_liste;
        while (curr) {
            envoyer_beuip('9', curr->adip, msg, NULL);
            curr = curr->next;
        }
        pthread_mutex_unlock(&mutex_liste);
    } else {
        pthread_mutex_lock(&mutex_liste);
        struct elt *curr = ma_liste;
        while (curr && strcmp(curr->nom, target) != 0) curr = curr->next;
        if (curr) envoyer_beuip('9', curr->adip, msg, NULL);
        pthread_mutex_unlock(&mutex_liste);
    }
}

void process_cmd(char *line) {
    char *cmd = strtok(line, " \n");
    if (!cmd || strcmp(cmd, "beuip") != 0) return;

    char *sub = strtok(NULL, " \n");
    if (strcmp(sub, "start") == 0 && !srv_active) {
        char *p = strtok(NULL, " \n");
        if (p) {
            strncpy(pseudo_global, p, LPSEUDO);
            pthread_create(&srv_thread, NULL, serveur_udp, pseudo_global);
            srv_active = 1;
        }
    } else if (strcmp(sub, "stop") == 0 && srv_active) {
        envoyer_beuip('0', BROADCAST_ADDR, NULL, NULL);
        pthread_cancel(srv_thread);
        pthread_join(srv_thread, NULL);
        viderListe();
        srv_active = 0;
		running = 0;
    } else if (strcmp(sub, "list") == 0) {
        listeElts();
    } else if (strcmp(sub, "message") == 0) {
        char *to = strtok(NULL, " ");
        char *msg = strtok(NULL, "\n");
        if (to && msg) cmd_message(to, msg);
    }
}

int main() {
    char line[1024];

    while (printf("> ") && fgets(line, sizeof(line), stdin)) {
        process_cmd(line);
    }

    if (srv_active) {
        envoyer_beuip('0', BROADCAST_ADDR, NULL, NULL);
        running = 0;
        pthread_join(srv_thread, NULL);
    }

    viderListe();
    return 0;
}