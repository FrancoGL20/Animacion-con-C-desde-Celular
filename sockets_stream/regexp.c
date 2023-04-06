#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>

int main() {
    char *pattern = ":(-?[0-9]+[.][0-9]+).*:([-]?[0-9]+[.][0-9]+).*:(-?[0-9]+[.][0-9]+)";

    //  {data:{"gyroX":0.005093295592814684,"gyroY":-0.0009986853692680597,"gyroZ":0.0009321063989773393}}
    char *text = "{data:{\"gyroX\":-0.005093295592814684,\"gyroY\":-0.0009986853692680597,\"gyroZ\":-00.0009321063989773393}}";

    // Compile la expresión regular
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) {
        printf("No se pudo compilar la expresión regular.\n");
        return 1;
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
        float gyroX = atof(n1);
        float gyroY = atof(n2);
        float gyroZ = atof(n3);
        
        // Imprima las coincidencias
        printf("gyroX: %f\n", gyroX);
        printf("gyroY: %f\n", gyroY);
        printf("gyroZ: %f\n", gyroZ);
    }

    // Libere la memoria
    regfree(&regex);

    return 0;
}
