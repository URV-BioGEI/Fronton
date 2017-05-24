/*****************************************************************************/
/*                                                                           */
/*                           Fronton2.c                                      */
/*                                                                           */
/*  Programa inicial d'exemple per a les practiques 2 i 3 d'ISO.	     */
/*                                                                           */
/*  Compilar i executar:					  	     */
/*     El programa invoca les funcions definides a "winsuport.c", les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':				     */
/*									     */
/*	   $ gcc -c winsuport.c -o winsuport.o				     */
/*	   $ gcc fronton0.c winsuport.o -o fronton0 -lcurses		     */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>	/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>

#include "winsuport2.h"	/* incloure definicions de funcions propies */
#include "memoria.h"
#include "semafor.h"
#include "missatge.h"

#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

#define MIDA_PALETA 4	/* definicions constants del programa */
#define MAX_PILOTES 9
#define MAX_REBOTS 5

			/* variables globals */
char *descripcio[]={
"\n",
"Aquest programa implementa una versio basica del joc del fronto:\n",
"generar un camp de joc rectangular amb una porteria, una paleta que s\'ha\n",
"de moure amb el teclat per a cobrir la porteria, i una pilota que rebota\n",
"contra les parets del camp i contra la paleta. Quan la pilota surt per la\n",
"porteria, el programa acaba la seva execucio. Tambe es pot acabar prement\n",
"la tecla RETURN. El joc consisteix en aguantar la pilota el maxim temps.\n",
"\n",
"  Arguments del programa:\n",
"\n",
"       $ ./fronton0 fitxer_config [retard]\n",
"\n",
"     El primer argument ha de ser el nom d\'un fitxer de text amb la\n",
"     configuracio de la partida, on la primera fila inclou informacio\n",
"     del camp de joc (valors enters), i la segona fila indica posicio\n",
"     i velocitat de la pilota (valors reals):\n",
"          num_files  num_columnes  mida_porteria\n",
"          pos_fila   pos_columna   vel_fila  vel_columna\n",
"\n",
"     on els valors minims i maxims admesos son els seguents:\n",
"          6 < num_files     < 26\n",
"          9 < num_columnes  < 81\n",
"          0 < mida_porteria < num_files-2\n",
"        1.0 <= pos_fila     <= num_files-2\n",
"        1.0 <= pos_columna  <= num_columnes-1\n",
"       -1.0 <= vel_fila     <= 1.0\n",
"       -1.0 <= vel_columna  <= 1.0\n",
"\n",
"     Alternativament, es pot donar el valor 0 a num_files i num_columnes\n",
"     per especificar que es vol que el tauler ocupi tota la pantalla. Si\n",
"     tambe fixem mida_porteria a 0, el programa ajustara la mida d\'aquesta\n",
"     a 3/4 de l\'altura del camp de joc.\n",
"\n",
"     A mes, es pot afegir un segon argument opcional per indicar el\n",
"     retard de moviment del joc en mil.lisegons; el valor minim es 10,\n",
"     el valor maxim es 1000, i el valor per defecte d'aquest parametre\n",
"     es 100 (1 decima de segon).\n",
"\n",
"  Codis de retorn:\n",
"     El programa retorna algun dels seguents codis:\n",
"	0  ==>  funcionament normal\n",
"	1  ==>  numero d'arguments incorrecte\n",
"	2  ==>  no s\'ha pogut obrir el fitxer de configuracio\n",
"	3  ==>  algun parametre del fitxer de configuracio es erroni\n",
"	4  ==>  no s\'ha pogut crear el camp de joc (no pot iniciar CURSES)\n",
"\n",
"   Per a que pugui funcionar aquest programa cal tenir instal.lada la\n",
"   llibreria de CURSES (qualsevol versio).\n",
"\n",
"*"};		/* final de la descripcio */
pid_t tpid[MAX_PILOTES];		/* taula d'identificadors dels processos fill */
int num_pilotes = 1;
int num_rebots;
char caracter_paleta = ' ';
int fi1=0;	/* La condició de fi per rebots máxims la elimino i directament accedim a la variable, així ens estalviem un argument */
int n_fil, n_col;       /* numero de files i columnes del taulell */
int m_por;		/* mida de la porteria (en caracters) */
int f_pal, c_pal;       /* posicio del primer caracter de la paleta */
int f_pil[MAX_PILOTES], c_pil[MAX_PILOTES];	/* posicio de la pilota, en valor enter */
float pos_f[MAX_PILOTES], pos_c[MAX_PILOTES];	/* posicio de la pilota, en valor real */
float vel_f[MAX_PILOTES], vel_c[MAX_PILOTES];	/* velocitat de la pilota, en valor real */
int retard;		/* valor del retard de moviment, en mil.lisegons */
pthread_t idpa;		/* Variable per al thread */
char strin[65];		/* variable per a generar missatges de text */
void *p_win;		/* Variables del tauler */
int id_win;
pid_t tpid[MAX_PILOTES];		/* taula d'id dels processos fill */
int fin2, rebotes, *fi2, *rebots;
int sem_rebots;	    
int sem_fi2;
char Aid_mis[MAX_PILOTES][10];
int id_mis[MAX_PILOTES];
char Anum_pil[25], Aretard[25], Afin2[25], Arebotes[25], An_col[25], An_fil[25], Aid_win[25], Asem_fi2[25], Asem_rebots[25], Anum_pilotes[25];

char Aid_mis1[25], Aid_mis2[25], Aid_mis3[25], Aid_mis4[25], Aid_mis5[25], Aid_mis6[25], Aid_mis7[25], Aid_mis8[25], Aid_mis9[25];
/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE *fit)
{
  int ret=0;
  int p_count = 0;
  fscanf(fit,"%d %d %d \n",&n_fil,&n_col,&m_por);	   /* camp de joc */
  while (!feof(fit))/* pilota */
  {
      fscanf(fit,"%f %f %f %f\n",&pos_f[p_count],&pos_c[p_count],&vel_f[p_count],&vel_c[p_count]);
    if ((n_fil!=0) || (n_col!=0))			/* si no dimensions maximes */
    {
      if ((n_fil < 7) || (n_fil > 25) || (n_col < 10) || (n_col > 80))
  	ret=1;
      else
      if (m_por > n_fil-3)
  	ret=2;
      else
      if ((pos_f[p_count] < 1) || (pos_f[p_count] > n_fil-2) || (pos_c[p_count] < 1) || (pos_c[p_count] > n_col-1))
  	ret=3;
    }
    if ((vel_f[p_count] < -1.0) || (vel_f[p_count] > 1.0) || (vel_c[p_count] < -1.0) || (vel_c[p_count] > 1.0))
    	ret=4;

    if (ret!=0)		/* si ha detectat algun error */
    {
      fprintf(stderr,"Error en fitxer de configuracio:\n");
      switch (ret)
      {
        case 1:	fprintf(stderr,"\tdimensions del camp de joc incorrectes:\n");
  		fprintf(stderr,"\tn_fil= %d \tn_col= %d\n",n_fil,n_col);
  		break;
        case 2:	fprintf(stderr,"\tmida de la porteria incorrecta:\n");
  		fprintf(stderr,"\tm_por= %d\n",m_por);
  		break;
        case 3:	fprintf(stderr,"\tposicio de la pilota incorrecta:\n");
  		fprintf(stderr,"\tpos_f= %.2f \tpos_c= %.2f\n",pos_f[p_count],pos_c[p_count]);
  		break;
        case 4:	fprintf(stderr,"\tvelocitat de la pilota incorrecta:\n");
  		fprintf(stderr,"\tvel_f= %.2f \tvel_c= %.2f\n",vel_f[p_count],vel_c[p_count]);
  		break;
       }
       return(ret);
    }
    p_count++;
  }
  fclose(fit);
  num_pilotes = p_count;
  return(ret);
}


/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
/* retorna diferent de zero si hi ha algun problema */
int inicialitza_joc(void)
{
  int i, retwin;
  int i_port, f_port;		/* inici i final de porteria */

  retwin = win_ini(&n_fil,&n_col,'+',INVERS);	/* intenta crear taulell */

  if (retwin < 0)	/* si no pot crear l'entorn de joc amb les curses */
  { fprintf(stderr,"Error en la creacio del taulell de joc:\t");
    switch (retwin)
    {	case -1: fprintf(stderr,"camp de joc ja creat!\n");
    		 break;
	case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n");
		 break;
	case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n");
		 break;
	case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n");
		 break;
     }
     return(retwin);
  }

	/* VARIABLES QUE S'HAN DE PASSAR PER PARÁMETRE
	fi2, num_rebots 			*/

	fin2 = ini_mem(sizeof(int));	/* Inicialitzem memoria compartida */
	fi2 = map_mem(fin2);	/* obtenir adres. de mem. compartida */
	*fi2 = 0;	/* inicialitza variable compartida */
	sprintf (Afin2, "%i", fin2);

	rebotes = ini_mem(sizeof(int)); 	/* Inicialitzem memoria compartida */
	rebots = map_mem(rebotes);	/* obtenir adres. de mem. compartida */
	*rebots = MAX_REBOTS;	/* inicialitza variable compartida */
	sprintf (Arebotes, "%i", rebotes);
	
	/* Afegit!!! */
	id_win = ini_mem(retwin);	/* crear zona mem. compartida */
	p_win = map_mem(id_win);	/* obtenir adres. de mem. compartida */
	win_set(p_win,n_fil,n_col);		/* crea acces a finestra oberta */
	
	/* FASE 4 semaforo, es un semaforo*/
	sem_rebots = ini_sem(1);	    /* crear semafor IPC inicialment obert */
	sem_fi2 = ini_sem(1);
	sprintf(Asem_rebots,"%i",sem_rebots);	    /* convertir identificador sem. en string */
	sprintf(Asem_fi2,"%i",sem_fi2);	    /* convertir identificador sem. en string */
	
	/* Bústies */

	for (i = 0; i < num_pilotes ; i++)
	{
		id_mis[i] = ini_mis();	    /* crear una bustia IPC per cada pilota*/
		//fprintf(stderr, "\nPrimer: %i", id_mis[i]);
	}
	/* Ho volia fer passant una matriu però no m'he n'he sortit així que he tirat pel dret */
	sprintf (Aid_mis1, "%i", id_mis[0]);
	sprintf (Aid_mis2, "%i", id_mis[1]);
	sprintf (Aid_mis3, "%i", id_mis[2]);
	sprintf (Aid_mis4, "%i", id_mis[3]);
	sprintf (Aid_mis5, "%i", id_mis[4]);
	sprintf (Aid_mis6, "%i", id_mis[5]);
	sprintf (Aid_mis7, "%i", id_mis[6]);
	sprintf (Aid_mis8, "%i", id_mis[7]);
	sprintf (Aid_mis9, "%i", id_mis[8]);	

  if (m_por > n_fil-2)
	m_por = n_fil-2;	/* limita valor de la porteria */
  if (m_por == 0)
  	m_por = 3*(n_fil-2)/4;		/* valor porteria per defecte */

  i_port = n_fil/2 - m_por/2 -1;	/* crea el forat de la porteria */
  f_port = i_port + m_por -1;
  for (i = i_port; i <= f_port; i++)
	win_escricar(i,0,' ',NO_INV);

  n_fil = n_fil-1;		/* descompta la fila de missatges */

  f_pal = 1;			/* posicio inicial de la paleta per defecte */
  c_pal = 3;
  for (i=0; i< MIDA_PALETA; i++){       /* dibuixar paleta inicialment */
	win_escricar(f_pal+i,c_pal,'0',INVERS);
	caracter_paleta = win_quincar(f_pal+i, c_pal);
    }
  for(i = 0; i < num_pilotes; i++){
	  if (pos_f[i] > n_fil-1)
		pos_f[i] = n_fil-1;	/* limita posicio inicial de la pilota */
	  if (pos_c[i] > n_col-1)
		pos_c[i] = n_col-1;
	  f_pil[i] = pos_f[i];
	  c_pil[i] = pos_c[i];			 /* dibuixar la pilota inicialment */
	  win_escricar(f_pil[i],c_pil[i],'1'+i,INVERS);
  }

  sprintf(strin,"Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir\n",
  							TEC_AMUNT,TEC_AVALL);
  win_escristr(strin);
  win_update();
  return(0);
}

/* funcio per moure la paleta en segons la tecla premuda */
void * mou_paleta(void * nul)
{
    int tecla, result;
    result = 0;
    while (result!=1)
    {
	waitS(sem_rebots);
        tecla = win_gettec();
        if (tecla != 0)
        {
            if ((tecla == TEC_AVALL) && ((f_pal+MIDA_PALETA)< n_fil-1))
            {
                win_escricar(f_pal,c_pal,' ',NO_INV);	/* esborra primer bloc */
                f_pal++;				/* actualitza posicio */
                win_escricar(f_pal+MIDA_PALETA-1,c_pal,'0',INVERS); /*esc. ultim bloc*/
            }
            if ((tecla == TEC_AMUNT) && (f_pal> 1))
            {
                win_escricar(f_pal+MIDA_PALETA-1,c_pal,' ',NO_INV); /*esborra ultim bloc*/
                f_pal--;				/* actualitza posicio */
                win_escricar(f_pal,c_pal,'0',INVERS);	/* escriure primer bloc */
            }
            if (tecla == TEC_RETURN) result=1;		/* final per pulsacio RETURN */
        }
	signalS(sem_rebots); 
        win_retard(retard);
    }
fi1=1;
return((void *) 0);
}



/* programa principal                               */
int main(int n_args, char *ll_args[])
{
  int i, j;
  FILE *fit_conf;
char Avel_f[25], Avel_c[25], Af_pil[25], Ac_pil[25], Apos_f[25], Apos_c[25];
if ((n_args != 2) && (n_args !=3))	/* si numero d'arguments incorrecte */
  { i=0;
  do fprintf(stderr,"%s",descripcio[i++]);	/* imprimeix descripcio */
    while (descripcio[i][0] != '*');		/* mentre no arribi al final */
    exit(1);
  }

  fit_conf = fopen(ll_args[1],"rt");		/* intenta obrir el fitxer */
  if (!fit_conf)
  {  fprintf(stderr,"Error: no s'ha pogut obrir el fitxer \'%s\'\n",ll_args[1]);
     exit(2);
  }

  if (carrega_configuracio(fit_conf) !=0)	/* llegir dades del fitxer  */
     exit(3);	/* aborta si hi ha algun problema en el fitxer */

  if (n_args == 3)		/* si s'ha especificat parametre de retard */
  {	retard = atoi(ll_args[2]);	/* convertir-lo a enter */
  	if (retard < 10) retard = 10;	/* verificar limits */
  	if (retard > 1000) retard = 1000;
  }
  else retard = 100;		/* altrament, fixar retard per defecte */

  printf("Joc del Fronto: prem RETURN per continuar:\n"); getchar();

  if (inicialitza_joc() !=0)	/* intenta crear el taulell de joc */
	exit(4);	/* aborta si hi ha algun problema amb taulell */

	//fprintf(stderr, "main_errors\n");
	pthread_create(&idpa,NULL,mou_paleta, (void *)1);
	sprintf (Aid_win, "%i", id_win);
			//printf ("%i, %f, %f, %i, %i, %f, %f, %i, %f, %f", j, vel_f[j], vel_c[j], f_pil[j], c_pil[j], pos_f[j], pos_c[j]);

	for( j = 0; j < num_pilotes; j++)
	{
	    tpid[j] = fork();
		if (tpid[j] == 0)		/* Procés fill */
		{			/* Variables enviades per valor*/
				
			sprintf (Anum_pil, "%i", (j+1));	/* Li passem l'índex +1 perque comencem comptant desde 0 */
			sprintf (Avel_f, "%f", vel_f[j]);
			sprintf (Avel_c, "%f", vel_c[j]);
			sprintf (Af_pil, "%i", f_pil[j]);
			sprintf (Ac_pil, "%i", c_pil[j]);
			sprintf (Apos_f, "%f", pos_f[j]);
			sprintf (Apos_c, "%f", pos_c[j]);
			sprintf (An_col, "%i", n_col);
			sprintf (An_fil, "%i", n_fil);
			sprintf (Aretard, "%i", retard);
			sprintf (Anum_pilotes, "%i", num_pilotes);
            //fprintf(stderr, "\nPilota %s (abans de crear procés fill):\n%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s", Anum_pil, Avel_f, Avel_c, Af_pil, Ac_pil, Apos_f, Apos_c, An_col, An_fil, Aretard, Arebotes, Afin2, Aid_win);
			execlp("./pilota4", "pilota4", Anum_pil, Avel_f, Avel_c, Af_pil, Ac_pil, Apos_f, Apos_c, An_col, An_fil, Aretard, Arebotes, Afin2, Aid_win, Asem_rebots, Asem_fi2, Anum_pilotes, Aid_mis1, Aid_mis2, Aid_mis3, Aid_mis4, Aid_mis5, Aid_mis6, Aid_mis7, Aid_mis8, Aid_mis9, (char *)0);
			printf("error: No s'ha pogut executar el process fill %i \n", j);
			exit(1);	/* Retornem error */
			/*codi fill */
		}
		else if (tpid[j] <  0 )			/* ERROR*/
		{
			fprintf(stderr, "Error en la creació de processos");
		}

	}
do		/********** bucle principal del joc **********/
{
    win_retard(retard);		/* retard del joc */
    waitS(sem_rebots);
    win_update();			/* actualitza visualitzacio CURSES */
    signalS(sem_rebots);
} while (!fi1 && !(*fi2) && ((*rebots)!=0));
/* Destrucció de recursos */
//elim_sem(sem_fi2);
//elim_sem(sem_rebots);
win_fi();				/* tanca les curses */
if (*fi2) printf("Final joc perque la pilota ha sortit per la porteria!\n\n");
if (fi1)  printf("Final joc perque s'ha premut RETURN!\n\n");
if (*rebots==0) {printf("Has fet tots els rebots! Has guanyat!!\n");}
else {printf("faltaven %d rebots\n", *rebots);}
/* waitS(sem_fi2);		 Enviem senyal de fi per als processos fills posant la variable fi2 a 1
*fi2 = 1;
signalS(sem_fi2); */
return(0);			/* retorna sense errors d'execucio */

}


