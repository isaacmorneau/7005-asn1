#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>

#include "client.h"
#include "socket.h"
#include "file.h"


int client(char * address, char * port, char * filepath, int connect_mode) {
    int datafd, sockfd, acceptedfd, filefd;
    sockfd = make_connected(address, port);
    if (sockfd == -1) {
        return 1;
    }
    datafd = make_bound(port);
    if (datafd == -1) {
        return 2;
    }

    if (connect_mode == 1) {//send
        filefd = open(filepath, O_RDONLY);
        if (filefd == 1) {
            perror("open");
            close(sockfd);
            close(datafd);
            return 3;
        }
        if (write(sockfd, "S ", 2) == -1
                || write(sockfd, filepath, strlen(filepath)) == -1) {
            perror("write");
            close(sockfd);
            close(datafd);
            return 4;
        }
    } else if (connect_mode == 2) {//request
        filefd = open(filepath, O_WRONLY);
        if (filefd == 1) {
            perror("open");
            close(sockfd);
            close(datafd);
            return 5;
        }
        if (write(sockfd, "G ", 2) == -1
                || write(sockfd, filepath, strlen(filepath)) == -1) {
            perror("write");
            close(sockfd);
            close(datafd);
            return 6;
        }
    }

    if (listen(datafd, 1) == -1) {
        perror("listen");
        close(datafd);
        close(sockfd);
        return 7;
    }

    acceptedfd = accept(datafd, 0, 0);
    if (acceptedfd == -1) {
        perror("accept");
        close(datafd);
        close(sockfd);
        return 8;
    }
    close(datafd);

    if (connect_mode == 1) { //send
        zero_copy_write(acceptedfd, filefd);
    } else { //get
        zero_copy_read(acceptedfd, filefd);
    }

    close(sockfd);
    close(acceptedfd);
    return 0;
}
