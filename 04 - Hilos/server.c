#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define THREAD_POOL_SIZE 10

// Estructura para pasar parámetros al hilo de manejo de clientes
typedef struct {
    int client_socket;
    int thread_id; // ID del hilo asignado
    int priority;  // Prioridad del cliente
} ThreadArgs;

// Estructura para información de cada hilo en el pool de hilos
typedef struct {
    pthread_t thread_id;
    int is_busy; // Flag para indicar si el hilo está ocupado o no
    ThreadArgs *args;
} ThreadInfo;

// Declaración global del pool de hilos
ThreadInfo thread_pool[THREAD_POOL_SIZE];

// Estructura para la cola de prioridad
typedef struct PriorityQueueNode {
    ThreadArgs *client_args;
    struct PriorityQueueNode *next;
} PriorityQueueNode;

// Declaración global de la cola de prioridad
PriorityQueueNode *priority_queue = NULL;

// Declaración global del socket del servidor
int server_fd;

// Función para insertar en la cola de prioridad
void insert_priority_queue(ThreadArgs *client_args) {
    PriorityQueueNode *new_node = (PriorityQueueNode *)malloc(sizeof(PriorityQueueNode));
    new_node->client_args = client_args;
    new_node->next = NULL;

    if (priority_queue == NULL || priority_queue->client_args->priority < client_args->priority) {
        new_node->next = priority_queue;
        priority_queue = new_node;
    } else {
        PriorityQueueNode *current = priority_queue;
        while (current->next != NULL && current->next->client_args->priority >= client_args->priority) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

// Función para obtener el próximo cliente de mayor prioridad
ThreadArgs *get_next_client() {
    if (priority_queue == NULL) {
        return NULL;
    }
    PriorityQueueNode *node = priority_queue;
    priority_queue = priority_queue->next;
    ThreadArgs *client_args = node->client_args;
    free(node);
    return client_args;
}

// Función de manejo de clientes en el hilo
void *handle_client(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    int client_socket = thread_args->client_socket;
    int thread_id = thread_args->thread_id;

    char buffer[1024] = {0};
    int valread;
    while ((valread = read(client_socket, buffer, 1024)) > 0) {
        printf("[%d] Mensaje de Cliente: %s\n", thread_id, buffer);

        // Solicitar al usuario que ingrese el mensaje de respuesta
        printf("Ingrese la respuesta al Cliente %d: ", thread_id);
        fgets(buffer, sizeof(buffer), stdin);

        // Enviar la respuesta al cliente
        write(client_socket, buffer, strlen(buffer));

        memset(buffer, 0, sizeof(buffer)); // Limpiar el buffer para el próximo mensaje
    }

    printf("[%d] Cliente desconectado\n", thread_id);
    close(client_socket);
    free(args); // Liberar memoria de los argumentos del hilo

    // Marcar el hilo como no ocupado
    thread_pool[thread_id - 1].is_busy = 0;

    return NULL;
}

// Función para cerrar todos los clientes y los hilos
void close_all_clients() {
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        if (thread_pool[i].is_busy) {
            printf("Cerrando la conexión del cliente para el hilo %d\n", thread_pool[i].args->thread_id);
            close(thread_pool[i].args->client_socket);
            thread_pool[i].is_busy = 0;
        }
    }
}

// Manejador de señal para la desconexión del servidor
void handle_sigint(int sig) {
    printf("Atrapada la senial %d, cerrando server y clientes...\n", sig);
    close_all_clients();
    close(server_fd);
    exit(0);
}

int main() {
    // Asignar el manejador de señal
    signal(SIGINT, handle_sigint);

    // Inicializar el pool de hilos
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        thread_pool[i].is_busy = 0; // Inicializar todos los hilos como no ocupados
        thread_pool[i].args = NULL; // Inicializar los argumentos de los hilos como NULL
    }

    // Configurar el socket del servidor
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    printf("Creando socket...\n");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Creacion fallida de socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    printf("Uniendo socket al puerto %d...\n", PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Union fallida");
        exit(EXIT_FAILURE);
    }

    printf("Escuchando proximas conexiones...\n");
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Escucha fallida");
        exit(EXIT_FAILURE);
    }

    printf("Server escuchando en puerto %d...\n", PORT);

    // Loop principal para aceptar conexiones de clientes
    while (1) {
        printf("Esperando por ingreso de conexiones...\n");

        // Declarar client_socket dentro del bucle
        int client_socket;

        // Esperar una conexión de cliente
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Aceptacion fallida");
            continue; // Saltar al siguiente intento de aceptar una conexión
        }

        printf("Nuevo cliente conectado\n");

        // Recibir la prioridad del cliente (suponiendo que el cliente envía su prioridad primero)
        char priority_buffer[10];
        int valread = read(client_socket, priority_buffer, sizeof(priority_buffer));
        int client_priority = atoi(priority_buffer);

        // Crear los argumentos del hilo con la prioridad del cliente
        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            perror("Locacion de memoria fallida");
            close(client_socket);
            continue; // Saltar al siguiente cliente si falla la asignación de memoria
        }
        args->client_socket = client_socket; // Establecer el socket del cliente en los argumentos
        args->priority = client_priority; // Establecer la prioridad del cliente
        args->thread_id = -1; // ID del hilo se establecerá cuando se asigne un hilo

        // Insertar en la cola de prioridad
        insert_priority_queue(args);

        // Encontrar el próximo hilo disponible en el pool de hilos y asignar el cliente de mayor prioridad
        for (int i = 0; i < THREAD_POOL_SIZE; i++) {
            if (!thread_pool[i].is_busy) {
                ThreadArgs *client_args = get_next_client();
                if (client_args != NULL) {
                    thread_pool[i].is_busy = 1; // Marcar el hilo como ocupado
                    client_args->thread_id = i + 1; // Asignar el ID del hilo (comenzando desde 1)
                    thread_pool[i].args = client_args; // Asignar los argumentos al hilo
                    pthread_create(&thread_pool[i].thread_id, NULL, handle_client, (void *)client_args); // Crear el hilo
                    printf("Cliente con prioridad %d asignado al hilo %d\n", client_args->priority, client_args->thread_id);
                }
                break;
            }
        }
    }

    // Nunca se alcanzará este punto, pero aquí es donde se liberaría cualquier recurso adicional si fuera necesario
    return 0;
}
