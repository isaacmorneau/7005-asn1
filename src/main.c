/*
 * SOURCE FILE: main.c - Parses arguments and runs either the server or client
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 26, 2017
 *
 * FUNCTIONS:
 *      void print_help(void);
 *      int main (int argc, char *argv[]);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <getopt.h>

#include "server.h"
#include "client.h"

#define SOCKOPTS "cstrhp:d:a:f:"

/*
 *  FUNCTION: print_help
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      void print_help(void);
 *
 *  PARAMETERS:
 *      void
 *
 *  RETURNS:
 *  void
 */
void print_help(void){
    printf("usage options:\n"
            "\t[c]lient - set the mode to client\n"
            "\t[s]erver - set the mode to server\n"
            "\t[p]ort <1-65535>> - the port to connect to for commands\n"
            "\t[d]ata <1-65535>> - the port to connect to for data\n"
            "\t[a]ddress <ip or url> - only used by client for connecting to a server\n"
            "\t[t]ransmit - set connection mode to SEND\n"
            "\t[r]ecieve - set connection mode to GET\n"
            "\t[f]ile <path> - the location of the file you send or want to recieve\n"
            "\t[h]elp - this message\n");
}

/*
 *  FUNCTION: main
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int main (int argc, char *argv[]);
 *
 *  PARAMETERS:
 *      int argc     - The number of arguments
 *      char *argv[] - The arguments passed
 *
 *  RETURNS:
 *  int 0 for no error or positive int to indicate error
 */
int main (int argc, char *argv[]) {
    if (argc == 1) {
        print_help();
        return 0;
    }
    int c;

    bool server_mode = 0;
    bool client_mode = 0;
    int connect_mode = 0;

    char * port = 0;
    char * data = 0;
    char * address = 0;
    char * file = 0;

    while (1) {
        int option_index = 0;

        static struct option long_options[] = {
            {"client",      no_argument,       0, 'c' },
            {"server",      no_argument,       0, 's' },
            {"help",        no_argument,       0, 'h' },
            {"transmit",    no_argument,       0, 't' },
            {"recieve",     no_argument,       0, 'r' },
            {"port",        required_argument, 0, 'p' },
            {"data",        required_argument, 0, 'd' },
            {"address",     required_argument, 0, 'a' },
            {"file",        required_argument, 0, 'f' },
            {0,             0,                 0, 0   }
        };

        c = getopt_long(argc, argv, SOCKOPTS, long_options, &option_index);
        if (c == -1) {
            break;
        }

        switch (c) {
            case 'c':
                if (server_mode) {
                    printf("Server and client requested, exiting\n");
                    return 1;
                }
                client_mode = 1;
                printf("Client\n");
                break;
            case 's':
                if (client_mode) {
                    printf("Client and server requested, exiting\n");
                    return 1;
                }
                server_mode = 1;
                printf("Server\n");
                break;
            case 't':
                connect_mode = 1;
                printf("Transmit\n");
                break;
            case 'r':
                connect_mode = 2;
                printf("Recieve\n");
                break;
            case 'd':
                data = optarg;
                printf("Data port with value '%s'\n", optarg);
                break;
            case 'p':
                port = optarg;
                printf("Command port with value '%s'\n", optarg);
                break;
            case 'a':
                address = optarg;
                printf("Address with value '%s'\n", optarg);
                break;
            case 'f':
                file = optarg;
                printf("File with value '%s'\n", optarg);
                break;
            case 'h':
            case '?':
            default:
                print_help();
                return 0;
        }
    }

    if (server_mode) {
        return server(port, data);
    } else if (client_mode && connect_mode) {
        return client(address, port, data, file, connect_mode);
    } else {
        printf("Mode not specified, exiting\n");
        return -1;
    }

}
