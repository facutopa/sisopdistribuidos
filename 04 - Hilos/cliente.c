#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Ingreso: %s <server_ip> <priority>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int priority = atoi(argv[2]);

    int client_socket;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};

    // Crear el socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Creacion erronea de Socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convertir dirección IP de texto a binario
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("Direccion incorrecta o no soportoda");
        exit(EXIT_FAILURE);
    }

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Conexion fallida");
        exit(EXIT_FAILURE);
    }

    printf("Conectado al servidor\n");

    // Enviar la prioridad al servidor
    char priority_buffer[10];
    sprintf(priority_buffer, "%d", priority);
    if (send(client_socket, priority_buffer, strlen(priority_buffer), 0) < 0) {
        perror("Envio fallido");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Mantener la comunicación con el servidor
    while (1) {
        printf("Ingrese el mensaje: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Enviar el mensaje al servidor
        if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
            perror("Envio fallido");
            break;
        }

        // Leer la respuesta del servidor
        int valread = read(client_socket, buffer, sizeof(buffer) - 1);
        if (valread < 0) {
            perror("Lectura fallida");
            break;
        } else if (valread == 0) {
            printf("Server desconectado\n");
            break;
        }

        buffer[valread] = '\0';
        printf("Respuesta de Server: %s\n", buffer);
    }

    // Cerrar el socket
    close(client_socket);

    return 0;
}
