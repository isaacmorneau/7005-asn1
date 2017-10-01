#ifndef SOCKET_H
#define SOCKET_H

int make_non_blocking(int sfd);
int make_bound(char * port);
int make_connected(char * address, char * port);
int make_reverse_connected(int sockfd, char * port);

#endif
