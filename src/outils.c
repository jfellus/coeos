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
#include "gere_coudes.h"
#include "string.h"
#include "basic_tools.h"


void true_dprints(const char *fmt, ...) /* version simplifiee du kernel pour permettre la compilation des librairies*/
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
}

/* Fonction qui retire l'extension .script */
void get_base_path_name(char *filename)
{
	char *pt;

	/* recherche l'adr du debut de l'extension */
	pt = rindex(filename, '.');
	if (pt != NULL) *pt = '\0'; /* on oublie l'extension du fichier: on met fin de chaine au debut de l'extension*/
}

/* Fonction qui retire l'extension .script et le path d'un nom de fichier */
void get_base_name(char *filename)
{
	char *pt;
	char basename[NAME_MAX+1];

	/* recherche l'adr du debut de l'extension */
	pt = rindex(filename, '.');
	if (pt != NULL) *pt = '\0'; /* on oublie l'extension du fichier: on met fin de chaine au debut de l'extension*/
	pt = rindex(filename, '/'); /* trouve le dernier '/' pour enlever le path du nom*/
	if (pt != NULL){
		strcpy(basename, &(pt[1])); /* on saute le '/' */
		strcpy(filename, basename);
	}

}

/*---------------------------------------------------------------*/
/*      CREATION DU TABLEAU CONTENANT LE RESEAU                  */
/*---------------------------------------------------------------*/

type_tableau creer_reseau(int n) /*liste des neurones              */
{
	int a;
	type_tableau t2;
	a = n + 2;
	t2 = (type_tableau) calloc(a, sizeof(type_neurone));
	if (t2 == NULL)
	{
		EXIT_ON_ERROR("\n\n ERREUR : le Calloc a echoue !?!?!?!?!?!?!?....\n\ncreer_reseau\n");
	}
	return (t2);
}

void free_reseau(donnees_script *script, type_tableau t2)
{
	int i;

	printf("free neurons and coeff nbre_neurone=%d \n", sc->nbre_neurone);
	for (i = 0; i < script->nbre_neurone; i++)
	{
		if (script->neurone[i].coeff != NULL)
		{
			/*  printf("neurone %d: \n",i);*/
			free_coeff(script->neurone[i].coeff);
		}
	}
	free(t2);
}

type_noeud **creer_groupe(int n) /*liste neurones du groupe  */
{
	int i;
	type_noeud **t2;
	t2 = (type_noeud **) calloc(n, sizeof(type_noeud**)); /* calloc(n, 4); */
	if (t2 == NULL)
	{
		printf("\n\n ERREUR : le Calloc a echoue !?!?!?!?!?!?!?....\ncreer_groupe\n\n");
		exit(0);
	}
	for (i = 0; i < n; i++)
		t2[i] = (type_noeud *) 0;
	return (t2);
}

void free_groupe(type_noeud** t2)
{
	/*attention faire un free des zones pointees par le tableau de pointeurs avant */
	free(t2);
}

/*      reservation de la place pour 1 coefficient synaptique   */

type_coeff *creer_coeff()
{
	type_coeff *t2;
	t2 = (type_coeff *) calloc(1, sizeof(type_coeff));
	if (t2 == NULL)
	{
		printf("\n\n ERREUR : le Calloc a echoue !?!?!?!?!?!?!?....\n\ncreer_coeff\n");
		exit(0);
	}
	return (t2);
}

/* eviter la lourdeur de la solution recursive... */
void free_coeff(type_coeff *coeff)
{
	type_coeff *coeff_old;

	while (coeff->s != NULL)
	{
		coeff_old = coeff;
		coeff = coeff->s;
		free(coeff_old);
		/*     printf(".");*/
	}
	free(coeff);
	/*  printf("\n");*/
}

/*     reservation de la place pour 1 neurone d'un groupe        */

type_noeud *creer_noeud()
{
	type_noeud *t2;
	t2 = (type_noeud *) calloc(1, sizeof(type_noeud));
	if (t2 == NULL)
	{
		printf("\n\n ERREUR : le Calloc a echoue !?!?!?!?!?!?!?....\n\ncreer_coeff\n");
		exit(0);
	}
	return (t2);
}

void free_noeud(type_noeud *t)
{
	free(t);
}

/*---------------------------------------------------------------*/
/*             CREATION DU TABLEAU CONTENANT LES DONNEES         */
/*---------------------------------------------------------------*/

type_matrice creer_matrice(int n, int m) /*nbre de lignes , nbre de colonnes */
{
	int i, a, b;
	type_matrice t2;
	a = m + 2;
	b = n + 2;
	t2 = (float **) calloc(b, sizeof(float *));
	if (t2 == NULL)
	{
		printf("\n\n ERREUR : le Calloc a echoue !?!?!?!?!?!?!?....\n\n creer_matrice \n");
		exit(0);
	}
	for (i = 0; i < n; i++)
		t2[i] = (float *) calloc(a, sizeof(float));

	return (t2);
}

void free_matrice(type_matrice *t, int n)
{
	int i;

	for (i = 0; i < n; i++) /* libere chaque ligne avant de liberer le tableau global*/
		free(t[i]);

	free(t);
}

/*-----------------------------------------------------------------*/
/*                CREATION VECTEUR                                 */
/*-----------------------------------------------------------------*/

type_vecteur_entier creer_vecteur(int n)
{
	int i, a;
	type_vecteur_entier t2;
	a = n + 2;
	t2 = (int *) calloc(a, sizeof(int));
	if (t2 == NULL)
	{
		printf("\n\n ERREUR : le Calloc a echoue !?!?!?!?!?!?!?....\n\ncreer_vecteur\n");
		exit(0);
	}
	for (i = 0; i < a; i++)
		t2[i] = 0;
	return (t2);
}

void free_vecteur(type_vecteur_entier *t)
{
	free(t);
}

/*--------------------------------------------------------------*/
/*        Utilitaire recherche adr a partir du numero           */
/*--------------------------------------------------------------*/

type_groupe *trouver_groupe(int gpe)
{
	type_groupe *groupe;
	groupe = sc->deb_groupe;
	if (groupe == NULL) EXIT_ON_ERROR("trouver_groupe(): Pas de groupe dans la liste ! \n");

	while (groupe != NULL)
	{
		/*printf("groupe %d -> %s\gpe",groupe->no,groupe->no_name);*/
		if (groupe->no == gpe) return (groupe);
		groupe = groupe->s;
	}

	EXIT_ON_ERROR("Error (trouver_groupe): group %d not found \n", gpe);
	return NULL; /* pour eviter erreur systeme */
}

type_groupe *trouver_groupe_par_nom_old(char *nom)
{
	type_groupe *groupe;
	/*printf("trouver_groupe %d \n",n);*/
	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		if (strcmp(groupe->no_name, nom) == 0)
		{
			/* printf("groupe %d -> %s\n",groupe->no,groupe->no_name);*/
			return (groupe);
		}
		groupe = groupe->s;
	}
	printf("Error (trouver_groupe): group %s not found \n", nom);
	return (type_groupe *) NULL; /* pour eviter erreur systeme */
}

type_groupe *trouver_groupe_par_nom(char *nom, TxDonneesFenetre *onglet_leto)
{
	return find_group_associated_to_symbolic_name(nom, onglet_leto->hashtab);
}

type_liaison *trouver_liaison_par_nom(char *no_groupe_depart_name, char *no_groupe_arrivee_name)
{
	type_liaison *liaison;

	liaison = sc->deb_liaison;
	while (liaison != NULL)
	{
		if (strcmp(liaison->depart_name, no_groupe_depart_name) == 0 && strcmp(liaison->arrivee_name, no_groupe_arrivee_name) == 0) return (liaison);
		liaison = liaison->s;
	}
	printf("Error liaison non trouvee \n");
	return (type_liaison *) NULL;
}

/*-------------------------------------------------------------*/
/*   Allocation de la memoire pour la creation des noeuds      */
/*-------------------------------------------------------------*/

type_groupe *creer_groupeb(type_groupe * prec)
{
	type_groupe *nouv;
	nouv = (type_groupe *) ALLOCATION(type_groupe);
	memset(nouv, 0, sizeof(type_groupe));

	if (prec != (type_groupe *) NULL) prec->s = nouv;
	nouv->s = (type_groupe *) NULL;

	sc->last_groupe_number++;
	nouv->no = sc->last_groupe_number; /*sc->nbre_groupe;*/

	MY_FloatAffect(nouv->seuil, 0.);
	MY_FloatAffect(nouv->alpha, 0.);
	MY_FloatAffect(nouv->learning_rate, 1.);
	MY_FloatAffect(nouv->simulation_speed, 1.);
	nouv->type = No_Fonction_Algo;
	MY_IntAffect(nouv->type2, 0);
	nouv->ech_temps = 0;

	MY_IntAffect(nouv->taillex, 1);
	MY_IntAffect(nouv->tailley, 1);
	MY_IntAffect(nouv->nbre, 1);

	MY_FloatAffect(nouv->tolerance, 0.);
	MY_FloatAffect(nouv->seuil, 0.);

	MY_IntAffect(nouv->dvp, -1);
	MY_IntAffect(nouv->dvn, -1);

	MY_FloatAffect(nouv->nbre_de_1, -1.);
	MY_FloatAffect(nouv->sigma_f, -1.);
	MY_FloatAffect(nouv->noise_level, 0.);

	nouv->reverse = 0;
	memcpy(nouv->nom, "???", (strlen("???") + 1) * sizeof(char));

	nouv->debug = -1; /* la position du groupe pour promethe doit etre calculee */
	return (nouv);
}

void affiche_liste_groupes()
{
	type_groupe *groupe;
	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		groupe = groupe->s;
	}
}

void free_liste_groupes(type_groupe *groupe)
{
	type_groupe *old_groupe;
	/*printf("trouver_groupe %d \n",n);*/
	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		old_groupe = groupe;
		groupe = groupe->s;
		free(old_groupe);
		printf("un groupe efface \n");
	}
}

type_liaison *creer_liaison(type_liaison * prec)
{
	type_liaison *nouv;
    nouv = ALLOCATION(type_liaison);
	if (prec != NULL) prec->s = nouv;
	memset(nouv, 0, sizeof(type_liaison));
	return (nouv);
}

void free_liaison(type_liaison * liaison)
{
	if (liaison->s != NULL) free_liaison(liaison->s);
	free(liaison);
}

/*--------------------------------------------------------------*/
/* pointe vers dernier coeff du neurone pour le cas ou on en a  */
/* deja cree avec la procedure creer_liaisons_entre_groupe()    */
/*--------------------------------------------------------------*/

type_coeff *pointe_vers_dernier(int j)
{
	type_coeff *pt, *pt1;
	pt1 = NULL;
	pt = sc->neurone[j].coeff;
	while (pt != NULL)
	{
		pt1 = pt;
		pt = pt->s;
	}
	return (pt1);
}

/*-------------------------------------------------------------------------*/
/*  lorsqu'il y a plusieurs voies de liaisons creer les micro neurones si  */
/*  le groupe est un PTM ou un winner_colonne                              */
/*-------------------------------------------------------------------------*/

int compte_neurone()
{
	int nbre;
	type_groupe *groupe;
	type_liaison *liaison;
	int nbre_voie;

	int groupe_nbre, taillex, tailley;

	nbre = 0;
	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		nbre_voie = 0;
		liaison = sc->deb_liaison;
		while (liaison != NULL)
		{
			if (liaison->arrivee == groupe->no && liaison->type != No_l_algorithmique && liaison->type != No_l_neuro_mod && liaison->mode < NEUROMOD) nbre_voie++;
			liaison = liaison->s;
		}
		if (nbre_voie > 1
				&& (groupe->type == No_PTM || groupe->type == No_Winner_Macro || groupe->type == No_PLG || groupe->type == No_Winner_Colonne || groupe->type == No_But || groupe->type == No_Pyramidal || groupe->type == No_Pyramidal_plan
						|| groupe->type == No_Sigma_PI || groupe->type == No_Macro_Colonne || groupe->type == No_KO_Discret || groupe->type == No_KO_Continu || groupe->type == No_Selverston))
		{
			/* #ifndef SYMBOLIQUE_VERSION */
			/*             groupe->nbre = */
			/*                 (groupe->taillex * groupe->tailley) * (1 + nbre_voie); */
			/* #else */
			/*             sprintf(groupe->nbre, "%s * %s * %d", groupe->taillex, */
			/*                     groupe->tailley, (1 + nbre_voie)); */
			/* #endif */

			taillex = MY_Int2Int(groupe->taillex);
			tailley = MY_Int2Int(groupe->tailley);
			groupe_nbre = (taillex * tailley) * (1 + nbre_voie);

		}
		else
		{
			/* #ifndef SYMBOLIQUE_VERSION */
			/*             groupe->nbre = groupe->taillex * groupe->tailley; */
			/* #else */
			/*             sprintf(groupe->nbre, "%s * %s", groupe->taillex, */
			/*                     groupe->tailley); */
			/* #endif */

			taillex = MY_Int2Int(groupe->taillex);
			tailley = MY_Int2Int(groupe->tailley);
			groupe_nbre = (taillex * tailley);

		}

		nbre += groupe_nbre;
		groupe->nbre_voie = nbre_voie;

#ifndef SYMBOLIQUE_VERSION
		groupe->nbre = groupe_nbre;
#else
		sprintf(groupe->nbre,"%d",groupe_nbre);
#endif
		groupe = groupe->s;
	}
	return (nbre);
}

float alea(float x)
{
	return (x - 2. * x * drand48());
}

int sorte_liaison(int No_liaison)
{
	switch (No_liaison)
	{
	case No_l_1_1_modif:
		return (1);
	case No_l_1_a:
		return (1);
	case No_l_1_v:
		return (1);
	case No_l_1_1_non_modif:
		return (0);
	case No_l_1_t:
		return (1);
	case No_l_1_1_non_modif_bloqueur:
		return (0);
	case No_l_1_v_non_modif:
		return (0);
	default:
		printf("ERREUR : le type de liaison %d n'est pas identifie", No_liaison);
		printf("         dans la fonction sorte_liaison \n");
	}
	return (-1);
}

/*------------------------------------------------------------------*/
/* calcule les coordonnees ou devront etre afficher les differents  */
/* groupes dans promethe. Si debug<0 => doit etre calculer sinon    */
/* prendre les valeurs deja utilisees                               */
/*------------------------------------------------------------------*/

void prepare_affichage()
{
	int taille_bloc = 128;
	type_groupe *groupe;
	int orig_x, orig_y;

	orig_x = 0;
	orig_y = 0;
	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		if (groupe->debug > 0)
		{
			groupe->p_posx = orig_x * (taille_bloc + 20);
			groupe->p_posy = orig_y * (taille_bloc + 20) + 20;
		}
		orig_x++;
		if (orig_x > 5)
		{
			orig_x = 0;
			orig_y++;
		}
		groupe = groupe->s;
	}
}

void replace_tout()
{
	int taille_bloc = 128;
	type_groupe *groupe;
	int orig_x, orig_y;

	orig_x = 0;
	orig_y = 0;
	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		groupe->p_posx = orig_x * (taille_bloc + 20);
		groupe->p_posy = orig_y * (taille_bloc + 20) + 20;
		if (groupe->debug < 0) groupe->debug = 1;
		if (groupe->debug == 0)
		{
			printf("\nWARNING: the group %d will not be plotted in promethe\n", groupe->no);
			printf("         write debug= +/- 1 for the group %d in the script file\n\n", groupe->no);
		}
		orig_x++;
		if (orig_x > 5)
		{
			orig_x = 0;
			orig_y++;
		}
		groupe = groupe->s;
	}
}

/* la fonction renvoie 1 si les groupes en entree et en sortie du lien sont dans la zone de selection */
/* la fonction renvoie 0 sinon */

int selected_link(type_liaison *liaison, TxDonneesFenetre *onglet_leto)
{
#ifndef AVEUGLE
	type_groupe *groupe_depart, *groupe_arrivee;

	if (liaison->depart == -1 && liaison->arrivee == -1) return 0; /* avant =1;  pour eviter de tjs selectionner le lien -1 a -1 */

	groupe_depart = trouver_groupe_par_nom(liaison->depart_name, onglet_leto);
	if (groupe_depart == NULL)
	{
		EXIT_ON_ERROR("input group %s does not exist \n", liaison->depart_name);
	}
	if (is_selected(groupe_depart) == NULL) return 0;

	groupe_arrivee = trouver_groupe_par_nom(liaison->arrivee_name, onglet_leto);
	if (groupe_arrivee == NULL)
	{
	   EXIT_ON_ERROR(" output group %s does not exist \n", liaison->arrivee_name);
	}

	if (is_selected(groupe_arrivee) == NULL) return 0;
	return 1;
#else
	(void)liaison;
	(void)onglet_leto;
	return 0;
#endif
}

/* verifie si un plan d'affichage est deja utilise par au moins un groupe */
int plane_used(int plan)
{
	type_groupe *groupe;

	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		if (abs(groupe->reverse) == plan) return 1;
		groupe = groupe->s;
	}
	return 0;
}

/* renvoie le no du prochain plan libre pour y mettre une macro */
/* si no_macro_used est different de NULL alors on recupere aussi un no de macro propose par find_new_plane_for_new_macro */
int find_new_plane_for_new_macro(int *no_macro_used)
{
	int no_macro_used_2;
	int selected_plane;

	if (no_macro_used == NULL) no_macro_used = &no_macro_used_2;

	*no_macro_used = sc->nbre_macro_lues;
	selected_plane = (100 + sc->nbre_macro_lues * 7);

	while (plane_used(selected_plane) == 1)
	{
		printf("Attention plan %d deja utilise j'en cherche un autre \n", selected_plane);
		(*no_macro_used)++;
		selected_plane = (100 + (*no_macro_used) * 7);
	}

	return selected_plane;
}

/* ici on initialise tous les coudes pour les liaisons n'ayant pas de liste de coudes definies */
void initialise_liaisons()
{
#ifndef AVEUGLE
	type_liaison *liaison_courante;
	liaison_courante = sc->deb_liaison;

	while (liaison_courante != NULL)
	{
		initialise_coudes_liaison(liaison_courante);
		liaison_courante = liaison_courante->s;
	}
#endif
}

