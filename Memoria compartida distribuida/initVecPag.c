#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

#define TAMPAGINA 32

int main(int argc, char * argv)
{

  int * vecPuntPag[512/TAMPAGINA];

  int shmid = shmget(0xa,0,0);
  if (shmid < 0)
   printf("Error creacion de shm %d\n",shmid);
  else
    printf("SHMget exito %d\n",shmid);

  int * dir = (int *) shmat(shmid,0,0);
  int * diraux = dir ;
  int i ;
  for (i = 0 ; i < 512/TAMPAGINA ; i++)
     vecPuntPag[i] = diraux+(i*8) ;
  
  for (i = 0 ; i < 512/TAMPAGINA ; i++)
     printf("vecPuntPag[%d] = %p\n",i,vecPuntPag[i]) ;

//
  int * dirvec = vecPuntPag[0] ;
  for( i = 0 ; i < 8 ; i++)
  {
    *dirvec = i ;
     dirvec++ ;
  }

  dirvec = vecPuntPag[0] ;
  for( i = 0 ; i < 8 ; i++)
  {
    printf("%p -> %d\n",dirvec,*dirvec) ;
     dirvec++ ;
  }

  shmdt(dir);
  exit(0);
}
