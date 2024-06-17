#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

#define IP "192.168.0.209" // Direccion del servidor
#define PUERTO 8888        // Puerto de escucha del servidor

void DameHoraMaquina(char *);

int main(int argc, char *argv[]) {
    struct sockaddr_in s_sock, c_sock;
    int idsocks, idsockc;
    int lensock = sizeof(struct sockaddr_in);
    idsocks = socket(AF_INET, SOCK_STREAM, 0);
    printf("idsocks %d\n", idsocks);

    s_sock.sin_family = AF_INET;
    s_sock.sin_port = htons(PUERTO);
    s_sock.sin_addr.s_addr = inet_addr(IP);

    memset(s_sock.sin_zero, 0, 8);

    printf("bind %d\n", bind(idsocks, (struct sockaddr *) &s_sock, lensock));
    printf("listen %d\n", listen(idsocks, 5));

    while (1) {
        printf("Esperando conexion\n");
        idsockc = accept(idsocks, (struct sockaddr *)&c_sock, &lensock);

        if (idsockc != -1) {
            if (!fork()) {
                printf("Conexion aceptada desde el cliente\n");
                char hora[128];
                DameHoraMaquina(hora);
                printf("Hora enviada al cliente: %s\n", hora);
                send(idsockc, hora, strlen(hora), 0);
                close(idsockc);
                exit(0);
            }
        } else {
            printf("Conexion rechazada %d \n", idsockc);
        }
    }
    close(idsocks);
    return 0;
}

void DameHoraMaquina(char *Hora) {
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    strftime(Hora, 128, "%Y%m%d %H:%M:%S", tlocal);
}
