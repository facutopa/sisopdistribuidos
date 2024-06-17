#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/socket.h>

#define ipServidor "192.168.0.215"   // IP del proceso 1
#define puertoServidor 8888         // puerto de escucha del proceso 1

void DameHoraMaquina(char *);

int main() {
    int dccliente;
    struct sockaddr_in remoto;

    dccliente = socket(AF_INET, SOCK_STREAM, 0); // Obtiene el identificador del socket

    if (dccliente == -1) {
        printf("socket fallo----\n");
        exit(0);
    }

    // Rellenamos la estructura sockaddr_in remoto
    remoto.sin_family = AF_INET; // Dominio de Internet
    remoto.sin_port = htons(puertoServidor); // puerto de escucha del servidor
    remoto.sin_addr.s_addr = inet_addr(ipServidor); // dirección IPv4 donde está corriendo el servidor
    memset(remoto.sin_zero, 0, 8); // Completamos la estructura con ceros

    // Establecemos conexión
    if (connect(dccliente, (struct sockaddr *)&remoto, sizeof(remoto)) == -1) {
        printf("Conexion fallo...\n");
        exit(0);
    }

    printf("Conexion establecida con exito...\n");

    char bufin[512];
    printf("\nHora de envio del mensaje A: ");
    memset(bufin, 0, 512);
    DameHoraMaquina(bufin);
    printf("%s\n", bufin);

    if (send(dccliente, bufin, strlen(bufin), 0) == -1) {
        printf("send fallo...\n");
        exit(0);
    }

    close(dccliente);
    return 0;
}

void DameHoraMaquina(char *Hora) {
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    strftime(Hora, 128, "%H:%M:%S", tlocal);
}
