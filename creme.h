#ifndef CREME_H
#define CREME_H

#define PORT 9998
#define BROADCAST_ADDR "192.168.88.255"
#define PROTO_TAG "BEUIP"

void envoyer_beuip(char code, const char *dest_ip, const char *data, const char *extra);
void *serveur_udp(void *p);

#endif