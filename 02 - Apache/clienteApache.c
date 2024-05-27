#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define IP "192.168.0.134"
#define PUERTO 80

void eliminarEtiquetasHTML(char *source, char *dest) {
    int in_tag = 0;
    while (*source) {
        if (*source == '<') {
            in_tag = 1;
        } else if (*source == '>') {
            in_tag = 0;
        } else if (!in_tag) {
            *dest++ = *source;
        }
        source++;
    }
    *dest = '\0';
}

int main(int argc, char *argv[]) {
    char file_name[256] = "index.html";
    char buf[8192];
    char message[8192];
    char plain_text[8192];
    int sd;
    struct sockaddr_in pin;
    pin.sin_family = AF_INET;
    pin.sin_addr.s_addr = inet_addr(IP);
    pin.sin_port = htons(PUERTO);
    bzero(&pin.sin_zero, sizeof(pin.sin_zero));

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Error al abrir el socket\n");
        exit(1);
    }

    if (connect(sd, (void *)&pin, sizeof(pin)) == -1) {
        printf("Error al conectar el socket\n");
        exit(1);
    }

    sprintf(message, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", file_name, IP);

    if (send(sd, message, strlen(message), 0) == -1) {
        printf("Error al enviar\n");
        exit(1);
    }

    printf("Mensaje %s enviado al servidor apache...\n", message);

    if (recv(sd, buf, 8192, 0) == -1) {
        printf("Error al recibir la respuesta\n");
        exit(1);
    }

    printf("Respuesta del servidor: \n%s\n", buf);

    eliminarEtiquetasHTML(buf, plain_text);
    printf("Contenido sin etiquetas HTML: \n%s\n", plain_text);

    close(sd);
    return EXIT_SUCCESS;
}