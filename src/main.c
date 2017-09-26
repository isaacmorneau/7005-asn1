#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <getopt.h>

#include "server.h"

#define SOCKOPTS "csitrhp:a:f:"

void print_help(){
    printf("[c]lient - set the mode to client\n"
            "[s]erver - set the mode to server\n"
            "[p]ort <1-65535>> - the port to listen on for server, the port to recieve on for client\n"
            "[a]ddress <ip or url> - only used by client for connecting to a server\n"
            "[i]nteractive - only used by the client for setting mode to interactive\n"
            "[t]ransmit - set connection mode to SEND\n"
            "[r]ecieve - set connection mode to GET\n"
            "[f]ile <path> - the location of the file you send or want to recieve\n"
            "[h]elp - this message\n");
}

int main (int argc, char *argv[]) {
    if (argc == 1) {
        print_help();
        return 0;
    }
    int c;

    bool server_mode = 0;
    bool client_mode = 0;
    bool interactive = 0;
    bool transmit = 0;
    bool recieve = 0;

    char * port = 0;
    char * address = 0;
    char * file = 0;

    while (1) {
        int option_index = 0;

        static struct option long_options[] = {
            {"client",      no_argument,       0, 'c' },
            {"server",      no_argument,       0, 's' },
            {"interactive", no_argument,       0, 'i' },
            {"help",        no_argument,       0, 'h' },
            {"transmit",    no_argument,       0, 't' },
            {"recieve",     no_argument,       0, 'r' },
            {"port",        required_argument, 0, 'p' },
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
                printf("client\n");
                break;

            case 's':
                if (client_mode) {
                    printf("Client and server requested, exiting\n");
                    return 1;
                }
                server_mode = 1;
                printf("server\n");
                break;

            case 'i':
                interactive = 1;
                printf("interactive\n");
                break;

            case 't':
                transmit = 1;
                printf("transmit\n");
                break;

            case 'r':
                recieve = 1;
                printf("recieve\n");
                break;

            case 'p':
                port = optarg;
                printf("port with value '%s'\n", optarg);
                break;

            case 'a':
                address = optarg;
                printf("address with value '%s'\n", optarg);
                break;

            case 'f':
                file = optarg;
                printf("file with value '%s'\n", optarg);
                break;

            case 'h':
            case '?':
            default:
                print_help();
                return 0;
        }
    }
    if (server_mode) {
        return server(port);
    } else if (client_mode) {
        //call client code
    } else {
        printf("Mode not specified, exiting\n");
        return -1;
    }

}
