#include <stdio.h>

#include "graphic_Tx.h"
#include "script.h"
#include "outils.h"


/** insertion de la structure utilisee pour connaitre les modes possibles */
#include "mode_link_data_def.h"


donnees_script les_scripts[NB_MAX_SCRIPTS];
donnees_script *sc=&les_scripts[0]; /* le script courant : celui a l'affichage */

#ifndef AVEUGLE
  TxDonneesFenetre fenetre2;      /* affichage et saisie des donnees relatives a un groupe */
  TxDonneesFenetre fenetre1;      /* fenetre principale et affichage du RN */
  TxDonneesFenetre fenetre3;      /* dialogue general */
  TxDonneesFenetre fenetre4;      /* affichage et saisie des donnees relatives a un lien */
#endif

void init_global_var_un_script(donnees_script *le_script)
{
  /* init de la partie IHM */

  le_script->premiere_lecture = 0;

  /** Init tab of mode link depending on group type */
  init_group_mode_link_tab();

#ifndef AVEUGLE
  le_script->first_time = 1;
  le_script->modified=0;
  le_script->flag_create_link = LINK_CREATE_NO_ACTION;       /* 0 ou 1 utilise le flag modify_link */
  le_script->type_create_link = LINK_ALGO;       /* 0 ou 1 utilise le flag modify_link */
  le_script->flag_rectangle_selection = 0;
  le_script->flag_drag_group = 0;        /* pour gere deplacement d'un groupe  */ 
  le_script->flag_drag_coude = 0;
  le_script->show_sub_networks=0;

  le_script->fm_window = NULL;
  le_script->couleur_fond = blanc;
  le_script->couleur_texte = noir;

  le_script->liaison_courante=NULL;
  le_script->coude_courant=NULL;
  le_script->groupes_courants = NULL;

#endif

  le_script->flag_symb = 0;

  /* script.c */
  le_script->first_comment_group = NULL;
  le_script->first_comment_link = NULL;
  le_script->nbre_macro_lues=0;
  le_script->nbre_groupes_lus=0;
  le_script->last_groupe_number=0;
 

  /* init de la partie reseau de neurone */
  le_script->nbre_liaison = 0;
  le_script->nbre_neurone = 0;
  le_script->nbre_groupe = 0;

  le_script->deb_groupe = le_script->fin_groupe = NULL;
  le_script->deb_liaison = le_script->fin_liaison = NULL;

/*---------------------------------------------------------------*/

  le_script->nbre_couche = 1;            /* pour compatibilite avec vieux reseau */
  le_script->t = creer_vecteur(1);
  le_script->ngc = creer_vecteur(1);
  le_script->n = creer_vecteur(1);
  le_script->backup_id = 0;
}



