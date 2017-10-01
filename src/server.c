#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "server.h"
#include "socket.h"

#define MAXEVENTS 64
#define MAXFDS 65636
#define DEFAULT_BUF 1024

int server(char * port) {
    int sfd, s;
    int efd;
    int * sock_to_files;
    struct epoll_event event;
    struct epoll_event *events;

    sfd = make_bound(port);
    if (sfd == -1) {
        return 1;
    }

    s = make_non_blocking(sfd);
    if (s == -1) {
        return 2;
    }

    s = listen(sfd, SOMAXCONN);
    if (s == -1) {
        perror("listen");
        return 3;
    }

    efd = epoll_create1(0);
    if (efd == -1) {
        perror ("epoll_create");
        return 4;
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET | EPOLLEXCLUSIVE;
    s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
    if (s == -1) {
        perror("epoll_ctl");
        return 5;
    }

    // Buffer where events are returned
    events = calloc(MAXEVENTS, sizeof(event));

    //this is our lookup table for sockets
    //currently uploading files, yes its huge
    //but this maintains an O(1) lookup at the cost
    //of 256KB of mem.
    sock_to_files = calloc(MAXFDS, sizeof(int));

#pragma omp parallel
    while (1) {
        int n, i;

        n = epoll_wait(efd, events, MAXEVENTS, -1);
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                // An error has occured on this fd, or the socket is not
                // ready for reading (why were we notified then?)
                fprintf (stderr, "epoll error\n");
                close(events[i].data.fd);
                continue;
            } else if (sfd == events[i].data.fd) {
                // We have a notification on the listening socket, which
                // means one or more incoming connections.
                while (1) {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    in_len = sizeof in_addr;
                    infd = accept(sfd, &in_addr, &in_len);
                    if (infd == -1) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            // We have processed all incoming
                            // connections.
                            break;
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    s = getnameinfo(&in_addr, in_len, hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
                    if (s == 0) {
                        printf("Accepted connection on descriptor %d "
                                "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                    }

                    // Make the incoming socket non-blocking and add it to the
                    // list of fds to monitor.
                    s = make_non_blocking(infd);
                    if (s == -1) {
                        abort();
                    }

                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
                    if (s == -1) {
                        perror("epoll_ctl");
                        abort();
                    }
                }
                continue;
            } else {
                int done = 0;
                int output_fd;

                //if there is not currently a file open for writing then
                //echo it to output
                if (!(output_fd = sock_to_files[events[i].data.fd])) {
                    output_fd = 1;//1 default echo to stdout
                }

                while (1) {
                    ssize_t count;
                    char buf[DEFAULT_BUF];

                    count = read(events[i].data.fd, buf, sizeof buf);
                    if (count == -1) {
                        // If errno == EAGAIN, that means we have read all
                        // data. So go back to the main loop.
                        if (errno != EAGAIN) {
                            perror("read");
                            done = 1;
                        }
                        break;
                    } else if (count == 0) {
                        // End of file. The remote has closed the
                        // connection.
                        done = 1;
                        break;
                    }

                    if (output_fd == 1) { //its a command
                        buf[count] = 0;

                        int datafd = make_reverse_connected(events[i].data.fd, port);

                        if(datafd == -1) {
                            close(events[i].data.fd);
                            continue;
                        }

                        if (*buf == 'S') { // uploading a file
                            s = sock_to_files[datafd] = open((buf+2), O_WRONLY);
                            if (s = -1) {
                                perror("fopen");
                                close(events[i].data.fd);
                                continue;
                            }
                        } else if (*buf == 'G') {//downloading a file
                            //spawn off a thread to download the whole file blocking
                        } else {
                            printf("Malformed request: '%s'", buf);
                        }
                    }

                    // Write the buffer to standard output
                    s = write(output_fd, buf, count);
                    if (s == -1) {
                        perror("write");
                        abort();
                    }
                }

                if (done) {
                    printf("Closed connection on descriptor %d\n",
                            events[i].data.fd);

                    // Closing the descriptor will make epoll remove it
                    // from the set of descriptors which are monitored.
                    close(events[i].data.fd);
                }
            }
        }
    }
    free(sock_to_files);
    free(events);
    close(sfd);
    return 0;
}
