#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8888
#define MAX_CLIENTS 3

int main() {
    srand(time(NULL));
    float server_local_clock = rand() % 10;  // Simulamos el reloj del servidor
    printf("Reloj Local del Servidor: %.2f\n", server_local_clock); // Mostrar la hora inicial del servidor

    int server_socket_fd, new_socket, valread;
    int client_sockets[MAX_CLIENTS];
    float client_clocks[MAX_CLIENTS];
    int num_clients = 0;
    char buffer[1024] = {0};

    struct sockaddr_in server_address, client_addr;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    int opt = 1;

    if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Server: fallo socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Server: setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(server_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Server: bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket_fd, 7) < 0) {
        perror("Server: listen");
        exit(EXIT_FAILURE);
    }

    printf("Server: server esta esperando...\n");

    while (num_clients < MAX_CLIENTS) {
        socklen_t length = sizeof(client_addr);
        if ((new_socket = accept(server_socket_fd, (struct sockaddr *)&client_addr, &length)) < 0) {
            perror("Server: aceptado");
            exit(EXIT_FAILURE);
        }

        client_sockets[num_clients] = new_socket;
        num_clients++;

        printf("Server: nuevo Cliente aceptado. Client num.: %d\n", num_clients);

        const char *msg = "Hola desde Servidor, decime el valor de tu hora local.";
        send(new_socket, msg, strlen(msg), 0);

        valread = read(new_socket, buffer, 1024);
        printf("Server: recibido '%s'\n", buffer);

        // Extract the last part of the message which contains the client's clock
        sscanf(buffer, "Hola desde Cliente, el valor de mi hora local es %f", &client_clocks[num_clients - 1]);
    }

    float sum_clocks = server_local_clock;
    for (int i = 0; i < num_clients; i++) {
        sum_clocks += client_clocks[i];
    }
    float average_clock = sum_clocks / (num_clients + 1);

    // Display the calculated average clock
    printf("Reloj promedio calculado: %.2f\n", average_clock);

    // Send adjustment to each client and adjust server's clock
    for (int i = 0; i < num_clients; i++) {
        float adjustment = average_clock - client_clocks[i];
        sprintf(buffer, "Ajusta tu reloj en %f segundos", adjustment);
        send(client_sockets[i], buffer, strlen(buffer), 0);
    }

    // Adjust the server's own clock
    server_local_clock = average_clock;
    printf("Reloj local ajustado del servidor: %.2f\n", server_local_clock);

    close(server_socket_fd);
    return 0;
}
