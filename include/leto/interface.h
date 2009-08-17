#ifndef INTERFACE_H
#define INTERFACE_H

#ifndef AVEUGLE
#include "gennet.h"
#endif

#define POPUP_MENU_NO_SELECTION 0
#define POPUP_MENU_GROUP 1
#define POPUP_MENU_LINK 2
#define POPUP_MENU_BEND 3

extern void create_fenetre1(TxDonneesFenetre * onglet_leto);
extern GtkWidget* create_read_link(TxDonneesFenetre *onglet_leto);
extern GtkWidget* create_read_group(TxDonneesFenetre *onglet_leto);

extern void create_fenetre_dialogue(TxDonneesFenetre * fenetre, TxDonneesFenetre *onglet_leto);

GtkWidget *entry_group;
GtkWidget *entry_taille_x;

extern GtkWidget *reverse_button;
extern GtkWidget *debug_button;
extern GtkWidget *reset_pos_button;
extern GtkWidget *combo_type_groupe_entry;
extern GtkWidget *combo_type_link_entry;
extern GtkWidget *combo_mode_link_entry;
extern GtkWidget *combo_reverse_groupe;
extern GtkWidget *combo_debug_groupe;
/*-------------------------------------------------------------*/

typedef struct type_lien_no_nom
{
    int no;
    char nom[256];
    int editable[256];          /* nombre max d'item que les liens ou les groupes peuvent avoir ... */

} type_lien_no_nom;


/*-------------------------------------------------------------*/

#define No_item_groupe		0
#define No_item_nom 		1
#define No_item_taillex 	2
#define No_item_tailley 	3
#define No_item_nbre 		4
#define No_item_seuil 		5
#define No_item_learning_rate 	6
#define No_item_simulation_speed 7
#define No_item_type2 		8
#define No_item_ech_temps 	9
#define No_item_noise_level     10  /* ajout recent PG - place liee a pos dans IHM */
#define No_item_tolerance 	11
#define No_item_alpha 		12
#define No_item_nbre_de_1	13
#define No_item_dvp 		14
#define No_item_dvn 		15
#define No_item_sigma_f 	16
#define No_item_plan     	17

#define Number_of_items         18

#define No_item_comment     	18


#define Number_of_items_total 	19

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

#define No_mode_link_product_compet 0   /* value obtained after compet */
#define No_mode_link_distance_compet 1
#define No_mode_link_product_analog 2   /* direct analog value */
#define No_mode_link_distance_analog 3

#define Number_of_mode_links 4


/*----------------------------------------*/


extern TxChampsFormulaire formulaire_group[Number_of_items_total];
extern TxChampsFormulaire formulaire_link[Number_of_link_items_total];


extern type_lien_no_nom lien_no_nom_type_groupe[nbre_type_groupes];
extern type_lien_no_nom lien_no_nom_type_link[nbre_type_links];
extern type_lien_no_nom lien_no_nom_mode_link[Number_of_mode_links];

GtkWidget *get_menubar_menu(TxDonneesFenetre * onglet_leto);
void show_popup_menu(int popup_menu_type, GdkEventButton *event, TxDonneesFenetre *onglet_leto);
void show_status(TxDonneesFenetre *onglet_leto, const char *fmt, ...);
void set_widgets_sensitivity(TxDonneesFenetre *onglet_leto);
/* void change_dir(); */
char *get_file_extension(char *file);
void set_file_with_ext(char *file, char *ext);
void set_title(TxDonneesFenetre *onglet_leto);
void scroll_window(int x, int y,TxDonneesFenetre *onglet_leto);
GtkWidget *lookup_widget(GtkWidget * widget, const gchar * widget_name);

void scroll(TxPoint point, TxDonneesFenetre *onglet_leto);

extern int modified;

/* fonction a moi */
#ifndef AVEUGLE
void create_menubar_leto(t_gennet_script *pscript, GtkWidget *vbox, int nPage);
GtkWidget* create_scroll_leto(TxDonneesFenetre *onglet_leto, GtkWidget* viewport1, GtkWidget *vbox);
void create_drawingArea_leto(TxDonneesFenetre *onglet_leto, GtkWidget *viewport1);
void create_dialog_leto(TxDonneesFenetre *onglet_leto);
void create_status_bar_leto(TxDonneesFenetre *onglet_leto, GtkWidget *vbox);
extern void get_menu_items(GtkActionEntry *menu);
#endif
#endif
