#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 8888
#define MAX_BUFFER 1024

// Función que maneja la conexión con cada cliente en un hilo separado
void *atender(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <IP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in addr_in, addrcli_in;

    // Crear un socket TCP/IP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(PUERTO);
    addr_in.sin_addr.s_addr = inet_addr(argv[1]);
    memset(addr_in.sin_zero, 0, sizeof(addr_in.sin_zero));

    // Enlazar el socket a la dirección y puerto especificados
    if (bind(sockfd, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0) {
        perror("Error en bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes, con una cola de hasta 5 conexiones
    if (listen(sockfd, 5) < 0) {
        perror("Error en listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor en puerto %d esperando conexión del cliente\n", PUERTO);
    
    socklen_t addrlen = sizeof(addrcli_in);
    while (1) {
        // Aceptar una nueva conexión entrante
        int sockclifd = accept(sockfd, (struct sockaddr *)&addrcli_in, &addrlen);
        if (sockclifd >= 0) {
            // Crear un nuevo hilo para manejar la conexión con el cliente
            pthread_t hilo;
            int *pclient = malloc(sizeof(int));
            *pclient = sockclifd;
            pthread_create(&hilo, NULL, atender, pclient);
            pthread_detach(hilo);  // Separar el hilo para que se maneje independientemente
        } else {
            perror("Error en accept");
        }
    }

    close(sockfd);
    return 0;
}

// Función que se ejecuta en cada hilo para manejar las solicitudes del cliente
void *atender(void *arg) {
    int sockclifd = *((int *)arg);
    free(arg);
    char buffer[MAX_BUFFER];
    memset(buffer, 0, sizeof(buffer));

    while (1) {
        // Recibir solicitud del cliente
        int nb = recv(sockclifd, buffer, sizeof(buffer) - 1, 0);
        if (nb > 0) {
            buffer[nb] = '\0';  // Terminar la cadena recibida
            printf("Solicitud del cliente: %s\n", buffer);

            if (buffer[0] == '1') {
                // Enviar listado del directorio actual al cliente
                DIR *dir = opendir(".");
                if (dir != NULL) {
                    struct dirent *midirent;
                    struct stat mistat;
                    while ((midirent = readdir(dir)) != NULL) {
                        if (stat(midirent->d_name, &mistat) == 0) {
                            snprintf(buffer, sizeof(buffer), "%s %ld\n", midirent->d_name, mistat.st_size);
                            send(sockclifd, buffer, strlen(buffer), 0);
                        }
                    }
                    closedir(dir);
                } else {
                    perror("Error al abrir el directorio");
                }
                strcpy(buffer, "\nEND\n");  // Indicar el fin del listado
                send(sockclifd, buffer, strlen(buffer), 0);
            } else if (buffer[0] == '2') {
                // Enviar contenido de un archivo al cliente
                char *filename = buffer + 1;
                filename[strcspn(filename, "\n")] = '\0';  // Eliminar carácter de nueva línea
                int fd = open(filename, O_RDONLY);
                if (fd < 0) {
                    perror("Error al abrir el archivo");
                    strcpy(buffer, "Error al abrir el archivo");
                    send(sockclifd, buffer, strlen(buffer), 0);
                } else {
                    int bytes_read;
                    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
                        buffer[bytes_read] = '\0';
                        send(sockclifd, buffer, bytes_read, 0);
                    }
                    close(fd);
                }
                strcpy(buffer, "\nEND\n");  // Indicar el fin del archivo
                send(sockclifd, buffer, strlen(buffer), 0);
            } else if (buffer[0] == '3') {
                // Modificar el contenido de un archivo
                char *filename = buffer + 1;
                char *content = strstr(buffer, "\n") + 1;
                filename[strcspn(filename, "\n")] = '\0';  // Eliminar carácter de nueva línea

                // Leer el contenido actual del archivo
                char old_content[MAX_BUFFER];
                int fd = open(filename, O_RDWR);
                if (fd < 0) {
                    perror("Error al abrir el archivo");
                    strcpy(buffer, "Error al abrir el archivo");
                    send(sockclifd, buffer, strlen(buffer), 0);
                } else {
                    int bytes_read = read(fd, old_content, sizeof(old_content) - 1);
                    old_content[bytes_read] = '\0';

                    // Modificar el contenido del archivo basado en la solicitud del cliente
                    char *line = strtok(content, "\n");
                    while (line != NULL) {
                        if (strstr(line, "agregar ") == line) {
                            // Agregar producto
                            line += strlen("agregar ");
                            strcat(old_content, line);
                            strcat(old_content, "\n");
                        } else if (strstr(line, "eliminar ") == line) {
                            // Eliminar producto
                            line += strlen("eliminar ");
                            char *pos = strstr(old_content, line);
                            if (pos != NULL) {
                                char *end_of_line = strstr(pos, "\n");
                                if (end_of_line != NULL) {
                                    memmove(pos, end_of_line + 1, strlen(end_of_line + 1) + 1);
                                } else {
                                    *pos = '\0';
                                }
                            }
                        } else {
                            // Modificar producto
                            char producto[64];
                            int cantidad;
                            sscanf(line, "%s %d", producto, &cantidad);
                            char *pos = strstr(old_content, producto);
                            if (pos != NULL) {
                                char *end_of_line = strstr(pos, "\n");
                                if (end_of_line != NULL) {
                                    memmove(pos, end_of_line + 1, strlen(end_of_line + 1) + 1);
                                } else {
                                    *pos = '\0';
                                }
                                strcat(old_content, producto);
                                strcat(old_content, " ");
                                char cantidad_str[10];
                                sprintf(cantidad_str, "%d", cantidad);
                                strcat(old_content, cantidad_str);
                                strcat(old_content, "\n");
                            }
                        }
                        line = strtok(NULL, "\n");
                    }

                    // Escribir el contenido modificado en el archivo
                    lseek(fd, 0, SEEK_SET);  // Volver al inicio del archivo
                    ftruncate(fd, 0);  // Limpiar el archivo
                    if (write(fd, old_content, strlen(old_content)) < 0) {
                        perror("Error al escribir en el archivo");
                        strcpy(buffer, "Error al escribir en el archivo");
                        send(sockclifd, buffer, strlen(buffer), 0);
                    } else {
                        strcpy(buffer, "Archivo modificado con éxito");
                        send(sockclifd, buffer, strlen(buffer), 0);
                    }
                    close(fd);
                }
                strcpy(buffer, "\nEND\n");  // Indicar el fin de la modificación
                send(sockclifd, buffer, strlen(buffer), 0);
            } else if (buffer[0] == '4') {
                // Eliminar un archivo
                char *filename = buffer + 1;
                filename[strcspn(filename, "\n")] = '\0';  // Eliminar carácter de nueva línea

                if (remove(filename) == 0) {
                    strcpy(buffer, "Archivo eliminado con éxito");
                } else {
                    perror("Error al eliminar el archivo");
                    strcpy(buffer, "Error al eliminar el archivo");
                }
                send(sockclifd, buffer, strlen(buffer), 0);
                strcpy(buffer, "\nEND\n");  // Indicar el fin de la eliminación
                send(sockclifd, buffer, strlen(buffer), 0);
            } else if (buffer[0] == '5') {
                // Crear un nuevo archivo con contenido
                char *filename = buffer + 1;
                char *content = strstr(buffer, "\n") + 1;
                filename[strcspn(filename, "\n")] = '\0';  // Eliminar carácter de nueva línea

                int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd < 0) {
                    perror("Error al crear el archivo");
                    strcpy(buffer, "Error al crear el archivo");
                    send(sockclifd, buffer, strlen(buffer), 0);
                } else {
                    if (write(fd, content, strlen(content)) < 0) {
                        perror("Error al escribir en el archivo");
                        strcpy(buffer, "Error al escribir en el archivo");
                        send(sockclifd, buffer, strlen(buffer), 0);
                    } else {
                        strcpy(buffer, "Archivo creado con éxito");
                        send(sockclifd, buffer, strlen(buffer), 0);
                    }
                    close(fd);
                }
                strcpy(buffer, "\nEND\n");  // Indicar el fin de la creación
                send(sockclifd, buffer, strlen(buffer), 0);
            } else if (buffer[0] == '9') {
                // Cerrar la conexión con el cliente
                strcpy(buffer, "Conexión cerrada");
                send(sockclifd, buffer, strlen(buffer), 0);
                break;
            }
        } else if (nb == 0) {
            // El cliente cerró la conexión
            printf("Cliente desconectado\n");
            break;
        } else {
            // Error al recibir mensaje
            perror("Error al recibir el mensaje");
            break;
        }
        memset(buffer, 0, sizeof(buffer));  // Limpiar el buffer para la próxima solicitud
    }

    close(sockclifd);
    return NULL;
}
