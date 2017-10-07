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
    loff_t in_off = 0;
    loff_t out_off = 0;
    int filedes[2];
    int err = -1;
    int finished = 0;
    if(pipe(filedes) < 0) {
        perror("pipe");
        return -1;
    }

    while (!finished) {
        err = splice(infd, &in_off, filedes[1], 0, DEFAULT_PAGE, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (err == -1) {
            perror("splice");
            close(filedes[0]);
            close(filedes[1]);
            return -1;
        } else if (err == 0) {
            //finished reading
            finished = 1;
        }
        err = splice(filedes[0], 0, outfd, &out_off, DEFAULT_PAGE, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (err == -1) {
            perror("splice2");
            close(filedes[0]);
            close(filedes[1]);
            return -1;
        }
    }
    return 1;
}
