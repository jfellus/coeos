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
#ifndef INTERFACE_H
#define INTERFACE_H

#ifndef AVEUGLE
#include "gennet.h"
#endif

#define taille_max_fenetre1_x 6000
#define taille_max_fenetre1_y 3000

#define POPUP_MENU_NO_SELECTION 0
#define POPUP_MENU_GROUP 1
#define POPUP_MENU_LINK 2
#define POPUP_MENU_BEND 3




extern void create_fenetre1(TxDonneesFenetre * onglet_leto);
extern GtkWidget* create_read_link(TxDonneesFenetre *onglet_leto,type_liaison *link);
extern GtkWidget* create_read_group(TxDonneesFenetre *onglet_leto);
extern void create_fenetre_dialogue(TxDonneesFenetre * fenetre, TxDonneesFenetre *onglet_leto);
extern type_groupe* get_groupOut(type_liaison *link);
extern void init_group_mode_link_tab(void);

GtkWidget *entry_group;
GtkWidget *entry_taille_x;

extern GtkWidget *reverse_button;
extern GtkWidget *debug_button;
extern GtkWidget *reset_pos_button;
extern GtkWidget *combo_type_groupe_entry;
extern GtkWidget *combo_type_link_entry;
extern GtkWidget *combo_mode_link_entry;
extern GtkWidget *combo_reverse_groupe;
extern GtkWidget *combo_group_function_name;
extern GtkWidget *combo_debug_groupe;
/*-------------------------------------------------------------*/

/** definition de type_lien_no_nom : cf. mode_link_def.h dans
 * prom_kernel/include/ */


/*-------------------------------------------------------------*/

#define No_item_groupe		0
/*#define No_item_nom 		1*/
#define No_item_taillex 	1
#define No_item_tailley 	2
#define No_item_nbre 		3
#define No_item_seuil 		4
#define No_item_learning_rate 	5
#define No_item_simulation_speed 6
#define No_item_type2 		7
#define No_item_ech_temps  8
#define No_item_noise_level     9  /* ajout recent PG - place liee a pos dans IHM */
#define No_item_tolerance 	10
#define No_item_alpha 		11
#define No_item_nbre_de_1	12
#define No_item_dvp 		13
#define No_item_dvn 		14
#define No_item_sigma_f 	15
#define No_item_plan     	16

#define Number_of_items         17

#define No_item_comment     	17
#define Number_of_items_total 	18

/*--------------------------------------------------------------*/

#define No_item_link_depart 0
#define No_item_link_arrivee 1
#define No_item_link_temps 2
#define No_item_link_stemps 3
#define No_item_link_nbre 4
#define No_item_link_secondaire 5
#define No_item_link_mode 6
#define No_item_link_norme 7
#define No_item_link_dvx 8
#define No_item_link_dvy 9
#define No_item_link_proba 10
#define No_item_link_name 11
#define No_item_link_style 12
#define No_item_link_comment 13

#define Number_of_link_items    13
#define Number_of_link_items_total 14

/*------------------------------------------------------------*/
/* gestion du champs mode des liens entre groupes de neurones */

/** cf. mode_link_def.h dans prom_kernel/include/ */


/*----------------------------------------*/


extern TxChampsFormulaire formulaire_group[Number_of_items_total];
extern TxChampsFormulaire formulaire_link[Number_of_link_items_total];


extern type_lien_no_nom lien_no_nom_type_groupe[nbre_type_groupes];
extern type_lien_no_nom lien_no_nom_type_link[nbre_type_links];

GtkWidget *get_menubar_menu(TxDonneesFenetre * onglet_leto);
void show_popup_menu(int popup_menu_type, GdkEventButton *event, TxDonneesFenetre *onglet_leto);
void show_status(TxDonneesFenetre *onglet_leto, const char *fmt, ...);
/* void change_dir(); */
char *get_file_extension(const char *file);
void set_file_with_ext(char *file, const char *ext);
void set_title(TxDonneesFenetre *onglet_leto);
void scroll_window(int x, int y,TxDonneesFenetre *onglet_leto);
GtkWidget *lookup_widget(GtkWidget * widget, const gchar * widget_name);

void scroll(TxPoint point, TxDonneesFenetre *onglet_leto);

extern int modified;

/* fonction a moi */
#ifndef AVEUGLE
void create_menubar_leto(t_gennet_script *pscript, GtkWidget *vbox, int nPage);
GtkWidget* create_scroll_leto(TxDonneesFenetre *onglet_leto, GtkWidget* viewport1, GtkWidget *vbox);
void create_drawingArea_leto(t_gennet_script *script_gui, GtkWidget *viewport1);
void create_dialog_leto(TxDonneesFenetre *onglet_leto);
void create_status_bar_leto(t_gennet_script *script_gui, GtkWidget *vbox);
extern void get_menu_items(GtkActionEntry *menu);
#endif
#endif
