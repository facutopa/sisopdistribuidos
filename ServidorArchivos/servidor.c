#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 8888

void atender(void *) ;

int main(int argc, char * argv[])
{
    if (argc != 2 )
     {
       printf("Linea de entrada error\n");
       exit(-1) ;
     }
     socklen_t addrlen ;
     int sockfd ;
     struct sockaddr_in addr_in , addrcli_in ;
     sockfd = socket(AF_INET, SOCK_STREAM , 0);
     printf("socket = %d\n",sockfd);

     //completar la estrutura addr_in
     addr_in.sin_family = AF_INET ;
     addr_in.sin_port   = htons(PUERTO) ;
     addr_in.sin_addr.s_addr  = inet_addr(argv[1]) ;
     memset(addr_in.sin_zero,0,8) ;

     addrlen = sizeof(addr_in);
     int bn = bind(sockfd, (struct sockaddr *)&addr_in , addrlen);
     printf("bind = %d\n",bn);

     int lst = listen(sockfd,5) ;
     printf("listen = %d\n",lst);

     while(1)
     {
       printf("servidor en puerto %d esperando conexion del cliente\n",PUERTO);
       int sockclifd = accept(sockfd, (struct sockaddr *)&addrcli_in, &addrlen);
       if ( sockclifd > 0)
         {
            pthread_t hilo ;
       pthread_create(&hilo,NULL,(void*)&atender,(void*)&sockclifd);
         }
     }
}


void atender(void * sockclifd)
{
   printf("sockclifd = %d\n",*((int*)sockclifd));
   int * sockcli = (int *) sockclifd ;
   char buffer[1024];
   memset(buffer,'\0',1024);

   DIR * dir = opendir(".");
   if (dir != NULL )
   {
      struct dirent * midirent ;
      struct stat mistat ;
      while((midirent = readdir(dir)) != NULL)
      {
        printf("___Registro del directorio archivo_______\n");
        printf("inodo %d\n",midirent->d_ino);   /* Inode number */
        printf("largo registro %d\n",midirent->d_reclen); /*Length of this record */
        printf("type %d\n",midirent->d_type);      /* Type of file; not supported */
        printf("nombre %s\n",midirent->d_name);    /* Null-terminated filename */
        printf("__________________________________\n");
        if (stat(midirent->d_name, &mistat) != -1)
        {
          printf("     __informacion del archivo___\n");
          printf("     inode %d\n",mistat.st_ino); /* Inode number */
          printf("     mode %d\n",mistat.st_mode);/* File type and mode */
          printf("     links %d\n",mistat.st_nlink);/* Number of hard links */
          printf("     uid %d\n",mistat.st_uid);  /* User ID of owner */
          printf("     gid %d\n",mistat.st_gid);  /* Group ID of owner */
          printf("     tamaño %d\n",mistat.st_size); /* Total size, in bytes */
          printf("     __________________________________\n");
        }
        char sizeArchivo[16];
        memset(sizeArchivo,'\0',16);
        strcat(buffer,midirent->d_name);
        strcat(buffer," ");
        sprintf(sizeArchivo,"%d",mistat.st_size);
        strcat(buffer,sizeArchivo);
        strcat(buffer,"\n");
      }
      strcat(buffer,"\0");
   }
   closedir(dir);
   send(*sockcli, buffer, strlen(buffer),0);

   memset(buffer,'\0',1024);
   int nb = recv(*sockcli, buffer, 1024, 0);
   buffer[nb] = '\0' ;
   printf("cliente dice-->%s\n",buffer);
   close(*sockcli);
   pthread_exit(NULL);
}
