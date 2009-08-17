#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>


/****************************/




extern void entry_group_callback_no_name(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_nom(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_taillex(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_tailley(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_seuil(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_learning_rate(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_simulation_speed(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_type2(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_ech_temps(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_tolerance(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_alpha(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_nbre_de_1(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_dvp(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_dvn(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_sigma_f(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void combo_debug_groupe_callback(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_noise_level(GtkWidget * widget,  GtkWidget * entry, TxDonneesFenetre * onglet_leto);

extern void combo_reverse_groupe_callback(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void entry_group_callback_plan(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);

/***************************/

extern void on_aide1_activate(GtkMenuItem * menuitem, gpointer user_data);

extern void group_validate_button_callback(GtkWidget * widget, gpointer data);
extern void group_cancel_button_callback(GtkWidget * widget, gpointer data);

extern void check_button_reverse_callback(GtkWidget * widget, gpointer data);
extern void check_button_debug_callback(GtkWidget * widget, gpointer data);

extern void enter_callback(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);

extern void entry_group_callback(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_taille_x_callback(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void combo_nom_groupe_callback(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre * onglet_leto);
extern void combo_nom_groupe_changed(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_toggle_editable(GtkWidget * checkbutton, GtkWidget * entry);
extern void entry_toggle_visibility(GtkWidget * checkbutton,
                                    GtkWidget * entry);

extern void print_hello(GtkWidget * widget, gpointer data);

extern void print_toggle(gpointer callback_data, guint callback_action,
                         GtkWidget * menu_item);
void print_selected(GtkAction *action, gpointer callback_data);
extern void file_quit_cmd_callback(GtkWidget * widget, gpointer data);


extern gboolean scribble_motion_notify_event(GtkWidget * widget,
                                             GdkEventMotion * event,
                                             gpointer data);
extern gboolean scribble_expose_event(GtkWidget * widget,
                                      GdkEventExpose * event, gpointer data);
extern gboolean scribble_configure_event(GtkWidget * widget,
                                         GdkEventConfigure * event,
                                         gpointer data);


extern void file_ok_sel(GtkWidget * w, gpointer data);
extern void file_cancel_sel(GtkWidget * w, gpointer data);
extern void creer_acces_fichier(GtkWidget * w, gpointer data);


extern void refresh(GtkWidget * widget, gpointer data);
extern void save(GtkWidget * widget, gpointer data);



/*********************************************/


extern void entry_link_callback_input(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_output(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_temps(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_stemps(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_nbre(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_secondaire(GtkWidget * widget,
                                           GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_mode(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_dvx(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_dvy(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_proba(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_name(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_style(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void entry_link_callback_norme(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);



extern void combo_type_link_callback(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void combo_mode_link_callback(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void combo_type_link_changed(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void combo_mode_link_changed(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto);
extern void link_validate_button_callback(GtkWidget * widget, gpointer data);
extern void link_cancel_button_callback(GtkWidget * widget, gpointer data);

#ifdef EXPORT
extern void export_cb(GtkWidget * widget, gpointer data);
#endif

void new_script(GtkWidget *widget, gpointer data);
void open_file(GtkWidget * widget, gpointer data);
void save_file(GtkWidget * widget, gpointer data);
void save_file_as(GtkWidget * widget, gpointer data);

extern void sauve_selection(GtkWidget * widget, gpointer data);
extern void sauve_selection_a_plat(GtkWidget * widget, gpointer data);
extern void copy_selection(GtkWidget * widget, gpointer data);
extern void paste_selection(GtkWidget * widget, gpointer data);


void slide_groups_up(GtkWidget * widget, gpointer data);
void slide_groups_down(GtkWidget * widget, gpointer data);
void slide_groups_left(GtkWidget * widget, gpointer data);
void slide_groups_right(GtkWidget * widget, gpointer data);

void about();

void modify_callback(GtkWidget * widget, gpointer data);
void delete_callback(GtkWidget * widget, gpointer data);

void dupliquer_coude_callback(GtkWidget * widget, gpointer data);
void delete_coude_callback(GtkWidget * widget, gpointer data);
void delete_all_coudes_callback(GtkWidget * widget, gpointer data);

void about(GtkWidget * widget, gpointer data);
void edit_config(GtkWidget * widget, gpointer data);
gboolean set_binary_file(GtkWidget * widget, gpointer data);
gboolean leto_quit(GtkWidget * widget, gpointer data);
void changedx(GtkWidget * widget, gpointer data);
void changedy(GtkWidget * widget, gpointer data);
void resized_cb(GtkWidget * widget, gpointer data);
void reload_script(GtkWidget * widget, gpointer data);

void block_transfo(GtkWidget * widget, gpointer data);

/* #ifndef SYMBOLIQUE_VERSION */
void creation_cb(GtkWidget * widget, gpointer data);
/* #endif */

/* fonction a moi */
void changed_tab(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer data);
void menu_addLeto (GtkUIManager * p_uiManager, GtkWidget * p_widget, gpointer data);
void                tabLeto_removed(GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer data);
gboolean tabLeto_quit(GtkWidget * widget, gpointer data);
int find_new_name_script(TxDonneesFenetre *onglet_leto, int option);
int erase_without_saving(gpointer data);

gboolean mouse_clicked(GtkWidget * widget, GdkEventButton * event, gpointer data);
gboolean mouse_released(GtkWidget * widget, GdkEventButton * event, gpointer data);

void set_random_gen(GtkWidget * widget, gpointer data);
#endif
