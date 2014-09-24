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
#ifndef LETO_H
#define LETO_H
#include "graphic_Tx.h"
#include "reseau.h"
#include "gennet.h"

#define EndOfLine                FALSE
          /* pour ecriture_reseau.h != HP_system6.c */
#ifndef pi
#define pi 3.14159
#endif


#ifndef epsilon
#define epsilon 0.00001
#endif


/*---------------------------------------------------------------*/

extern void show_hide_sub_networks(GtkWidget * widget, gpointer data);
extern void aide();

extern void select_plane(int plane,TxDonneesFenetre *onglet_leto);

extern int gere_modification_lien(TxPoint point, t_gennet_script *script_gui);

extern void automatic_group_rename(t_gennet_script *script_gui);

void script_slide(donnees_script *script, TxPoint translation);
void script_zoom(donnees_script *script, TxPoint center, float scale);

extern void fonction_reverse( /*w, rien, c */ );

extern void creation_groupe(GtkWidget * widget, t_gennet_script *script_gui);
extern void create_link_callback(GtkWidget * widget, gpointer data);
extern void create_one_to_one_link_callback(GtkWidget * widget, gpointer data);

extern void detruit_liaison(type_liaison * liaison,TxDonneesFenetre *onglet_leto);
extern void detruit_groupe(type_groupe *group, TxDonneesFenetre *onglet_leto);
void detruit_macro(type_groupe *groupe_cible, TxDonneesFenetre *onglet_leto);

extern int detruit_ensemble_groupes(TxDonneesFenetre *onglet_leto);

extern int formulaire_lien(type_liaison * liaison_courante, t_gennet_script *script_gui);
int formulaire_groupe(t_gennet_script *script_gui);

extern void zoom(GtkWidget * widget, gpointer data);


extern TxDonneesFenetre fenetre1;   /* correspond toujours a la fenetre mere */
extern TxDonneesFenetre fenetre2;   /* fenetre de dialogue - description groupe */
extern TxDonneesFenetre fenetre3;   /* dialogue general */
extern TxDonneesFenetre fenetre4;   /* affichage et saisie des donnees relatives a un lien */

extern void cree_lien_micro_macro();
extern void init_target_table();

extern void fill_link_dialog(type_liaison * link, int type);
extern void fill_group_dialog(int type);

extern void zoom_in(GtkWidget * widget, gpointer data);
extern void zoom_out(GtkWidget * widget, gpointer data);

extern void swap_group_numbers(int m, int n);
extern void swap_links_group(type_groupe *m,type_groupe *n);

extern void create_dir_point_leto(); /* creation du repertoire .leto */

/* mes fonctions a moi */
void run_leto(const char *fichier_script, const char *fichier_draw, const char *fichier_res, const char *fichier_var, TxDonneesFenetre *onglet_leto, int idx, int numPage, long seed);
#endif
