#include "creme.h"
#include "list.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int running = 1;
#define MAX_BUF 1024

void envoyer_beuip(char code, const char *dest_ip, const char *data, const char *extra) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) return;

	struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(PORT) };
	inet_aton(dest_ip, &addr.sin_addr);

	if (strcmp(dest_ip, BROADCAST_ADDR) == 0) {
		int opt = 1;
		setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
	}

	char buffer[MAX_BUF]; 
	memset(buffer, 0, MAX_BUF);

	buffer[0] = code; // Octet 1 : le code [1, 2]
	memcpy(buffer + 1, PROTO_TAG, 5); // Octets 2-6 : "BEUIP" [1]
	
	int offset = 6;
	if (data) {
		size_t len = strlen(data) + 1;
		if (offset + len >= MAX_BUF) return;
		memcpy(buffer + offset, data, len);
		offset += len;
	}
	if (extra) { 
		strcpy(buffer + offset, extra); 
		offset += strlen(extra) + 1; 
	}
	
	sendto(sock, buffer, offset, 0, (struct sockaddr*)&addr, sizeof(addr));
	close(sock);
}

void handle_msg(char *buf, struct sockaddr_in *src, char *mon_pseudo) {
    char code = buf[0];

    char ip[16];
    inet_ntop(AF_INET, &src->sin_addr, ip, sizeof(ip));

    char *pseudo = buf + 6;

    if (code == '1') {
        ajouteElt(pseudo, ip);
        envoyer_beuip('2', ip, mon_pseudo, NULL);
    } else if (code == '2') {
        ajouteElt(pseudo, ip);
    } else if (code == '9') {
        printf("\nMessage de %s: %s\n> ", ip, pseudo);
        fflush(stdout);
    } else if (code == '0') {
        supprimeElt(ip);
    }
}


void *serveur_udp(void *p) {
	char *mon_pseudo = (char *)p;

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) return NULL;

	struct timeval tv = {1, 0};
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr.s_addr = INADDR_ANY
	};

	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(sock);
		return NULL;
	}

	envoyer_beuip('1', BROADCAST_ADDR, mon_pseudo, NULL);

	char buffer[MAX_BUF];
	struct sockaddr_in src;
	socklen_t len = sizeof(src);

	while (running) {
		int n = recvfrom(sock, buffer, MAX_BUF, 0,
						(struct sockaddr*)&src, &len);

		if (n > 0 && memcmp(buffer + 1, PROTO_TAG, 5) == 0) {
			handle_msg(buffer, &src, mon_pseudo);
		}
	}

	close(sock);
	return NULL;
}