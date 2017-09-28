#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "client.h"
#include "socket.h"


int client(char * address, char * port, char * filepath, int connect_mode) {
    int sockfd = make_connected(address, port);
    if (sockfd == -1) {
        return -1;
    }

    if (connect_mode == 1) {//send
        write(sockfd, "SEND ", 5);
        write(sockfd, filepath, strlen(filepath));
    } else if (connect_mode == 2) {//request
        write(sockfd, "GET ", 4);
        write(sockfd, filepath, strlen(filepath));
    }
    close(sockfd);

    return 0;
}
