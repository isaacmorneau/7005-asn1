/*
 * SOURCE FILE: file.c - Kernel level file descriptor copying
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Oct 6, 2017
 *
 * FUNCTIONS:
 *  int kernel_copy(int infd, int outfd);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
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
#include <limits.h>

#include "file.h"

//default page size in linux
#define DEFAULT_PAGE 4096

/*
 *  FUNCTION: kernel_copy
 *
 *  DATE: Oct 6, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int kernel_copy(int infd, int outfd);
 *
 *  PARAMETERS:
 *      int infd  - The file descriptor to read from
 *      int outfd - The file descriptor to write to
 *
 *  RETURNS:
 *  int - returns 0 for again, 1 for no error or -1 for error
 *
 *  NOTES:
 *      Great read put me on to splice
 *      http://yarchive.net/comp/linux/splice.html
 */

int kernel_copy(int infd, int outfd) {
    int pipefd[2];
    if(pipe(pipefd) < 0) {
        perror("pipe");
        return -1;
    }

	while (1) {
	    //read max standard pipe allocation size
		int nr = splice(infd, 0, pipefd[1], 0, USHRT_MAX, SPLICE_F_MOVE | SPLICE_F_MORE | SPLICE_F_NONBLOCK);
        if (nr == -1 && errno != EAGAIN) {
            perror("splice");
        }
		if (nr <= 0) {
			break;
        }
		do {
			int ret = splice(pipefd[0], 0, outfd, 0, nr, SPLICE_F_MOVE | SPLICE_F_MORE);
			if (ret <= 0) {
			    if (ret == -1) {
			        perror("splice2");
                }
				break;
			}
			nr -= ret;
		} while (nr);
	}
	close(pipefd[0]);
	close(pipefd[1]);
    return 0;
}
