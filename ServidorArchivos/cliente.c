#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LONGMENSAJE 8*1024

void enviameDirectorio(int);
void enviameUnArchivo(int);
void cierraConexion(int);

int menu(void);

int main(int argc, char * argv[])
{
    if (argc != 3 )
     {
       printf("sintaxis error ingrese ./comando ip puerto\n");
       exit(-1) ;
     }
     int sockclifd ;
     struct sockaddr_in addrcli_in ;
     sockclifd = socket(AF_INET, SOCK_STREAM , 0);
     printf("socket %d \n",sockclifd);

     //completar la estrutura addr_in
     addrcli_in.sin_family = AF_INET ;
     addrcli_in.sin_port   = htons(atoi(argv[2])) ;
     addrcli_in.sin_addr.s_addr  = inet_addr(argv[1]) ;
     memset(addrcli_in.sin_zero,0,8) ;

     int error = connect(sockclifd, (struct sockaddr *)&addrcli_in, (socklen_t)sizeof(addrcli_in));
     if ( error == -1 )
        printf("ERROR CONNECT\n");
     else
         {
              printf("Cliente conectado con el servidor\n");
              int opcion ;
              while((opcion = menu()) != 9)
              {
                 switch(opcion)
                 {
                    case 1:
                         {
                           enviameDirectorio(sockclifd);
                           break;
                         }
                    case 2:
                         {
                           enviameUnArchivo(sockclifd);
                           break;
                         }
                    case 3: break;
                 }
              }
              cierraConexion(sockclifd);
              close(sockclifd);
         }
}


void enviameDirectorio(int sockclifd)
{
              char mensaje[LONGMENSAJE];
              memset(mensaje,'\0',LONGMENSAJE);
              strcat(mensaje,"1");
              send(sockclifd,mensaje,strlen(mensaje),0);
              memset(mensaje,'\0',LONGMENSAJE);
              int nb = recv(sockclifd,mensaje,LONGMENSAJE,0);
              mensaje[nb] = '\0';
              printf("Archivos del directorio de trabajo->\n%s\n",mensaje);
}

void enviameUnArchivo(int sockclifd)
{
  char nombreArchivo[64];
  memset(nombreArchivo,'\0',64);
  char mensaje[LONGMENSAJE];
  memset(mensaje,'\0',LONGMENSAJE);
  strcat(mensaje,"2");
  printf("Ingrese el nombre del archivo--> ");
  scanf("%s",nombreArchivo);
  strcat(mensaje,nombreArchivo);
  strcat(mensaje,"\0");
  send(sockclifd,mensaje,strlen(mensaje),0);
  memset(mensaje,'\0',LONGMENSAJE);
  int nb = recv(sockclifd,mensaje,LONGMENSAJE,0);
  mensaje[nb] = '\0';
  printf("Contenido del Archivo->\n%s\n",mensaje);
}

void cierraConexion(int sockclifd)
{
  char mensaje[LONGMENSAJE];
  memset(mensaje,'\0',LONGMENSAJE);
  strcpy(mensaje,"9\0");
  send(sockclifd,mensaje,strlen(mensaje),0);
  memset(mensaje,'\0',LONGMENSAJE);
  int nb = recv(sockclifd,mensaje,LONGMENSAJE,0);
  mensaje[nb] = '\0';
  printf("Cierre de conexion->\n%s\n",mensaje);
}


int menu(void)
{
  printf("Menu de opciones del Cliente\n");
  printf("1 Enviame el Directorio\n");
  printf("2 Pedir un archivo\n");
  printf("3 \n");
  printf("4 \n");
  printf("9 Cerrar conexion\n");
  printf("_________________\n");
  printf("Ingrese opcion: \n");
  int opcion ;
  scanf("%d",&opcion);
  return opcion;
}