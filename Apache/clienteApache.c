#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define IP "192.168.0.134"
#define PUERTO 80
int main (int argc, char *argv[])
 {
   char file_name[256] = "index.html\0";
   char buf[8192];
   char message[8192];
   int sd;
   struct sockaddr_in pin;
   pin.sin_family = AF_INET;
   pin.sin_addr.s_addr = inet_addr(IP);
   pin.sin_port = htons(PUERTO);
   bzero(&pin.sin_zero, sizeof(pin.sin_zero));
   if ((sd=socket(AF_INET,SOCK_STREAM,0)) == -1)
        {
                printf("Error al abrir el socket\n");
                exit(1);
      }
    if(connect(sd, (void *)&pin,sizeof(pin)) == -1)
        {
                printf("Error al conectar el socket\n");
                exit(1);
       }
      sprintf(message, "GET /%s \n",file_name);
      if(send(sd,message,strlen(message),0) == -1)
        {
                printf("Error al enviar");
                exit(1);
        }
        printf("mensaje %s enviado al servidor apache...\n",message);
        if(recv(sd,buf,8192,0) == -1)
        {
                printf("Error al recibir la repuesta..\n");
                exit(1);
        }
        printf("Respuesta del servidor: \n%s\n",buf);
        close(sd);
        return EXIT_SUCCESS;
}