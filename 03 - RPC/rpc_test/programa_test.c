#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

void funcionip(char * hostname);

//Para ejecutar el programa escribi '/.programa_test google.com'

int main(int argc, char *argv[]) {
        if (argc != 2) {
        printf("Uso: %s <nombre_del_host>\n", argv[0]);
        return 1;
        }

        char *hostname = argv[1];
        funcionip(hostname);

        return 0;
}

void funcionip(char * hostname)
{
    struct in_addr **addr_list;
    struct hostent *he = gethostbyname(hostname);

    if (he)
      {
        int i;
        puts(he->h_name);
        printf("Nombre del Host : %s\n", he->h_name);
        printf(" Direcciones IP  ");
        addr_list = (struct in_addr **)he->h_addr_list;
        for(i = 0; addr_list[i] != NULL; i++)
           {
                printf("%s \n", inet_ntoa(*addr_list[i]));
          }
        printf("\n");
      }
    else
        printf("error gethostbyname\n");
}