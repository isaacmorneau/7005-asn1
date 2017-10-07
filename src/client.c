/*
 * SOURCE FILE: client.c - The functions for running the client side of the application
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 30, 2017
 *
 * FUNCTIONS:
 *  int client(char * address, char * port, char * data, char * filepath, int connect_mode);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>

#include "client.h"
#include "socket.h"
#include "file.h"

#define DEFAULT_BUF 1024

/*
 *  FUNCTION: client
 *
 *  DATE:
 *  Sept. 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int client(char * address, char * port, char * data, char * filepath, int connect_mode);
 *
 *  PARAMETERS:
 *      char * address  - The address to connect to
 *      char * port     - The port to connect to for commands
 *      char * data     - The data port for file transfers
 *      char * filepath - The file to request or transmit
 *      int connect_mode- The mode, 1 for Send 2 for Get
 *
 *  RETURNS:
 *  int - returns 0 for no error or positive int to indicate error
 */
int client(char * address, char * port, char * data, char * filepath, int connect_mode) {
    int datafd, sockfd, acceptedfd, filefd;
    char buf[DEFAULT_BUF];
    printf("Connecting\n");
    sockfd = make_connected(address, port);
    if (sockfd == -1) {
        return 1;
    }

    printf("Creating Data Socket\n");
    datafd = make_bound(data);

    if (datafd == -1) {
        return 2;
    }

    if (listen(datafd, 1) == -1) {
        perror("listen");
        close(datafd);
        close(sockfd);
        return 3;
    }

    printf("Waiting for server reverse connection\n");
    acceptedfd = accept(datafd, 0, 0);
    if (acceptedfd == -1) {
        perror("accept");
        close(datafd);
        close(sockfd);
        return 4;
    }
    close(datafd);
    printf("Server Connected\n");


    FILE * fp;
    if (connect_mode == 1) {//send
        fp = fopen(filepath, "r");
        if (fp == 0) {
            perror("fopen read");
            close(sockfd);
            close(datafd);
            return 5;
        }
        filefd = fileno(fp);
        sprintf(buf, "S %s", filepath);
        if (write(sockfd, buf, strlen(buf)) == -1) {
            perror("write");
            close(sockfd);
            close(datafd);
            return 6;
        }
    } else if (connect_mode == 2) {//request
        fp = fopen(filepath, "w");
        if (fp == 0) {
            perror("fopen write");
            close(sockfd);
            close(datafd);
            return 7;
        }
        filefd = fileno(fp);
        sprintf(buf, "G %s", filepath);
        if (write(sockfd, buf, strlen(buf)) == -1) {
            perror("write");
            close(sockfd);
            close(datafd);
            return 8;
        }
    }

    printf("Started File Trasfer\n");
    if (connect_mode == 1) { //send
        kernel_copy(filefd, acceptedfd);
    } else { //get
        kernel_copy(acceptedfd, filefd);
    }

    printf("Cleaning Up\n");
    close(sockfd);
    close(acceptedfd);
    return 0;
}
