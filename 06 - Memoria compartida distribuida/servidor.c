/*
    Servidor de páginas de memoria compartida distribuida
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define IP "192.168.0.209" // Ubicación del servidor
#define PUERTO 8888
#define TAMPAGINA 32
#define NUM_PAGINAS (512 / TAMPAGINA)
#define LONGMENSAJE 8 * 1024

char *vecPuntPag[NUM_PAGINAS];
int paginasEnUso[NUM_PAGINAS];
sem_t semaforo;

void *maneja_cliente(void *arg);
void UbicacionDelCliente(struct sockaddr_in);
void inicializa_memoria();
void imprime_info_paginas();

int main(int argc, char *argv[]) {
    struct sockaddr_in s_sock, c_sock;
    int idsocks, idsockc;
    int lensock = sizeof(struct sockaddr_in);
    pthread_t thread_id;

    idsocks = socket(AF_INET, SOCK_STREAM, 0);
    printf("idsocks %d\n", idsocks);
    s_sock.sin_family = AF_INET;
    s_sock.sin_port = htons(PUERTO);
    s_sock.sin_addr.s_addr = inet_addr(IP);
    memset(s_sock.sin_zero, 0, 8);

    printf("bind %d\n", bind(idsocks, (struct sockaddr *)&s_sock, lensock));
    printf("listen %d\n", listen(idsocks, 5));

    inicializa_memoria();
    sem_init(&semaforo, 0, 1);

    while (1) {
        imprime_info_paginas();
        printf("esperando conexion\n");
        idsockc = accept(idsocks, (struct sockaddr *)&c_sock, &lensock);
        if (idsockc != -1) {
            printf("conexion aceptada desde el cliente\n");
            UbicacionDelCliente(c_sock);

            int *arg = malloc(sizeof(*arg));
            *arg = idsockc;
            if (pthread_create(&thread_id, NULL, maneja_cliente, arg) != 0) {
                perror("pthread_create");
            }
        } else {
            printf("conexion rechazada %d \n", idsockc);
        }
    }

    sem_destroy(&semaforo);
    exit(0);
}

void inicializa_memoria() {
    int shmid = shmget(IPC_PRIVATE, TAMPAGINA * NUM_PAGINAS, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }
    char *dir = (char *)shmat(shmid, NULL, 0);
    if (dir == (char *)-1) {
        perror("shmat");
        exit(1);
    }
    for (int i = 0; i < NUM_PAGINAS; i++) {
        vecPuntPag[i] = dir + (i * TAMPAGINA);
        memset(vecPuntPag[i], '-', TAMPAGINA); // Inicializa cada página con '-'
        paginasEnUso[i] = -1;
    }
}

void *maneja_cliente(void *arg) {
    int idsockc = *((int *)arg);
    free(arg);

    char mensaje[LONGMENSAJE];
    char opcion;

    while (1) {
        memset(mensaje, '\0', LONGMENSAJE);
        int nb = recv(idsockc, mensaje, LONGMENSAJE, 0);
        if (nb <= 0) {
            perror("recv");
            break;
        }
        mensaje[nb] = '\0';
        opcion = mensaje[0];
        printf("cliente %d dice -> %s\n", idsockc, mensaje); // Corrección del mensaje

        if (opcion == '1') { // Cliente pide el contenido de una página
            int pagina = mensaje[1] - '0';
            if (pagina >= 0 && pagina < NUM_PAGINAS) {
                sem_wait(&semaforo);
                strncpy(mensaje, vecPuntPag[pagina], TAMPAGINA);
                sem_post(&semaforo);
            } else {
                strcpy(mensaje, "Pagina invalida\n");
            }
        } else if (opcion == '2') { // Cliente modifica una página
            int pagina = mensaje[1] - '0';
            if (pagina >= 0 && pagina < NUM_PAGINAS) {
                sem_wait(&semaforo);
                strncpy(vecPuntPag[pagina], &mensaje[2], TAMPAGINA);
                sem_post(&semaforo);
                strcpy(mensaje, "Actualizacion exitosa\n");
            } else {
                strcpy(mensaje, "Pagina invalida\n");
            }
        } else if (opcion == '9') { // Cliente desea salir
            strcpy(mensaje, "Desconectando\n");
            send(idsockc, mensaje, strlen(mensaje), 0);
            break;
        } else {
            strcpy(mensaje, "Opcion invalida\n");
        }

        send(idsockc, mensaje, strlen(mensaje), 0);
        imprime_info_paginas(); // Imprime el contenido de todas las páginas después de cada interacción
    }

    close(idsockc);
    pthread_exit(NULL);
}

void imprime_info_paginas() {
    printf("Contenido de todas las páginas:\n");
    for (int i = 0; i < NUM_PAGINAS; i++) {
        printf("Pagina %d: %.*s\n", i, TAMPAGINA, vecPuntPag[i]);
    }
}

void UbicacionDelCliente(struct sockaddr_in c_sock) {
    printf("............c_sock.sin_family %d\n", c_sock.sin_family);
    printf("............c_sock.sin_port %d\n", ntohs(c_sock.sin_port));
    printf("............c_sock.sin_addr.s_addr %s\n\n", inet_ntoa(c_sock.sin_addr));
}
