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

int kernel_copy(int infd, int outfd) {
    int filedes[2];
    int count = -1;
    int finished = 0;
    if(pipe(filedes) < 0) {
        perror("pipe");
        return -1;
    }

    while (!finished) {
        count = splice(infd, 0, filedes[1], 0, DEFAULT_PAGE, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (count == -1) {
            if (errno != EAGAIN) {
                perror("splice");
                close(filedes[0]);
                close(filedes[1]);
                return -1;
            } else {
                return 0;
            }
        } else if (count == 0) {
            //finished reading
            finished = 1;
        }
        count = splice(filedes[0], 0, outfd, 0, count, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (count == -1) {
            perror("splice2");
            close(filedes[0]);
            close(filedes[1]);
            return -1;
        }
    }
    return 1;
}
