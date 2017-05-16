/* He inclòs totes les llibreries presents en el fronton3.c */
#include <stdio.h>	/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport2.h"	/* incloure definicions de funcions propies */
#include "memoria.h"

/* funcio per moure la pilota: retorna un 1 si la pilota surt per la porteria,*/
/* altrament retorna un 0 */

/* Necessita rebre per valor:
vel_f, vel_c,f_pil, c_pil, pos_f, pos_c
Necessita rebre per parámetre:
fi2, num_rebots
*/
int main(int n_args, char *ll_args[]){

	/* Variables locals */
	int f_h, c_h;
	char rh,rv,rd;
    //fprintf(stderr,"\nPilota %s (arguments rebuts):\n%s, %s, %s, %s, %s, %s, ",(ll_args[1]),(ll_args[2]),(ll_args[3]),(ll_args[4]),(ll_args[5]),(ll_args[6]), (ll_args[7]));
   // fprintf(stderr,"%s, %s, %s, %s, %s, %s\n",(ll_args[8]),(ll_args[9]),(ll_args[10]),(ll_args[11]), (ll_args[12]), (ll_args[13]));

	/* Variables passades per valor */
	int num_pil = atoi(ll_args[1])-1;
	float vel_f = 1;
	float vel_c = 1;
	int f_pil = 10;
	int c_pil = 10;
	float pos_f = 10.5;
	float pos_c = 10.5;
	int n_col = atoi (ll_args[8]);
	int n_fil = atoi (ll_args[9]);
	int retard = atoi (ll_args[10]);	/* Variables passades per parámetre */
   // fprintf(stderr, "Impressió de les variables locals que presenten el conflicte\n%f, %f, %i, %i, %f, %f\n ", vel_f, vel_c, f_pil, c_pil, pos_f, pos_c);

	int rebotes = atoi (ll_args[11]);
	int *num_rebots = map_mem(rebotes);		/* obtenir adres. de mem. compartida */

	int fin2 = atoi (ll_args[12]);
	int *fi2 = map_mem(fin2);

	int id_win = atoi (ll_args[13]);
	void *p_win;
	p_win = map_mem (id_win);

	win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */

	do
	{
		f_h = pos_f+vel_f;		/* posicio hipotetica de la pilota (entera) */
		c_h = pos_c+vel_c;
		rh = rv = rd = ' ';
		if ((f_h != f_pil) || (c_h != c_pil))
        {
			if (f_h != f_pil) 		/* provar rebot vertical */
			{
                rv = win_quincar(f_h,c_pil);	/* veure si hi ha algun obstacle */
				if (rv != ' ')			/* si hi ha alguna cosa */
				{   vel_f = -vel_f;		/* canvia sentit velocitat vertical */
				    f_h = pos_f+vel_f;		/* actualitza posicio hipotetica */
				    if (rv == '0')
				    {
					(*num_rebots)--;
                    }
				}
            }
            if (c_h != c_pil) 		/* provar rebot horitzontal */
            {
            rh = win_quincar(f_pil,c_h);	/* veure si hi ha algun obstacle */
            if (rh != ' ')			/* si hi ha algun obstacle */
            {
                vel_c = -vel_c;		/* canvia sentit vel. horitzontal */
                c_h = pos_c+vel_c;		/* actualitza posicio hipotetica */
                if (rh == '0')
                {
                    (*num_rebots)--;
                }
            }
        }
        if ((f_h != f_pil) && (c_h != c_pil))	/* provar rebot diagonal */
        {
            rd = win_quincar(f_h,c_h);
            if (rd != ' ')				/* si hi ha obstacle */
            {
                vel_f = -vel_f; vel_c = -vel_c;	/* canvia sentit velocitats */
                f_h = pos_f+vel_f;
                c_h = pos_c+vel_c;		/* actualitza posicio entera */
                if (rd == '0')
                {
					(*num_rebots)--;
                }
            }
        }
        if (win_quincar(f_h,c_h) == ' ')	/* verificar posicio definitiva */
        {					/* si no hi ha obstacle */
            win_escricar(f_pil,c_pil,' ',NO_INV);  	/* esborra pilota */
            pos_f += vel_f; pos_c += vel_c;
            f_pil = f_h; c_pil = c_h;		/* actualitza posicio actual */
            if (c_pil != 0){	 		/* si ho surt del taulell, */
                win_escricar(f_pil,c_pil, '1'+num_pil ,INVERS); /* imprimeix pilota */
            }
            else
            {
                (*fi2)=1;		/* fianl per sortir  */
            }
        }
    }
    else { pos_f += vel_f; pos_c += vel_c; }
    win_retard(retard);
	}while (!(*fi2));

	return(0);			/* retorna sense errors d'execucio */
}

