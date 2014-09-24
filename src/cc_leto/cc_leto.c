/*
Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2014)
promethe@ensea.fr

Authors: P. Andry, J.C. Baccon, D. Bailly, A. Blanchard, S. Boucena, A. Chatty, N. Cuperlier, P. Delarboulas, P. Gaussier, 
C. Giovannangeli, C. Grand, L. Hafemeister, C. Hasson, S.K. Hasnain, S. Hanoune, J. Hirel, A. Jauffret, C. Joulain, A. Karaouzène,  
M. Lagarde, S. Leprêtre, M. Maillard, B. Miramond, S. Moga, G. Mostafaoui, A. Pitti, K. Prepin, M. Quoy, A. de Rengervé, A. Revel ...

See more details and updates in the file AUTHORS 

This software is a computer program whose purpose is to simulate neural networks and control robots or simulations.
This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software. 
You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info".
As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, 
users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability. 
In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software, 
that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge. 
Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured 
and, more generally, to use and operate it in the same conditions as regards security. 
The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
*/

/*------------------------------------------------------------	*/
/*      CREATION D'UN RESEAU DE NEURONES                      	*/
/* en mode aveugle , version modifiee de leto.c dans src      	*/
/* tres utile pour les lancement en batch et les tests        	*/
/* PG 2005                                                    	*/
/* modif juillet 2005: suppression des parties communes       	*/
/* avec leto.c pour supprimer les pbs de mise a jour double...	*/
/*																*/
/* Modif Novembre 2013 : ajout d'un flag de compilation.	 	*/
/* Differencie la compilation du script+res de la compilation	*/
/* du script uniquement. 										*/
/*																*/
/*------------------------------------------------------------	*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


#include "graphic_Tx.h"

#include "reseau.h"
#include "outils_externes.h"

#include "script.h"
#include "creation.h"

#include <locale.h>

void vkprints(const char *fmt, va_list ap)
{
	vfprintf(stdout, fmt, ap);
}

void kprints(const char *fmt, ...) /* version simplifiee du kernel pour permettre la compilation des librairies*/
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
}

void print_warning(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	kprints("\n\033[1;33m %s \t %s \t %i :\n \t Warning: ", name_of_file, name_of_function, numero_of_line);
	vkprints(message, arguments);
	kprints("\033[0m\n\n");
	va_end(arguments);
	exit(EXIT_FAILURE);
}

void fatal_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	kprints("\n\033[1;31m %s \t %s \t %i :\n \t Error: ", name_of_file, name_of_function, numero_of_line);
	vkprints(message, arguments);
	kprints("\033[0m\n\n");
	va_end(arguments);
	exit(EXIT_FAILURE);
}


/*---------------------------------------------------------------*/
/*                      PROG PRINCIPAL                           */
/*---------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	int val;
	int flag_save_res = 1; /*flag de compilation  : 1 par defaut compile le res et l'enregistre.  0 compile le res mais ne l'enregistre pas*/


	if (argc != 3)
	{
		if(argc == 4 && strcmp(argv[3],"--no_res")==0 ){
			flag_save_res = 0;
			printf ("cc_leto  %s %s %s \n",argv[1],argv[2],flag_save_res);
		}
		else{
			fprintf(stderr, "\nincorrect number of arguments ! ");
			fprintf(stderr, "cc_leto system.script systen.res [--no_res]\n");
			exit(1);
		}
	}
	if(argc == 3) printf ("cc_leto : %s %s \n",argv[1],argv[2]);

	printf("initial value of the pseudo random generator : ");
	if (scanf("%d", &val) != 1) PRINT_WARNING("Wrong value (not integer) of generator");
	srand48((long) val);

	sc->draw[0]='\0';

	init_global_var_un_script(sc);
	/*     cree_lien_micro_macro();*/    /* pour pouvoir avoir dans le script les info
	 * sur les liens dans le macro neurone */
	/*     init_display_planes(); */
	/*     init_target_table(); */

	memcpy(sc->nomfich1, argv[1], (strlen(argv[1])+1) * sizeof(char));
	memcpy(sc->freseau, argv[2], (strlen(argv[2])+1) * sizeof(char));
	strcpy(sc->directory, "."); /* Il faudrait prendre le repertoire de monfich AB.*/

	/*gtk_set_locale (); */
	setlocale(LC_ALL, "C");

	script_load(sc, sc->nomfich1, 1, NULL);  /* lecture recursive a priori */
	printf("\nscript read: starting res file generation\n");
	creation(NULL,flag_save_res);
	/*   ecrit_reseau();
	 save_script(1,1);*/ /* on garde les commentaires , 0 pour les enlever*/
	/* on sauvegarde les groupes et liens des sous reseau (tout mis a plat) pour promethe */

	return 0;
}
