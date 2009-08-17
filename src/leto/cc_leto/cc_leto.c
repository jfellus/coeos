
/*------------------------------------------------------------*/
/*      CREATION D'UN RESEAU DE NEURONES                      */
/* en mode aveugle , version modifiee de leto.c dans src      */
/* tres utile pour les lancement en batch et les tests        */
/* PG 2005                                                    */
/* modif juillet 2005: suppression des parties communes       */
/* avec leto.c pour supprimer les pbs de mise a jour double...*/
/*------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


#include "graphic_Tx.h"

#include "reseau.h"
#include "outils_externes.h"

#include "outils.h"
#include "script.h"
#include "creation.h"
#include "leto_global_var.h"

#include <locale.h>


/*---------------------------------------------------------------*/
/*                      PROG PRINCIPAL                           */
/*---------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   int val, res;

    if (argc != 3)
    {
       fprintf(stderr, "\nincorrect number of arguments ! ");
       fprintf(stderr, "cc_leto system.script systen.res\n");
       exit(1);
    }

    printf("initial value of the pseudo random generator : ");
    res = scanf("%d", &val);
    srand48((long) val);

    sc->draw[0]='\0';
    
    init_global_var_un_script(sc);
    /*     cree_lien_micro_macro();*/    /* pour pouvoir avoir dans le script les info
					  * sur les liens dans le macro neurone */
/*     init_display_planes(); */
/*     init_target_table(); */

    memcpy(sc->nomfich1, argv[1], (strlen(argv[1])+1) * sizeof(char));
    memcpy(sc->freseau, argv[2], (strlen(argv[2])+1) * sizeof(char));

    /*gtk_set_locale (); */
    setlocale(LC_ALL, "C");

    lecture(1, NULL);  /* lecture recursive a priori */
    creation(NULL);
    /*   ecrit_reseau();
	 save_script(1,1);*/ /* on garde les commentaires , 0 pour les enlever*/
                      /* on sauvegarde les groupes et liens des sous reseau (tout mis a plat) pour promethe */

    return 0;
}
