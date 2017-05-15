/*****************************************************************************/
/*                                                                           */
/*                           Fronton0.c                                      */
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
#include "winsuport.h"	/* incloure definicions de funcions propies */


#define MIDA_PALETA 4	/* definicions constants del programa */

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
int f_pil, c_pil;	/* posicio de la pilota, en valor enter */
float pos_f, pos_c;	/* posicio de la pilota, en valor real */
float vel_f, vel_c;	/* velocitat de la pilota, en valor real */
int retard;		/* valor del retard de moviment, en mil.lisegons */

char strin[65];		/* variable per a generar missatges de text */


/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE *fit)
{
  int ret=0;
  
  fscanf(fit,"%d %d %d\n",&n_fil,&n_col,&m_por);	   /* camp de joc */
  fscanf(fit,"%f %f %f %f\n",&pos_f,&pos_c,&vel_f,&vel_c); /* pilota */
  if ((n_fil!=0) || (n_col!=0))			/* si no dimensions maximes */
  {
    if ((n_fil < 7) || (n_fil > 25) || (n_col < 10) || (n_col > 80))
	ret=1;
    else
    if (m_por > n_fil-3)
	ret=2;
    else
    if ((pos_f < 1) || (pos_f > n_fil-2) || (pos_c < 1) || (pos_c > n_col-1))
	ret=3;
  }
  if ((vel_f < -1.0) || (vel_f > 1.0) || (vel_c < -1.0) || (vel_c > 1.0))
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
		fprintf(stderr,"\tpos_f= %.2f \tpos_c= %.2f\n",pos_f,pos_c);
		break;
      case 4:	fprintf(stderr,"\tvelocitat de la pilota incorrecta:\n");
		fprintf(stderr,"\tvel_f= %.2f \tvel_c= %.2f\n",vel_f,vel_c);
		break;
     }
  }
  fclose(fit);
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

  if (pos_f > n_fil-1)
	pos_f = n_fil-1;	/* limita posicio inicial de la pilota */
  if (pos_c > n_col-1)
	pos_c = n_col-1;
  f_pil = pos_f;
  c_pil = pos_c;			 /* dibuixar la pilota inicialment */
  win_escricar(f_pil,c_pil,'1',INVERS);

  sprintf(strin,"Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir\n",
  							TEC_AMUNT,TEC_AVALL);
  win_escristr(strin);
  return(0);
}




/* funcio per moure la pilota: retorna un 1 si la pilota surt per la porteria,*/
/* altrament retorna un 0 */
int mou_pilota(void)
{
  int f_h, c_h, result;
  char rh,rv,rd;

  f_h = pos_f+vel_f;		/* posicio hipotetica de la pilota (entera) */
  c_h = pos_c+vel_c;
  result = 0;			/* inicialment suposem que la pilota no surt */
  rh = rv = rd = ' ';
  if ((f_h != f_pil) || (c_h != c_pil))
  {		/* si posicio hipotetica no coincideix amb la posicio actual */
    if (f_h != f_pil) 		/* provar rebot vertical */
    {	rv = win_quincar(f_h,c_pil);	/* veure si hi ha algun obstacle */
	if (rv != ' ')			/* si hi ha alguna cosa */
	{   vel_f = -vel_f;		/* canvia sentit velocitat vertical */
	    f_h = pos_f+vel_f;		/* actualitza posicio hipotetica */
	}
    }
    if (c_h != c_pil) 		/* provar rebot horitzontal */
    {	rh = win_quincar(f_pil,c_h);	/* veure si hi ha algun obstacle */
	if (rh != ' ')			/* si hi ha algun obstacle */
	{    vel_c = -vel_c;		/* canvia sentit vel. horitzontal */
	     c_h = pos_c+vel_c;		/* actualitza posicio hipotetica */
	}
    }
    if ((f_h != f_pil) && (c_h != c_pil))	/* provar rebot diagonal */
    {	rd = win_quincar(f_h,c_h);
	if (rd != ' ')				/* si hi ha obstacle */
	{    vel_f = -vel_f; vel_c = -vel_c;	/* canvia sentit velocitats */
	     f_h = pos_f+vel_f;
	     c_h = pos_c+vel_c;		/* actualitza posicio entera */
	}
    }
    if (win_quincar(f_h,c_h) == ' ')	/* verificar posicio definitiva */
    {					/* si no hi ha obstacle */
	win_escricar(f_pil,c_pil,' ',NO_INV);  	/* esborra pilota */
	pos_f += vel_f; pos_c += vel_c;
	f_pil = f_h; c_pil = c_h;		/* actualitza posicio actual */
	if (c_pil != 0)		 		/* si ho surt del taulell, */
		win_escricar(f_pil,c_pil,'1',INVERS); /* imprimeix pilota */
	else
		result = 1;	/* codi de finalitzacio de partida */
    }
  }
  else { pos_f += vel_f; pos_c += vel_c; }
  return(result);
}



/* funcio per moure la paleta en segons la tecla premuda */
int mou_paleta(void)
{
  int tecla, result;
  
  result = 0;
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
  return(result);
}



/* programa principal                               */
int main(int n_args, char *ll_args[])
{
  int i, fi1, fi2;
  FILE *fit_conf;

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

  do			/********** bucle principal del joc **********/
  {	fi1 = mou_paleta();
	fi2 = mou_pilota();
	win_retard(retard);		/* retard del joc */
  } while (!fi1 && !fi2);

  win_fi();				/* tanca les curses */
  if (fi2) printf("Final joc perque la pilota ha sortit per la porteria!\n\n");
  else  printf("Final joc perque s'ha premut RETURN!\n\n");

  return(0);			/* retorna sense errors d'execucio */
}
