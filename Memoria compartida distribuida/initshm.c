#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv)
{
  int shmid = shmget(0xa,0,0);
  if (shmid < 0)
   printf("Error creacion de shm %d\n",shmid);
  else
    printf("SHMget exito %d\n",shmid);

  int * dir = (int *) shmat(shmid,0,0);
//
//  memset(dir,0,512);
//
  int * diraux = dir ;
  if (dir != NULL)
  {
     int i ;
     for(i = 0 ; i < 512 ; i++)
     {
        *diraux = 0;    
        diraux++;
     }
  }
  shmdt(dir);
  exit(0);
}
