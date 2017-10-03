# File Headers

The collection of the headers from each of the files.

## main.c

```c
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
 ```

```c
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
 ```

```c
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
 ```

## socket.h

```c
/*
 * HEADER FILE: socket.h - The wrappers for creating or modifying sockets
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 26, 2017
 *
 * FUNCTIONS:
 * int make_non_blocking(int sfd);
 * int make_bound(char * port);
 * int make_connected(char * address, char * port);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
```

## socket.c

```c
/*
 * SOURCE FILE: socket.c - The wrappers for creating or modifying sockets
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 26, 2017
 *
 * FUNCTIONS:
 * int make_non_blocking(int sfd);
 * int make_bound(char * port);
 * int make_connected(char * address, char * port);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
```

```c
/*
 *  FUNCTION: make_non_blocking
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int make_non_blocking (int sfd);
 *
 *  PARAMETERS:
 *      int sfd - the socket to operate on
 *
 *  RETURNS:
 *  int - returns 0 for no error or -1 to indicate error
 */
```

```c
/*
 *  FUNCTION: make_connected
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int make_connected(char * address, char * port);
 *
 *  PARAMETERS:
 *  char * address  - The address to connect to
 *  char * port     - The port to connect on
 *
 *  RETURNS:
 *  int - returns the socket for no error or -1 to indicate error
 */
```

```c
/*
 *  FUNCTION: make_bound
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int make_bound(char * port);
 *
 *  PARAMETERS:
 *  char * port     - The port to listen on
 *
 *  RETURNS:
 *  int - returns the socket for no error or -1 to indicate error
 */
 ```

## server.h

```c
/*
 * HEADER FILE: server.h - The core server components
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 26, 2017
 *
 * FUNCTIONS:
 *  int server(char * port, char * data);
 *  void * downloadfile(void * pair);
 *
 * STRUCTS:
 * struct sock_file_pair; - The structure for passing file descriptors to the
 *                              pthread downloading function.
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
 ```

## server.c

```c
/*
 * SOURCE FILE: server.c - The core server components
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 26, 2017
 *
 * FUNCTIONS:
 *  int server(char * port, char * data);
 *  void * downloadfile(void * pair);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
 ```

```c
/*
 *  FUNCTION: server
 *
 *  DATE: Sept 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int server(char * port, char * data) {
 *
 *  PARAMETERS:
 *      char * port     - The port to listen to for commands
 *      char * data     - The data port for file transfers
 *
 *  RETURNS:
 *  int - returns 0 for no error or positive int to indicate error
 */
```

```c
/*
 *  FUNCTION: downloadfile
 *
 *  DATE: Oct 1, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int server(char * port, char * data);
 *
 *  PARAMETERS:
 *      void * pair - expects a struct of sock_file_pair
 *
 *  RETURNS:
 *  int - returns 0
 */
 ```

## client.h

```c
/*
 * HEADER FILE: client.h - The functions for running the client side of the application
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 30, 2017
 *
 * FUNCTIONS:
 *  int client(char * address, char * port, char * data, char * filepath, int connect_mode);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
 ```

## client.c

```c
/*
 * SOURCE FILE: client.c - The functions for running the client side of the application
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Sept 30, 2017
 *
 * FUNCTIONS:
 *  int client(char * address, char * port, char * data, char * filepath, int connect_mode);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
 ```

```c
/*
 *  FUNCTION: client
 *
 *  DATE:
 *  Sept. 30, 2017
 *
 *  DESIGNER: Isaac Morneau
 *
 *  PROGRAMMER: Isaac Morneau
 *
 *  INTERFACE:
 *      int client(char * address, char * port, char * data, char * filepath, int connect_mode);
 *
 *  PARAMETERS:
 *      char * address  - The address to connect to
 *      char * port     - The port to connect to for commands
 *      char * data     - The data port for file transfers
 *      char * filepath - The file to request or transmit
 *      int connect_mode- The mode, 1 for Send 2 for Get
 *
 *  RETURNS:
 *  int - returns 0 for no error or positive int to indicate error
 */
 ```
