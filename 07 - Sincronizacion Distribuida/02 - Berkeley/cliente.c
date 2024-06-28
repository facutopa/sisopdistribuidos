#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SERVER "192.168.0.209"
#define PORT 8888

// Helper function to split strings based on a delimiter
int split(const char *str, char c, char ***arr) {
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0') {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0') {
        if (*p == c) {
            (*arr)[i] = (char*) malloc(sizeof(char) * token_len);
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc(sizeof(char) * token_len);
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0') {
        if (*p != c && *p != '\0') {
            *t = *p;
            t++;
        } else {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}

int main() {
    srand(time(NULL));
    float client_local_clock = rand() % 10;
    printf("Client inicia. Client pid es %d\n", getpid());
    printf("Reloj local del Client es %f\n\n", client_local_clock);

    int client_socket_fd, valread;
    char client_read_buffer[1024] = {0};
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if ((client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nClient: Error de creacion de Socket\n");
        return -1;
    }

    if (inet_pton(AF_INET, SERVER, &server_addr.sin_addr) <= 0) {
        printf("\nClient: Direccion erronea\n");
        return -1;
    }

    if (connect(client_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("\nClient: Connection Failed\n");
        return -1;
    }

    char server_ip[INET_ADDRSTRLEN] = "";
    inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, INET_ADDRSTRLEN);
    printf("Client: conectado al servidor(%s:%d).\n", server_ip, ntohs(server_addr.sin_port));

    // First communication round
    valread = read(client_socket_fd, client_read_buffer, 1024);
    printf("Client: lee: '%s'\n", client_read_buffer);

    if (strcmp(client_read_buffer, "Hola desde Servidor, decime el valor de tu reloj local.") == 0) {
        char msg[1024];
        snprintf(msg, sizeof(msg), "Hola desde Cliente, el valor de mi reloj local es %.2f", client_local_clock);
        send(client_socket_fd, msg, strlen(msg), 0);
        printf("Client: envia mensaje: '%s'\n", msg);
    }

    // Second communication round
    valread = read(client_socket_fd, client_read_buffer, 1024);
    printf("Client: lee: '%s'\n", client_read_buffer);

    char **parts;
    int count = split(client_read_buffer, ' ', &parts);
    if (count > 2 && strcmp(parts[count-3], "adjustment") == 0) {
        float offset = atof(parts[count-1]);
        if (strcmp(parts[count-2], "add") == 0) {
            client_local_clock += offset;
        } else if (strcmp(parts[count-2], "minus") == 0) {
            client_local_clock -= offset;
        }
        printf("Reloj local de Cliente es %f\n\n", client_local_clock);
    }

    close(client_socket_fd);
    return 0;
}
