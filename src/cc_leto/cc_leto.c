
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
