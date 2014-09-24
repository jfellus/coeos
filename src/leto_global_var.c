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



