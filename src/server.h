#ifndef SERVER_H
#define SERVER_H

int server(char * port, char * data);
typedef struct sock_file_pair {
    int filefd;
    int sockfd;
} sock_file_pair;
void * downloadfile(void * pair);

#endif
