#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/socket.h>

#define ipServidor "192.168.0.209"   // IP del servidor de tiempo
#define puertoServidor 8888          // Puerto de escucha del servidor de tiempo

void DameHoraMaquina(char *);
struct tm string_to_time(const char *);
double calcular_latencia(struct timespec, struct timespec);

int main() {
    int dccliente;
    struct sockaddr_in remoto;
    struct timespec t0, t1;

    dccliente = socket(AF_INET, SOCK_STREAM, 0); // Obtiene el identificador del socket

    if (dccliente == -1) {
        printf("socket fallo----\n");
        exit(0);
    }

    // Rellenamos la estructura sockaddr_in remoto
    remoto.sin_family = AF_INET; // Dominio de Internet
    remoto.sin_port = htons(puertoServidor); // puerto de escucha del servidor
    remoto.sin_addr.s_addr = inet_addr(ipServidor); // dirección IPv4 donde está corriendo el servidor
    memset(remoto.sin_zero, 0, 8); // Completamos la estructura con ceros

    // Medir el tiempo de inicio
    clock_gettime(CLOCK_REALTIME, &t0);

    // Establecemos conexión
    if (connect(dccliente, (struct sockaddr *)&remoto, sizeof(remoto)) == -1) {
        printf("Conexion fallo...\n");
        exit(0);
    }

    printf("Conexion establecida con exito...\n");

    char bufin[128];
    if (recv(dccliente, bufin, sizeof(bufin), 0) == -1) {
        printf("fallo...\n");
        exit(0);
    }

    sleep(10);
    // Medir el tiempo de fin
    clock_gettime(CLOCK_REALTIME, &t1);

    close(dccliente);

    printf("Hora recibida del servidor: %s\n", bufin);

    double latencia = calcular_latencia(t0, t1);
    printf("Latencia calculada: %f segundos\n", latencia);

    struct tm hora_servidor = string_to_time(bufin);
    time_t t_servidor = mktime(&hora_servidor);
    t_servidor += (time_t)(latencia / 2);

    struct tm *hora_ajustada = localtime(&t_servidor);
    char hora_ajustada_str[128];
    strftime(hora_ajustada_str, 128, "%Y%m%d %H:%M:%S", hora_ajustada);

    printf("Hora ajustada: %s\n", hora_ajustada_str);

    return 0;
}

void DameHoraMaquina(char *Hora) {
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    strftime(Hora, 128, "%Y%m%d %H:%M:%S", tlocal);
}

struct tm string_to_time(const char *time_str) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    sscanf(time_str, "%4d%2d%2d %2d:%2d:%2d",
           &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
           &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;  // Ajustar para la estructura tm
    tm.tm_mon -= 1;      // Ajustar para la estructura tm
    return tm;
}

double calcular_latencia(struct timespec t0, struct timespec t1) {
    double inicio = t0.tv_sec + (t0.tv_nsec / 1.0e9);
    double fin = t1.tv_sec + (t1.tv_nsec / 1.0e9);
    return (fin - inicio);
}
