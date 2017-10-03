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

#define DEFAULT_BUFF 1024

int client(char * address, char * port, char * data, char * filepath, int connect_mode) {
    int datafd, sockfd, acceptedfd, filefd;
    char buf[DEFAULT_BUFF];
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


    if (connect_mode == 1) {//send
        filefd = fileno(fopen(filepath, "r"));
        if (filefd == -1) {
            perror("open");
            close(sockfd);
            close(datafd);
            return 5;
        }
        sprintf(buf, "S %s", filepath);
        if (write(sockfd, buf, strlen(buf)) == -1) {
            perror("write");
            close(sockfd);
            close(datafd);
            return 6;
        }
    } else if (connect_mode == 2) {//request
        filefd = fileno(fopen(filepath, "w"));
        if (filefd == -1) {
            perror("open");
            close(sockfd);
            close(datafd);
            return 7;
        }
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
        zero_copy_write(acceptedfd, filefd);
    } else { //get
        zero_copy_read(acceptedfd, filefd);
    }

    close(sockfd);
    close(acceptedfd);
    printf("Cleaning Up\n");
    return 0;
}
