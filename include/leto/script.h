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
#ifndef SCRIPT_H
#define SCRIPT_H

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "script_parserxml.h"
#include "outils_script.h"

#ifndef AVEUGLE
#include "graphic_Tx.h"
#include "group_selection.h"
#endif

#include <search.h>  /* pour la table de hachage */

#define NB_MAX_SCRIPTS 255 /* ca devrait suffire pour quelques annees, j'espere */

#define LONGUEUR_NOM 255        /* defini dans leto.h habituellement */

#define LINK_CREATE_NO_ACTION 0
#define LINK_CREATE_CHOOSE_START 1
#define LINK_CREATE_CHOOSE_END 2

#define LINK_ALGO 0
#define LINK_ONE_TO_ONE 1

#define nbPoint_max 100
#define nb_max_planes 2000

/* bords des boites representant les differentes echelles temporelles */
#define limite_nbre_max_ech 200


typedef struct donnees_script
{
  int premiere_lecture;

#ifndef AVEUGLE
  TxPoint point_courant_leto;
  TxPoint pointer_selection_center;

  int flag_rectangle_selection;
  int couleur_fond;
  int couleur_texte;

  /* bords des boites representant les differentes echelles temporelles */
  int boite_x_min[limite_nbre_max_ech];
  int boite_y_min[limite_nbre_max_ech];
  int boite_x_max[limite_nbre_max_ech];
  int boite_y_max[limite_nbre_max_ech];

  int display_plane[nb_max_planes];

  int xmin, ymin;
  int xmax, ymax;

  int modified;

  int flag_create_link;  /* 0 ou 1 utilise le flag modify_link */
  int type_create_link;  /* 0 ou 1 utilise le flag modify_link */

  int flag_drag_group;              /* pour gere deplacement d'un groupe  */

  int first_time;             /* affichage au demarrage du reseau lu */
  int show_sub_networks;

  /* numero de l'onglet pour metaleto */
  int num_onglet;

  /* pour find_and_manage groupes */
  GtkWidget *fm_window;
  GtkTreeModel *fm_group_list_store;

  selected_group *groupes_courants;
#endif

  type_liaison *liaison_courante;
  t_polyline *coude_courant;

  int flag_symb;              /* pour gerer les versions symboliques */
  int flag_drag_coude;


  /*--------------------------------------------------------------*/

  char groupe_depart_name[SIZE_NO_NAME], groupe_arrivee_name[SIZE_NO_NAME];

  /*--------------------------------------------------------------*/
  /*       structure de donnee pour le reseau de neurones         */
  /*--------------------------------------------------------------*/

  type_tableau neurone;    /*tableau contenant l'ens du reseau   */

  int nbre_neurone;        /* nombre de neurones dans le reseau  */
  int last_groupe_number;         /* no seulement incremente a partir du dernier groupe lu */
  int nbre_entree;         /* nbre de vecteurs utilises en entree */
  int nbre_sortie;         /* nbre de neurones utilises en sortie */
  int nbre_couche;         /* nbre de couches dans le reseau     */
  float ca, ck;            /* constantes pour la sigmoide        */

  char nom_groupe[200][256];

  int nbre_max_ech;

  type_vecteur_entier t;
  type_vecteur_entier c;   /* type du groupe pour apprentissage  */
  type_vecteur_entier n;   /* nbre de neurones dans un groupe    */
  type_vecteur_entier ngc; /* nbre de groupes par couche         */

   char nomfich1[PATH_MAX], freseau[PATH_MAX], draw[PATH_MAX], fvar[PATH_MAX];
   char directory[PATH_MAX];
   char fscript[PATH_MAX];

  int nbre_groupe, nbre_liaison;

  type_groupe *deb_groupe, *fin_groupe;
  type_liaison *deb_liaison, *fin_liaison;

  /* script.c */
  type_noeud_comment *first_comment_group;
  type_noeud_comment *first_comment_link;

  int nbre_macro_lues;
  int nbre_groupes_lus;
  int backup_id;

  long int seed;
} donnees_script;

extern donnees_script les_scripts[NB_MAX_SCRIPTS];
extern donnees_script *sc; /* le script courant : celui a l'affichage */

extern void init_global_var_un_script(donnees_script *le_script);

void script_load(donnees_script *script, char *filename, int recursive_load, struct hsearch_data *hash_table);

 int read_macro(char *base_base, char *nom_fichier,int px, int py, int relatif, int mode, int *selected_plane, struct hsearch_data *hash_table);
 void load_script(donnees_script *script, char *filename, int recursive_load, struct hsearch_data *hash_table);
 int script_set_modified(donnees_script *script);


/* variables globales pour la gestion des commentaires non attaches a un groupe */
/* Il s'agit des commentaires precedent le debut des groupes et des commentaires */
/* precedent le debut des liens (si c'est possible! / correspond fin d'un groupe non?) */

extern void save_script(int comment,int save_sub_networks, TxDonneesFenetre *onglet_leto);
extern void save_script_selected(int comment,int save_sub_networks,char *nom, TxDonneesFenetre *onglet_leto);
extern void sauvegarde();
extern void sauvegarde_avec_comment();
extern void lecture(int recursive_load,TxDonneesFenetre *onglet_leto);











/* outils */

void fatal_error(const char *name_of_file, const char* name_of_function,  int numero_of_line, const char *message, ...);


extern type_tableau creer_reseau(int );
extern void free_reseau(donnees_script *script, type_tableau t2);

extern type_noeud **creer_groupe(int );
extern void free_groupe(type_noeud** t2);

extern type_coeff *creer_coeff();
extern void free_coeff(type_coeff *coeff);

extern type_noeud *creer_noeud();
extern void free_noeud(type_noeud *t);

extern type_matrice creer_matrice(int n, int m);
extern void free_matrice(type_matrice *t, int n);

extern type_vecteur_entier creer_vecteur(int n);
extern void free_vecteur(type_vecteur_entier *t);

extern type_groupe *creer_groupeb(type_groupe * prec);
extern void free_liste_groupes(type_groupe *groupe);


extern type_groupe *trouver_groupe_ante_par_nom(char *nom);
extern type_groupe *trouver_groupe(int n);
extern type_groupe *trouver_groupe_par_nom(char *nom, TxDonneesFenetre *onglet_leto);
extern type_liaison *trouver_liaison_par_nom(char *no_groupe_depart_name,char *no_groupe_arrivee_name);
extern type_groupe *creer_groupeb(type_groupe * prec);

extern type_liaison *creer_liaison(type_liaison * prec);
extern void free_liaison(type_liaison * liaison);

extern type_coeff *pointe_vers_dernier(int j);
extern int compte_neurone();
extern float alea(float x);
extern int sorte_liaison(int No_liaison);

extern void prepare_affichage();
extern void replace_tout();

extern int selected_link(type_liaison *liaison, TxDonneesFenetre *onglet_leto);
extern int plane_used(int plan);
extern int find_new_plane_for_new_macro(int *no_macro_used);

extern void affiche_liste_groupes();

void initialise_liaisons();

/* fonction a moi */
extern void get_base_path_name(char *filename);
extern void get_base_name(char *filename);

#endif
