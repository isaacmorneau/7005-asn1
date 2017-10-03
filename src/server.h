/*
 * HEADER FILE: server.h - The core server components
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 26, 2017
 *
 * FUNCTIONS:
 *  int server(char * port, char * data);
 *  void * downloadfile(void * pair);
 *
 * STRUCTS:
 * struct sock_file_pair; - The structure for passing file descriptors to the
 *                              pthread downloading function.
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
#ifndef SERVER_H
#define SERVER_H

typedef struct sock_file_pair {
    int filefd;
    int sockfd;
} sock_file_pair;

int server(char * port, char * data);
void * downloadfile(void * pair);

#endif
