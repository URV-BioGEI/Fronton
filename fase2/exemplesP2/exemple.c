#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "memoria.h"

int a=0;
int id_b;
int *pb;

main()
{
  int t,fi;
  a=10;

  id_b = ini_mem(sizeof(int));
  assert(id_b);
  pb = map_mem(id_b);
  assert(pb);

  *pb=30;
  
  printf("Soc %d i a val %d id_b:%d\n",getpid(),a,id_b);

  t=fork();
  if (t==0) 
   {
    a=20;
    *pb=40;
    printf("Soc %d i a val %d\n",getpid(),a);
    execlp("/bin/cat","cat","exemple.c",0);
    printf("Soc %d despres de l'execlp \n",getpid());
    exit(0);
   }
  else if (t>0) 
   {
    a=15;
    printf("Pare %d, crea a %d i a val %d b:%d\n",getpid(),t,a,*pb);
    waitpid(t,&fi,NULL);
    printf("Pare %d, ha esperat a %d i torna %d i b:%d\n",getpid(),t,fi,*pb);
    elim_mem(id_b);
    exit(0);
   }
  else if (t<0)
    perror("No s'ha creat fill\n");
}
