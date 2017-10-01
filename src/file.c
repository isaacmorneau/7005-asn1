#define _GNU_SOURCE
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "file.h"

//default page size in linux
#define DEFAULT_PAGE 4096

int zero_copy_read(int sockfd, int filefd) {
    int finished = 0;
    loff_t in_off = 0;
    loff_t out_off = 0;
    static int buf_size = DEFAULT_PAGE;
    off_t len;
    int filedes[2];
    int err = -1;
    struct stat stbuf;
    if(pipe(filedes) < 0) {
        perror("pipe");
        return -1;
    }
    if(fstat(sockfd, &stbuf) < 0) {
        perror("fstat");
        goto out_close;
    }
    len = stbuf.st_size;
    while(len > 0 && !finished) {
        if(buf_size > len) {
            buf_size = len;
        }
        err = splice(sockfd, &in_off, filedes[1], 0, buf_size, SPLICE_F_MOVE | SPLICE_F_MORE);
        if(err < 0) {
            perror("splice");
            goto out_close;
        } else if (err == 0) {
            //finished reading
            finished = 1;
        }
        err = splice(filedes[0], 0, filefd, &out_off, buf_size, SPLICE_F_MOVE | SPLICE_F_MORE);
        if(err < 0) {
            perror("splice2");
            goto out_close;
        }
        len -= err;
    }
    err = 0;
    out_close:
    close(filedes[0]);
    close(filedes[1]);
    return err;
}

int zero_copy_write(int filefd, int sockfd) {
    static int buf_size = DEFAULT_PAGE;
    off_t len;
    int err = -1;
    struct stat stbuf;
    if(fstat(sockfd, &stbuf) < 0) {
        perror("fstat");
        return -1;
    }
    len = stbuf.st_size;
    while(len > 0) {
        if(buf_size > len) {
            buf_size = len;
        }
        err = sendfile(sockfd, filefd, 0, buf_size);
        if(err < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                //interuped, retry
                continue;
            }
            perror("sendfile");
            return -1;
        }
        len -= err;
    }
    return 0;
}
