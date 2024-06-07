
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

#define IP "192.168.0.209" //Ubicacion del servidor
#define PUERTO 8888

void UbicacionDelCliente(struct sockaddr_in);
void RecibeEnviaMensaje(int);
void DameHoraMaquina (char *);



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
     printf("esperando conexion\n");
     idsockc = accept(idsocks,(struct sockaddr *)&c_sock,&lensock);

     if(idsockc != -1)
     {
         if (!fork()) 
             {  
               /* Ubicacion del Cliente */    
               printf("conexion aceptada desde el cliente\n");
               //UbicacionDelCliente(c_sock);
               /*--------------------------------------------------*/  
               //Recibiendo tamaño del proceso en bytes
		
		
		    char buf[30];
		    int nb;
		    int defout ;
		    nb = read(idsockc,buf,30);
		    
		    buf[nb] = '\0';
		    printf("Hora de envio del mensaje B: %s\n" ,buf);
		    
		    char hora[128];
		    printf("\nHora de recepcion del mensaje B: ");
			
		    DameHoraMaquina(hora);
		    	
		    printf("%s\n", hora);

		    int hr, min, seg;
		    int hr2, min2, seg2;	
		    hr = atoi( strtok(buf, ":"));		
		    min= atoi(strtok(NULL, ":"));
		    seg= atoi(strtok(NULL, ":"));

		    hr2 = atoi( strtok(hora, ":"));		
		    min2= atoi(strtok(NULL, ":"));
		    seg2= atoi(strtok(NULL, ":"));

		    
		    if(hr>hr2)
		    {
		      hr2=hr+1;	
		      printf("\nCorregir");
		      printf("\n Nueva hora %d:%d: %d", hr2 , min2 , seg2 );
		    }
		    else
		      if(hr==hr2)
		       	{
		 	  if(min>min2)
			  {
			    min2=min+1;
			    printf("\nCorregir");
			    printf("\n Nueva hora %d:%d: %d", hr2 , min2 , seg2 );			
		    	  }
		 	  else
			    if(min==min2)	
		    	    {
				if(seg>seg2 || seg==seg2)
				{
				  seg2=seg+1;
				  printf("\nCorregir");
				  printf("\n Nueva hora %d:%d: %d", hr2 , min2 , seg2 );
				}
			    }
			}
             }   
     }
     else
     {
        printf("Servidor dice conexion rechazada %d \n",idsockc);
     }
   }
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
