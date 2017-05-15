/************************************************************************/
/*				multifil.c				*/
/*									*/
/*	Compilar i executar:						*/
/*		$ gcc  multifil.c -o multifil  -lpthread		*/
/*		$ multifil  num_threads  n_vegades  n_lletres		*/
/*									*/
/*	Executa tants threads com indica el parametre num_threads,	*/
/*	on cada thread visualitza un caracter identificatiu		*/
/*	(thread 0 -> 'a', thread 1 -> 'b', ...) tantes vegades com	*/
/*	indiqui el segon parametre n_vegades, esperant un temps		*/
/*	aleatori entre dues visualitzacions; el programa acaba quan	*/
/*	s'han escrit un total de n_lletres entre tots els fils.		*/
/************************************************************************/

#define _REENTRANT
#include <pthread.h>
#include <stdio.h>


#define MAX_THREADS	10
#define MAX_VEGADES	50
#define MAX_LLETRES	100

					/* Variables Globals */
pthread_t tid[MAX_THREADS];	/* taula d'identificadors dels threads */
int lletres;				/* numero de lletres escrites */
int n_veg; 
 
/* escriure una marca corresponent al num. de thread ('a'+pthread_self())  */
/* en intervals aleatoris d’entre 1 i 3 segons, les vegades que indiqui el */
/* parametre (n_v) */
void * caracter(void *id)
{
  int i;

  for (i=0; i <  n_veg; i++)	/* per a totes les vegades (n_v) */
  {
    if (lletres > 0)			/* si falten lletres */
    {
	sleep(1 + rand() % 3);			/* dormir entre 1 i 3 segons */
	printf("%c",'a'+(int)id);	/* escriure marca del thread */
 	lletres--;				/* una lletra menys */
     }
     else pthread_exit((void *) i);	/* sino, forcar sortida thread */
  }
  return((void *) i);	/* retorna el numero de lletres que ha impres el fil */
}


int main(int n_args, char * ll_args[])
{
    int i,n,t,t_total,n_thr;
 
    if (n_args != 4)
    {   fprintf(stderr,"comanda: multifil num_threads n_vegades n_lletres\n");
        exit(1);
    }
    n_thr = atoi(ll_args[1]);		/* convertir arguments a num. enter */
    n_veg = atoi(ll_args[2]);
    lletres = atoi(ll_args[3]);
    if (n_thr < 1) n_thr = 1;		/* i filtrar el seu valor */
    if (n_thr > MAX_THREADS) n_thr = MAX_THREADS;
    if (n_veg < 1) n_veg = 1;
    if (n_veg > MAX_VEGADES) n_veg = MAX_VEGADES;
    if (lletres < 1) lletres = 1;
    if (lletres > MAX_LLETRES) lletres = MAX_LLETRES;

    srand(getpid());		/* inicialitza la "llavor" dels aleatoris */
    setbuf(stdout,NULL);	/* anular buffer de sortida estandard */
    printf("Main thread del proces (%d) : ", getpid());
    n = 0;
    for ( i = 0; i < n_thr; i++)
    {
	if (pthread_create(&tid[n],NULL,caracter,(void *) i+1) == 0)
 		n++;
    }
    printf("he creat %d threads, espero que acabin!\n\n",n);
 
    t_total = 0;
    for ( i = 0; i < n; i++)
    {
	pthread_join(tid[i], (void **)&t);
 	printf("el thread (%d) ha escrit %d lletres\n",i+1,t);
 	t_total += t;
    }
    printf("\nJa han acabat tots els threads creats!\n");
    printf("Entre tots els threads han escrit %d lletres.\n",t_total);
    return(0);
}

