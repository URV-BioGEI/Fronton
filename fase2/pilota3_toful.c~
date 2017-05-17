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
/*	   $ gcc -c winsuport2.c -o winsuport2.o				*/
/*	   $ gcc fronton0.c winsuport.o -o fronton0 -lcurses		     */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>	/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport2.h"	/* incloure definicions de funcions propies */
#include "memoria.h"

/* programa principal                               */
int main(int n_args, char *ll_args[])
{
	int f_h, c_h;
	char rh,rv,rd;
	int index = atoi(ll_args[1]);
	int f_pil, c_pil;
	float pos_f, pos_c;
	float vel_f, vel_c;
	void *p_win;
	int mida_camp, n_fil, n_col;
	int retard;							/* valor del retard de moviment, en mil.lisegons */
	int id_fi1, *fi1, id_fi2, *fi2, id_fiRebots, *fiRebots;		/* convertim les condicions de fi en globals*/
	int MAX_REBOTS= atoi(ll_args[15]);


//fprintf(stderr,"\n%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s\n",(ll_args[1]),(ll_args[2]),(ll_args[3]),(ll_args[4]),(ll_args[5]),(ll_args[6]),(ll_args[7]),(ll_args[8]),(ll_args[9]),(ll_args[10]),(ll_args[11]));

	pos_f= atof(ll_args[6]);  
	pos_c= atof(ll_args[7]);
	vel_f= atof(ll_args[8]);
	vel_c= atof(ll_args[9]);
	f_pil = atoi(ll_args[10]);
	c_pil = atoi(ll_args[11]);
	id_fi1 = atoi(ll_args[12]);
	id_fi2 = atoi(ll_args[13]);
	id_fiRebots = atoi(ll_args[14]);

	fi1 = map_mem(id_fi1);		/* obtenir adres. de mem. compartida */
	fi2 = map_mem(id_fi2);		/* obtenir adres. de mem. compartida */
	fiRebots = map_mem(id_fiRebots);		/* obtenir adres. de mem. compartida */

	mida_camp = atoi(ll_args[2]);
	p_win = map_mem(mida_camp);		/*direcció mem compartida*/

	n_fil = atoi(ll_args[3]);		/* obtenir dimensions del camp de joc */
	n_col = atoi(ll_args[4]);

	win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */

	retard = atoi(ll_args[5]);

	do
	{
		f_h = pos_f+vel_f;		/* posicio hipotetica de la pilota (entera) */
		c_h = pos_c+vel_c;
		rh = rv = rd = ' ';
		if ((f_h != f_pil) || (c_h != c_pil))
		{		/* si posicio hipotetica no coincideix amb la posicio actual */
			if (f_h != f_pil) 		/* provar rebot vertical */
			{	
				//pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
				rv = win_quincar(f_h,c_pil);	/* veure si hi ha algun obstacle */
				//pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/
				if (rv != ' ')			/* si hi ha alguna cosa */
				{   vel_f = -vel_f;		/* canvia sentit velocitat vertical */
				    f_h = pos_f+vel_f;		/* actualitza posicio hipotetica */
				    if (rv == '0')
				    { 
					//pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
					(*fiRebots)++;
					//pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/
	    			    }			
				}
    			}
    			if (c_h != c_pil) 		/* provar rebot horitzontal */
    			{	
				//pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
				rh = win_quincar(f_pil,c_h);	/* veure si hi ha algun obstacle */
				//pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/	
				if (rh != ' ')			/* si hi ha algun obstacle */
				{    vel_c = -vel_c;		/* canvia sentit vel. horitzontal */
	     			     c_h = pos_c+vel_c;		/* actualitza posicio hipotetica */
	     			     if (rh == '0')
				     { 
					//pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
					(*fiRebots)++;
					//pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/
				     }
				}
    			}
    			if ((f_h != f_pil) && (c_h != c_pil))	/* provar rebot diagonal */
    			{	
				////pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
				rd = win_quincar(f_h,c_h);
				//pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/	
				if (rd != ' ')				/* si hi ha obstacle */
				{    vel_f = -vel_f; vel_c = -vel_c;	/* canvia sentit velocitats */
	     			     f_h = pos_f+vel_f;
	     			     c_h = pos_c+vel_c;		/* actualitza posicio entera */
	     			     if (rd == '0')
	     			     { 
					//pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
					(*fiRebots)++;
					//pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/
	     			     }
				}
    			}
    			//pthread.mutex.lock(&mutex);	/* tanquem el semàfor*/	
    			if (win_quincar(f_h,c_h) == ' ')	/* verificar posicio definitiva */
   			{					/* si no hi ha obstacle */
				win_escricar(f_pil,c_pil,' ',NO_INV);  	/* esborra pilota */		
				pos_f += vel_f; pos_c += vel_c;
				f_pil = f_h; c_pil = c_h;		/* actualitza posicio actual */
				if (c_pil != 0){	 		/* si ho surt del taulell, */	
					win_escricar(f_pil,c_pil, '1'+index ,INVERS); /* imprimeix pilota */	
				}
				else
				{
					(*fi2)=1;		/* codi de finalitzacio de partida */
 			  	}
				//pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/	
 			}
			//else	{}//pthread.mutex.unlock(&mutex);	/* obrim el semàfor*/
  		}
  		else { pos_f += vel_f; pos_c += vel_c; }
  		win_retard(retard);
		//win_update(); 
	}while (!(*fi1) && !(*fi2) && (*fiRebots)<MAX_REBOTS);

	return(0);			/* retorna sense errors d'execucio */
}
