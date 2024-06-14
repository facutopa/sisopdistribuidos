#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_THREADS 2 // Define el número máximo de hilos estáticos
#define BUFFER_SIZE 8192

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

// Definición de un nodo de la lista enlazada
typedef struct Node {
    int data;
    int priority; // Agregamos un campo de prioridad
    struct Node* next;
} Node;

// Definición de la cola
typedef struct {
    Node *front, *rear;
    int size; // Tamaño de la cola
} Queue;

Queue sockQueue = { .front = NULL, .rear = NULL, .size = 0 }; // Inicialización de la cola. `sockQueue` es una instancia de la cola, inicialmente vacía.


int available_threads[MAX_THREADS]; // Array to keep track of available threads

void enqueue(int sock, int priority);
int dequeue();
void mostrarEstadoCola();
void mostrarEstadoHilos();
void informarClienteEspera(int sock);
void UbicacionDelCliente(struct sockaddr_in);
void *RecibeEnviaComandos(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
       printf("sintaxis error ingrese ./comando ip puerto\n");
       exit(-1);
    }

    socklen_t addrlen;
    int sockfd;
    struct sockaddr_in addr_in, addrcli_in;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("socket = %d\n", sockfd);

    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(atoi(argv[2]));
    addr_in.sin_addr.s_addr = inet_addr(argv[1]);
    memset(addr_in.sin_zero, 0, 8);

    addrlen = sizeof(addr_in);
    int bn = bind(sockfd, (struct sockaddr *)&addr_in, addrlen);
    printf("bind = %d\n", bn);

    int lst = listen(sockfd, 5);
    printf("listen = %d\n", lst);

    pthread_t thread_pool[MAX_THREADS]; // Estos hilos continuamente intentan obtener un socket de la cola para procesar las solicitudes del cliente.

    for (int i = 0; i < MAX_THREADS; ++i) {
        available_threads[i] = 1; // Inicializar todos los hilos como disponibles
        pthread_create(&thread_pool[i], NULL, RecibeEnviaComandos, (void*)(intptr_t)i);
    }

    while (1) {
        printf("Servidor en puerto %s esperando conexion del cliente\n", argv[2]);
        int sockclifd = accept(sockfd, (struct sockaddr *)&addrcli_in, &addrlen);
        if (sockclifd > 0) {
            pthread_mutex_lock(&mutex);

            int all_threads_occupied = 1;
            for (int i = 0; i < MAX_THREADS; ++i) {
                if (available_threads[i]) {
                    all_threads_occupied = 0;
                    break;
                }
            }

            if (all_threads_occupied) {
                informarClienteEspera(sockclifd); // Informar al cliente que debe esperar
            }

            int priority = rand() % 10; // Asignar una prioridad aleatoria para la demostración
            enqueue(sockclifd, priority); // Encolar con prioridad

            pthread_cond_broadcast(&cond_var); // Despertar hilos esperando

            pthread_mutex_unlock(&mutex);
            UbicacionDelCliente(addrcli_in);
        } else {
            printf("Conexion Rechazada %d \n", sockclifd);
        }
    }
    exit(0);
}

void enqueue(int sock, int priority) { // Añadimos un nuevo parámetro de prioridad
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->data = sock;
    newNode->priority = priority; // Asignamos la prioridad
    newNode->next = NULL;

    if (sockQueue.rear == NULL || sockQueue.front == NULL) {
        sockQueue.front = newNode;
        sockQueue.rear = newNode;
    } else {
        Node *temp = sockQueue.front;
        Node *prev = NULL;

        // Insertar el nodo en la posición correcta según su prioridad
        while (temp != NULL && temp->priority >= priority) {
            prev = temp;
            temp = temp->next;
        }

        if (prev == NULL) { // Insertar al inicio de la cola
            newNode->next = sockQueue.front;
            sockQueue.front = newNode;
        } else { // Insertar en medio o al final de la cola
            newNode->next = temp;
            prev->next = newNode;
            if (newNode->next == NULL) { // Actualizar el rear si es necesario
                sockQueue.rear = newNode;
            }
        }
    }
    sockQueue.size++;
    printf("Cliente encolado en espera con prioridad %d.\n", priority);
    mostrarEstadoCola();
}

int dequeue() {
    if (sockQueue.front == NULL) {
        return -1; // Cola vacía
    }

    int sock = sockQueue.front->data;
    Node *temp = sockQueue.front;
    sockQueue.front = sockQueue.front->next;
    if (sockQueue.front == NULL) {
        sockQueue.rear = NULL;
    }
    free(temp);
    sockQueue.size--;
    return sock;
}

void mostrarEstadoCola() {
    Node *current = sockQueue.front;
    printf("Estado actual de la cola: ");
    while (current != NULL) {
        printf("[Sock: %d, Pri: %d] -> ", current->data, current->priority);
        current = current->next;
    }
    printf("NULL\n");
}

void mostrarEstadoHilos() {
    printf("Estado actual de los hilos: ");
    for (int i = 0; i < MAX_THREADS; ++i) {
        if (available_threads[i]) {
            printf("[Hilo %d: Disponible] ", i);
        } else {
            printf("[Hilo %d: Ocupado] ", i);
        }
    }
    printf("\n");
}

void informarClienteEspera(int sock) {
    const char *mensaje = "Todos los hilos estan ocupados. Por favor, espere...\n";
    send(sock, mensaje, strlen(mensaje), 0);
}

void *RecibeEnviaComandos(void *arg) {
    int thread_id = (intptr_t)arg;

    while (1) {
        pthread_mutex_lock(&mutex);

        while (sockQueue.size == 0) {
            pthread_cond_wait(&cond_var, &mutex);
        }

        int idsockcli = dequeue(); // Obtener socket de la cola
        available_threads[thread_id] = 0; // Marcar hilo como ocupado

        pthread_mutex_unlock(&mutex);

        mostrarEstadoHilos();

        if (idsockcli != -1) {
            printf("Esperando el comando -> \n");
            char comando[30];
            char salida[8192];
            int p[2];
            int nb;
            int defout;
            while (1) {
                nb = recv(idsockcli, comando, 30, 0);
                if (nb <= 0) {
                    break;
                }
                comando[nb] = '\0';
                printf(" -> Recibido del cliente %d : %s\n", idsockcli, comando);
                if (strncmp(comando, "exit", 4) == 0) {
                    break;
                }
                pipe(p);
                defout = dup(1);
                dup2(p[1], 1);
                system(comando);
                dup2(defout, 1);
                close(defout);
                close(p[1]);
                memset(salida, '\0', 8192);
                nb = read(p[0], salida, 8192);
                salida[nb] = '\0';
                close(p[0]);
                send(idsockcli, salida, strlen(salida), 0);

                // Enviar la secuencia especial para indicar el final de la transmisión
                const char *end_seq = "\nEND\n";
                send(idsockcli, end_seq, strlen(end_seq), 0);
            }
            close(idsockcli);

            pthread_mutex_lock(&mutex);
            available_threads[thread_id] = 1; // Marcar hilo como disponible
            mostrarEstadoHilos();
            pthread_cond_broadcast(&cond_var); // Despertar hilos esperando
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}

void UbicacionDelCliente(struct sockaddr_in c_sock) {
    printf("............c_sock.sin_family %d\n", c_sock.sin_family);
    printf("............c_sock.sin_port %d\n", c_sock.sin_port);
    printf("............c_sock.sin_addr.s_addr %s\n\n", inet_ntoa(c_sock.sin_addr));
}
