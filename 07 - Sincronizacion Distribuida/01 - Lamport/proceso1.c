#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define IP "192.168.0.215" //Ubicacion
#define PUERTO 8888

void UbicacionDelCliente(struct sockaddr_in);
void RecibeEnviaMensaje(int);
void DameHoraMaquina (char *);
int enviarMensajeB();

int main(int argc, char *argv[])
{
   struct sockaddr_in s_sock,c_sock;
   int idsocks,idsockc;
   int lensock = sizeof(struct sockaddr_in);
   idsocks = socket(AF_INET, SOCK_STREAM, 0);
   printf("idsocks %d\n",idsocks);

   s_sock.sin_family        = AF_INET;
   s_sock.sin_port          = htons(PUERTO);
   s_sock.sin_addr.s_addr   = inet_addr(IP);

   memset(s_sock.sin_zero,0,8);

   printf("bind %d\n", bind(idsocks,(struct sockaddr *) &s_sock,lensock));
   printf("listen %d\n",listen(idsocks,5));

   while(1)
   {
     printf("Esperando conexion\n");
     idsockc = accept(idsocks,(struct sockaddr *)&c_sock,&lensock);

     if(idsockc != -1)
     {
         if (!fork())
             {
               /* Ubicacion del Cliente */
               printf("Conexion aceptada desde el cliente\n");
               //UbicacionDelCliente(c_sock);
               /*--------------------------------------------------*/
               RecibeEnviaMensaje(idsockc);
               close(idsockc);
               exit(0);
             }
     }
     else
     {
        printf("Conexion rechazada %d \n",idsockc);
     }
   }
}

void RecibeEnviaMensaje(int idsockc)
{
    char buf[30];
    int nb;
    int defout ;
    nb = read(idsockc,buf,30);

    buf[nb] = '\0';
    printf("Hora de envio del mensaje A: %s\n" ,buf);

    char hora[128];
    printf("\nHora de recepcion del mensaje A: ");

    sleep(10);

    DameHoraMaquina(hora);

    printf("%s\n", hora);

    int hr, min, seg;
    int hr2, min2, seg2;
    hr = atoi(strtok(buf, ":"));
    min= atoi(strtok(NULL, ":"));
    seg= atoi(strtok(NULL, ":"));

    hr2 = atoi( strtok(hora, ":"));
    min2= atoi(strtok(NULL, ":"));
    seg2= atoi(strtok(NULL, ":"));


    if(hr>hr2)
    {
      hr2=hr+1;
      printf("\nCorregir");
      printf("\n Nueva hora %02d:%02d:%02d \n", hr2 , min2 , seg2 );
    }
    else
      if(hr==hr2)
        {
          if(min>min2)
          {
            min2=min+1;
            printf("\nCorregir");
            printf("\n Nueva hora %02d:%02d:%02d \n", hr2 , min2 , seg2 );
          }
          else
            if(min==min2)
            {
                if(seg>seg2 || seg==seg2)
                {
                  seg2=seg+1;
                  printf("\nCorregir");
                  printf("\n Nueva hora %02d:%02d:%02d \n", hr2 , min2 , seg2 );
                }
            }
        }

     enviarMensajeB();

}


void UbicacionDelCliente(struct sockaddr_in c_sock)
{
  printf("............c_sock.sin_family %d\n",c_sock.sin_family);
  printf("............c_sock.sin_port %d\n",c_sock.sin_port);
  printf("............c_sock.sin_addr.s_addr %s\n\n", inet_ntoa(c_sock.sin_addr));
}

void DameHoraMaquina(char * Hora)
{
  time_t tiempo = time(0);
  struct tm * tlocal= localtime(&tiempo);
  strftime(Hora,128, "%H:%M:%S", tlocal);
}

int enviarMensajeB()
{
   int sd;
   struct sockaddr_in pin;

   pin.sin_family = AF_INET;
   pin.sin_addr.s_addr = inet_addr("192.168.0.215");
   pin.sin_port = htons(9999);
   bzero(&pin.sin_zero, sizeof(pin.sin_zero));

   if ((sd = socket(AF_INET,SOCK_STREAM,0)) == -1)  return -1;
   if (connect(sd,(void *)&pin,sizeof(pin)) == -1)  return -2;

   //Enviando mensaje B
   char bufin[512] ;
   printf("\nHora de envio del mensaje B: \n");
   memset(bufin,0,512);
   DameHoraMaquina(bufin);
   printf("%s\n", bufin);
   send(sd,bufin, strlen(bufin),0) ;
   close(sd);
   return 0 ;

}
