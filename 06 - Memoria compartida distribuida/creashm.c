#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

int main(int argc, char * argv)
{
  int shmid = shmget(0xa,512,IPC_CREAT|IPC_EXCL|0600);
  if (shmid < 0)
   printf("Error creacion de shm %d\n",shmid);
  else
    printf("SHM creada %d\n",shmid);
  exit(0);
}
