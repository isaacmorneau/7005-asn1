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
#include "file.h"

#define MAXEVENTS 64
#define MAXFDS 65636
#define DEFAULT_BUF 1024

int server(char * port, char * data) {
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
                    int infd, datafd;
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

                    //if the data connection fails kill the client and let them reconnect
                    datafd = make_connected(hbuf, data);
                    if (datafd == -1) {
                        close(datafd);
                        close(infd);
                        break;
                    }
                    printf("Established reverse connection to client on port %s\n",data);

                    sock_to_files[infd] = -1 * datafd;

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
                //check for open file or data socket
                //if the value is negative it points to the data socket and this is a command socket
                //if the value is positive this is a data socket and it points to an open file
                if ((output_fd = sock_to_files[events[i].data.fd]) < 0) {
                    output_fd *= -1;
                    while (1) {
                        ssize_t count;
                        char buf[DEFAULT_BUF];

                        count = read(events[i].data.fd, buf, DEFAULT_BUF);
                        if (count == -1) {
                            // If errno == EAGAIN, that means we have read all
                            // data. So go back to the main loop.
                            if (errno != EAGAIN) {
                                perror("read");
                                close(events[i].data.fd);
                            }
                            break;
                        } else if (count == 0) {
                            //remote closed the connection
                            close(events[i].data.fd);
                            break;
                        }

                        buf[count] = 0;

                        //now that a command has come in we can decide if we need to assign this reverse connection
                        //a reading or writing task

                        FILE * fp;
                        if (*buf == 'S') { // uploading a file
                            printf("uploading '%s'\n", (buf+2));
                            fp = fopen((buf+2), "w");
                            if (fp == 0) {
                                perror("open");
                                close(output_fd);
                                close(events[i].data.fd);
                                break;
                            }
                            sock_to_files[output_fd] = fileno(fp);
                            s = make_non_blocking(output_fd);
                            if (s == -1) {
                                close(output_fd);
                                close(events[i].data.fd);
                                break;
                            }

                            //data connection setup was successful, listen for incomming data
                            event.data.fd = output_fd;
                            event.events = EPOLLIN | EPOLLET;
                            s = epoll_ctl(efd, EPOLL_CTL_ADD, output_fd, &event);
                            if (s == -1) {
                                perror("epoll_ctl");
                                abort();
                            }
                        } else if (*buf == 'G') {//downloading a file
                            //spawn off a thread to download the whole file blocking
                            printf("[get not yet implemented]\n");
                        } else {
                            printf("Malformed request: '%s'", buf);
                        }
                    }
                } else {//since its incomming data it must be an upload
                    //implement zero copy read if time permits
                    //zero_copy_read(events[i].data.fd, sock_to_files[events[i].data.fd]);
                    char buf[DEFAULT_BUF];
                    int count = 0;
                    while(1) {
                        count = read(events[i].data.fd, buf, DEFAULT_BUF);
                        printf("read\n");
                        if (count == 0) {
                        printf("nothing\n");
                            if (errno != EAGAIN) {
                                close(sock_to_files[events[i].data.fd]);
                                close(events[i].data.fd);
                            }
                            break;
                        } else if (count == -1) {
                        printf("errors\n");
                            if (errno != EAGAIN) {
                                perror("read data socket");
                                close(sock_to_files[events[i].data.fd]);
                                close(events[i].data.fd);
                                break;
                            }
                        }
                        printf("writing\n");
                        if (write(sock_to_files[events[i].data.fd], buf, count) == -1) {
                        printf("errors\n");
                            perror("file write");
                            close(sock_to_files[events[i].data.fd]);
                            close(events[i].data.fd);
                            break;
                        }
                        printf("Recieved %d\n", count);
                    }
                }
            }
        }
    }
    free(sock_to_files);
    free(events);
    close(sfd);
    return 0;
}
