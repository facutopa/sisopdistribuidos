#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];

    int client_socket;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};

    // Crear el socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convertir dirección IP de texto a binario
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // Mantener la comunicación con el servidor
    while (1) {
        printf("Enter message: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Enviar el mensaje al servidor
        send(client_socket, buffer, strlen(buffer), 0);

        // Leer la respuesta del servidor
        int valread = read(client_socket, buffer, sizeof(buffer));
        printf("Server response: %s\n", buffer);
    }

    // Cerrar el socket
    close(client_socket);

    return 0;
}