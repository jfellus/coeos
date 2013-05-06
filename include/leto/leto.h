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
