/*
 * SOURCE FILE: socket.c - The wrappers for creating or modifying sockets
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
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "socket.h"

/*
 *  FUNCTION: make_non_blocking
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int make_non_blocking (int sfd);
 *
 *  PARAMETERS:
 *      int sfd - the socket to operate on
 *
 *  RETURNS:
 *  int - returns 0 for no error or -1 to indicate error
 */
int make_non_blocking (int sfd) {
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1){
        perror("fcntl get");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) {
        perror("fcntl set");
        return -1;
    }

    return 0;
}

/*
 *  FUNCTION: make_connected
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int make_connected(char * address, char * port);
 *
 *  PARAMETERS:
 *  char * address  - The address to connect to
 *  char * port     - The port to connect on
 *
 *  RETURNS:
 *  int - returns the socket for no error or -1 to indicate error
 */
int make_connected(char * address, char * port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // Return IPv4 and IPv6 choices
    hints.ai_socktype = SOCK_STREAM; // We want a TCP socket
    hints.ai_flags = AI_PASSIVE;     // All interfaces

    s = getaddrinfo(address, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror (s));
        return -1;
    }

    for (rp = result; rp != 0; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        s = connect(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
            break;
        }

        close(sfd);
    }

    if (!rp) {
        fprintf(stderr, "Unable to connect\n");
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
}

/*
 *  FUNCTION: make_bound
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int make_bound(char * port);
 *
 *  PARAMETERS:
 *  char * port     - The port to listen on
 *
 *  RETURNS:
 *  int - returns the socket for no error or -1 to indicate error
 */
int make_bound(char * port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // Return IPv4 and IPv6 choices
    hints.ai_socktype = SOCK_STREAM; // We want a TCP socket
    hints.ai_flags = AI_PASSIVE;     // All interfaces

    s = getaddrinfo(0, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror (s));
        return -1;
    }

    for (rp = result; rp != 0; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }
        int enable = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
            perror("Socket options failed");
            exit(EXIT_FAILURE);
        }

        s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
            // We managed to bind successfully!
            break;
        }

        close(sfd);
    }

    if (!rp) {
        fprintf(stderr, "Unable to bind\n");
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
}
