/*
 Cliente en Linux para mini Telnet de un servidor de comandos Debian
 Hay que agregarle el ingreso de un usuario y contraseña
 para que sea validado por el servidor.
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

#define ipServidor "192.168.0.209"
#define puertoServidor 8888

void UbicacionDelServidor(struct in_addr);

int main ()
{
    int dccliente; // Contiene el descriptor de socket cliente
    struct sockaddr_in remoto; // Contiene los datos de conexión del equipo remoto

    dccliente = socket(AF_INET, SOCK_STREAM, 0); // Obtiene el identificador del socket
    if (dccliente == -1)
    {
        printf("socket fallo----\n");
        exit(0);
    }

    // Rellenamos la estructura sockaddr_in remoto
    remoto.sin_family = AF_INET; // Dominio de Internet
    remoto.sin_port = htons(puertoServidor); // Puerto de escucha del servidor
    remoto.sin_addr.s_addr = inet_addr(ipServidor); // Dirección IPv4 donde está corriendo el servidor
    memset(remoto.sin_zero, 0, sizeof(remoto.sin_zero)); // Completamos la estructura con ceros

    // Establecemos conexión
    if (connect(dccliente, (struct sockaddr *)&remoto, sizeof(remoto)) == -1)
    {
        printf("Conexion fallo...\n");
        exit(0);
    }

    printf("Conexion establecida con exito...Datos del Servidor...\n");
    UbicacionDelServidor(remoto.sin_addr);

    char bufin[512];
    while (1)
    {
        printf("\nIngrese comando Linux: ");
        memset(bufin, 0, sizeof(bufin));
        fgets(bufin, sizeof(bufin), stdin);
        bufin[strcspn(bufin, "\n")] = 0; // Elimina el carácter de nueva línea

        if (strcmp(bufin, "exit") == 0)
        {
            break;
        }

        if (send(dccliente, bufin, strlen(bufin), 0) == -1)
        {
            printf("send fallo...\n");
            exit(0);
        }

        char buf[4096];
        int nb;
        while ((nb = recv(dccliente, buf, sizeof(buf) - 1, 0)) > 0)
        {
            buf[nb] = '\0';
            printf("%s", buf);

            // Verifica si la secuencia especial está presente
            if (strstr(buf, "\nEND\n") != NULL)
            {
                break;
            }
        }
        if (nb == -1)
        {
            printf("recv fallo...\n");
            exit(0);
        }
    }

    if (send(dccliente, bufin, strlen(bufin), 0) == -1)
    {
        printf("send fallo...\n");
        exit(0);
    }

    printf("Conexion terminada\n");
    close(dccliente);
    return 0;
}

void UbicacionDelServidor(struct in_addr addr)
{
    struct hostent *remoteHost;
    char **pAlias;
    remoteHost = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (remoteHost == NULL)
    {
        printf("Fallo gethostbyaddr\n");
    }
    else
    {
        printf("Nombre Servidor: %s\n", remoteHost->h_name);
        for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++)
        {
            printf("Nombre Alternativo: %s\n", *pAlias);
        }
        printf("Tipo Address: ");
        switch (remoteHost->h_addrtype)
        {
        case AF_INET:
            printf("AF_INET\n");
            break;
        case AF_INET6:
            printf("AF_INET6\n");
            break;
        default:
            printf("%d\n", remoteHost->h_addrtype);
            break;
        }
        printf("Bytes Address: %d\n", remoteHost->h_length);
        if (remoteHost->h_addrtype == AF_INET)
        {
            int i = 0;
            while (remoteHost->h_addr_list[i] != 0)
            {
                addr.s_addr = *(u_long *)remoteHost->h_addr_list[i++];
                printf("IPv4 Address #%d: %s\n", i, inet_ntoa(addr));
            }
        }
        else if (remoteHost->h_addrtype == AF_INET6)
        {
            printf("IPv6 address\n");
        }
    }
}
