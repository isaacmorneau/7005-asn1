#ifndef FILE_H
#define FILE_H

int zero_copy_read(int sockfd, int filefd);
int zero_copy_write(int filefd, int sockfd);

#endif
