// ? (Sockets) Fuente: https://docs.oracle.com/cd/E19120-01/open.solaris/817-4415/6mjum5sou/index.html#sockets-87164
// ? (ChatGPT) Fuente: Expresiones regulares en C

/**================================================================================================
 *?                                        Librerias
 *================================================================================================**/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#define TRUE 1

/**================================================================================================
 *?                                   Variables globales
 *================================================================================================**/




int main()
{
    int sock, length;
    struct sockaddr_in6 server;
    int msgsock;
    char buf[1024];
    int rval;
    // json
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;

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
    server.sin6_port = 63020; // Static Socket port: 11510
    if (bind(sock, (struct sockaddr *)&server, sizeof server) == -1)
    {
        perror("Binding stream socket");
        exit(1);
    }

    /* Find out assigned port number and print it out. */
    length = sizeof server;
    if (getsockname(sock, (struct sockaddr *)&server, (socklen_t *)&length) == -1) {
        perror("Getting socket name");
        exit(1);
    }
    printf("Socket port: %d\n", ntohs(server.sin6_port));
    
    /* Start accepting connections. */
    listen(sock, 5);
    do {
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
                {
                    /* assumes the data is printable */
                    printf("-->%s\n", buf); // !! IMPORTANTE
                    
                    // Escribir en un archivo el contenido del string buf, si el archivo no existe, crearlo, si existe, sobreescribirlo
                    FILE *fp;
                    fp = fopen("data.json", "w");
                    fprintf(fp, "%s", buf);
                    fclose(fp);
                }
            } while (rval > 0);
        close(msgsock);
    } while (TRUE);

    exit(0);
}