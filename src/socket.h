/*
 * HEADER FILE: socket.h - The wrappers for creating or modifying sockets
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 26, 2017
 *
 * FUNCTIONS:
 * int make_non_blocking(int sfd);
 * int make_bound(char * port);
 * int make_connected(char * address, char * port);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
#ifndef SOCKET_H
#define SOCKET_H

int make_non_blocking(int sfd);
int make_bound(char * port);
int make_connected(char * address, char * port);

#endif
