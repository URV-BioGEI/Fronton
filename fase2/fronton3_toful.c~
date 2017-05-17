/*****************************************************************************/
/*                                                                           */
/*                           Fronton3.c                                      */
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
/*	   $ gcc -c winsuport2.c -o winsuport2.o	             	     */
/*	   $ gcc fronton0.c winsuport.o -o fronton0 -lcurses		     */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>	/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport2.h"	/* incloure definicions de funcions propies */
#include <pthread.h>

#include "memoria.h"
#include <sys/wait.h>
#include <unistd.h>

#define MIDA_PALETA 4	/* definicions constants del programa */
#define MAX_REBOTS 10
#define PILOTES 9

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


int n_fil, n_col;       /* numero de files i columnes del taulell */
int m_por;		/* mida de la porteria (en caracters) */
int f_pal, c_pal;       /* posicio del primer caracter de la paleta */
int Gf_pil[PILOTES], Gc_pil[PILOTES];	/* posicio de la pilota, en valor enter */
float Gpos_f[PILOTES], Gpos_c[PILOTES];	/* posicio de la pilota, en valor real */
float Gvel_f[PILOTES], Gvel_c[PILOTES];	/* velocitat de la pilota, en valor real */
int retard;		/* valor del retard de moviment, en mil.lisegons */

void *p_win;		/*pos mem compartida taulell*/
int id_win;
char num_pilota[20], mida_camp[20],files_camp[10],col_camp[10],id_fi1c[10],id_fi2c[10],id_fiRebotsc[10];
char max_rebots[20];
pid_t tpid[PILOTES];		/* taula d'identificadors dels processos fill */

int pilotes;

char strin[65];		/* variable per a generar missatges de text */

int id_fi1, *fi1, id_fi2, *fi2, id_fiRebots, *fiRebots;		/* convertim les condicions de fi en globals*/

pthread_t tidPaleta;	/*adreça id del thread encrregat del moviment de la paleta*/

//pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;   /* crea un sem. Global*/

/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE *fit)
{
  int i=0, ret=0;
  
  fscanf(fit,"%d %d %d\n",&n_fil,&n_col,&m_por);	   /* camp de joc */
  while(!feof(fit)){
  //for(i=0; i<PILOTES; i++){
	fscanf(fit,"%f %f %f %f\n",&Gpos_f[i],&Gpos_c[i],&Gvel_f[i],&Gvel_c[i]); /* pilota */
	if ((n_fil!=0) || (n_col!=0))			/* si no dimensions maximes */
	{
		if ((n_fil < 7) || (n_fil > 25) || (n_col < 10) || (n_col > 80))
			ret=1;
		else
			if (m_por > n_fil-3)
				ret=2;
			else
				if ((Gpos_f[i] < 1) || (Gpos_f[i] > n_fil-2) || (Gpos_c[i] < 1) || (Gpos_c[i] > n_col-1))
					ret=3;
	}
	if ((Gvel_f[i] < -1.0) || (Gvel_f[i] > 1.0) || (Gvel_c[i] < -1.0) || (Gvel_c[i] > 1.0))	
		ret=4;
	i++;
  }
  pilotes=i;

  if (ret!=0)		
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
		//fprintf(stderr,"\tpos_f= %.2f \tpos_c= %.2f\n",pos_f,pos_c);
		break;
      case 4:	fprintf(stderr,"\tvelocitat de la pilota incorrecta:\n");
		//fprintf(stderr,"\tvel_f= %.2f \tvel_c= %.2f\n",vel_f,vel_c);
		break;
   }
  }
  fclose(fit);
  return(0);
}


/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
/* retorna diferent de zero si hi ha algun problema */
int inicialitza_joc(void)
{
  int i, tamanycamp;
  int i_port, f_port;		/* inici i final de porteria */

  tamanycamp = win_ini(&n_fil,&n_col,'+',INVERS);	/* intenta crear taulell --> retorna tamany*/
  
  if (tamanycamp < 0)	/* si no pot crear l'entorn de joc amb les curses */
  { fprintf(stderr,"Error en la creacio del taulell de joc:\t");
    switch (tamanycamp)
    {	case -1: fprintf(stderr,"camp de joc ja creat!\n");
    		 break;
	case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n");
		 break;
	case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n");
		 break;
	case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n");
		 break;
     }
     return(tamanycamp);
  }

/////////////////////////////////////////////////////////////////////////////////////////////taulell a memòria
  id_win = ini_mem(tamanycamp);			/* crear zona mem. compartida */
  p_win = map_mem(id_win);			/* obtenir adres. de mem. compartida */
  
  sprintf(mida_camp,"%i",id_win);
  sprintf(files_camp,"%i",n_fil);		/* convertir mides camp en string */
  sprintf(col_camp,"%i",n_col);
  
  win_set(p_win,n_fil,n_col);			/* crea acces a finestra oberta */
/////////////////////////////////////////////////////////////////////////////////////////////
  id_fi1 = ini_mem(sizeof(int));	/* crear zona mem. compartida */
  fi1 = map_mem(id_fi1);		/* obtenir adres. de mem. compartida */
  *fi1 = 0;				/* inicialitza variable compartida */
  sprintf(id_fi1c,"%i",id_fi1);

  id_fi2 = ini_mem(sizeof(int));	/* crear zona mem. compartida */
  fi2 = map_mem(id_fi2);		/* obtenir adres. de mem. compartida */
  *fi2 = 0;
  sprintf(id_fi2c,"%i",id_fi2);

  id_fiRebots = ini_mem(sizeof(int));	/* crear zona mem. compartida */
  fiRebots = map_mem(id_fiRebots);		/* obtenir adres. de mem. compartida */
  *fiRebots = 0;
  sprintf(id_fiRebotsc,"%i",id_fiRebots);
/////////////////////////////////////////////////////////////////////////////////////////////

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
  for (i=0; i< MIDA_PALETA; i++)       /* dibuixar paleta inicialment */
	win_escricar(f_pal+i,c_pal,'0',INVERS);

  for(i=0; i<pilotes; i++){
	  if (Gpos_f[i] > n_fil-1)
		Gpos_f[i] = n_fil-1;	/* limita posicio inicial de la pilota */
	  if (Gpos_c[i] > n_col-1)
		Gpos_c[i] = n_col-1;
	  Gf_pil[i] = Gpos_f[i];
	  Gc_pil[i] = Gpos_c[i];			 /* dibuixar la pilota inicialment */
	  win_escricar(Gf_pil[i],Gc_pil[i],'1'+i,INVERS);
  }


  sprintf(strin,"Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir\n",
  							TEC_AMUNT,TEC_AVALL);
  win_escristr(strin);
  win_update();
  return(0);
}


/* funcio per moure la paleta en segons la tecla premuda */
void * mou_paleta(void * null)
{
  int tecla;

do
{
  //pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
  tecla = win_gettec();
  //pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/
  if (tecla != 0)
  {
    //pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
    if ((tecla == TEC_AVALL) && ((f_pal+MIDA_PALETA)< n_fil-1) && (win_quincar(f_pal+MIDA_PALETA,c_pal)==' '))
    {	
	win_escricar(f_pal,c_pal,' ',NO_INV);	/* esborra primer bloc */
	f_pal++;				/* actualitza posicio */
	win_escricar(f_pal+MIDA_PALETA-1,c_pal,'0',INVERS); /*esc. ultim bloc*/
    }
    //pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/

    //pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/
    if ((tecla == TEC_AMUNT) && (f_pal> 1) && (win_quincar(f_pal-1,c_pal)==' '))
    {
	win_escricar(f_pal+MIDA_PALETA-1,c_pal,' ',NO_INV); /*esborra ultim bloc*/
	f_pal--;				/* actualitza posicio */
	win_escricar(f_pal,c_pal,'0',INVERS);	/* escriure primer bloc */
    }
    //pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/

    if (tecla == TEC_RETURN) (*fi1)=1;		/* final per pulsacio RETURN */
  }
  win_retard(retard);
}while (!(*fi1) && !(*fi2) && (*fiRebots)<MAX_REBOTS);
return 0;
}



/* programa principal                               */
int main(int n_args, char *ll_args[])
{
  int i;
  FILE *fit_conf;
  char pos_f[20], pos_c[20], vel_f[20], vel_c[20],f_pil[10],c_pil[10],retards[10];

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

  /********** bucle principal del joc **********/
  
  //pthread_mutex_init(&mutex, NULL);           /* inicialitza el semafor */

  pthread_create(&tidPaleta, NULL, mou_paleta, 1);
  int n=0;
  sprintf(max_rebots,"%i",MAX_REBOTS);
  for ( i = 0; i < pilotes; i++)
  {
    tpid[n] = fork();		/* crea un nou proces */
    if (tpid[n] == (pid_t) 0)		/* branca del fill */
    {
	sprintf(num_pilota,"%i",(i/*+1*/));
	sprintf(pos_f,"%f",Gpos_f[i]);
	sprintf(pos_c,"%f",Gpos_c[i]);
	sprintf(vel_f,"%f",Gvel_f[i]);
	sprintf(vel_c,"%f",Gvel_c[i]);
	sprintf(f_pil,"%i",Gf_pil[i]);
	sprintf(c_pil,"%i",Gc_pil[i]);
	sprintf(retards,"%i",retard);
	//fprintf(stderr, "pilota%s\n", num_pilota);
	execlp("./pilota3", "pilota3", num_pilota, mida_camp, files_camp, col_camp, retards, pos_f, pos_c, vel_f, vel_c, f_pil, c_pil, id_fi1c, id_fi2c, id_fiRebotsc, max_rebots, (char *)0);
	fprintf(stderr,"error: no puc executar el process fill \'pilota3\'\n");
	exit(0);
    }
    else if (tpid[n] < 0)	fprintf(stderr,"error: no s'ha pogut crear el process fill \'pilota3\'\n");
  }
  printf("S'han creat %d processos\n", i);

  do	
  {
	//for(i=0; i<pilotes; i++){
		//waitpid(tpid[i], fi2, NULL);		/* espera finalitzacio d'un fill */
		win_retard(retard);		/* retard del joc */
		win_update();
	//}
  } while (!(*fi1) && !(*fi2) && (*fiRebots)<MAX_REBOTS);
	
  //pthread_mutex_destroy(&mutex);              /* destrueix el semafor */

  win_fi();				/* tanca les curses */
  if (*fi2) printf("Final joc perque la pilota ha sortit per la porteria!\n\n");
  else  if (*fi1) printf("Final joc perque s'ha premut RETURN!\n\n");
  printf("Nombre de rebots: %i de %i\n\n", *fiRebots, MAX_REBOTS);

  elim_mem(id_win);
  return(0);			/* retorna sense errors d'execucio */
}
