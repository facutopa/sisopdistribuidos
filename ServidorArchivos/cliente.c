#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LONGMENSAJE 8*1024

void enviameDirectorio(int sockclifd);
void enviameUnArchivo(int sockclifd);
void modificaArchivo(int sockclifd);
void eliminaArchivo(int sockclifd);
void creaArchivo(int sockclifd);
void cierraConexion(int sockclifd);
int menu(void);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP> <Puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockclifd;
    struct sockaddr_in addrcli_in;

    if ((sockclifd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    addrcli_in.sin_family = AF_INET;
    addrcli_in.sin_port = htons(atoi(argv[2]));
    addrcli_in.sin_addr.s_addr = inet_addr(argv[1]);
    memset(addrcli_in.sin_zero, 0, sizeof(addrcli_in.sin_zero));

    if (connect(sockclifd, (struct sockaddr *)&addrcli_in, sizeof(addrcli_in)) < 0) {
        perror("Error en connect");
        close(sockclifd);
        exit(EXIT_FAILURE);
    }

    printf("Cliente conectado con el servidor\n");
    int opcion;
    while ((opcion = menu()) != 9) {
        switch (opcion) {
            case 1:
                enviameDirectorio(sockclifd);
                break;
            case 2:
                enviameUnArchivo(sockclifd);
                break;
            case 3:
                modificaArchivo(sockclifd);
                break;
            case 4:
                eliminaArchivo(sockclifd);
                break;
            case 5:
                creaArchivo(sockclifd);
                break;
            default:
                printf("Opción no válida\n");
                break;
        }
    }
    cierraConexion(sockclifd);
    close(sockclifd);
    return 0;
}

void enviameDirectorio(int sockclifd) {
    char mensaje[LONGMENSAJE];
    memset(mensaje, 0, sizeof(mensaje));
    strcpy(mensaje, "1");
    send(sockclifd, mensaje, strlen(mensaje), 0);

    printf("Archivos del directorio de trabajo:\n");
    while (1) {
        int nb = recv(sockclifd, mensaje, sizeof(mensaje) - 1, 0);
        if (nb > 0) {
            mensaje[nb] = '\0';
            if (strcmp(mensaje, "FIN") == 0) {
                break;
            }
            printf("%s", mensaje);
        } else {
            if (nb < 0) perror("Error al recibir el mensaje");
            break;
        }
    }
    printf("\n");
}

void enviameUnArchivo(int sockclifd) {
    char nombreArchivo[64];
    memset(nombreArchivo, 0, sizeof(nombreArchivo));
    char mensaje[LONGMENSAJE];
    memset(mensaje, 0, sizeof(mensaje));
    strcpy(mensaje, "2");
    printf("Ingrese el nombre del archivo: ");
    scanf("%63s", nombreArchivo);
    strcat(mensaje, nombreArchivo);
    send(sockclifd, mensaje, strlen(mensaje), 0);

    // Recepción del archivo en partes
    printf("Contenido del Archivo:\n");
    while (1) {
        int nb = recv(sockclifd, mensaje, sizeof(mensaje) - 1, 0);
        if (nb > 0) {
            mensaje[nb] = '\0';
            if (strcmp(mensaje, "FIN") == 0) {
                break;
            }
            printf("%s", mensaje);
        } else {
            if (nb < 0) perror("Error al recibir el mensaje");
            break;
        }
    }
    printf("\n"); // Añadir un salto de línea al final del contenido del archivo
}

void modificaArchivo(int sockclifd) {
    char nombreArchivo[64];
    char nuevoContenido[LONGMENSAJE];
    memset(nombreArchivo, 0, sizeof(nombreArchivo));
    memset(nuevoContenido, 0, sizeof(nuevoContenido));
    char mensaje[LONGMENSAJE];
    memset(mensaje, 0, sizeof(mensaje));
    strcpy(mensaje, "3");
    printf("Ingrese el nombre del archivo a modificar: ");
    scanf("%63s", nombreArchivo);
    getchar();  // Para limpiar el salto de línea del buffer de entrada
    printf("Ingrese el nuevo contenido del archivo: ");
    fgets(nuevoContenido, LONGMENSAJE, stdin);
    strcat(mensaje, nombreArchivo);
    strcat(mensaje, "\n");
    strcat(mensaje, nuevoContenido);
    send(sockclifd, mensaje, strlen(mensaje), 0);

    // Recepción de la confirmación
    printf("Respuesta del servidor:\n");
    while (1) {
        int nb = recv(sockclifd, mensaje, sizeof(mensaje) - 1, 0);
        if (nb > 0) {
            mensaje[nb] = '\0';
            if (strcmp(mensaje, "FIN") == 0) {
                break;
            }
            printf("%s\n", mensaje);
        } else {
            if (nb < 0) perror("Error al recibir el mensaje");
            break;
        }
    }
    printf("\n");
}

void eliminaArchivo(int sockclifd) {
    char nombreArchivo[64];
    memset(nombreArchivo, 0, sizeof(nombreArchivo));
    char mensaje[LONGMENSAJE];
    memset(mensaje, 0, sizeof(mensaje));
    strcpy(mensaje, "4");
    printf("Ingrese el nombre del archivo a eliminar: ");
    scanf("%63s", nombreArchivo);
    strcat(mensaje, nombreArchivo);
    send(sockclifd, mensaje, strlen(mensaje), 0);

    // Recepción de la confirmación
    printf("Respuesta del servidor:\n");
    while (1) {
        int nb = recv(sockclifd, mensaje, sizeof(mensaje) - 1, 0);
        if (nb > 0) {
            mensaje[nb] = '\0';
            if (strcmp(mensaje, "FIN") == 0) {
                break;
            }
            printf("%s\n", mensaje);
        } else {
            if (nb < 0) perror("Error al recibir el mensaje");
            break;
        }
    }
    printf("\n");
}

void creaArchivo(int sockclifd) {
    char nombreArchivo[64];
    char contenido[LONGMENSAJE];
    memset(nombreArchivo, 0, sizeof(nombreArchivo));
    memset(contenido, 0, sizeof(contenido));
    char mensaje[LONGMENSAJE];
    memset(mensaje, 0, sizeof(mensaje));
    strcpy(mensaje, "5");
    printf("Ingrese el nombre del nuevo archivo: ");
    scanf("%63s", nombreArchivo);
    getchar();  // Para limpiar el salto de línea del buffer de entrada
    printf("Ingrese el contenido del nuevo archivo: ");
    fgets(contenido, LONGMENSAJE, stdin);
    strcat(mensaje, nombreArchivo);
    strcat(mensaje, "\n");
    strcat(mensaje, contenido);
    send(sockclifd, mensaje, strlen(mensaje), 0);

    // Recepción de la confirmación
    printf("Respuesta del servidor:\n");
    while (1) {
        int nb = recv(sockclifd, mensaje, sizeof(mensaje) - 1, 0);
        if (nb > 0) {
            mensaje[nb] = '\0';
            if (strcmp(mensaje, "FIN") == 0) {
                break;
            }
            printf("%s\n", mensaje);
        } else {
            if (nb < 0) perror("Error al recibir el mensaje");
            break;
        }
    }
    printf("\n");
}

void cierraConexion(int sockclifd) {
    char mensaje[LONGMENSAJE];
    memset(mensaje, 0, sizeof(mensaje));
    strcpy(mensaje, "9");
    send(sockclifd, mensaje, strlen(mensaje), 0);
    memset(mensaje, 0, sizeof(mensaje));
    int nb = recv(sockclifd, mensaje, sizeof(mensaje) - 1, 0);
    if (nb > 0) {
        mensaje[nb] = '\0';
        printf("Cierre de conexión: %s\n", mensaje);
    } else {
        perror("Error al recibir el mensaje");
    }
}

int menu(void) {
    printf("Menu de opciones del Cliente\n");
    printf("1. Enviame el Directorio\n");
    printf("2. Pedir un archivo\n");
    printf("3. Modificar un archivo\n");
    printf("4. Eliminar un archivo\n");
    printf("5. Crear un archivo\n");
    printf("9. Cerrar conexion\n");
    printf("_________________\n");
    printf("Ingrese opción: ");
    int opcion;
    scanf("%d", &opcion);
    return opcion;
}
