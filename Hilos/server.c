#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define THREAD_POOL_SIZE 10

// Estructura para pasar parámetros al hilo de manejo de clientes
typedef struct {
    int client_socket;
    int thread_id; // ID del hilo asignado
} ThreadArgs;

// Estructura para información de cada hilo en el pool de hilos
typedef struct {
    pthread_t thread_id;
    int is_busy; // Flag para indicar si el hilo está ocupado o no
    ThreadArgs *args;
} ThreadInfo;

// Declaración global del pool de hilos
ThreadInfo thread_pool[THREAD_POOL_SIZE];

// Variable global para el índice del próximo hilo disponible
int next_available_thread = 0;

// Función de manejo de clientes en el hilo
void *handle_client(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    int client_socket = thread_args->client_socket;
    int thread_id = thread_args->thread_id;

    char buffer[1024] = {0};
    int valread;
    while ((valread = read(client_socket, buffer, 1024)) > 0) {
        printf("[%d] Client message: %s\n", thread_id, buffer);

        // Solicitar al usuario que ingrese el mensaje de respuesta
        printf("Enter response to client %d: ", thread_id);
        fgets(buffer, sizeof(buffer), stdin);

        // Enviar la respuesta al cliente
        write(client_socket, buffer, strlen(buffer));

        memset(buffer, 0, sizeof(buffer)); // Limpiar el buffer para el próximo mensaje
    }

    printf("[%d] Client disconnected\n", thread_id);
    close(client_socket);
    free(args); // Liberar memoria de los argumentos del hilo

    return NULL;
}

int main() {
    // Inicializar el pool de hilos
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        thread_pool[i].is_busy = 0; // Inicializar todos los hilos como no ocupados
        thread_pool[i].args = NULL; // Inicializar los argumentos de los hilos como NULL
    }

    // Configurar el socket del servidor
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    printf("Creating socket...\n");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    printf("Binding socket to port %d...\n", PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening for incoming connections...\n");
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is now listening on port %d...\n", PORT);

    // Loop principal para aceptar conexiones de clientes
    while (1) {
        printf("Waiting for incoming connections...\n");

        // Esperar una conexión de cliente
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue; // Saltar al siguiente intento de aceptar una conexión
        }

        printf("New client connected\n");

        // Encontrar el siguiente hilo disponible en el pool de hilos (Round Robin)
        while (thread_pool[next_available_thread].is_busy) {
            next_available_thread = (next_available_thread + 1) % THREAD_POOL_SIZE;
        }

        // Asignar el cliente al hilo disponible
        thread_pool[next_available_thread].is_busy = 1; // Marcar el hilo como ocupado
        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            perror("Memory allocation failed");
            close(client_socket);
            continue; // Saltar al siguiente cliente si falla la asignación de memoria
        }
        args->client_socket = client_socket; // Establecer el socket del cliente en los argumentos
        args->thread_id = next_available_thread + 1; // Asignar el ID del hilo (comenzando desde 1)
        thread_pool[next_available_thread].args = args; // Asignar los argumentos al hilo
        pthread_create(&thread_pool[next_available_thread].thread_id, NULL, handle_client, (void *)args); // Crear el hilo
        printf("Client assigned to thread %d\n", args->thread_id);
        next_available_thread = (next_available_thread + 1) % THREAD_POOL_SIZE; // Actualizar el índice del próximo hilo disponible
    }

    // Nunca se alcanzará este punto, pero aquí es donde se liberaría cualquier recurso adicional si fuera necesario
    return 0;
}