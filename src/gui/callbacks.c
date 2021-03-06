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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "block_create.h"
#include "manage_comments.h"
#include "gere_coudes.h"
#include "creation.h"
#include "interface.h"
#include "draw_leto.h"
#include "leto.h"
#include "callbacks.h"

#ifdef EXPORT
#include "export.h"
#endif

#define COMMAND_LINE_MAX 4096


#include "gennet.h"

char bin_leto_prom_path[] = "~/bin_leto_prom";

void callback_undo(GtkWidget * widget, t_gennet_script *script_gui)
{

	char filename[PATH_MAX];
	(void) widget;

	if (script_gui->sc->backup_id > 1) script_gui->sc->backup_id--;
	else
	{
		show_status(script_gui->onglet_leto, "Nothing to cancel");
		return;
	}

	sprintf(filename, "%s.bak%i", script_gui->sc->nomfich1, script_gui->sc->backup_id - 1);
	script_load(script_gui->sc, filename, 1, script_gui->onglet_leto->hashtab);
	regenerer_test(script_gui->onglet_leto);

}

void callback_redo(GtkWidget * widget, t_gennet_script *script_gui)
{
	char filename[PATH_MAX];
	struct stat buf;
	(void) widget;

	sprintf(filename, "%s.bak%i", script_gui->sc->nomfich1, script_gui->sc->backup_id);
	if (stat(filename, &buf) != 0)
	{
		show_status(script_gui->onglet_leto, "Cannot redo anymore");
	}
	else
	{
		script_load(script_gui->sc, filename, 1, script_gui->onglet_leto->hashtab);
		regenerer_test(script_gui->onglet_leto);
		script_gui->sc->backup_id++;
	}
}

/*****************************************************************************/
/* Saisie ou mise a jour des donnees sur un groupe                           */
/* Nouvelle version Aout 2005                                                */
/* La plupart des donnees sur les groupes et les liens sont gerees comme des */
/* chaines de caracteres (permettre l'emploi de variables symboliques).      */
/*****************************************************************************/

void entry_group_callback(GtkWidget * widget, GtkWidget * entry, void* user_data)
{
	(void) widget;
	(void) user_data;
	gtk_entry_get_text(GTK_ENTRY(entry));
}

void entry_group_callback_no_name(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	t_gennet_script *script_gui = user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) == 0)
	{
		return;
	}

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		rename_group_and_associated_links(sel_group->group, (char *) entry_text, script_gui);
	}
}

void combo_nom_groupe_callback(GtkWidget * widget, GtkWidget * entry, void* user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	int i;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) == 0) return;

	for (i = 0; i < nbre_type_groupes; i++)
		if (strcmp(lien_no_nom_type_groupe[i].nom, entry_text) == 0)
		{
			for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
			{
				sel_group->group->type = lien_no_nom_type_groupe[i].no;
			}
			return;
		}
}

void combo_group_function_name_callback(GtkWidget * widget, GtkWidget * combo, void* user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	entry_text = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(combo)->entry));

	if (strlen(entry_text) == 0 && entry_text[0] == 'X') return;

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		strcpy(sel_group->group->nom, entry_text);
	}
	return;
}

void combo_nom_groupe_changed(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	int i;

	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) == 0) return;

	for (i = 0; i < nbre_type_groupes; i++)
	{
		if (strcmp(lien_no_nom_type_groupe[i].nom, entry_text) == 0)
		{
			fill_group_dialog(lien_no_nom_type_groupe[i].no);
			return;
		}
	}
}

void combo_group_algo_function_name_callback(GtkWidget * widget, GtkWidget * combo, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	entry_text = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(combo)->entry));

	if (strlen(entry_text) > 0 && entry_text[0] == 'X') return;

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		strcpy(sel_group->group->nom, entry_text);
	}
	return;
}
void combo_debug_groupe_callback(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	int debug = 0;
	selected_group *sel_group;
	(void)widget;
	(void)user_data;


	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	if (strlen(entry_text) == 0) return;

	if (strcmp("Debug group on", entry_text) == 0)
	{
		debug = 1;
	}
	else if (strcmp("Debug group off", entry_text) == 0)
	{
		debug = 0;
	}
	else if (strcmp("Debug group normalized", entry_text) == 0)
	{
		debug = 2;
	}

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		sel_group->group->debug = debug;
	}
}

void combo_reverse_groupe_callback(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	TxPoint point;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	if (strlen(entry_text) == 0)
	{
		return;
	}

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{

		if (sel_group->group->reverse == 0)
		{
			sel_group->group->reverse = 1;
		}

		if (strcmp("Reverse group on", entry_text) == 0)
		{

			sel_group->group->reverse = abs(sel_group->group->reverse);
		}
		else if (strcmp("Reverse group off", entry_text) == 0)
		{
			sel_group->group->reverse = -abs(sel_group->group->reverse);
		}

		point.x = sel_group->group->posx;
		point.y = sel_group->group->posy; /* pas de changement de pos mais changement orientation */
		nouvelle_position_groupe(sel_group->group, point);
	}
}

void entry_group_callback_taillex(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) > 0 && entry_text[0] != 'X')
	{
		for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
		{
			Str2MY_Int(sel_group->group->taillex, (char *) entry_text);
		}
	}
}

void entry_group_callback_tailley(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) > 0 && entry_text[0] != 'X')
	{
		for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
		{
			Str2MY_Int(sel_group->group->tailley, (char *) entry_text);
		}
	}
}

void entry_group_callback_seuil(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Float(sel_group->group->seuil, (char *) entry_text);
	}
}

void entry_group_callback_learning_rate(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Float(sel_group->group->learning_rate, (char *) entry_text);
	}
}

void entry_group_callback_simulation_speed(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Float(sel_group->group->simulation_speed, (char *) entry_text);
	}
}

void entry_group_callback_type2(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) > 0 && entry_text[0] != 'X')
	{
		for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
		{
			Str2MY_Int(sel_group->group->type2, (char *) entry_text);
		}
	}
}

void entry_group_callback_ech_temps(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) > 0 && entry_text[0] != 'X')
	{
		for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
		{
			sel_group->group->ech_temps = atoi(entry_text);
		}
	}
}

void entry_group_callback_noise_level(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Float(sel_group->group->noise_level, entry_text);
	}
}

void entry_group_callback_tolerance(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Float(sel_group->group->tolerance, entry_text);
	}
}

void entry_group_callback_alpha(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Float(sel_group->group->alpha, entry_text);
	}
}

void entry_group_callback_nbre_de_1(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Float(sel_group->group->nbre_de_1, entry_text);
	}
}

void entry_group_callback_dvp(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Int(sel_group->group->dvp, entry_text);
	}
}

void entry_group_callback_dvn(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Int(sel_group->group->dvn, entry_text);
	}
}

void entry_group_callback_sigma_f(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	{
		Str2MY_Float(sel_group->group->sigma_f, entry_text);
	}
}

void entry_group_callback_plan(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	TxPoint point;
	int plan, reverse;
	selected_group *sel_group;
	(void) widget;
	(void) user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) > 0 && entry_text[0] != 'X')
	{
		plan = abs(atoi(entry_text));

		for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
		{
			reverse = sel_group->group->reverse;

			if (reverse > 0)
			{
				reverse = 1;
			}
			else
			{
				reverse = -1;
			}

			sel_group->group->reverse = plan * reverse;
			point.x = sel_group->group->posx;
			point.y = sel_group->group->posy; /* pas de changement de pos mais changement orientation */
			nouvelle_position_groupe(sel_group->group, point);
		}
	}
}


/* Insere le menu dans l'onglet leto */
void menu_addLeto(GtkUIManager * p_uiManager, GtkWidget * p_widget, gpointer data)
{
	(void) p_uiManager;
	gtk_box_pack_start(GTK_BOX(data), p_widget, FALSE, FALSE, 0);
	return;
}

extern GtkWidget *combo_type_groupe_entry;
extern GtkWidget *combo_group_function_name;
extern GtkWidget *combo_debug_groupe;
extern GtkWidget *combo_reverse_groupe;

void group_validate_button_callback(GtkWidget * widget, t_gennet_script *script_gui)
{
	int i;

	for (i = 0; i < Number_of_items; i++)
	{
		formulaire_group[i].entry_widget_callback(widget, formulaire_group[i].widget_entry, (void*) script_gui);
	}

	formulaire_group[No_item_comment].entry_widget_callback(widget, formulaire_group[No_item_comment].widget_entry, script_gui);

	combo_nom_groupe_callback(widget, combo_type_groupe_entry, script_gui);
	/*combo_group_function_name_callback(widget, combo_group_function_name, ((TxDonneesFenetre *) data));*/
	combo_reverse_groupe_callback(widget, combo_reverse_groupe, script_gui);
	combo_debug_groupe_callback(widget, combo_debug_groupe, script_gui);
	combo_group_algo_function_name_callback(widget, combo_group_function_name, script_gui);

	if (script_set_modified(sc)) set_title(script_gui->onglet_leto);

}

void entry_toggle_editable(GtkWidget * checkbutton, GtkWidget * entry)
{
	gtk_editable_set_editable(GTK_EDITABLE(entry), GTK_TOGGLE_BUTTON(checkbutton)->active);
}

void entry_toggle_visibility(GtkWidget * checkbutton, GtkWidget * entry)
{
	gtk_entry_set_visibility(GTK_ENTRY(entry), GTK_TOGGLE_BUTTON(checkbutton)->active);
}

/*****************************************************************************/
/* gestion des liens */

void entry_link_callback_input(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	type_groupe *new_input_group;
	t_gennet_script *script_gui = user_data;
	(void)widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	new_input_group = lookup_hash_table(script_gui->onglet_leto->hashtab, (char *) entry_text);
	if (new_input_group != NULL)
	{

		memcpy(sc->liaison_courante->depart_name, new_input_group->no_name, (strlen(new_input_group->no_name) + 1) * sizeof(char));
		sc->liaison_courante->depart = new_input_group->no;
	}
	else
	{
		PRINT_WARNING("input group %s doesn't exists => keeping old link input group\n", entry_text);
	}
}

void entry_link_callback_output(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	type_groupe *new_output_group;
	t_gennet_script *script_gui = user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	new_output_group = lookup_hash_table(script_gui->onglet_leto->hashtab, (char *) entry_text);
	if (new_output_group != NULL)
	{
		memcpy(sc->liaison_courante->arrivee_name, new_output_group->no_name, (strlen(new_output_group->no_name) + 1) * sizeof(char));
		sc->liaison_courante->arrivee = new_output_group->no;
	}
	else
	{
		fprintf(stderr, "WARNING: output group %s doesn't exists => keeping old link output group\n", entry_text);
	}
}

void entry_link_callback_temps(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	Str2MY_Float(sc->liaison_courante->temps, entry_text);
}

void entry_link_callback_stemps(GtkWidget * widget, GtkWidget * entry, void *user_data)
{

	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	Str2MY_Float(sc->liaison_courante->stemps, entry_text);
}

void entry_link_callback_nbre(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;
	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	Str2MY_Int(sc->liaison_courante->nbre, entry_text);
}

void entry_link_callback_secondaire(GtkWidget * widget, GtkWidget * entry, void *user_data)
{

	const gchar *entry_text;
	(void) user_data;
	(void) widget;
	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	sc->liaison_courante->secondaire = atoi(entry_text);
}

void entry_link_callback_mode(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	sc->liaison_courante->mode = atoi(entry_text);
}

void entry_link_callback_dvx(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	Str2MY_Float(sc->liaison_courante->dv_x, entry_text);
}

void entry_link_callback_dvy(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	Str2MY_Float(sc->liaison_courante->dv_y, entry_text);
}

void entry_link_callback_proba(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	Str2MY_Float(sc->liaison_courante->proba, entry_text);
}

void entry_link_callback_name(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) == 0) memcpy(sc->liaison_courante->nom, ".", (strlen(".") + 1) * sizeof(char));
	else memcpy(sc->liaison_courante->nom, entry_text, (strlen(entry_text) + 1) * sizeof(char));

}

void entry_link_callback_style(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	sc->liaison_courante->style = atoi(entry_text);
}

void entry_link_callback_norme(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	(void) user_data;
	(void) widget;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE) return;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (sc->liaison_courante->type != No_l_1_patern_non_modif && sc->liaison_courante->type != No_l_1_patern_modif && sc->liaison_courante->type != No_l_algorithmique && sc->liaison_courante->type != No_l_neuro_mod) memcpy(sc->liaison_courante->nom, entry_text, (strlen(entry_text) + 1) * sizeof(char));

	Str2MY_Float(sc->liaison_courante->norme, entry_text);

}

void combo_type_link_changed(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	int i;
	(void) widget;
	(void)user_data;


	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) == 0)
	{
		return;
	}

	for (i = 0; i < nbre_type_links; i++)
	{
		if (strncmp(lien_no_nom_type_link[i].nom, entry_text, 100) == 0)
		{
			fill_link_dialog(sc->liaison_courante, lien_no_nom_type_link[i].no);
			break;
		}
	}
}

void combo_type_link_callback(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	int i;
	(void)widget;
	(void)user_data;

	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) == 0)
	{
		return;
	}

	for (i = 0; i < nbre_type_links; i++)
		if (strncmp(lien_no_nom_type_link[i].nom, entry_text, 100) == 0)
		{
			sc->liaison_courante->type = lien_no_nom_type_link[i].no;
			break;
		}
}

void combo_mode_link_callback(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	type_groupe *groupe;
	int i;
	mode_lien mode_link;
	(void) user_data;
	(void) widget;


	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) == 0)
	{
		return;
	}

	groupe = get_groupOut(sc->liaison_courante);
	for (i = 0; i < group_mode_link_tab[groupe->type].nb_mode; i++)
	{
		mode_link = group_mode_link_tab[groupe->type].mode_tab[i];
		if (strncmp(mode_link.type_lien.nom, entry_text, 100) == 0)
		{
			sc->liaison_courante->mode = mode_link.type_lien.no;
			break;
		}
	}
}

void combo_mode_link_changed(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	const gchar *entry_text;
	int i;
	type_groupe *groupe;
	mode_lien mode_link;

	(void) user_data;
	(void) widget;


	if (gtk_editable_get_editable(GTK_EDITABLE(entry)) == FALSE)
	{
		return;
	}

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (strlen(entry_text) == 0)
	{
		return;
	}

	groupe = get_groupOut(sc->liaison_courante);
	for (i = 0; i < group_mode_link_tab[groupe->type].nb_mode; i++)
	{
		mode_link = group_mode_link_tab[groupe->type].mode_tab[i];
		if (strncmp(mode_link.type_lien.nom, entry_text, 100) == 0)
		{
			gtk_entry_set_text(GTK_ENTRY(formulaire_link[No_item_link_mode].widget_entry), Int2Str(mode_link.type_lien.no));
			break;
		}
	}
}

void link_validate_button_callback(GtkWidget * widget, void *user_data)
{
	int i;

	t_gennet_script *script_gui = user_data;

	for (i = 0; i < Number_of_link_items; i++)
	{
		formulaire_link[i].entry_widget_callback(widget, formulaire_link[i].widget_entry, script_gui);
	}

	formulaire_link[No_item_link_comment].entry_widget_callback(widget, formulaire_link[No_item_link_comment].widget_entry, script_gui);

	combo_type_link_callback(widget, combo_type_link_entry, script_gui);
	combo_mode_link_callback(widget, combo_mode_link_entry, script_gui);

	rescale_link(sc->liaison_courante);

	if (script_set_modified(sc)) set_title(script_gui->onglet_leto);

}


/*****************************************************************************/
/* gestion lecture fichier                                                   */

/**
 * Get the selected filename and print it to the console
 */
void file_ok_sel(GtkWidget * w, gpointer data)
{
	char nom[1024];
	int selected_plane; /*necessaire pour stocker var de read_macro mais pas utile ici*/
	(void)w;

	snprintf(nom, sizeof(nom), "%s", gtk_file_selection_get_filename(GTK_FILE_SELECTION(((t_gennet_script *) data)->onglet_leto->filew)));
	if (read_macro(NULL, nom, 0, 0, 0, 0, &selected_plane, ((t_gennet_script *) data)->onglet_leto->hashtab) > 0) /* inclusion directe: mode =0 */
		gtk_widget_hide(((t_gennet_script *) data)->onglet_leto->filew);
}

void file_cancel_sel(GtkWidget * w, gpointer data)
{
	(void)w;
	gtk_widget_hide(((t_gennet_script *) data)->onglet_leto->filew);
}

void creer_acces_fichier(GtkWidget * w, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void)w;
	if (((t_gennet_script *) data)->onglet_leto->filew == NULL)
	{
		((t_gennet_script *) data)->onglet_leto->filew = gtk_file_selection_new("File selection");

		g_signal_connect(G_OBJECT(((t_gennet_script *) data)->onglet_leto->filew), "destroy", G_CALLBACK(gtk_widget_hide), ((t_gennet_script *) data)->onglet_leto->filew);

		g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(((t_gennet_script *) data)->onglet_leto->filew)->ok_button), "clicked", G_CALLBACK(file_ok_sel), data);

		g_signal_connect_swapped(G_OBJECT(GTK_FILE_SELECTION(((t_gennet_script *) data)->onglet_leto->filew)->cancel_button), "clicked", G_CALLBACK(gtk_widget_hide), G_OBJECT(((t_gennet_script *) data)->onglet_leto->filew));

		gtk_file_selection_complete(GTK_FILE_SELECTION(((t_gennet_script *) data)->onglet_leto->filew), "*.script");
	}
	gtk_widget_show(((t_gennet_script *) data)->onglet_leto->filew);
}

/*****************************************************************************/
/*                             SCRIPT CALLBACKS                              */
/*****************************************************************************/

/**
 * When the user has modified a file and tries to close it without having saved
 * it, call this function.
 *
 * @return 1 if user wants to cancel his action, 0 otherwise
 */
int close_without_saving(GtkWidget *window)
{
	GtkWidget *msg_dialog;
	GtkWidget *Winmain;
	int result = 0;

#ifndef LETO
	Winmain = lookup_widget(window, "Winmain");
#else
	Winmain = window;
#endif

	msg_dialog = gtk_message_dialog_new(GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE, "Save the changes before closing ?");
	gtk_dialog_add_buttons(GTK_DIALOG(msg_dialog), "Close _without saving", GTK_RESPONSE_NO, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_YES, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(msg_dialog), GTK_RESPONSE_CANCEL);
	result = gtk_dialog_run(GTK_DIALOG(msg_dialog));
	gtk_widget_destroy(msg_dialog);

	if (result == GTK_RESPONSE_YES)
	{
		if (strlen(sc->nomfich1) == 0)
		{
			save_file_as(NULL, NULL);
			return 0;
		}
		save_file(NULL, NULL);
		return 1;
	}
	else if (result == GTK_RESPONSE_NO)
	{
		return 1;
	}
	else return 0;
}

/* Demande un nom au script ne correspondant a aucun script deja ouvert 
 * l'option sert a savoir si on modifie le nom de l'onglet actuel ou non
 * (car si cette fonction est appelee par la fonction
 * new_script_dans_un_nouvel_onglet, cela poserai des problemes )
 * retourne -1 en cas d'erreur */
int find_new_name_script(TxDonneesFenetre *onglet_leto, int option)
{
	GtkWidget *window;
	GtkWidget *WinMain;
	GtkWidget *hBox;
	GtkWidget *label;
	GtkWidget *e_val;
	GtkWidget *Name_error;
	gint result = -1;
	char *name = NULL;
	int i;

#ifndef LETO
	WinMain = lookup_widget(onglet_leto->window, "Winmain");
#else
	WinMain = onglet_leto->window;
#endif

	window = gtk_dialog_new_with_buttons("Nouveau script", GTK_WINDOW(WinMain), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	hBox = gtk_hbox_new(FALSE, 5);

	label = gtk_label_new("Nom : ");
	gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, TRUE, 0);

	e_val = gtk_entry_new_with_max_length(10);
	gtk_box_pack_start(GTK_BOX(hBox), e_val, FALSE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->vbox), hBox);
	gtk_widget_show_all(window);

	result = gtk_dialog_run(GTK_DIALOG(window));

	switch (result)
	{
	case GTK_RESPONSE_ACCEPT:
		name = ((char *) gtk_entry_get_text(GTK_ENTRY(e_val)));
		if (name == NULL)
		{
			gtk_widget_destroy(window);
			return -1;
		}
		/* on verifie si c'est deja utilisee */
		i = 0;
		while ((strcmp(les_scripts[i].nomfich1, name) != 0) && i < NB_MAX_SCRIPTS)
			i++;
		if (i < NB_MAX_SCRIPTS)
		{
			Name_error = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Nom de script deja��������� utilisee !");
			gtk_dialog_run(GTK_DIALOG(Name_error));
			gtk_widget_destroy(Name_error);
			gtk_widget_destroy(window);
			return -1;
		}
		break;
	default:
		gtk_widget_destroy(window);
		return -1;
		break;
	}

	/* mise a jour du nom du fichier */
	sprintf(sc->nomfich1, "%s.script", name);

	/* changement du nom de l'onglet */
	if (option)
	{
		GtkWidget *pNotebook;
		GtkWidget *page;
		int num_page;

		pNotebook = lookup_widget(onglet_leto->window, "Notebook");

		num_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(pNotebook));

		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(pNotebook), num_page);

		gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(pNotebook), page, name);
	}
	gtk_widget_destroy(window);
	return 0;
}

/**
 * When the user has modified a file and tries to erase it by a new script 
 * without having saved it, call this function.
 *
 * @return 1 if user wants to cancel his action, 0 otherwise
 */
int erase_without_saving(gpointer data)
{
	GtkWidget *msg_dialog;
	GtkWidget *Winmain;
	int result = 0;

#ifndef LETO
	Winmain = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Winmain");
#else
	Winmain = ((t_gennet_script *) data)->onglet_leto->window;
#endif

	msg_dialog = gtk_message_dialog_new(GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE, "Save the changes before erasing ?");
	gtk_dialog_add_buttons(GTK_DIALOG(msg_dialog), "Erase _without saving", GTK_RESPONSE_NO, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_YES, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(msg_dialog), GTK_RESPONSE_CANCEL);
	result = gtk_dialog_run(GTK_DIALOG(msg_dialog));
	gtk_widget_destroy(msg_dialog);

	if (result == GTK_RESPONSE_YES)
	{
		if (strlen(sc->nomfich1) == 0)
		{
			save_file_as(NULL, data);
			return 0;
		}
		save_file(NULL, data);
		return 1;
	}
	else if (result == GTK_RESPONSE_NO)
	{
		return 1;
	}
	else return 0;
}

/* creer un nouveau script vierge */
void new_script(GtkWidget *widget, gpointer data)
{
	(void) widget;

	if (sc->modified != 0 && erase_without_saving((t_gennet_script *) data) == 0) return;

	init_global_var_un_script(sc);
	cree_lien_micro_macro();
	init_target_table();
	/* on reinitialise la table de hachage */
	remove_hash_table(((t_gennet_script *) data)->onglet_leto->hashtab);
	init_hash_table(((t_gennet_script *) data)->onglet_leto->hashtab);

	sc->nomfich1[0] = '\0';

	/* tant que l'utilisateur n'aura pas donnee un nom, on lui redemandera */
	while ((find_new_name_script(((t_gennet_script *) data)->onglet_leto, 0)) != 0)
		;

	sc->freseau[0] = '\0';
	lecture(1, ((t_gennet_script *) data)->onglet_leto); /* lecture recursive */
	sc->liaison_courante = NULL;
	sc->coude_courant = NULL;
	regenerer_test(((t_gennet_script *) data)->onglet_leto);
	show_status(((t_gennet_script *) data)->onglet_leto, "");
	sc->modified = 0;
	set_title(((t_gennet_script *) data)->onglet_leto);
}

/**
 * Displays a file chooser to allow user to load a file.
 */
void open_file(GtkWidget * widget, gpointer data)
{

	GtkWidget *dialog;
	GtkWidget *Winmain;
	char *filename;
	GtkFileFilter *filter;
#ifndef LETO
	GtkWidget *Name_error;
	int i;
	/*int			nb_script;*/

	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	if (sc->modified != 0) if (erase_without_saving(((t_gennet_script *) data)) == 0) return;

#ifndef LETO
	Winmain = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Winmain");
#else
	Winmain = ((t_gennet_script *) data)->onglet_leto->window;
#endif

	dialog = gtk_file_chooser_dialog_new("Open a file...", GTK_WINDOW(Winmain), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "Script");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), "*.script");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), "*.symb");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(filter));

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char filename1[PATH_MAX];
		char *basename = NULL;
#ifndef LETO
		char filename2[PATH_MAX];
		GtkWidget *pNotebook;
		GtkWidget *page;
		int num_page;
#endif

		memset(filename1, 0, PATH_MAX * sizeof(char));
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		if (strlen(filename) == 0) return;

		/* verification si le script n'est pas deja ouvert
		 * si il est deja ouvert, alors on l'ouvre avec un numero */

		/* on copie filename et on enleve l'extension .script */
#ifdef LETO
		chdir(g_path_get_dirname(filename));
#endif /* LETO */
		basename = g_path_get_basename(filename);
		memcpy(filename1, basename, (strlen(basename) + 1) * sizeof(char));

#ifndef LETO
		get_base_name(filename);

		/*nb_script=0;*/
		for (i = 0; i < NB_MAX_SCRIPTS; i++)
		{

			/* a chaque iteration on enleve l'extension .script au nomfich1 de chaque script pour faire la comparaison */
			memset(filename2, 0, sizeof(char) * PATH_MAX); /* reinitialise le tableau */
			memcpy(filename2, les_scripts[i].nomfich1, (strlen(les_scripts[i].nomfich1) + 1) * sizeof(char));
			get_base_name(filename2);

			/* si le script est deja ouvert, alors on le signal */
			if (strncmp(filename, filename2, strlen(filename)) == 0)
			{
				Name_error = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Script already opened.");
				gtk_dialog_run(GTK_DIALOG(Name_error));
				gtk_widget_destroy(Name_error);
				gtk_widget_destroy(dialog);
				return;
			}
			/*	nb_script++;*/
		}

		/* changement du nom de l'onglet */
		pNotebook = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Notebook");

		num_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(pNotebook));

		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(pNotebook), num_page);

		/*memcpy(filename2, filename, (strlen(filename) + 1) * sizeof(char));*/

		/* on ajoute un numero si on en a trouver plusieurs deja ouvert */
		/*if(nb_script>0)
     sprintf(filename2, "%s_%i",filename, nb_script);*/

		gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(pNotebook), page, filename);
		g_object_set_data_full(G_OBJECT(pNotebook), filename, data, NULL);

#endif
		memcpy(sc->nomfich1, filename1, (strlen(sc->nomfich1) + 1) * sizeof(char));

		/* on met a jour le .draw */
		get_base_path_name(filename1);
		strcat(filename1, ".draw");
		memcpy(sc->draw, filename1, (strlen(filename1) + 1) * sizeof(char));

		/* on met a jour le .res */
		get_base_path_name(filename1);
		strcat(filename1, ".res");
		memcpy(sc->freseau, filename1, (strlen(filename1) + 1) * sizeof(char));

		/* on reinitialise la table de hachage */
		remove_hash_table(((t_gennet_script *) data)->onglet_leto->hashtab);
		init_hash_table(((t_gennet_script *) data)->onglet_leto->hashtab);
		lecture(1, ((t_gennet_script *) data)->onglet_leto);

		/* on modifie le nom du fichier pour qu'il soit bien different des scripts precedents */
		/*strcat(filename2, ".script");
     memcpy(sc->nomfich1, filename2, (strlen(filname2) + 1) * sizeof(char));*/

		sc->liaison_courante = NULL;
		sc->coude_courant = NULL;
		sc->modified = 0;
		set_title(((t_gennet_script *) data)->onglet_leto);
		show_status(((t_gennet_script *) data)->onglet_leto, "Loaded script '%s'", sc->nomfich1);
		regenerer_test(((t_gennet_script *) data)->onglet_leto);

		if (filename != NULL) g_free(filename);
	}
	gtk_widget_destroy(dialog);
}

/* sauvegarde un script */
void save_file(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	if (strlen(sc->nomfich1) == 0) save_file_as(widget, data);
	else
	{
		save_script(1, 0, ((t_gennet_script *) data)->onglet_leto); /*sauve les commentaires mais pas les sous scripts */
		sc->modified = 0;
		show_status(((t_gennet_script *) data)->onglet_leto, "Script saved");
	}
	set_title(((t_gennet_script *) data)->onglet_leto);
}

/* sauvegarde un script dans un chemin donnee */
void save_file_as(GtkWidget * widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *msg_dialog;
	GtkWidget *Winmain;

	char *basename = NULL;
	char *filename;
	char *dir;
	char title[40];
	char path[255];
	char filename2[255];

	FILE *file;
	int len;
	gint result;
	(void) widget;


#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;

	Winmain = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Winmain");
#else
	Winmain = ((t_gennet_script *) data)->onglet_leto->window;
#endif
	memset(path, 0, 255 * sizeof(char));
	memset(title, 0, 40 * sizeof(char));
	memset(filename2, 0, 255 * sizeof(char));

	memcpy(title, "Save script As...", (strlen("Save script As...") + 1) * sizeof(char));

	dialog = gtk_file_chooser_dialog_new(title, GTK_WINDOW(Winmain), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		file = fopen(filename, "r");
		if (file != NULL)
		{
			fclose(file);

			if ((dir = strrchr(filename, '/')) != NULL)
			{
				len = (int) (dir - filename);
				memcpy(path, filename, len * sizeof(char));
				path[len] = '\0';

				memcpy(filename2, &filename[len + 1], (strlen(&filename[len + 1]) + 1) * sizeof(char));
			}
			else memcpy(filename2, filename, (strlen(filename) + 1) * sizeof(char));

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "A file named \"%s\" already exists."
					"  Do you want to replace it?", filename2);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog), "The file already exists in "
					"\"%s\".  Replacing it will "
					"overwrite its contents.", path);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));
			gtk_widget_destroy(msg_dialog);

			if (result != GTK_RESPONSE_YES)
			{
				g_free(filename);
				gtk_widget_destroy(dialog);
				return;
			}
		}
		basename = g_path_get_dirname(filename);
#ifdef LETO
		chdir(basename);
#endif /* LETO */
		memcpy(sc->nomfich1, basename, (strlen(basename) + 1) * sizeof(char));

		save_script(1, 0, ((t_gennet_script *) data)->onglet_leto); /* sauve les commentaires mais pas les sous scripts */
		sc->modified = 0;
		set_title(((t_gennet_script *) data)->onglet_leto);
		show_status(((t_gennet_script *) data)->onglet_leto, "Script saved");

		g_free(filename);
	}

	gtk_widget_destroy(dialog);
}

void copy_selection(GtkWidget * widget, gpointer data)
{
	char nom[1024];
	(void) widget;

#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

	sprintf(nom, "%s/.leto/copy_buffer.script", getenv("HOME"));

	save_script_selected(1, 0, nom, ((t_gennet_script *) data)->onglet_leto);
	show_status(((t_gennet_script *) data)->onglet_leto, "Selection copied");
}

void paste_selection(GtkWidget * widget, gpointer data)
{
	int selected_plane; /*necessaire pour stocker var de read_macro mais pas utile ici*/

	char nom[1024];
	(void) widget;


#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

	sprintf(nom, "%s/.leto/copy_buffer.script", getenv("HOME"));

	/* le mode est mis a 0 pour inclusion directe avec affichage et sauvegarde (modif groupe->deja_active=0) */
	read_macro(NULL, nom, sc->point_courant_leto.x, sc->point_courant_leto.y, 1, 0, &selected_plane, ((t_gennet_script *) data)->onglet_leto->hashtab); /* position dans leto en relatif au point courant */
	reset_selection_groups();
	show_status(((t_gennet_script *) data)->onglet_leto, "Pasted");
	regenerer_test(((t_gennet_script *) data)->onglet_leto);
}

void enter_callback(GtkWidget * widget, GtkWidget * entry, void *user_data)
{
	(void) widget;
	(void)user_data;

	gtk_entry_get_text(GTK_ENTRY(entry));
}

/*---------------------------------------------------------------------------*/

/* on laisse les commentaires et on ne prend pas les sous reseaux*/
void sauve_selection(GtkWidget * widget, gpointer data)
{
	(void)widget;
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

	save_script_selected(1, 0, NULL, ((t_gennet_script *) data)->onglet_leto);
}

/* on laisse les commentaires et on  prend  les sous reseaux*/
void sauve_selection_a_plat(GtkWidget * widget, gpointer data)
{
	(void)widget;

#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

	save_script_selected(1, 1, NULL, ((t_gennet_script *) data)->onglet_leto);
}

gboolean set_binary_file(GtkWidget * widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *table1;
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *Winmain;

	int result;
	char buf[255];
	char buf2[255];
	gboolean res;
	(void)widget;

	memset(buf, 0, 255 * sizeof(char));
	memset(buf2, 0, 255 * sizeof(char));
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return TRUE;

	Winmain = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Winmain");
#else
	Winmain = ((t_gennet_script *) data)->onglet_leto->window;
#endif

	dialog = gtk_dialog_new_with_buttons("Set binary file (.res)...", GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);

	table1 = gtk_table_new(2, 1, FALSE);
	gtk_widget_ref(table1);
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), table1);

	label = gtk_label_new("Enter a file name :");
	entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

	snprintf(buf, 255, "%s", sc->freseau);

	if (strlen(buf) == 0) set_file_with_ext(buf, ".res");

	gtk_entry_set_text(GTK_ENTRY(entry), buf);

	gtk_widget_ref(label);
	gtk_widget_ref(entry);
	gtk_widget_show(label);
	gtk_widget_show(entry);
	gtk_table_attach(GTK_TABLE(table1), label, 0, 1, 0, 1, (GtkAttachOptions) GTK_SHRINK, (GtkAttachOptions) (0), 5, 5);
	gtk_table_attach(GTK_TABLE(table1), entry, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 5, 5);

	/* gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 200); */
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

	gtk_widget_show(dialog);

	result = gtk_dialog_run(GTK_DIALOG(dialog));

	if (result == GTK_RESPONSE_OK)
	{
		sscanf(gtk_entry_get_text(GTK_ENTRY(entry)), "%s", buf2);

		if (strlen(buf2) != 0)
		{
			memcpy(sc->freseau, buf2, (strlen(buf2) + 1) * sizeof(char));
			show_status(((t_gennet_script *) data)->onglet_leto, "Binary file set to : %s", buf2);
			res = TRUE;
		}
		else
		{
			show_status(((t_gennet_script *) data)->onglet_leto, "You must enter a valid file name");
			res = FALSE;
		}
	}
	else res = FALSE;
	gtk_widget_destroy(dialog);
	return res;
}

void set_random_gen(GtkWidget * widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *table1;
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *Winmain;

	static int val = 1;
	int result;
	char buf[80];
	(void)widget;


#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;

	Winmain = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Winmain");
#else
	Winmain = ((t_gennet_script *) data)->onglet_leto->window;
#endif

	dialog = gtk_dialog_new_with_buttons("Pseudo random generator", GTK_WINDOW(Winmain), GTK_DIALOG_MODAL, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);

	table1 = gtk_table_new(2, 1, FALSE);
	gtk_widget_ref(table1);
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), table1);

	label = gtk_label_new("Enter an integer value :");
	entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

	snprintf(buf, 80, "%d", val);
	gtk_entry_set_text(GTK_ENTRY(entry), buf);

	gtk_widget_ref(label);
	gtk_widget_ref(entry);
	gtk_widget_show(label);
	gtk_widget_show(entry);
	gtk_table_attach(GTK_TABLE(table1), label, 0, 1, 0, 1, (GtkAttachOptions) GTK_SHRINK, (GtkAttachOptions) (0), 5, 5);
	gtk_table_attach(GTK_TABLE(table1), entry, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 5, 5);

	/* gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 200); */
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
	gtk_widget_show(dialog);

	result = gtk_dialog_run(GTK_DIALOG(dialog));
	if (result == GTK_RESPONSE_OK)
	{
		sscanf(gtk_entry_get_text(GTK_ENTRY(entry)), "%d", &val);
		srand48((long) val);
		sc->seed = val;
		snprintf(buf, 80, "Pseudo random generator value set to : %d", val);
		show_status(((t_gennet_script *) data)->onglet_leto, buf);
	}

	gtk_widget_destroy(dialog);
}

/* For the radio buttons */
void print_selected(GtkAction *action, gpointer callback_data)
{
	char *text = NULL;
	int plane = -1;
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) callback_data)->onglet_leto) == 0) return;
#endif
	text = (char *) gtk_action_get_name(action);

	sscanf(text, "/View/Unselect plane/Plane %i", &plane);
	select_plane(plane, ((t_gennet_script *) callback_data)->onglet_leto);
}

/*****************************************************************************/

void refresh(GtkWidget * widget, gpointer data)
{
	(void)widget;

#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	regenerer_test(((t_gennet_script *) data)->onglet_leto);
}

/************************************************************/

/**
 * Create a new pixmap of the appropriate size to store our scribbles
 */
gboolean scribble_configure_event(GtkWidget * widget, GdkEventConfigure * event, t_gennet_script *script_gui)
{
	(void) event;

	if (script_gui->onglet_leto->pixmap == NULL)
	{
		script_gui->onglet_leto->pixmap = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
		script_gui->onglet_leto->window_pixmap = widget->window;
		script_gui->onglet_leto->widget_pixmap = widget;
		script_gui->onglet_leto->graphic = NULL;

	}

	/* Initialize the pixmap to white */
	gdk_draw_rectangle(script_gui->onglet_leto->pixmap, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);

	script_gui->onglet_leto->xmin = 0;
	script_gui->onglet_leto->ymin = 0;
	script_gui->onglet_leto->xmax = widget->allocation.width;
	script_gui->onglet_leto->ymax = widget->allocation.height;

	regenerer_test(script_gui->onglet_leto);

	TxDisplay(script_gui->onglet_leto);

	/* We've handled the configure event, no need for further processing. */
	return TRUE;
}

/**
 * Redraw the screen from the pixmap
 */
gboolean scribble_expose_event(GtkWidget * widget, GdkEventExpose * event, t_gennet_script *script_gui)
{
	/* We use the "foreground GC" for the widget since it already exists,
	 * but honestly any GC would work. The only thing to worry about
	 * is whether the GC has an inappropriate clip region set.
	 */

	gdk_draw_drawable(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], script_gui->onglet_leto->pixmap, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);

	return FALSE;
}

gboolean scribble_motion_notify_event(GtkWidget * widget, GdkEventMotion * event, t_gennet_script *script_gui)
{
	/*int x, y;*/
	TxPoint point, translation;

	/* This call is very important; it requests the next motion event.
	 * If you don't call gdk_window_get_pointer() you'll only get
	 * a single motion event. The reason is that we specified
	 * GDK_POINTER_MOTION_HINT_MASK to gtk_widget_set_events().
	 * If we hadn't specified that, we could just use event->x, event->y
	 * as the pointer location. But we'd also get deluged in events.
	 * By requesting the next event as we handle the current one,
	 * we avoid getting a huge number of events faster than we
	 * can cope.
	 */
	/* gdk_window_get_pointer(event->window, &x, &y, &state);*/

	if (event->is_hint)
	{
		if (event->state & GDK_BUTTON1_MASK)
		{
			point.x = event->x;
			point.y = event->y;

			if (sc->flag_drag_group == 1)
			{
				drag_selected_groups(point);
				scroll(point, script_gui->onglet_leto);
				regenerer_test(script_gui->onglet_leto);
			}
			else if (sc->flag_drag_coude == 1)
			{
				if (deplace_coude_courant(sc, point.x, point.y) == 1)
				{
					scroll(point, script_gui->onglet_leto);
					regenerer_test(script_gui->onglet_leto);
				}
			}
			else if (sc->flag_rectangle_selection == 1)
			{
				draw_selection(widget, point.x, point.y, script_gui->onglet_leto, event->state & GDK_CONTROL_MASK);
			}
		}
		else if ((event->state & GDK_BUTTON2_MASK) && (script_gui->sc->groupes_courants == NULL))
		{
			translation.x = event->x - script_gui->sc->pointer_selection_center.x;
			translation.y = event->y - script_gui->sc->pointer_selection_center.y;
			script_slide(script_gui->sc, translation);
			script_gui->sc->pointer_selection_center.x = event->x;
			script_gui->sc->pointer_selection_center.y = event->y;
			regenerer_test(script_gui->onglet_leto);
		}
		else return FALSE;
	}
	else return FALSE;

	/* We've handled it, stop processing */
	return TRUE;
}

/*si libg2 alors on ajoute la fonction export de scripts*/
#ifdef EXPORT
void export_cb(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if(tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto)==0) return;
#endif

	test_dessiner();
}
#endif

/**
 * Callback to modify a group or a link.
 */
void modify_callback(GtkWidget * widget, t_gennet_script *script_gui)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(script_gui->onglet_leto) == 0) return;
#endif
	(void) widget;

	if (sc->groupes_courants != NULL)
	{
		formulaire_groupe(script_gui);
	}
	else if (sc->liaison_courante != NULL)
	{
		formulaire_lien(sc->liaison_courante, script_gui);
	}

	regenerer_test(script_gui->onglet_leto);
}

/**
 * Callback to automatically rename groups
 */
void automatic_rename_callback(GtkWidget * widget, t_gennet_script *script_gui)
{
	TxDonneesFenetre *onglet_leto = script_gui->onglet_leto;

#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(onglet_leto) == 0) return;
#endif

	(void) widget;
	if (sc->groupes_courants != NULL)
	{
		automatic_group_rename(script_gui);
		show_status(onglet_leto, "Group(s) renamed");
	}

	regenerer_test(script_gui->onglet_leto);
}

/**
 * Callback to automatically comment groups
 */
void automatic_comment_callback(GtkWidget * widget, t_gennet_script *script_gui)
{
    TxDonneesFenetre *onglet_leto = script_gui->onglet_leto;

#ifndef LETO
    /* controle si on est dans un onglet Leto
     * ( l'onglet Metaleto etant le numero 0 )
     */
    if (tab_is_Metaleto(onglet_leto) == 0) return;
#endif

    (void) widget;
    if (sc->groupes_courants != NULL)
    {
        automatic_group_comment(script_gui);
        show_status(onglet_leto, "Group(s) commented");
    }

    regenerer_test(script_gui->onglet_leto);
}

/**
 * Callback to delete a group or a link.
 */
void delete_callback(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	if (sc->groupes_courants != NULL)
	{
		detruit_ensemble_groupes(((t_gennet_script *) data)->onglet_leto);
		show_status(((t_gennet_script *) data)->onglet_leto, "Group(s) deleted");
	}
	else if (sc->liaison_courante != NULL)
	{
		detruit_liaison(sc->liaison_courante, ((t_gennet_script *) data)->onglet_leto);
		sc->coude_courant = NULL;
		show_status(((t_gennet_script *) data)->onglet_leto, "Link deleted");
	}
	regenerer_test(((t_gennet_script *) data)->onglet_leto);
}

void delete_coude_callback(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	if (sc->coude_courant != NULL && sc->liaison_courante != NULL)
	{
		efface_coude();
		regenerer_test(((t_gennet_script *) data)->onglet_leto);
		show_status(((t_gennet_script *) data)->onglet_leto, "Bend deleted");
	}
}

void dupliquer_coude_callback(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

	if (sc->liaison_courante != NULL)
	{
		if (dupliquer_coude(widget, data) == 1)
		{
			regenerer_test(((t_gennet_script *) data)->onglet_leto);
		}
		show_status(((t_gennet_script *) data)->onglet_leto, "Bend inserted");
	}
}

void delete_all_coudes_callback(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	if (sc->liaison_courante != NULL)
	{
		efface_coudes();
		regenerer_test(((t_gennet_script *) data)->onglet_leto);
		show_status(((t_gennet_script *) data)->onglet_leto, "All bends deleted");
	}
}

/* fonction gerant les differentes actions a la souris:
 deplacement, selection, suppression, creation, modification (liens et groupes) ... */
gboolean mouse_clicked(GtkWidget * widget, GdkEventButton * event, t_gennet_script *script_gui)
{
	TxPoint point;
	type_groupe *group;
	int popup_menu_type = -1;
	(void) widget;

	/* Release dragged objects */
	sc->flag_drag_coude = 0;
	sc->flag_drag_group = 0;

	/*gdk_window_get_pointer(event->window, &x, &y, &state);*/
	point.x = event->x;
	point.y = event->y;
	sc->point_courant_leto = point;

	if ((event->state & GDK_CONTROL_MASK) && (event->button == 1))
	{
		group = selection_groupe(); /* ou de-selection si deja selectionne */
		if (group != NULL)
		{
			show_status(script_gui->onglet_leto, "Group %s added to selection", group->no_name);
		}
		sc->flag_rectangle_selection = 1;
		return TRUE;
	}

	if (event->button == 1 || event->button == 3)
	{
		/* Detecting objects clicked upon */
		if (gere_modification_lien(script_gui) == 0)
		{
			test_link_position(point);
			gere_coude(point, script_gui->onglet_leto);
			gere_groupe(point);
		}

		/* Handling object selection priority : bend > group > link */
		if (sc->coude_courant != NULL)
		{
			show_status(script_gui->onglet_leto, "Bend selected");
			reset_selection_groups();
			sc->flag_drag_group = 0;
			popup_menu_type = POPUP_MENU_BEND;
		}
		else if (sc->groupes_courants != NULL)
		{
			show_status(script_gui->onglet_leto, "Group(s) selected");
			sc->liaison_courante = NULL;
			popup_menu_type = POPUP_MENU_GROUP;
		}
		else if (sc->liaison_courante != NULL)
		{
			show_status(script_gui->onglet_leto, "Link selected");
			popup_menu_type = POPUP_MENU_LINK;
		}
		else
		{
			show_status(script_gui->onglet_leto, "");
			popup_menu_type = POPUP_MENU_NO_SELECTION;
			reset_selection_groups();
			sc->flag_rectangle_selection = 1;
		}
	}

	if (event->button == 1)
	{
		/* Double click => edit menu, deselect region */
		if (event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS)
		{
			if (sc->groupes_courants != NULL)
			{
				sc->flag_drag_group = 0;
				formulaire_groupe(script_gui);
			}
			else if (sc->liaison_courante != NULL)
			{
				/* Link edit */
				formulaire_lien(sc->liaison_courante, script_gui);
			}
		}
	}
	else if (event->button == 2) /* Click molette */
	{
		reset_selection_groups();
		sc->flag_drag_coude = 0;
		sc->flag_drag_group = 0;

		/*
     if (sc->coude_courant != NULL)
     {
     deplace_coude_courant(sc, point.x, point.y);
     }
     else if (sc->groupes_courants != NULL)
     {
     move_selected_groups(point);
     }
     else
     {
     }
     sc->flag_drag_group = 1;*/
		sc->pointer_selection_center.x = event->x;
		sc->pointer_selection_center.y = event->y;

	}
	else if (event->button == 3) /* click droit */
	{
		sc->flag_drag_group = 0;
		sc->flag_drag_coude = 0;
		show_popup_menu(popup_menu_type, event, script_gui->onglet_leto);
	}

	regenerer_test(script_gui->onglet_leto);

	return TRUE;
}

gboolean mouse_released(GtkWidget * widget, GdkEventButton * event, t_gennet_script *script_gui)
{
	(void)widget;

	(void) event;
	sc->flag_rectangle_selection = 0;

	if (sc->flag_drag_coude == 1)
	{
		if (script_set_modified(sc)) set_title(script_gui->onglet_leto);
		sc->flag_drag_coude = 0;
	}

	if (sc->flag_drag_group == 1)
	{
		if (script_set_modified(sc)) set_title(script_gui->onglet_leto);
		sc->flag_drag_group = 0;
	}

	regenerer_test(script_gui->onglet_leto);
	return TRUE;
}

gboolean mouse_scroll(GtkWidget *widget, GdkEventScroll *event, t_gennet_script *script_gui)
{
	TxPoint center;
	(void) widget;

	if (event->state & GDK_CONTROL_MASK)
	{
		center.x = event->x;
		center.y = event->y;

		switch (event->direction)
		{
		case GDK_SCROLL_DOWN:
			script_zoom(script_gui->sc, center, 0.75);
			regenerer_test(script_gui->onglet_leto);

			break;
		case GDK_SCROLL_UP:
			script_zoom(script_gui->sc, center, 1.33);
			regenerer_test(script_gui->onglet_leto);

			break;
		default:
			break;
		}
		return TRUE;
	}
	return FALSE;
}

/**
 * Displays information about the application
 */
void about(GtkWidget * widget, gpointer data)
{
	GtkWidget *Winmain;

	const gchar *authors[] =
	{ "P. Gaussier", "A. Revel", "M. Quoy", "C. Joulain", "S. Lepretre", "S. Moga", "J.C. Baccon", "P. Andry", "C. Giovannangeli", "M. Maillard", "N. Cuperlier", "F. Archambault", "...", "L. Bridelance", "N. Cazin", "N. Daniel", "N. Dorotte", NULL };

	static const gchar copyright[] = "Copyright \xc2\xa9 1991-2006 Neurocybernetic Team\n"
			"ETIS Lab., CNRS UMR 8051";

	static const gchar comments[] = "Leto is an editor and compiler for Artificial Neural Networks";

	int rev = 0;
	gchar version[20];
	(void) widget;


#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

	snprintf(version, 20, "$Rev: 614 $");
	sscanf(version, "$Rev: %d$", &rev);
	snprintf(version, 20, "5.1 build %d", rev);

#ifndef LETO
	Winmain = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Winmain");
#else
	Winmain = ((t_gennet_script *) data)->onglet_leto->window;
#endif

	gtk_show_about_dialog(GTK_WINDOW(Winmain), "authors", authors, "comments", comments, "copyright", copyright, "version", version, "website", "http://www-etis.ensea.fr/Equipes/Neuro", "name", "Leto", NULL);
}

/**
 * This function is called when leto is closed.
 * We check if modifications have been made but aren't saved yet.
 */
gboolean leto_quit(GtkWidget * widget, gpointer data)
{
	char command_line[PATH_MAX];
	gint res;
	(void) data;

	if (sc->modified != 0)
	{
		res = close_without_saving(widget);
		if (res == 0) return TRUE;
	}

	sprintf(command_line, "rm -f %s.bak*", sc->nomfich1);

	if (system(command_line) != 0) PRINT_WARNING("Fail suppressing history");
	printf("`%s`\nBye.\n", command_line);
	gtk_main_quit();
	return FALSE;
}

void tabLeto_removed(GtkNotebook *notebook, GtkWidget *child, int page_num, t_gennet *coeos)
{
	int i = 0;
	int nPage = 0;
	t_gennet_script *pscript = NULL;
	(void) child;

	nPage = gtk_notebook_get_current_page(notebook);

	/* old script */
	for (pscript = coeos->scripts; pscript != NULL; pscript = pscript->next)
	{
		if ((pscript->sc != NULL) && (pscript->sc->num_onglet == page_num))
		{
			break;
		}
	}

	if (pscript == NULL)
	{
		return;
	}

	/* on supprime les raccourcis clavier */
	/* Probleme: les raccourcis sont definis pour la fenetre principale, il ne faut pas les supprimer lorsqu'on enleve un onglet */
	/*Winmain = lookup_widget (pscript->onglet_leto->window ,"Winmain");
   accel_group = lookup_widget (pscript->onglet_leto->window ,"accel_group");
   gtk_window_remove_accel_group(GTK_WINDOW(Winmain),GTK_ACCEL_GROUP(accel_group));*/

	/* par precaution on vide la table de hachage de l'onglet */
	remove_hash_table(pscript->onglet_leto->hashtab);
	free(pscript->onglet_leto->hashtab);

	free(pscript->onglet_leto);
	pscript->onglet_leto = NULL;

	free(pscript->fenetre_dialogue);
	pscript->fenetre_dialogue = NULL;

	memset(pscript->sc, 0, sizeof(donnees_script));
	pscript->sc = NULL;
	sc = NULL;

	for (i = 0; i < NB_MAX_SCRIPTS; i++)
	{
		/* on met a jour le numero d'onglet des scripts */
		if (les_scripts[i].num_onglet > page_num)
		{
			les_scripts[i].num_onglet--;
		}

		if (les_scripts[i].num_onglet == nPage && nPage != 0)
		{
			sc = &les_scripts[i];
		}
	}

	changed_tab(notebook, NULL, nPage, coeos);
}

void slide_groups_up(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	slide_groups(0, -50, 0);

	if (script_set_modified(sc)) set_title(((t_gennet_script *) data)->onglet_leto);

	regenerer_test(((t_gennet_script *) data)->onglet_leto);
}

void slide_groups_down(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	slide_groups(0, 50, 0);

	if (script_set_modified(sc)) set_title(((t_gennet_script *) data)->onglet_leto);

	regenerer_test(((t_gennet_script *) data)->onglet_leto);
}

void slide_groups_left(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	slide_groups(-50, 0, 0);

	if (script_set_modified(sc)) set_title(((t_gennet_script *) data)->onglet_leto);

	regenerer_test(((t_gennet_script *) data)->onglet_leto);
}

void slide_groups_right(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	slide_groups(50, 0, 0);

	if (script_set_modified(sc)) set_title(((t_gennet_script *) data)->onglet_leto);

	regenerer_test(((t_gennet_script *) data)->onglet_leto);
}

void creer_point_script(GtkWidget * widget, gpointer data)
{
	char file_script[PATH_MAX], chemin_env[PATH_MAX];

	printf("Appel a la compilation du script\n");
	save_file(widget, data);
	memset(file_script, 0, sizeof(char) * PATH_MAX); /* initialise le tableau */

	memcpy(file_script, sc->nomfich1, (strlen(sc->nomfich1) + 1) * sizeof(char));


#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

	/**************************************************************
	 *                                                            *
	 * Si on est en version symbolique, c'est a dire              *
	 * qu'il y a des variables de definies au lieu de nombres,    *
	 * alors on demande ou se trouve le fichier.var qui contient  *
	 * les valeurs de chaque variable,                            *
	 * puis on compile par le lpprocess pour avoir le .script     *
	 * avant de compiler pour obtenir un .res                     *
	 *                                                            *
	 **************************************************************/
	if (sc->flag_symb == 1)
	{

		GtkWidget *dialog;
		GtkWidget *Winmain;
		gint result = -1;
		char *filename = NULL;
		GtkFileFilter *filter;
		int ret = -1;
		int preprocOK = 0;

#ifndef LETO
		Winmain = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Winmain");
#else
		Winmain = ((t_gennet_script *) data)->onglet_leto->window;
#endif

		while (preprocOK != 1)
		{ /* en fait juste test sur le .var */
			if (strlen(sc->fvar) == 0)
			{

				dialog = gtk_file_chooser_dialog_new("Open a file.var", GTK_WINDOW(Winmain), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

				filter = gtk_file_filter_new();
				gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "Variable");
				gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), "*.var");
				gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(filter));

				result = gtk_dialog_run(GTK_DIALOG(dialog));

				switch (result)
				{
				case GTK_RESPONSE_ACCEPT:
					filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
					break;
				case GTK_RESPONSE_REJECT:
					gtk_widget_destroy(dialog);
					return;
				default:
					gtk_widget_destroy(dialog);
					return;
				}
				gtk_widget_destroy(dialog);
				memcpy(sc->fvar, filename, (strlen(filename) + 1) * sizeof(char));
			}

			get_base_path_name(file_script);
			strcat(file_script, ".script");

			/* le script lpreprocess demande en argument le fichier.symb le fichier.var et le fichier.script */
			sprintf(chemin_env, "%s/lpreprocess.sh %s %s %s", bin_leto_prom_path, sc->nomfich1, sc->fvar, file_script);
			/* on cree un processus qui va executer le lpprocess
			 * car exec ecrase le processus courant */
			ret = system(chemin_env);
			ret = ret >> 8; /*code retour sur les 8bits de poids fort*/

			if (ret == 3)
			{ /* code .var missing. See lpreprocess.sh*/
				/* le nom actuel n'est pas correcte */
				memcpy(sc->fvar, "\0", 1);
				printf("File .var is missing: ASKING FOR .VAR\n");
			}
			else
			{
				/* if .var ok alors preproc ok (?)*/
				preprocOK = 1;
				/*copier le script dans la structure pour le passer a la compile du res*/
				memcpy(sc->fscript, file_script, (strlen(file_script) + 1) * sizeof(char));
				/* printf ("Compile script reussi : symb = %s, var = %s script = %s\n",sc->nomfich1,sc->fvar,sc->nomScript);*/
			}
		}
	}
	else
	{
		/*Si le fichier est deja un .script : copier directement son nom dans sc->fscript et l'envoyer se faire compiler le res*/

		memcpy(sc->fscript,file_script, (strlen(file_script) + 1) * sizeof(char));
		/*printf("Le fichier est un .script pas besoin de compiler le script : %s \n",sc->fscript );*/
	}


}

void compile_script_cb(GtkWidget * widget, gpointer data)
{
	/* donnees_script sc_tmp, *sc_save; */
	/* char script[MAX_PATH],file_res[MAX_PATH],draw[MAX_PATH]; */
	/* TxDonneesFenetre *onglet_leto_tmp; */
	char chemin_env[PATH_MAX];
	char flag_save[64] = "--no_res";

	printf("Callbacks : compilation script + res non ecrase\n");
	creer_point_script(widget, data);
	/*compiler le res sans l'enregistrer*/
	sprintf(chemin_env, "echo %li | %s/cc_leto %s %s %s", sc->seed, bin_leto_prom_path, sc->fscript, sc->freseau,flag_save);
		if (system(chemin_env) != 0) EXIT_ON_ERROR("Cannot executing system command '%s'", chemin_env);
}

void creation_cb(GtkWidget * widget, gpointer data)
{
	gboolean res = TRUE;
	int i;
	char chemin_env[PATH_MAX];

	/*dans le cas d un script simple le nom du script est dans sc->nomfich1 et non dans sc->fscript*/
	printf("Callbacks : compilation script + nouveau res\n");

	creer_point_script(widget, data);
	if (strlen(sc->nomfich1) == 0) return;

	if (strlen(sc->freseau) == 0) res = set_binary_file(widget, data);

	if (res == FALSE || strlen(sc->freseau) == 0) return;
	sprintf(chemin_env, "echo %li | %s/cc_leto %s %s", sc->seed, bin_leto_prom_path, sc->fscript, sc->freseau);
	if (system(chemin_env) != 0) EXIT_ON_ERROR("Cannot executing system command '%s'", chemin_env);

	/*******************************************************************
	 * Dans le cas de la version symbolique et donc d'un .symb         *
	 * on utilise un sc temporaire, on memorise le script courant (sc) *
	 * on fait une lecture sur le .script (creer par le preprocess)    *
	 * puis on effectue la creation du .res                            *
	 * pour finir on reprend l'ancien sc                               *
	 *******************************************************************/

	/* memset(&sc_tmp, 0,sizeof(donnees_script)); */

	/* if(sc->flag_symb==1) */
	/*   { */
	/*   if ((onglet_leto_tmp = malloc(sizeof(TxDonneesFenetre))) == NULL) */
	/*     { */
	/* 	g_critical("creation_cb : Memory error"); */
	/* 	/\* fermer ce qu'il faut *\/ */
	/* 	return ;			 */
	/*     } */
	/*   memset(onglet_leto_tmp, 0, sizeof(TxDonneesFenetre)); */
	/*   /\* initialise la table de hachage d'un onglet *\/ */
	/*   init_hash_table((void **)&onglet_leto_tmp->hashtab); */

	/*   memset(script,0,sizeof(char)*MAX_PATH); /\* initialise le tableau *\/ */
	/*   memcpy(script,sc->nomfich1, (strlen(sc->nomfich1) + 1) * sizeof(char)); */
	/*   get_base_path_name(script); */
	/*   strcat(script,".script"); */

	/*   memset(file_res,0,sizeof(char)*MAX_PATH); /\* initialise le tableau *\/ */
	/*   memcpy(file_res,sc->freseau, (strlen(sc->freseau) + 1) * sizeof(char)); */

	/*   memset(draw,0,sizeof(char)*MAX_PATH); /\* initialise le tableau *\/ */
	/*   memcpy(draw,sc->draw, (strlen(sc->draw) + 1) * sizeof(char)); */

	/*   sc_save = sc; */
	/*   sc=&sc_tmp; */

	/*   init_global_var_un_script(sc); */
	/*   cree_lien_micro_macro();   /\* pour pouvoir avoir dans le script les info  */
	/*                                  sur les liens dans le macro neurone *\/  */
	/*   /\*init_display_planes(); *\/ */
	/*   init_target_table(); */

	/*   memcpy(sc->nomfich1,script,(strlen(script)+1) * sizeof(char)); */
	/*   memcpy(sc->freseau,file_res,(strlen(file_res)+1) * sizeof(char)); */
	/*   memcpy(sc->draw,draw,(strlen(draw)+1) * sizeof(char)); */

	/*   onglet_leto_tmp->window=NULL; */
	/*   lecture(1,onglet_leto_tmp); */

	/*   creation(((t_gennet_script *) data)->onglet_leto); */

	/*   free(onglet_leto_tmp); */

	/*   sc = sc_save; */
	/* } */
	/* else */
	/*   creation(((t_gennet_script *) data)->onglet_leto); */

	show_status(((t_gennet_script *) data)->onglet_leto, "Compiled to binary file : %s", sc->freseau);
}

/* gestion de l'evenement : deplacement horizontal du scroll */
void changedx(GtkWidget * widget, gpointer data)
{
	sc->xmin = (int) gtk_adjustment_get_value(GTK_ADJUSTMENT(widget));
	sc->xmax = sc->xmin + (int) (GTK_ADJUSTMENT(widget)->page_size);
	regenerer_test(((TxDonneesFenetre *) data));
}

/* gestion de l'evenement : deplacement vertical du scroll */
void changedy(GtkWidget * widget, gpointer data)
{
	sc->ymin = (int) gtk_adjustment_get_value(GTK_ADJUSTMENT(widget));
	sc->ymax = sc->ymin + (int) (GTK_ADJUSTMENT(widget)->page_size);
	regenerer_test(((TxDonneesFenetre *) data));
}

/* gestion de l'evenement : redimensionnement de la fenetre */
void resized_cb(GtkWidget * widget, gpointer data)
{
	int width;
	int height;
	t_gennet_script *pscript = NULL;
#ifndef LETO
	int page_num;
	page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(((t_gennet *) data)->gui->tabbedWin));
	for (pscript = ((t_gennet *) data)->scripts; pscript != NULL; pscript = pscript->next)
	{
		if ((pscript->sc != NULL) && (pscript->sc->num_onglet == page_num) && (page_num != 0))
		{
			break;
		}
	}
	if (pscript == NULL) return;
#else
	pscript = (t_gennet_script *) data;
#endif /* LETO*/

	if (pscript->onglet_leto == NULL) return;

	gtk_window_get_size(GTK_WINDOW(widget), &width, &height);

	if (sc != NULL)
	{
		sc->xmax = sc->xmin + width;
		sc->ymax = sc->ymin + height;
		regenerer_test(pscript->onglet_leto);
	}
}

/* gestion de l'evenement : changement d'onglet */
void changed_tab(GtkNotebook *notebook, GtkNotebookPage *page, int page_num, t_gennet *coeos)
{
	t_gennet_script *pscript = NULL, *sel_script = NULL;
	int i = 0;
	(void) page;
	(void)notebook;

	/* recherche du script : les scripts ne commencant qu'a partir de la page 1,
	 * on test si la page courante est egal a 0 ou pas */

	if (page_num == 0)
	{
		sc = NULL;
		return;
	}

	while (les_scripts[i].num_onglet != page_num && i < NB_MAX_SCRIPTS)
		i++;

	if (i == NB_MAX_SCRIPTS)
	{
		fprintf(stderr, "changed_tab : aucun script trouve\n");
		return;
	}

	/* mise a jour du sc : le script courant */
	sc = &les_scripts[i];

	/* pscript new */
	for (pscript = coeos->scripts; pscript != NULL; pscript = pscript->next)
	{
		if (pscript->sc != NULL && pscript->sc == sc && pscript->sc->num_onglet == page_num)
		{
			/* active ActionGroup */
			gtk_action_group_set_sensitive(pscript->action_group, TRUE);
			sel_script = pscript;
		}
		else
		{
			gtk_action_group_set_sensitive(pscript->action_group, FALSE);
		}
	}

	if (sel_script == NULL) return;

	if (sc != sel_script->sc) fprintf(stdout, "INCOHERENT SCRIPT SC %p %p\n", (void *) sc, (void *) sel_script->sc);

	regenerer_test(sel_script->onglet_leto);
}

/* l'ancien script est simplement oublie... */
void reload_script(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif
	(void) widget;

	if (sc->modified != 0) if (erase_without_saving(((t_gennet_script *) data)) == 0) return;
	sc->modified = 0;
	set_title(((t_gennet_script *) data)->onglet_leto);
	sc->liaison_courante = NULL;

	/* on reinitialise la table de hachage */
	remove_hash_table(((t_gennet_script *) data)->onglet_leto->hashtab);
	init_hash_table(((t_gennet_script *) data)->onglet_leto->hashtab);

	lecture(1, ((t_gennet_script *) data)->onglet_leto);
	show_status(((t_gennet_script *) data)->onglet_leto, "Reloaded script '%s'", sc->nomfich1);
}

void edit_variables(GtkWidget * widget, gpointer data)
{
	char command_line[COMMAND_LINE_MAX];
	const char *editor;
	int error;
	t_gennet_script *gennet_script = data;
	t_prom_script *prom_script = gennet_script->prom_script;
	(void)widget;

#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(gennet_script->onglet_leto) == 0) return;
#endif

	editor = getenv("EDITOR");
	if (editor == NULL) editor = "gedit";

	snprintf(command_line, COMMAND_LINE_MAX, "%s %s/%s &", editor, prom_script->path_prom_deploy, prom_script->path_file_var);
	error = system(command_line);

	if (error != 0)
	{
		PRINT_WARNING("Error launching %s.\n\t Check that %s is correct.", editor, command_line);
	}
}

void block_transfo(GtkWidget * widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *msg_dialog;
	GtkWidget *Winmain;

	char *filename;
	char *dir;
	char title[40];
	char path[255];
	char filename2[255];

	FILE *file;
	int len;
	gint result;
	(void) widget;

	memset(title, 0, 40 * sizeof(char));
	memset(path, 0, 255 * sizeof(char));
	memset(filename2, 0, 255 * sizeof(char));
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;

	Winmain = lookup_widget(((t_gennet_script *) data)->onglet_leto->window, "Winmain");
#else
	Winmain = ((t_gennet_script *) data)->onglet_leto->window;
#endif

	memcpy(title, "Save Block As...", (strlen("Save Block As...") + 1) * sizeof(char));

	dialog = gtk_file_chooser_dialog_new(title, GTK_WINDOW(Winmain), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		file = fopen(filename, "r");
		if (file != NULL)
		{
			fclose(file);

			if ((dir = strrchr(filename, '/')) != NULL)
			{
				len = (int) (dir - filename);
				memcpy(path, filename, (len + 1) * sizeof(char));
				path[len] = '\0';

				memcpy(filename2, &filename[len + 1], (strlen(&filename[len + 1]) + 1) * sizeof(char));
			}
			else memcpy(filename2, filename, (strlen(filename) + 1) * sizeof(char));

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(((t_gennet_script *) data)->onglet_leto->window), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "A file named \"%s\" already exists."
					"  Do you want to replace it?", filename2);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog), "The file already exists in "
					"\"%s\".  Replacing it will "
					"overwrite its contents.", path);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));
			gtk_widget_destroy(msg_dialog);

			if (result != GTK_RESPONSE_YES)
			{
				g_free(filename);
				gtk_widget_destroy(dialog);
				return;
			}
		}
#ifdef LETO
		chdir(g_path_get_dirname(filename));
#endif
		/* strncpy(nomfich1, g_path_get_basename(filename), sizeof(nomfich1));*/

		save_script_selected(1, 0, filename2, ((t_gennet_script *) data)->onglet_leto);
		transfo_groups_in_block(filename2);
		printf("....apres transfo_groups_in_blocks \n");

		if (script_set_modified(sc)) set_title(((t_gennet_script *) data)->onglet_leto);
		g_free(filename);
	}

	gtk_widget_destroy(dialog);
}
