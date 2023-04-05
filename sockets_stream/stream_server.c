#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#define TRUE 1

/*
 * This program creates a socket and then begins an infinite loop.
 * Each time through the loop it accepts a connection and prints
 * data from it. When the connection breaks, or the client closes
 * the connection, the program accepts a new connection.
 */
int main()
{
    int sock, length;
    struct sockaddr_in6 server;
    int msgsock;
    char buf[1024];
    int rval;

    /* Create socket. */
    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Opening stream socket");
        exit(1);
    }

    /* Bind socket using wildcards.*/
    bzero(&server, sizeof(server));
    server.sin6_family = AF_INET6;
    server.sin6_addr = in6addr_any;
    server.sin6_port = 0;
    if (bind(sock, (struct sockaddr *)&server, sizeof server) == -1)
    {
        perror("Binding stream socket");
        exit(1);
    }

    /* Find out assigned port number and print it out. */
    length = sizeof server;
    if (getsockname(sock, (struct sockaddr *)&server, (socklen_t *)&length) == -1)
    {
        perror("Getting socket name");
        exit(1);
    }
    printf("Socket port #%d\n", ntohs(server.sin6_port));

    /* Start accepting connections. */
    listen(sock, 5);
    do
    {
        msgsock = accept(sock, (struct sockaddr *)0, (socklen_t *)0);
        if (msgsock == -1)
            perror("Accept");
        else
            do
            {
                memset(buf, 0, sizeof buf);
                if ((rval = read(msgsock, buf, sizeof(buf))) == -1)
                    perror("Reading stream message");
                if (rval == 0)
                    printf("Ending connection\n");
                else
                    /* assumes the data is printable */
                    printf("-->%s\n", buf);
            } while (rval > 0);
        close(msgsock);
    } while (TRUE);

    /*
     * Since this program has an infinite loop, the socket "sock" is
     * never explicitly closed. However, all sockets are closed
     * automatically when a process is killed or terminates normally.
     */
    exit(0);
}