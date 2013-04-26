/* fonction principale de leto permettant de generer un fichier .res */
/* a partir d'un fichier .script */
/* la gestion detaillee de la creation est faite dans leto_cle.c     */

#include <stdio.h>
#include <stdlib.h>
#include "graphic_Tx.h"
#include "outils.h"
#include "outils_externes.h"
#include "script.h"

/*-------------------------------------------------------------------------*/

void creation(TxDonneesFenetre *onglet_leto)
{
	int i, j, k;
	int s_deb, s_fin, e_deb, e_fin;
	int x, y, dx;
	type_groupe *groupe;
	type_liaison *liaison;
	type_liaison *neuromod_deb = NULL;
	type_liaison *neuromod_ptr = NULL;
	type_liaison *prec_liaison = NULL;
	int neuromod_list_tested = 0;
	int nbre_voie;
	int pas; /* taille du macro neurones pour trouver */
	/* les micron neurones par modulo        */
	float val;
	int no_gpe_liaison;

	int groupe_nbre, taillex;

	/** TODO : il faudrait ajouter le test sur neuromod pour determiner si on doit creer le neurone ou pas */
	sc->nbre_neurone = compte_neurone();
	printf("\nLe reseau comprend %d neurones \n", sc->nbre_neurone);
	sc->neurone = creer_reseau(sc->nbre_neurone);

	/* numerotation de neurones en fontion des groupes              */

	k = 0;
	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		groupe_nbre = MY_Int2Int(groupe->nbre); /* on convertit dans au cas ou on serait en version symbolique */
		for (j = 0; j < groupe_nbre; j++)
		{
			sc->neurone[k].groupe = groupe->no;
			k++;
		}
		groupe = groupe->s;
	}

	/*Creation des liaisons entre les neurones des different groupes*/

	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		i = groupe->no;
		nbre_voie = 0;
		if (groupe->nbre_voie > 1
				&& (groupe->type == No_PTM || groupe->type == No_Winner_Macro || groupe->type == No_PLG || groupe->type == No_Winner_Colonne || groupe->type == No_But || groupe->type == No_Pyramidal || groupe->type == No_Pyramidal_plan
						|| groupe->type == No_Sigma_PI || groupe->type == No_Macro_Colonne || groupe->type == No_KO_Discret || groupe->type == No_KO_Continu || groupe->type == No_Selverston)) pas = groupe->nbre_voie + 1;
		else pas = 1;

		taillex = MY_Int2Int(groupe->taillex); /* on convertit au cas ou on serait en version symbolique */

		dx = taillex;

		for (j = 0; j < sc->nbre_neurone; j++)
		{
			if (sc->neurone[j].groupe == i) break;
		}
		s_deb = j;
		x = y = 0;
		for (j = s_deb; j < sc->nbre_neurone; j++)

		{
			if (sc->neurone[j].groupe != i) break;
			sc->neurone[j].posx = (float) x;
			sc->neurone[j].posy = (float) y;
			sc->neurone[j].posz = (float) i;
			x++;
			if (x >= dx)
			{
				y++;
				x = 0;
			}
		}
		s_fin = j - 1;
		liaison = sc->deb_liaison;
		no_gpe_liaison = 1; /* le 0 correspond aux liens entre micro et macro neurones */
		while (liaison != NULL)
		{
			/*  	     printf("liaison (A) [%p] : type %d - mode : %d - liaison suivant : %p\n",(void*)liaison,liaison->type, liaison->mode, (void*)liaison->s); */

			/** creation des liens classique et tri des liens de
			 * neuro modulation */
			if (liaison->mode < NEUROMOD)
			{
				if (liaison->arrivee == i && liaison->type != No_l_algorithmique && liaison->type != No_l_neuro_mod)
				{
					k = liaison->depart;
					nbre_voie++;
					for (j = 0; j < sc->nbre_neurone; j++)
					{
						if (sc->neurone[j].groupe == k) break;
					}
					e_deb = j;
					for (j = e_deb; j < sc->nbre_neurone; j++)
					{
						if (sc->neurone[j].groupe != k) break;
					}
					e_fin = j - 1;
					creer_liaisons_entre_groupe(e_deb, e_fin, s_deb, s_fin, i, k, liaison, nbre_voie - 1, pas, no_gpe_liaison - 1);
				}
				no_gpe_liaison++;
				prec_liaison = liaison; /* only if not a NM link*/
			}
			else if (!neuromod_list_tested)
			{ /** on ne remplit la
			 * liste de neuromod que
			 * la premiere fois */
				/** ajouter les liens de neuromod dans la liste de neuromod*/
				if (neuromod_deb == NULL)
				{
					neuromod_deb = liaison;
					neuromod_ptr = liaison;
				}
				else
				{
					neuromod_ptr->s = liaison;
					neuromod_ptr = liaison;
				}
				/* enlever les liens de neuromod dans la liste des
				 * lien classique */
				if (prec_liaison == NULL) sc->deb_liaison = liaison->s;
				else prec_liaison->s = liaison->s;
			}
			/*  	     printf("liaison (Fin A) [%p] : type %d - mode : %d - liaison suivant : %p\n",(void*)liaison,liaison->type, liaison->mode, (void*)liaison->s); */
			liaison = liaison->s;
			/*  	     printf("liaison : %p\n",(void*)liaison);  */
		}
		/** concatenation de la liste des neuromods a la suite de la
		 * liste des liens si elle existe */
		if (prec_liaison != NULL) prec_liaison->s = neuromod_deb;

		/**fermeture de la liste des neuromods si elle existe*/
		if (neuromod_ptr != NULL) neuromod_ptr->s = NULL;

		/**liste deja parcourue - set corresponding flag*/
		neuromod_list_tested = 1;

		/** creation des micro liens entre le macro neurone et les
		 * micro neurones */
		if (nbre_voie > 1
				&& (groupe->type == No_PTM || groupe->type == No_Winner_Macro || groupe->type == No_PLG || groupe->type == No_Winner_Colonne || groupe->type == No_But || groupe->type == No_Pyramidal || groupe->type == No_Pyramidal_plan
						|| groupe->type == No_Sigma_PI || groupe->type == No_Macro_Colonne || groupe->type == No_KO_Discret || groupe->type == No_KO_Continu || groupe->type == No_Selverston))
		{
			if (groupe->type == No_But || groupe->type == No_Ou) val = 1.;
			else val = 1. / ((float) nbre_voie);
			creer_micro_liens(s_deb, s_fin, i, nbre_voie, val);
		}

		/** parcours de liens de neuromod - ne pas prendre des
		 * liaisons algo ni des liens de neuromod globale ! */
		liaison = neuromod_deb;
		while (liaison != NULL)
		{
			if (liaison->arrivee == i && liaison->type != No_l_algorithmique && liaison->type != No_l_neuro_mod)
			{
				k = liaison->depart;
				nbre_voie++;
				for (j = 0; j < sc->nbre_neurone; j++)
				{
					if (sc->neurone[j].groupe == k) break;
				}
				e_deb = j;
				for (j = e_deb; j < sc->nbre_neurone; j++)
				{
					if (sc->neurone[j].groupe != k) break;
				}
				e_fin = j - 1;
				creer_liaisons_entre_groupe(e_deb, e_fin, s_deb, s_fin, i, k, liaison, nbre_voie - 1, -1/* pas=-1 -> cluster management !*/, no_gpe_liaison - 1);
			}
			no_gpe_liaison++;
			liaison = liaison->s;
		}
		groupe = groupe->s;
	} /*       etude du groupe suivant       */
	ecrit_reseau();
	/*   save_script(1,0,onglet_leto);*/ /*CG: sauvegarde  le .script*/
	save_script(1, 1, onglet_leto); /*PG: sauve le script et met a plat les sous reseau utile avec le .res mais pb denomination */
}

