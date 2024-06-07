/*
    Cliente para servidor de páginas de memoria compartida distribuida
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TAMPAGINA 32
#define LONGMENSAJE 8 * 1024

void imprimirMenu() {
    printf("Menú:\n");
    printf("1. Ver contenido de la página\n");
    printf("2. Modificar contenido de la página\n");
    printf("9. Salir\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("La linea de comando es: ./cliente IP PUERTO\n");
        exit(0);
    }

    // Definicion de variables
    struct sockaddr_in s_sock;
    int idsock;

    // Llamada al sistema socket
    idsock = socket(AF_INET, SOCK_STREAM, 0);
    if (idsock == -1) {
        perror("socket");
        exit(1);
    }

    // Completamos la estructura sockaddr_in
    s_sock.sin_family = AF_INET;
    s_sock.sin_port = htons(atoi(argv[2]));
    s_sock.sin_addr.s_addr = inet_addr(argv[1]);
    memset(s_sock.sin_zero, 0, 8);

    // Conectar al servidor
    if (connect(idsock, (struct sockaddr *)&s_sock, sizeof(s_sock)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Conexion establecida con exito...\n");

    char mensaje[LONGMENSAJE];
    char opcion;

    while (1) {
        imprimirMenu();
        printf("Seleccione una opción: ");
        scanf(" %c", &opcion);
        getchar(); // Para consumir el carácter de nueva línea dejado por scanf

        if (opcion == '1') {
            // Ver contenido de la página
            int pagina;
            printf("Ingrese número de página (0-15): ");
            scanf("%d", &pagina);
            snprintf(mensaje, LONGMENSAJE, "1%d", pagina);
            send(idsock, mensaje, strlen(mensaje), 0);
            int nb = recv(idsock, mensaje, LONGMENSAJE, 0);
            if (nb > 0) {
                mensaje[nb] = '\0';
                printf("Contenido de la página %d: %s\n", pagina, mensaje);
            } else {
                perror("recv");
            }
        } else if (opcion == '2') {
            // Modificar contenido de la página
            int pagina;
            char nuevoContenido[TAMPAGINA];
            printf("Ingrese número de página (0-15): ");
            scanf("%d", &pagina);
            getchar(); // Para consumir el carácter de nueva línea dejado por scanf

            // Informar al servidor que la página está bloqueada
            snprintf(mensaje, LONGMENSAJE, "2%d", pagina);
            send(idsock, mensaje, strlen(mensaje), 0);
            int nb = recv(idsock, mensaje, LONGMENSAJE, 0);
            if (nb > 0) {
                mensaje[nb] = '\0';
                if (strcmp(mensaje, "Pagina invalida\n") == 0 || strcmp(mensaje, "Pagina en uso, intentelo mas tarde\n") == 0) {
                    printf("Respuesta del servidor: %s\n", mensaje);
                    continue;
                }
            } else {
                perror("recv");
                continue;
            }

            // Modificar contenido de la página
            printf("Ingrese nuevo contenido: ");
            fgets(nuevoContenido, TAMPAGINA, stdin);
            nuevoContenido[strcspn(nuevoContenido, "\n")] = '\0'; // Eliminar el carácter de nueva línea
            send(idsock, nuevoContenido, strlen(nuevoContenido), 0);
            nb = recv(idsock, mensaje, LONGMENSAJE, 0);
            if (nb > 0) {
                mensaje[nb] = '\0';
                printf("Respuesta del servidor: %s\n", mensaje);
            } else {
                perror("recv");
            }
        } else if (opcion == '9') {
            // Salir
            mensaje[0] = '9';
            mensaje[1] = '\0';
            send(idsock, mensaje, strlen(mensaje), 0);
            break;
        } else {
            printf("Opción inválida\n");
            continue;
        }
    }

    printf("Conexion terminada\n");
    close(idsock);
    return 0;
}
