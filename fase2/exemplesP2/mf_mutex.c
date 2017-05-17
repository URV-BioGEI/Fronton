/************************************************************************/
/*				mf_mutex.c				*/
/*									*/
/* 	Compilar i executar:					 	*/
/*		$ gcc  mf_mutex.c -o mf_mutex  -lpthread 		*/
/*		$ ./mf_mutex  num_threads  n_vegades  n_lletres		*/
/*									*/
/*	Executa tants threads com indica el parametre num_threads,	*/
/*	on cada thread visualitza un caracter identificatiu	  	*/
/*	(thread 0 -> 'a', thread 1 -> 'b', ...) tantes vegades com	*/
/*	indiqui el segon parametre n_vegades, esperant un temps		*/
/*	aleatori entre dues visualitzacions; el programa acaba quan	*/
/*	s'han escrit un total de n_lletres entre tots els fils.		*/
/*									*/
/*	Es diferencia de l'exemple 'multifil.c' en la inclusio d'un	*/
/*	semafor per fer exclusio mutua a l'hora d'accedir a la  	*/
/*	variable global que computa el numero de lletres a escriure.	*/
/************************************************************************/

#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_THREADS	10
#define MAX_VEGADES	50
#define MAX_LLETRES	100

			/* Variables Globals */
pthread_t tid[MAX_THREADS];	/* taula d'identificadors dels threads */
int lletres;			/* numero de lletres escrites */
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;	/* crea un sem. Global*/


/* escriure una marca corresponent al num. de thread ('a'+pthread_self()) */
/* en intervals aleatoris d'entre 1 i 3 segons, les vegades que indiqui el */
/* parametre (n_v) */
void * caracter(void *n_v)
{
  int i;
 
  for (i=0; i < (int) n_v; i++)		/* per a totes les vegades (n_v) */
  {
    sleep(1 + rand() % 3);		/* dormir entre 1 i 3 segons */
    pthread_mutex_lock(&mutex);		/* tanca semafor */
    if (lletres > 0)			/* si falten lletres */
    {
	printf("%c",'a'+pthread_self());	/* escriure marca del thread */
	lletres--;				/* una lletra menys */
	pthread_mutex_unlock(&mutex); 		/* obre semafor */  
    }
    else
    {
	pthread_mutex_unlock(&mutex); 		/* obre semafor */  
	pthread_exit((void *) i);	   /* sino, forcar sortida thread */
    }
  }
  return((void *) i);	/* retorna el numero de lletres que ha escrit el fil */
}


int main(int n_args, char * ll_args[])
{
    int i,n,t,t_total,n_thr,n_veg;
 
    if (n_args != 4)
    {	fprintf(stderr,"comanda: mf_mutex num_threads n_vegades n_lletres\n");
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
    setbuf(stdout,NULL);		/* anular buffer de sortida estandard */
    printf("Main thread [%d] del proces (%d) : ", pthread_self(),(int)getpid());

    pthread_mutex_init(&mutex, NULL);		/* inicialitza el semafor */
    n = 0;
    for ( i = 0; i < n_thr; i++)
    {
 	if (pthread_create(&tid[n],NULL,caracter,(void *) n_veg) == 0)
 		n++;
    }
    printf("he creat %d threads, espero que acabin!\n\n",n);

    t_total = 0;
    for ( i = 0; i < n; i++)
    {
 	pthread_join(tid[i], (void **)&t);
 	printf("el thread (%d) ha escrit %d lletres\n",tid[i],t);
	t_total += t;
    }
    pthread_mutex_destroy(&mutex);		/* destrueix el semafor */

    printf("\nJa han acabat tots els threads creats!\n");
    printf("Entre tots els threads han escrit %d lletres.\n",t_total);
    return(0);
}

