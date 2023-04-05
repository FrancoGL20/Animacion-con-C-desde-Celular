#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#define DATA "Half a league, half a league . . ."

/*
 * This program creates a socket and initiates a connection with
 * the socket given in the command line. Some data are sent over the
 * connection and then the socket is closed, ending the connection.
 * The form of the command line is: streamwrite hostname portnumber
 * Usage: pgm host port
 */

int main(int argc, char *argv[])
{
    int sock, errnum, h_addr_index;
    struct sockaddr_in6 server;
    struct hostent *hp;
    char buf[1024];

    /* Create socket. */
    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Opening stream socket");
        exit(1);
    }

    /* Connect socket using name specified by command line. */
    bzero(&server, sizeof(server));
    server.sin6_family = AF_INET6;
    hp = getipnodebyname(argv[1], AF_INET6, AI_DEFAULT, &errnum);

    /*
     * getipnodebyname returns a structure including the network address
     * of the specified host.
     */
    if (hp == (struct hostent *)0)
    {
        fprintf(stderr, "%s: Unknown host\n", argv[1]);
        exit(2);
    }

    h_addr_index = 0;
    while (hp->h_addr_list[h_addr_index] != NULL)
    {
        bcopy(hp->h_addr_list[h_addr_index], &server.sin6_addr,
              hp->h_length);
        server.sin6_port = htons(atoi(argv[2]));
        if (connect(sock, (struct sockaddr *)&server,
                    sizeof(server)) == -1)
        {
            if (hp->h_addr_list[++h_addr_index] != NULL)
            {
                /* Try next address */
                continue;
            }
            perror("Connecting stream socket");
            freehostent(hp);
            exit(1);
        }
        break;
    }
    freehostent(hp);
    if (write(sock, DATA, sizeof DATA) == -1)
        perror("Writing on stream socket");
    close(sock);
    freehostent(hp);
    exit(0);
}