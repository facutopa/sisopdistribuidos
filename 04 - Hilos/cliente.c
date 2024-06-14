#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 8192

void enviarComando(int sockcli) {
    char comando[30];
    printf("Enviar comando desde el Cliente al Servidor >> ");
    fgets(comando, sizeof(comando), stdin);
    comando[strcspn(comando, "\n")] = 0;  // Remove newline character
    send(sockcli, comando, strlen(comando), 0);
}

void recibirMensaje(int sockcli) {
    char buffer[BUFFER_SIZE];
    int n = recv(sockcli, buffer, BUFFER_SIZE, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Respuesta del servidor: %s\n", buffer);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <IP del servidor> <Puerto>\n", argv[0]);
        exit(1);
    }

    int sockcli;
    struct sockaddr_in serv_addr;

    sockcli = socket(AF_INET, SOCK_STREAM, 0);
    if (sockcli < 0) {
        perror("Error al crear el socket");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockcli, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error al conectar con el servidor");
        close(sockcli);
        exit(1);
    }

    while (1) {
        enviarComando(sockcli);
        recibirMensaje(sockcli);
    }

    close(sockcli);
    return 0;
}
