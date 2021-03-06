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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gdk/gdkkeysyms.h>

#include <group_function_pointers.h>
#include "gere_coudes.h"
#include "manage_comments.h"
#include "interface.h"
#include "callbacks.h"
#include "find_and_manage.h"
#include "draw_leto.h"
#include "leto.h"
#include "outils.h"

type_group_function_pointers NN_Core_function_pointers[] =
{

{ "debut", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_debut", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_stop_if_non_rt", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_min_period", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_ca3", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_ca3_2", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_qlearning", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_assocond", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_spike_prediction", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_vigilance2neurone", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_neurone2vigilance", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_flip_flop", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_transition_detect", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_even_first_transition_detect", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_STM", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_grabimages_cleanbuffer", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_amplifi", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_clean_next", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_clean_next_conditionel", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_cleans_conditionel", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_clean", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_cleans", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_raise_fault", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_DG_1", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_DG_t2", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_DG_v3", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_WTA_sorin", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_TimeGenerator", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_InitialiseHippo", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_EC", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_base_de_temps", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_observable_moyen", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_recupere_micro_neurone", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_seuil_et_memoire", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_ctrnn", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_lissage_temp", NULL, NULL, NULL, NULL, NULL, -1, -1 },
{ "f_fin_echelle", NULL, NULL, NULL, NULL, NULL, -1, -1 },


/* pour indiquer la fin du tableau*/
{ NULL, NULL, NULL, NULL, NULL, NULL, -1, -1 } };

char bus_id[]; // Pour eviter un problème au chargement des lib promethe



#define MODULE_REQUIRED 1

type_lien_no_nom lien_no_nom_type_groupe[nbre_type_groupes] =
{ /* par defaut tous les champs sont editables dans l'IHM */
{ No_Hebb, "Hebb",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 0 */
{ No_Winner, "Winner",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 1 */
{ No_Kohonen, "Kohonen",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 2 */
{ No_Widrow, "Pavlov (output = f(CR+UR))",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 3 */
{ No_Special, "Special",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 4 */
{ No_Ou, "OR",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 5 */
{ No_Et, "AND",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 6 */
{ No_Entree, "Input",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 7 */
{ No_Sortie, "Output",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 8 */
{ No_PTM, "PTM",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 9 */
{ No_Hebb/*_Seuil*/, "Hebb_Threshold",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 10 */
{ No_Winner_Special, "Winner_Special",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 11 */
{ No_Hopfield, "Hopfield",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 12 */
{ No_Winner_Colonne, "Winner_Column",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 13 */
{ No_Fonction_Algo, "Algo_Function",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 14 */
{ No_But, "Cognitive map (goals)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 15 */
{ No_Pyramidal, "Pyramidal Cells",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 16 */
{ No_Inter_neurone, "Inter_neuron",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 17 */
{ No_Fonction_Algo_mvt, "Algo mvt Function",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 18 */
{ No_Granular, "Granular Cells",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 19 */
{ No_Micro_Pyramid, "Micro_Pyramid",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 20 */
{ No_SV_cor, "SV_cor",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 21 */
{ No_Dyn, "Dyn",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 22 */
{ No_SAW, "SAW",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 23 */
{ No_Sigma_PI, "Sigma_PI",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 24 */
{ No_Winner_Selectif, "Selective Winner",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 25 */
{ No_Macro_Colonne, "Macro Column",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 26 */
{ No_BREAK, "BREAK",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 27 */
{ No_LMS, "LMS (not NN, output=CR)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 28 */
{ No_RTTOKEN, "RT token",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 29 */
{ No_KO_Discret, "Chaos Discret",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 30 */
{ No_KO_Continu, "Chaos Continu",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 31 */
{ No_Hebb_Seuil_binaire, "Hebb_Threshold_binaire",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 32 */
{ No_Pyramidal_plan, "Pyramidal for plan",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 33 */
{ No_LMS_2, "LMS_2",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 34 */
{ No_BCM, "BCM",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 35 */
{ No_PLG, "PLG",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 36 */
{ No_Mean_Mvt, "Mean_Mvt",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 37 */
{ No_Asso_Cond, "Asso_Cond",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 38 */
{ No_Sub_Network, "Sub_network",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 39 */
{ No_LMS_delayed, "LMS_delayed",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 40 */
{ No_Winner_Macro, "Winner_Macro",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 41 */
{ No_NLMS, "NLMS",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 42 */
{ No_Sutton_Barto, "Sutton_Barto",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 43 */
{ No_PCR, "PCR",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 44 */
{ No_Selverston, "Selverston",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 45 */
{ No_RCO, "RankCodingOrder",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 46:Alex */
{ No_selective_winner_modulated, "Selective Winner Modulated",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /* 47 */
{No_Kmean_R,"Kmean_R",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }, /*48:Nils*/
{No_CTRNN2,"CTRNN2",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } } /*49:Nils*/
};

static GtkToggleActionEntry menu_toggle_items[] =
{
{ "/View/Unselect plane/Plane 1", NULL, "Plane 1", NULL, NULL, G_CALLBACK(print_selected), 0 },
{ "/View/Unselect plane/Plane 2", NULL, "Plane 2", NULL, NULL, G_CALLBACK(print_selected), 0 },
{ "/View/Unselect plane/Plane 3", NULL, "Plane 3", NULL, NULL, G_CALLBACK(print_selected), 0 },
{ "/View/Unselect plane/Plane 4", NULL, "Plane 4", NULL, NULL, G_CALLBACK(print_selected), 0 },
{ "/View/Unselect plane/Plane 5", NULL, "Plane 5", NULL, NULL, G_CALLBACK(print_selected), 0 },
{ "/View/Unselect plane/Plane 6", NULL, "Plane 6", NULL, NULL, G_CALLBACK(print_selected), 0 } };

/* Our menu, an array of GtkActionEntry structures that defines each menu item
 * { NomAction, image predefinie, label, raccourci clavier, tooltip, callback } */
static GtkActionEntry menu_items[] =
{
{ "/_File", NULL, "_File", NULL, NULL, NULL },
#ifdef LETO
    { "/File/_New script", GTK_STOCK_NEW, "_New script", "<Control>N", "New script", G_CALLBACK(new_script)},
    { "/File/_Open...", GTK_STOCK_OPEN, "_Open script", "<Control>O", "Open...", G_CALLBACK(open_file)},
#endif
    { "/File/_Insert script...", NULL, "_Insert script...", NULL, NULL, G_CALLBACK(creer_acces_fichier) },
    { "/File/_Save", GTK_STOCK_SAVE, "_Save", "<Control>S", "Save", G_CALLBACK(save_file) },
    { "/File/Save _As...", GTK_STOCK_SAVE_AS, "Save _As...", "<Shift><Control>S", "Save As", G_CALLBACK(save_file_as) },
    { "/File/Reload script", GTK_STOCK_REVERT_TO_SAVED, "Reload script", NULL, "Reload script", G_CALLBACK(reload_script) },
    { "/File/Edit variables", NULL, "Edit variables", "<Shift><Control>V", "Edit variables", G_CALLBACK(edit_variables) },
    { "/File/_Save _selection", GTK_STOCK_SAVE_AS, "_Save _selection", NULL, "Save selection", G_CALLBACK(sauve_selection) },
    { "/File/_Save _selection _flat", GTK_STOCK_SAVE_AS, "_Save _selection _flat", NULL, "Save selection flat", G_CALLBACK(sauve_selection_a_plat) },
    { "/File/_Block _transfo", NULL, "_Block _transfo", "<Release>B", "Block transfo", G_CALLBACK(block_transfo) },

    /*si libg2 on compile en incluant le menu export
     * actuellement non implementee dans le fichier xml */
#ifdef EXPORT
    { "/File/_Export...", GTK_STOCK_CONVERT, "_Export...", "<Control>E", "Export", G_CALLBACK(export_cb)},
#endif
#ifdef LETO
    { "/File/_Quit", GTK_STOCK_QUIT, "_Quit", "<Control>Q", "Quit", G_CALLBACK(leto_quit)},
#else
    { "/File/_Quit", GTK_STOCK_QUIT, "_Quit", "<Control>Q", "Quit", G_CALLBACK(tabLeto_quit) },
#endif

    { "/_Edit", NULL, "_Edit", NULL, NULL, NULL },

    { "/Edit/Undo", GTK_STOCK_CANCEL, "Undo", "<Control>Z", "Undo", G_CALLBACK(callback_undo) },
    { "/Edit/Redo", GTK_STOCK_CANCEL, "Redo", "<Shift><Control>Z", "Redo", G_CALLBACK(callback_redo) },
    { "/Edit/Copy", GTK_STOCK_COPY, "Copy", "<Control>C", "Copy", G_CALLBACK(copy_selection) },
    { "/Edit/Paste", GTK_STOCK_PASTE, "Paste", "<Control>V", "Paste", G_CALLBACK(paste_selection) },
    { "/Edit/Search group Name", GTK_STOCK_FIND, "Search Name", "<Control>F", "Search group Name", G_CALLBACK(callback_search_group_by_name) },
    { "/Edit/Find group No", GTK_STOCK_INDEX, "Find No", "<Shift><Control>F", "Find group No", G_CALLBACK(callback_find_group) },
    { "/Edit/Compile _NN", GTK_STOCK_EXECUTE, "Compile _NN", "<Release>C", "Compile_NN", G_CALLBACK(creation_cb) },
    { "/Edit/Compile _script", GTK_STOCK_CONVERT, "Compile _script", "<Shift><Control>C", "Compile_script", G_CALLBACK(compile_script_cb) },
    /*{"/Edit/Edit configuration... (BETA)", "<CTRL>F",  edit_config,          0, "<StockItem>", GTK_STOCK_PREFERENCES },*/
    { "/Edit/Binary file (.res)...", GTK_STOCK_PREFERENCES, "Binary file (.res)...", NULL, "Binary file (.res)...", G_CALLBACK(set_binary_file) },
    { "/Edit/Random generator...", GTK_STOCK_PREFERENCES, "Random generator...", NULL, "Random generator...", G_CALLBACK(set_random_gen) },

    { "/_View", NULL, "_View", NULL, NULL, NULL },
    { "/View/Refresh", GTK_STOCK_REFRESH, "Refresh", "<Release>F5", "Refresh", G_CALLBACK(refresh) },
    { "/View/Invert colors", NULL, "Invert colors", "<Control>R", NULL, G_CALLBACK(invert_colors) },
    { "/View/Show _Hidden sub_networks", NULL, "Show _Hidden sub_networks", "<Control>H", NULL, G_CALLBACK(show_hide_sub_networks) },
    { "/View/Zoom In", GTK_STOCK_ZOOM_IN, "Zoom In", "<Control>equal", "Zoom In", G_CALLBACK(zoom_in) },
    { "/View/Zoom Out", GTK_STOCK_ZOOM_OUT, "Zoom Out", "<Control>minus", "Zoom Out", G_CALLBACK(zoom_out) },
    /*{ "/View/Zoom",                  "z",              zoom,                 0, "<StockItem>", GTK_STOCK_ZOOM_100 },*/

    { "/View/Unselect plane", NULL, "Unselect plane", NULL, NULL, NULL },
    /*{"/View/Unselect plane/Check",   NULL,             print_toggle,         1, "<CheckItem>"},*/
    /*{"/View/Unselect plane/sep",     NULL,             NULL,                 0, "<Separator>"},*/
    /*{ "/View/Select plane/All",      NULL,             print_selected,      -1, "<CheckItem>" },*/
    { "/View/Slide groups up", NULL, "Slide groups up", "<ALT>Up", NULL, G_CALLBACK(slide_groups_up) },
    { "/View/Slide groups down", NULL, "Slide groups down", "<ALT>Down", NULL, G_CALLBACK(slide_groups_down) },
    { "/View/Slide groups left", NULL, "Slide groups left", "<ALT>Left", NULL, G_CALLBACK(slide_groups_left) },
    { "/View/Slide groups right", NULL, "Slide groups right", "<ALT>Right", NULL, G_CALLBACK(slide_groups_right) },

    { "/_Group", NULL, "_Group", NULL, NULL, NULL },
    { "/Group/_New", GTK_STOCK_ADD, "_New group", "<Release>G", "New", G_CALLBACK(creation_groupe) },
    { "/Group/_Modify", NULL, "_Modify", "<Release>M", NULL, G_CALLBACK(modify_callback) },
    { "/Group/_Automatic rename", NULL, "_Automatic rename", "<Control>A", NULL, G_CALLBACK(automatic_rename_callback) },
    { "/Group/_Automatic comment", NULL, "_Automatic comment", "<Control>u", NULL, G_CALLBACK(automatic_comment_callback) },
    { "/Group/_Delete", GTK_STOCK_DELETE, "_Delete", "<Release>Delete", "Delete", G_CALLBACK(delete_callback) },
    { "/Group/_Reverse", GTK_STOCK_REDO, "_Reverse", "<Release>R", "Reverse", G_CALLBACK(fonction_reverse) },
    { "/_Link", NULL, "_Link", NULL, NULL, NULL },
    { "/Link/_New", GTK_STOCK_ADD, "_New link", "<Release>L", "New", G_CALLBACK(create_link_callback) },
    { "/Link/_New one-to-one", GTK_STOCK_ADD, "_New one-to-one link", "<Release>O", "New", G_CALLBACK(create_one_to_one_link_callback) },
    { "/Link/_Modify", NULL, "_Modify Link", "M", NULL, G_CALLBACK(modify_callback) },
    { "/Link/_Delete", GTK_STOCK_DELETE, "_Delete Link", "<Release>D", "Delete", G_CALLBACK(delete_callback) },

    { "/_Coude", NULL, "_Coude", NULL, NULL, NULL },
    { "/Coude/_Insert", GTK_STOCK_PASTE, "_Insert Bend", "<Release>I", "Insert", G_CALLBACK(dupliquer_coude_callback) },
    { "/Coude/_Delete", GTK_STOCK_DELETE, "_Delete Bend", "<Control>D", "Delete", G_CALLBACK(delete_coude_callback) },
    { "/Coude/_Delete All", GTK_STOCK_DELETE, "_Delete All Bends", "<Release>A", "Delete All", G_CALLBACK(delete_all_coudes_callback) },

    { "/_Help", NULL, "_Help", NULL, NULL, NULL },
    { "/Help/_About", GTK_STOCK_ABOUT, "_About", "<Release>F1", "About", G_CALLBACK(about) }, };

/* static gint nmenu_items = sizeof(menu_items) / sizeof(menu_items[0]); */



/* Ces variables ne servent à rien, c'est juste pour que les librairies puissent charger, sinon elle cherchent des variables
 qui ne sont définies nul part
 */




typedef struct
{
  void * rien;
} motivation;
typedef struct TAG_LIST_RFFTWND_PLAN
{
  void * rien;
} list_rfftwnd_plan;
int MOTOR_LOGICAL_STATUS[1], alealearn, nbre_liaison, EMISSION_ROBOT, learn_NN, nbre_entree, NbrFeaturesPoints, global_learn, echelle_temps[1], USE_ROBOT, continue_simulation_status;
type_tableau_voies liaison;
type_neurone *neurone;
type_groupe def_groupe[1];
pthread_mutex_t mutex_lecture_clavier, mutex_list_prom_fft;
char vrai_nom_image[1], GLOB_path_SAVE[1], args;
float global_temps, dP, MOTOR_1, MOTOR_2, MOTOR_3, MOTOR_4, MOTOR_5, Niveau_ressources[1], vigilence, Orientation, eps;
motivation motivations[1];
struct timeval prom_started_date, SystemTime;
typedef long snd_pcm_sframes_t;
snd_pcm_sframes_t snd_pcm_mmap_readn, snd_pcm_readi, snd_pcm_readn, snd_pcm_mmap_writei, snd_pcm_writei, snd_pcm_mmap_writen, snd_pcm_writen, snd_pcm_mmap_readi;
typedef struct _snd_config snd_config_t;
snd_config_t *snd_config;
list_rfftwnd_plan *list_prom_fft = NULL;


void vkprints(const char *fmt, va_list ap)
{
    vfprintf(stdout, fmt, ap);
}

void kprints(const char *fmt, ...) /* version simplifiee du kernel pour permettre la compilation des librairies*/
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}
/*
void print_warning(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    kprints("\n\033[1;33m %s \t %s \t %i :\n \t Warning: ", name_of_file, name_of_function, numero_of_line);
    vkprints(message, arguments);
    kprints("\033[0m\n\n");
    va_end(arguments);
}

void fatal_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    kprints("\n\033[1;31m %s \t %s \t %i :\n \t Error: ", name_of_file, name_of_function, numero_of_line);
    vkprints(message, arguments);
    kprints("\033[0m\n\n");
    va_end(arguments);
    raise(SIGABRT);
    exit(EXIT_FAILURE);
}
*/
/*
 * Envoie un message d'erreur avec name_of_file, name_of_function, number_of_line et affiche le message formate avec les parametres variables.
 * Puis exit le programme avec le parametre EXIT_FAILURE.
 */

/* 
void fatal_system_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    kprints("\n\033[1;31m %s \t %s \t %i :\n \t Error: ", name_of_file, name_of_function, numero_of_line);
    vkprints(message, arguments);
    kprints("System error: %s\n\n", strerror(errno));
    kprints("\033[0m\n\n");
    raise(SIGABRT);
    va_end(arguments);
    exit(EXIT_FAILURE);
}
*/

/* Permet de recuperer le menu_items dans d'autres sources */
void get_menu_items(GtkActionEntry *menu)
{
  (void)menu;
  menu = menu_items;
}

void init_group_visibility_properties()
{
  int *t = NULL;
  int i = 0;
  for (i = 0; i < nbre_type_groupes; i++)
  {
    lien_no_nom_type_groupe[i].editable[No_item_nbre] = 0;
  }
  /* 0 */
  t = lien_no_nom_type_groupe[No_Hebb].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  /*t[No_item_seuil]=-1; *//*Mathias 15/06/05 */
  /*t[No_item_simulation_speed]=-1; */

  t = lien_no_nom_type_groupe[No_Hebb_Seuil].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  /**************************/

  t = lien_no_nom_type_groupe[No_Winner].editable; /* fait */
  t[No_item_tolerance] = t[No_item_alpha] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Winner_Macro].editable;
  t[No_item_tolerance] = t[No_item_alpha] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Winner_Special].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Winner_Colonne].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Winner_Selectif].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = -1;
  t[No_item_simulation_speed] = -1;

  /**************************/

  t = lien_no_nom_type_groupe[No_Kohonen].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_seuil] = -1;

  t = lien_no_nom_type_groupe[No_Widrow].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_BCM].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Mean_Mvt].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_LMS_2].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Special].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  /***************************/

  t = lien_no_nom_type_groupe[No_Ou].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_seuil] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Et].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_seuil] = -1;
  t[No_item_simulation_speed] = -1;

  /***************************  ne sont plus utilises */

  t = lien_no_nom_type_groupe[No_Entree].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_seuil] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Sortie].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_seuil] = -1;
  t[No_item_simulation_speed] = -1;

  /***************************/

  t = lien_no_nom_type_groupe[No_PTM].editable;
  t[No_item_noise_level] = -1;
  /* rien d'interdit !!! */

  t = lien_no_nom_type_groupe[No_Hopfield].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  /*  t[No_item_seuil]=-1; */

  t = lien_no_nom_type_groupe[No_Fonction_Algo].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_But].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  /*  t[No_item_seuil]=-1; */

  t = lien_no_nom_type_groupe[No_Pyramidal].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_Pyramidal_plan].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_NLMS].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_Inter_neurone].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_Fonction_Algo_mvt].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Granular].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_Micro_Pyramid].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_SV_cor].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1; /* t[No_item_dvn]=-1; */
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_Dyn].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_SAW].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_Kmean_R].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_Sigma_PI].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_seuil] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Macro_Colonne].editable; /* fait */
  t[No_item_noise_level] = -1;
  /*t[No_item_dvp]=-1; t[No_item_dvn]=-1; */
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_BREAK].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_LMS].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_seuil] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_RTTOKEN].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_KO_Discret].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = 1;
  t[No_item_sigma_f] = 1;
  t[No_item_alpha] = t[No_item_nbre_de_1] = -1;

  t = lien_no_nom_type_groupe[No_KO_Continu].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = 1;
  t[No_item_sigma_f] = 1;
  t[No_item_alpha] = t[No_item_nbre_de_1] = -1;

  t = lien_no_nom_type_groupe[No_Hebb_Seuil_binaire].editable; /* fait */
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_alpha] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_PLG].editable;
  /* PG: champs a inhiber ? je ne sais pas ... */

  t = lien_no_nom_type_groupe[No_Asso_Cond].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  /*39*/
  t = lien_no_nom_type_groupe[No_Sub_Network].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;

  t = lien_no_nom_type_groupe[No_LMS_delayed].editable; /* fait */
  t[No_item_tolerance] = t[No_item_alpha] = -1;
  t[No_item_simulation_speed] = -1;

  t = lien_no_nom_type_groupe[No_Selverston].editable; /* fait */
  t[No_item_seuil] = -1;
  t[No_item_learning_rate] = -1;
  t[No_item_simulation_speed] = -1;
  t[No_item_type2] = -1;
  t[No_item_noise_level] = -1;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = -1;
  t[No_item_alpha] = -1;
  t[No_item_nbre_de_1] = -1;
  t[No_item_sigma_f] = 1;

  /* ALEX */
  t = lien_no_nom_type_groupe[No_RCO].editable;
  t[No_item_noise_level] = -1;
  t[No_item_dvp] = -1;
  t[No_item_dvn] = -1;
  t[No_item_tolerance] = t[No_item_nbre_de_1] = t[No_item_sigma_f] = -1;
}

/* fenetre pour le find */
void create_fenetre_dialogue(TxDonneesFenetre * fenetre, TxDonneesFenetre *onglet_leto)
{
  GtkWidget *content_area;
  GtkWidget *entry;
  GtkWidget *Winmain;

#ifndef LETO
  Winmain = lookup_widget(onglet_leto->window, "Winmain");
#else
  Winmain = onglet_leto->window;
#endif

  fenetre->window = gtk_dialog_new_with_buttons("Find symbolic name", GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
  gtk_window_set_default_size(GTK_WINDOW(fenetre->window), 250, 100);

  content_area = ((GtkDialog *) fenetre->window)->vbox;
  g_signal_connect(GTK_OBJECT(fenetre->window), "delete_event", GTK_SIGNAL_FUNC(callback_hide_find_group), NULL); /* definie dans find_and_manage.c */

  entry = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(entry), 250);
  /*g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(find_group), (gpointer) onglet_leto);*/
  gtk_entry_set_text(GTK_ENTRY(entry), "Tapez ici le groupe a chercher !");
  gtk_editable_select_region(GTK_EDITABLE(entry), 0, GTK_ENTRY(entry)->text_length);
  gtk_container_add(GTK_CONTAINER(content_area), entry);

  gtk_widget_show(entry);
  fenetre->entry = entry;
}

/********************************************************************************/

#ifndef SYMBOLIQUE_VERSION
#define NAME "METALETO"
#else
#define NAME "SYMBOLIC METALETO"
#endif

/**
 * Creates the main window
 */

/* creation du status bar */
void create_status_bar_leto(t_gennet_script *script_gui, GtkWidget *vbox)
{
  GtkWidget *statusbar;

  statusbar = gtk_statusbar_new();
  g_object_ref(statusbar);
  g_object_set_data_full(G_OBJECT(script_gui->onglet_leto->window), "statusbar", statusbar, (GDestroyNotify) g_object_unref);
#ifndef LETO
  gtk_box_pack_end(GTK_BOX(script_gui->onglet_leto->window), statusbar, FALSE, TRUE, 0);
#else
  gtk_box_pack_end(GTK_BOX(vbox), statusbar, FALSE, TRUE, 0);
#endif
}

/* creation de la zone de dessin */
void create_drawingArea_leto(t_gennet_script *script_gui, GtkWidget *viewport1)
{
  script_gui->onglet_leto->da = gtk_drawing_area_new();
  gtk_widget_set_size_request(script_gui->onglet_leto->da, taille_max_fenetre1_x, taille_max_fenetre1_y);
  script_gui->onglet_leto->width = taille_max_fenetre1_x;
  script_gui->onglet_leto->height = taille_max_fenetre1_y;


  gtk_container_add(GTK_CONTAINER(viewport1), script_gui->onglet_leto->da);

  script_gui->onglet_leto->pixmap = NULL; /* voir scribble_configure_event pour l'init
   * et l'affichage */
  script_gui->onglet_leto->filew = NULL;

  g_signal_connect(script_gui->onglet_leto->da, "expose_event", G_CALLBACK(scribble_expose_event), script_gui);
  g_signal_connect(script_gui->onglet_leto->da, "configure_event", G_CALLBACK(scribble_configure_event), script_gui);

  /* Event signals */

  /*select rectangle */
  g_signal_connect(script_gui->onglet_leto->da, "motion_notify_event", G_CALLBACK(scribble_motion_notify_event), script_gui);
  g_signal_connect(script_gui->onglet_leto->da, "button_press_event", G_CALLBACK(mouse_clicked), script_gui);
  g_signal_connect(script_gui->onglet_leto->da, "button_release_event", G_CALLBACK(mouse_released), script_gui);
  g_signal_connect(script_gui->onglet_leto->da, "scroll-event", G_CALLBACK(mouse_scroll), script_gui);


  /* Ask to receive events the drawing area doesn't normally
   * subscribe to
   */
  gtk_widget_set_events(script_gui->onglet_leto->da, /*gtk_widget_get_events(script_gui->onglet_leto->da) |*/ GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_EXPOSURE_MASK | GDK_SCROLL_MASK);
}

/* creation des scrolls de leto */
GtkWidget *create_scroll_leto(TxDonneesFenetre *onglet_leto, GtkWidget* viewport1, GtkWidget *vbox)
{
  GtkWidget *scrolledwindow1;

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  g_object_ref(scrolledwindow1);
  g_object_set_data_full(G_OBJECT(onglet_leto->window), "scrolledwindow1", scrolledwindow1, (GDestroyNotify) g_object_unref);
#ifndef LETO
  gtk_box_pack_start(GTK_BOX(onglet_leto->window), scrolledwindow1, TRUE, TRUE, 0);
#else
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow1, TRUE, TRUE, 0);
#endif
  viewport1 = gtk_viewport_new(NULL, NULL);
  g_object_ref(viewport1);
  g_object_set_data_full(G_OBJECT(onglet_leto->window), "viewport1", viewport1, (GDestroyNotify) g_object_unref);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), viewport1);

  onglet_leto->adj_h = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolledwindow1));
  g_signal_connect(onglet_leto->adj_h, "value_changed", G_CALLBACK(changedx), onglet_leto);
  onglet_leto->adj_v = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow1));
  g_signal_connect(onglet_leto->adj_v, "value_changed", G_CALLBACK(changedy), onglet_leto);
  return viewport1;
}

/* creation du menu de leto */
void create_menubar_leto(t_gennet_script *pscript, GtkWidget *vbox, int nPage)
{
  GtkUIManager *menubar;
  GtkAccelGroup *accel_group;
  GtkWidget *WinMain;
  GtkActionGroup *actionGroup;
  char *env = NULL;
  char menu_path[PATH_MAX];
  char id_ag[1024];
  (void)nPage;

#ifndef LETO
  WinMain = lookup_widget(pscript->onglet_leto->window, "Winmain");
#else
  WinMain = pscript->onglet_leto->window;
#endif

  /* creation du menubar */
  menubar = gtk_ui_manager_new();

  /* add ActionGroup */
  sprintf(id_ag, "action_group");
  actionGroup = gtk_action_group_new(id_ag);
  gtk_action_group_add_actions(actionGroup, menu_items, G_N_ELEMENTS(menu_items), pscript);
  gtk_action_group_add_toggle_actions(actionGroup, menu_toggle_items, G_N_ELEMENTS(menu_toggle_items), pscript);
  gtk_ui_manager_insert_action_group(menubar, actionGroup, 0);
  pscript->action_group = actionGroup;

  /* creation des accelerateurs */
  accel_group = gtk_ui_manager_get_accel_group(menubar);
  gtk_window_add_accel_group(GTK_WINDOW(WinMain), accel_group);

  /* tout le contenu du menubar est repertorie dans un fichier xml que l'on charge */
  env = getenv("HOME");
#ifdef LETO
  sprintf(menu_path, "%s/bin_leto_prom/menu_leto.xml", env);
#else
  sprintf(menu_path, "%s/bin_leto_prom/menu_leto_coeos.xml", env);
#endif

  gtk_ui_manager_add_ui_from_file(menubar, menu_path, NULL );

  g_signal_connect(menubar, "add_widget", G_CALLBACK(menu_addLeto), vbox);

  /* on memorise le menubar pour y avoir acces facilement */
  g_object_ref(menubar);
  g_object_set_data_full(G_OBJECT(pscript->onglet_leto->window), "menubar", menubar, (GDestroyNotify) g_object_unref);
}

int modified;

/*****************************************************************************/
GtkWidget *entry_group;
GtkWidget *entry_taille_x;

TxChampsFormulaire formulaire_group[Number_of_items_total] =
{
{ 0, 0, "group", "-10", entry_group_callback_no_name, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
/*{4, 0, "name", "xxx", entry_group_callback_nom, "", NULL, NULL, NULL},*/
{ 0, 1, "x size", "1", entry_group_callback_taillex, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 2, 1, "y size", "1", entry_group_callback_tailley, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 1, "number of neurons", "-4", entry_group_callback, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 2, "threshold", "0.", entry_group_callback_seuil, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 2, 2, "learning rate", "0.1", entry_group_callback_learning_rate, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 2, "simul. speed", "1.", entry_group_callback_simulation_speed, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 3, "type2", "-6", entry_group_callback_type2, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 2, 3, "time scale", "-7", entry_group_callback_ech_temps, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 3, "noise level", "0.", entry_group_callback_noise_level, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 6, "tolerance", "0.", entry_group_callback_tolerance, "", NULL, NULL, NULL , NULL, NULL, NULL, 0},
{ 2, 6, "alpha", "0.", entry_group_callback_alpha, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 6, "nb of 1", "0", entry_group_callback_nbre_de_1, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 7, "dpn", "0", entry_group_callback_dvp, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 2, 7, "dnn", "0", entry_group_callback_dvn, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 7, "sigma f", "0.", entry_group_callback_sigma_f, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 4, "plan", "0", entry_group_callback_plan, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 9, "comment", "entrer vos commentaires ici ...", entry_group_callback_comment, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 } };

/* Create a new hbox with an image and a label packed into it
 * and return the box. */

GtkWidget *xpm_label_box(gchar * xpm_filename, gchar * label_text)
{
  GtkWidget *box;
  GtkWidget * label;
  GtkWidget *image;

  /* Create box for image and label */
  box = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(box), 2);

  /* Now on to the image stuff */
  image = gtk_image_new_from_file(xpm_filename);

  /* Create a label for the button */
  label = gtk_label_new(label_text);

  /* Pack the image and label into the box */
  gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 3);
  gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 3);

  gtk_widget_show(image);
  gtk_widget_show(label);

  return box;
}

GtkWidget *reverse_button;
GtkWidget *debug_button;
GtkWidget *reset_pos_button;
GtkWidget *combo_type_groupe_entry;
GtkWidget *combo_group_function_name;

GtkWidget *combo_debug_groupe;
GtkWidget *combo_reverse_groupe;
GtkWidget *combo_name_groupe_entry;

type_group_function_pointers *load_list_functions(const char *module_name, int required)
{
  void *module;
  char buffer[MAXBUF], ld_prom_path[MAXBUF];
  type_group_function_pointers *group_function_pointers;

  sprintf(ld_prom_path, "%s/bin_leto_prom/Libraries/prom_user", getenv("HOME"));

#ifdef Darwin
  sprintf(buffer,"%s/%s.dylib",ld_prom_path, module_name);
#else
  sprintf(buffer, "%s/%s.so", ld_prom_path, module_name);
#endif

  /* Load dynamically loaded library */
  module = dlopen(buffer, RTLD_LAZY);

  if (!module)
  {
    if (required)
    {
      EXIT_ON_ERROR("Module required '%s' not found in path: %s \n\tError: %s", module_name, ld_prom_path, dlerror());
    }
    else
    {
      /*  kprints("Optional %s not loaded. %s\n", nom_fichier, dlerror());*/
      return NULL;
    }
  }

  group_function_pointers = dlsym(module, "group_function_pointers");

  return group_function_pointers;
}

/**
 * Creates a dialog window to modify a group.
 */
GtkWidget* create_read_group(TxDonneesFenetre *onglet_leto)
{
  GtkWidget *group_dialog;
  GtkWidget *Winmain;
  int i = 0;
  int z = 0;
  GtkWidget *table1;
  GtkWidget *combo1;
  GtkWidget *combo2;
  GtkWidget *combo3;
  GtkWidget *scrolledwindow3a;
  GtkWidget *entry_comment;

  GList *combo1_items = NULL;
  GList *combo2_items = NULL;
  GList *combo3_items = NULL;
  GList *combo_name_items = NULL;

  GtkListStore *store;
  GtkTreeIter iter;
  GtkEntryCompletion *completion;
  GtkTreeModel *completion_model;

  type_group_function_pointers *SigProc_libdyn, *IHM_libdyn, *IO_Robot_libdyn, *Parellel_Comm_libdyn, *NN_IO_libdyn, *Sensors_libdyn;

  init_group_visibility_properties();

#ifndef LETO
  Winmain = lookup_widget(onglet_leto->window, "Winmain");
#else
  Winmain = onglet_leto->window;
#endif

  group_dialog = gtk_dialog_new_with_buttons("Group properties...", GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);

  g_object_set_data(G_OBJECT(group_dialog), "window1", group_dialog);

  table1 = gtk_table_new(9, 10, FALSE);
  g_object_ref(table1);
  g_object_set_data_full(G_OBJECT(group_dialog), "table1", table1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(table1);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(group_dialog)->vbox), table1);

  for (i = 0; i < Number_of_items; i++)
  {
    TxInsereChampsdansFormulaire(group_dialog, table1, &formulaire_group[i]);
    gtk_editable_set_editable(GTK_EDITABLE(formulaire_group[i].widget_entry), FALSE);
  }

  entry_comment = InsereCommentdansFormulaire(group_dialog, table1, &formulaire_group[No_item_comment]);
  /* formulaire_group[No_item_comment].widget_entry; */

  /********* group type combo list *********/

  combo1 = gtk_combo_new();
  g_object_ref(combo1);
  g_object_set_data_full(G_OBJECT(group_dialog), "combo1", combo1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo1);
  gtk_table_attach(GTK_TABLE(table1), combo1, 3, 4, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  for (i = 0; i < nbre_type_groupes; i++)
    combo1_items = g_list_append(combo1_items, (gpointer) lien_no_nom_type_groupe[i].nom);
  gtk_combo_set_popdown_strings(GTK_COMBO(combo1), combo1_items);
  g_list_free(combo1_items);

  combo_type_groupe_entry = GTK_COMBO(combo1)->entry;
  g_object_ref(combo_type_groupe_entry);
  g_object_set_data_full(G_OBJECT(group_dialog), "combo_type_groupe_entry", combo_type_groupe_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo_type_groupe_entry);
  gtk_combo_disable_activate(GTK_COMBO(combo1));
  gtk_entry_set_activates_default(GTK_ENTRY(combo_type_groupe_entry), TRUE);

  gtk_entry_set_text(GTK_ENTRY(combo_type_groupe_entry), "Hebb");
  /* g_signal_connect(G_OBJECT(combo_type_groupe_entry), "activate",
   G_CALLBACK(combo_nom_groupe_callback),
   (gpointer) combo_type_groupe_entry); */

  g_signal_connect(G_OBJECT(combo_type_groupe_entry), "changed", G_CALLBACK(combo_nom_groupe_changed), (gpointer) combo_type_groupe_entry);

  /********* debug combo list *********/

  combo2 = gtk_combo_new();
  g_object_ref(combo2);
  g_object_set_data_full(G_OBJECT(group_dialog), "combo2", combo2, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo2);
  gtk_table_attach(GTK_TABLE(table1), combo2, 3, 4, 4, 5, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  combo2_items = g_list_append(combo2_items, (gpointer) "Debug group on");
  combo2_items = g_list_append(combo2_items, (gpointer) "Debug group off");
  combo2_items = g_list_append(combo2_items, (gpointer) "Debug group normalized");

  gtk_combo_set_popdown_strings(GTK_COMBO(combo2), combo2_items);
  g_list_free(combo2_items);

  combo_debug_groupe = GTK_COMBO(combo2)->entry;
  g_object_ref(combo_debug_groupe);
  g_object_set_data_full(G_OBJECT(group_dialog), "combo_debug_groupe", combo_debug_groupe, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo_debug_groupe);
  gtk_combo_disable_activate(GTK_COMBO(combo2));
  gtk_entry_set_activates_default(GTK_ENTRY(combo_debug_groupe), TRUE);

  gtk_entry_set_text(GTK_ENTRY(combo_debug_groupe), "Debug group on");

  g_signal_connect(G_OBJECT(combo_debug_groupe), "changed", G_CALLBACK(combo_debug_groupe_callback), (gpointer) combo_debug_groupe);

  /********* reverse combo list *********/

  combo3 = gtk_combo_new();
  g_object_ref(combo3);
  g_object_set_data_full(G_OBJECT(group_dialog), "combo3", combo3, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo3);
  gtk_table_attach(GTK_TABLE(table1), combo3, 1, 2, 4, 5, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  combo3_items = g_list_append(combo3_items, (gpointer) "Reverse group off");
  combo3_items = g_list_append(combo3_items, (gpointer) "Reverse group on");

  gtk_combo_set_popdown_strings(GTK_COMBO(combo3), combo3_items);
  g_list_free(combo3_items);

  combo_reverse_groupe = GTK_COMBO(combo3)->entry;
  g_object_ref(combo_reverse_groupe);
  g_object_set_data_full(G_OBJECT(group_dialog), "combo_reverse_groupe", combo_reverse_groupe, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo_reverse_groupe);
  gtk_combo_disable_activate(GTK_COMBO(combo3));
  gtk_entry_set_activates_default(GTK_ENTRY(combo_reverse_groupe), TRUE);

  gtk_entry_set_text(GTK_ENTRY(combo_reverse_groupe), "Reverse group off");

  /********* group name combo list *********/
  combo_group_function_name = gtk_combo_new();
  g_object_ref(combo_group_function_name);
  g_object_set_data_full(G_OBJECT(group_dialog), "combo_group_function_name", combo_group_function_name, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo_group_function_name);
  gtk_table_attach(GTK_TABLE(table1), combo_group_function_name, 5, 6, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  store = gtk_list_store_new(1, G_TYPE_STRING);
  if (sc->groupes_courants->group->type == No_Fonction_Algo || sc->groupes_courants->group->type == No_Fonction_Algo_mvt)
  {
    while (NN_Core_function_pointers[z].name != NULL)
    {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, (char *) NN_Core_function_pointers[z].name, -1);
      combo_name_items = g_list_append(combo_name_items, (char *) NN_Core_function_pointers[z].name);
      z++;
    }

    z = 0;
    SigProc_libdyn = load_list_functions("libSigProc_blind_release", MODULE_REQUIRED);
    while (SigProc_libdyn[z].name != NULL)
    {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, (char *) SigProc_libdyn[z].name, -1);
      combo_name_items = g_list_append(combo_name_items, (char *) SigProc_libdyn[z].name);
      z++;
    }

    z = 0;
    IHM_libdyn = load_list_functions("libIHM_blind_release", MODULE_REQUIRED);
    while (IHM_libdyn[z].name != NULL)
    {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, (char *) IHM_libdyn[z].name, -1);
      combo_name_items = g_list_append(combo_name_items, (char *) IHM_libdyn[z].name);
      z++;
    }

    z = 0;
    IO_Robot_libdyn = load_list_functions("libIO_Robot_blind_release", MODULE_REQUIRED);
    while (IO_Robot_libdyn[z].name != NULL)
    {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, (char *) IO_Robot_libdyn[z].name, -1);
      combo_name_items = g_list_append(combo_name_items, (char *) IO_Robot_libdyn[z].name);
      z++;
    }

    z = 0;
    Parellel_Comm_libdyn = load_list_functions("libParallel_Comm_blind_release", MODULE_REQUIRED);
    while (Parellel_Comm_libdyn[z].name != NULL)
    {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, (char *) Parellel_Comm_libdyn[z].name, -1);
      combo_name_items = g_list_append(combo_name_items, (char *) Parellel_Comm_libdyn[z].name);
      z++;
    }

    z = 0;
    NN_IO_libdyn = load_list_functions("libNN_IO_blind_release", MODULE_REQUIRED);
    while (NN_IO_libdyn[z].name != NULL)
    {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, (char *) NN_IO_libdyn[z].name, -1);
      combo_name_items = g_list_append(combo_name_items, (char *) NN_IO_libdyn[z].name);
      z++;
    }

    z = 0;
    Sensors_libdyn = load_list_functions("libSensors_blind_release", MODULE_REQUIRED);
    while (Sensors_libdyn[z].name != NULL)
    {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, (char *) Sensors_libdyn[z].name, -1);
      combo_name_items = g_list_append(combo_name_items, (char *) Sensors_libdyn[z].name);
      z++;
    }
  }
  gtk_combo_set_popdown_strings(GTK_COMBO(combo_group_function_name), combo_name_items);
  g_list_free(combo_name_items);


  completion = gtk_entry_completion_new();
  gtk_entry_set_completion(GTK_ENTRY(GTK_COMBO(combo_group_function_name)->entry), completion);
  g_object_unref(completion);

  completion_model = GTK_TREE_MODEL(store);
  gtk_entry_completion_set_model(completion, completion_model);
  g_object_unref(completion_model);

  gtk_entry_completion_set_text_column(completion, 0);



  combo_name_groupe_entry = GTK_WIDGET(GTK_COMBO(combo_group_function_name)->entry);
  g_object_ref(combo_name_groupe_entry);
  g_object_set_data_full(G_OBJECT(group_dialog), "combo_name_groupe_entry", combo_name_groupe_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo_group_function_name);
  gtk_combo_disable_activate(GTK_COMBO(combo_group_function_name));
  gtk_entry_set_activates_default(GTK_ENTRY(combo_name_groupe_entry), TRUE);

  gtk_entry_set_text(GTK_ENTRY(combo_name_groupe_entry), sc->groupes_courants->group->nom);



  /* sous fenetre avec ascenseur pour les commentaires oooo */

  scrolledwindow3a = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_size_request(scrolledwindow3a, 100, 50);
  gtk_widget_ref(scrolledwindow3a);
  gtk_object_set_data_full(GTK_OBJECT(group_dialog), "scrolledwindow3a", scrolledwindow3a, (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_show(scrolledwindow3a);

  gtk_table_attach(GTK_TABLE(table1), entry_comment, 1, 6, 9, 10, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_widget_show(entry_comment);

  g_signal_connect(group_dialog, "close", G_CALLBACK(gtk_widget_hide), group_dialog);
  return group_dialog;
}

/************************************************************************/

TxChampsFormulaire formulaire_link[Number_of_link_items_total] =
{
{ 0, 0, "input group", "-1", entry_link_callback_input, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 2, 0, "output group", "-1", entry_link_callback_output, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 2, "temps", "-5", entry_link_callback_temps, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 2, 2, "stemps", "-6", entry_link_callback_stemps, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 2, "nbre", "-20", entry_link_callback_nbre, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 3, "secondaire", "-6", entry_link_callback_secondaire, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 2, 3, "mode", "-7", entry_link_callback_mode, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 4, "Norm", "-5", entry_link_callback_norme, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 6, "dvx", "-5", entry_link_callback_dvx, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 2, 6, "dvy", "-6", entry_link_callback_dvy, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 6, "proba", "-20", entry_link_callback_proba, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 4, 7, "name", "-20", entry_link_callback_name, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 7, "style (0:line,1:bezier)", "0", entry_link_callback_style, "", NULL, NULL, NULL, NULL, NULL, NULL, 0 },
{ 0, 9, "comment", "entrer vos commentaires ici ...", entry_link_callback_comment, "", NULL, NULL, NULL, NULL,NULL, NULL, 0 } };

type_lien_no_nom lien_no_nom_type_link[nbre_type_links] =
{

/* No_item_link_depart, No_item_link_arrivee, No_item_link_temps , No_item_link_stemps, No_item_link_nbre,
 No_item_link_secondaire, No_item_link_mode, No_item_link_norme, No_item_link_dvx, No_item_link_dvy, No_item_link_proba, No_item_link_name
 No_item_link_style
 (voir interface.h)
 */

{ No_l_1_1_modif, "one to one modifiable",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_1_a, "one to random (modifiable)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_1_v, "one to neighborhood (modifable)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_1_1_non_modif, "one to one (NON modifiable)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_1_t, "one to all (modifiable)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_algorithmique, "algorithmic link (virtual)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_1_1_non_modif_bloqueur, "one to one blocking (vig. control)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_1_v_non_modif, "one to neighborhood (NON modifiable)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_neuro_mod, "global neuro modulation link (virtual)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_1_patern_non_modif, "one to patern (non modifiable)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
{ No_l_1_patern_modif, "one to patern (modifiable)",
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } } };

void init_link_visibility_properties()
{
  int i;
  for (i = 0; i < nbre_type_links; i++) /* init non visible general properties */
  {
    (lien_no_nom_type_link[i].editable)[No_item_link_nbre] = 0;
    (lien_no_nom_type_link[i].editable)[No_item_link_dvx] = -1;
    (lien_no_nom_type_link[i].editable)[No_item_link_dvy] = -1;
    (lien_no_nom_type_link[i].editable)[No_item_link_proba] = -1;
    (lien_no_nom_type_link[i].editable)[No_item_link_name] = -1;
    (lien_no_nom_type_link[i].editable)[No_item_link_mode] = 0; /* le mode est change uniquemenet par le combo,
     l'affichage numerique n'est la que pour le debug des vieux scripts */
  }

  /*exceptions , links particularities */
  lien_no_nom_type_link[No_l_1_v].editable[No_item_link_dvx] = 1;
  lien_no_nom_type_link[No_l_1_v].editable[No_item_link_dvy] = 1;
  lien_no_nom_type_link[No_l_1_v].editable[No_item_link_proba] = 1;
  lien_no_nom_type_link[No_l_1_v_non_modif].editable[No_item_link_proba] = 1;
  lien_no_nom_type_link[No_l_1_v_non_modif].editable[No_item_link_dvy] = 1;
  lien_no_nom_type_link[No_l_1_v_non_modif].editable[No_item_link_dvx] = 1;
  lien_no_nom_type_link[No_l_1_a].editable[No_item_link_proba] = 1;

  /* Mathias Quoy 01/03/2005 : proba code l'ecart type de la valeur aleatoire des coeffs */
  lien_no_nom_type_link[No_l_1_t].editable[No_item_link_proba] = 1;

  lien_no_nom_type_link[No_l_algorithmique].editable[No_item_link_name] = 1;
  lien_no_nom_type_link[No_l_1_patern_modif].editable[No_item_link_name] = 1;
  lien_no_nom_type_link[No_l_1_patern_non_modif].editable[No_item_link_name] = 1;
  lien_no_nom_type_link[No_l_neuro_mod].editable[No_item_link_name] = 1;
  lien_no_nom_type_link[No_l_1_1_non_modif_bloqueur].editable[No_item_link_name] = 1;

  /*J'interdis l'utilisation de temps et de stemps :
   ca pourra etre changee mais pour le moment ils sont pas utilises :
   on a rien defini dessus M.Maillard */
  lien_no_nom_type_link[No_l_neuro_mod].editable[No_item_link_temps] = 0;
  lien_no_nom_type_link[No_l_neuro_mod].editable[No_item_link_stemps] = 0;

}

GtkWidget *combo_type_link_entry;
GtkWidget *combo_mode_link_entry;

type_groupe *get_groupOut(type_liaison *link)
{
  type_groupe *groupe;
  int groupe_not_found = 1;
  /** retrouve le type du groupe en sortie du lien */
  groupe = sc->deb_groupe;
  while (groupe != NULL && groupe_not_found)
  {
    if (groupe->no == link->arrivee)
    {
      groupe_not_found = 0;
    }
    else groupe = groupe->s;
  }
  return groupe;
}

/**
 * Creates a dialog to modify a link.
 */
GtkWidget* create_read_link(TxDonneesFenetre *onglet_leto, type_liaison *link)
{
  GtkWidget *link_dialog;
  GtkWidget *Winmain;
  int i;
  GtkWidget *table1;
  GtkWidget *combo1;
  GtkWidget *combo2;
  GList *combo1_items = NULL;
  GList *combo2_items = NULL;
  GtkWidget *entry_comment;
  type_groupe *groupe;
  mode_lien *mode_link;

  init_link_visibility_properties();

#ifndef LETO
  Winmain = lookup_widget(onglet_leto->window, "Winmain");
#else
  Winmain = onglet_leto->window;
#endif

  link_dialog = gtk_dialog_new_with_buttons("Link properties...", GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);

  gtk_object_set_data(GTK_OBJECT(link_dialog), "window1", link_dialog);

  table1 = gtk_table_new(8, 8, FALSE);
  gtk_widget_ref(table1);
  gtk_object_set_data_full(GTK_OBJECT(link_dialog), "table1", table1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(table1);

  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(link_dialog)->vbox), table1);

  for (i = 0; i < Number_of_link_items; i++)
    TxInsereChampsdansFormulaire(link_dialog, table1, &formulaire_link[i]);

  entry_comment = InsereCommentdansFormulaire(link_dialog, table1, &formulaire_link[No_item_link_comment]);

  /***************** types combo list ***************/

  combo1 = gtk_combo_new();
  gtk_widget_ref(combo1);
  gtk_object_set_data_full(GTK_OBJECT(link_dialog), "combo1", combo1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo1);
  gtk_table_attach(GTK_TABLE(table1), combo1, 5, 6, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  for (i = 0; i < nbre_type_links; i++)
    combo1_items = g_list_append(combo1_items, (gpointer) lien_no_nom_type_link[i].nom);
  gtk_combo_set_popdown_strings(GTK_COMBO(combo1), combo1_items);
  g_list_free(combo1_items);

  combo_type_link_entry = GTK_COMBO(combo1)->entry;
  gtk_widget_ref(combo_type_link_entry);
  gtk_object_set_data_full(GTK_OBJECT(link_dialog), "combo_type_link_entry", combo_type_link_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo_type_link_entry);
  gtk_combo_disable_activate(GTK_COMBO(combo1));
  gtk_entry_set_activates_default(GTK_ENTRY(combo_type_link_entry), TRUE);

  /* g_signal_connect(G_OBJECT(combo_type_link_entry), "activate",
   G_CALLBACK(combo_type_link_callback),
   (gpointer) combo_type_link_entry); */

  gtk_entry_set_text(GTK_ENTRY(combo_type_link_entry), "one to all (modifiable)");

  g_signal_connect(G_OBJECT(combo_type_link_entry), "changed", G_CALLBACK(combo_type_link_changed), (gpointer) combo_type_link_entry);

  /***************** modes combo list ***************/

  combo2 = gtk_combo_new();
  gtk_widget_ref(combo2);
  gtk_object_set_data_full(GTK_OBJECT(link_dialog), "combo2", combo2, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo2);
  gtk_table_attach(GTK_TABLE(table1), combo2, 3, 4, 4, 5, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  /** get groupe out type */
  groupe = get_groupOut(link);

  for (i = 0; i < group_mode_link_tab[groupe->type].nb_mode; i++)
  {
    mode_link = &(group_mode_link_tab[groupe->type].mode_tab[i]);
    combo2_items = g_list_append(combo2_items, (gpointer) mode_link->type_lien.nom);
  }

  gtk_combo_set_popdown_strings(GTK_COMBO(combo2), combo2_items);
  g_list_free(combo2_items);

  combo_mode_link_entry = GTK_COMBO(combo2)->entry;
  gtk_widget_ref(combo_mode_link_entry);
  gtk_object_set_data_full(GTK_OBJECT(link_dialog), "combo_mode_link_entry", combo_mode_link_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(combo_mode_link_entry);
  gtk_combo_disable_activate(GTK_COMBO(combo2));
  gtk_entry_set_activates_default(GTK_ENTRY(combo_mode_link_entry), TRUE);

  g_signal_connect(G_OBJECT(combo_mode_link_entry), "changed", G_CALLBACK(combo_mode_link_changed), (gpointer) combo_mode_link_entry);
  /* g_signal_connect(G_OBJECT (combo_mode_link_entry), "activate",
   G_CALLBACK (combo_mode_link_callback),
   (gpointer) combo_mode_link_entry); */

  gtk_entry_set_text(GTK_ENTRY(combo_mode_link_entry), "product (after compet)");

  gtk_table_attach(GTK_TABLE(table1), entry_comment, 1, 6, 9, 10, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_widget_show(entry_comment);

  /* gtk_widget_hide_on_delete(link_dialog); */
  g_signal_connect(link_dialog, "close", G_CALLBACK(gtk_widget_hide), link_dialog);
  return link_dialog;
}

/**
 * Show the menu when the right mouse button is pressed
 */
void show_popup_menu(int popup_menu_type, GdkEventButton *event, TxDonneesFenetre *onglet_leto)
{
  /*GtkItemFactory *item_factory;*/
  GtkUIManager *menubar;
  GtkWidget * menu;

  menubar = GTK_UI_MANAGER(lookup_widget(onglet_leto->window, "menubar"));

  switch (popup_menu_type)
  {
  case POPUP_MENU_GROUP:
    menu = gtk_ui_manager_get_widget(menubar, "/popupGroup");
    break;

  case POPUP_MENU_LINK:
    menu = gtk_ui_manager_get_widget(menubar, "/popupLink");
    break;

  case POPUP_MENU_BEND:
    menu = gtk_ui_manager_get_widget(menubar, "/popupCoude");
    break;

  case POPUP_MENU_NO_SELECTION:
  default:
    menu = gtk_ui_manager_get_widget(menubar, "/popup");
  }

  gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button, event->time);
}

/**
 * Display a message in the status bar.
 */
void show_status(TxDonneesFenetre *onglet_leto, const char *fmt, ...)
{
  GtkWidget* statusbar = NULL;
  char msg[255];
  va_list ap;
  /* char *s; */
  if (statusbar == NULL) statusbar = lookup_widget(onglet_leto->window, "statusbar");
  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  /* s = g_convert(message, -1, "UTF-8", "ISO-8859-1", 0,0,0); */
  /* fprintf(stdout, "%s\n", msg); */
  gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 0);
  gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0, msg);
  va_end(ap);
}

/**
 * Pops up a dialog box with a warning.
 */
void show_error(char *message, TxDonneesFenetre *onglet_leto)
{
  GtkWidget *dialog;
  GtkWidget *Winmain;

#ifndef LETO
  Winmain = lookup_widget(onglet_leto->window, "Winmain");
#else
  Winmain = onglet_leto->window;
#endif

  dialog = gtk_message_dialog_new(GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, message, "Error");
  gtk_widget_show(dialog);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

#define NB_GROUP_ITEMS   6
#define NB_LINK_ITEMS    2
#ifdef EXPORT
#define NB_SCRIPT_ITEMS  2
#else
#define NB_SCRIPT_ITEMS  1
#endif

void set_title(TxDonneesFenetre *onglet_leto)
{
#ifdef LETO
  char title[255];

  if (strlen(sc->nomfich1) == 0)
  {
    memcpy(title, "Untitled script", strlen("Untitled script")+1 * sizeof(char));
  }
  else
  {
    memcpy(title, g_path_get_basename(sc->nomfich1), strlen(g_path_get_basename(sc->nomfich1))+1 * sizeof(char));
  }

  if (sc->modified != 0)
  {
    strncat(title, "*", sizeof(title) - strlen(title));
  }

  strncat(title, " - Leto", sizeof(title) - strlen(title));
  gtk_window_set_title(GTK_WINDOW(onglet_leto->window), title);
#else
  GtkWidget *pNotebook;
  GtkWidget *page;
  int num_page;
  char title[255];

  if (strlen(sc->nomfich1) == 0)
  {
    memcpy(title, "Untitled script", strlen("Untitled script") + 1 * sizeof(char));
  }
  else
  {
    memcpy(title, sc->nomfich1, strlen(sc->nomfich1) + 1 * sizeof(char));
    get_base_name(title);
  }

  if (sc->modified != 0)
  {
    strncat(title, "*", sizeof(title) - strlen(title));
  }

  /* changement du nom de l'onglet */
  pNotebook = lookup_widget(onglet_leto->window, "Notebook");

  num_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(pNotebook));

  page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(pNotebook), num_page);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(pNotebook), page, title);
#endif
}



char *get_file_extension(const char *file)
{
  char *base;
  char *ext;

  if (file == NULL) return NULL;

  base = (char *) g_path_get_basename(file);
  ext = strrchr(base, '.');

  if ((ext == NULL) || (ext == base)) return (base + strlen(base));
  return (ext + 1);
}

void set_file_with_ext(char *file, const char *ext)
{

  char *ptr;
  char buf[PATH_MAX];

  if (strlen(sc->nomfich1) == 0)
  {
    fprintf(stderr, "You must enter a name for the script file\n");
    return;
  }

  memcpy(buf, sc->nomfich1, (strlen(sc->nomfich1) + 1) * sizeof(char));
  ptr = get_file_extension(buf);

  if (ptr == NULL) return;

  if (strlen(ptr) == 0) strncat(buf, ext, PATH_MAX - 1);
  else memcpy(ptr, &ext[1], (strlen(&ext[1]) + 1) * sizeof(char));

  memcpy(file, buf, (strlen(buf) + 1) * sizeof(char));
}

/* recherche la widget qui a ete associe a la widget donnee */
GtkWidget *lookup_widget(GtkWidget * widget, const char * widget_name)
{
  GtkWidget *found_widget;

  found_widget = (GtkWidget *) g_object_get_data(G_OBJECT(widget), widget_name);
  if (!found_widget) g_warning("Widget not found: %s", widget_name);
  return found_widget;
}

void scroll(TxPoint point, TxDonneesFenetre *onglet_leto)
{
  /* TODO: verifier vitesse scroll sur differentes machines */
  if (point.x - 20 < sc->xmin)
  {
    scroll_window(-40, 0, onglet_leto);
  }
  else if (point.x + 40 > sc->xmax)
  {
    scroll_window(40, 0, onglet_leto);
  }

  if (point.y - 20 < sc->ymin)
  {
    scroll_window(0, -40, onglet_leto);
  }
  else if (point.y + 120 > sc->ymax)
  {
    scroll_window(0, 40, onglet_leto);
  }
}

void scroll_window(int x, int y, TxDonneesFenetre *onglet_leto)
{
  gdouble value;
  GtkAdjustment *adj;
  GtkScrolledWindow * sw;

  sw = GTK_SCROLLED_WINDOW(lookup_widget(onglet_leto->window, "scrolledwindow1"));
  if (x < 0) sc->xmin += x;
  else sc->xmax += x;
  if (y < 0) sc->ymin += y;
  else sc->ymax += y;
  regenerer_test(onglet_leto);

  adj = gtk_scrolled_window_get_hadjustment(sw);
  value = gtk_adjustment_get_value(adj);
  if (value + x > adj->upper - adj->page_size) value = adj->upper - adj->page_size;
  else value += x;
  gtk_adjustment_set_value(adj, value);

  adj = gtk_scrolled_window_get_vadjustment(sw);
  value = gtk_adjustment_get_value(adj);
  if (value + y > adj->upper - adj->page_size) value = adj->upper - adj->page_size;
  else value += y;
  gtk_adjustment_set_value(adj, value);
}

