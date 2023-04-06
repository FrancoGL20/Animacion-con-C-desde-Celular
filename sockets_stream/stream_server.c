// ? (Sockets) Fuente: https://docs.oracle.com/cd/E19120-01/open.solaris/817-4415/6mjum5sou/index.html#sockets-87164
// ? (ChatGPT) Fuente: Expresiones regulares en C

/**================================================================================================
 *                                            ABOUT
 * @author         :  
 * @email          :  
 * @repo           :  
 * @createdOn      :  
 * @description    :  
 *================================================================================================**/
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
float gyroX;
float gyroY;
float gyroZ;

void parseString(char *text){
    char *pattern = ":(-?[0-9]+[.][0-9]+).*:([-]?[0-9]+[.][0-9]+).*:(-?[0-9]+[.][0-9]+)";

    //  {data:{"gyroX":0.005093295592814684,"gyroY":-0.0009986853692680597,"gyroZ":0.0009321063989773393}}
    // char *text = "{data:{\"gyroX\":-0.005093295592814684,\"gyroY\":-0.0009986853692680597,\"gyroZ\":-0.0009321063989773393}}";

    // Compile la expresión regular
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) {
        printf("No se pudo compilar la expresión regular.\n");
        exit(1);
    }

    // Busque la primera coincidencia
    regmatch_t matches[4];
    ret = regexec(&regex, text, 4, matches, 0);
    if (ret == 0) {
        // Obtenga las coincidencias y guárdelas en variables independientes
        char n1[100];
        char n2[100];
        char n3[100];
        strncpy(n1, text + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
        strncpy(n2, text + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
        strncpy(n3, text + matches[3].rm_so, matches[3].rm_eo - matches[3].rm_so);
        n1[matches[1].rm_eo - matches[1].rm_so] = '\0';
        n2[matches[2].rm_eo - matches[2].rm_so] = '\0';
        n3[matches[3].rm_eo - matches[3].rm_so] = '\0';


        // parseo de string a float
        gyroX = atof(n1);
        gyroY = atof(n2);
        gyroZ = atof(n3);
        
        // Imprima las coincidencias
        printf("\ngyroX: %f\n", gyroX);
        printf("gyroY: %f\n", gyroY);
        printf("gyroZ: %f\n\n", gyroZ);
    }

    // Libere la memoria
    regfree(&regex);
}

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
                    // printf("-->%s\n", buf); // !! IMPORTANTE
                    parseString(buf);
                }
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