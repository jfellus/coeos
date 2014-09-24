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
#include <gtk/gtk.h>

/*#define DEBUG 1*/
#include "outils_script.h"
#include "outils.h"
#include "draw_leto.h"
#include "gennet.h"
#include "interface.h"

/*Macros pour le find and manage*/
#define REPONSE_SUIVANT 26
/* recentre l'affichage sur le groupe considere */
int highlight_group_selected(char *nom, TxDonneesFenetre *onglet_leto)
{
	int taillex, tailley;
	type_groupe *groupe_local;
	TxPoint centre;

	groupe_local = trouver_groupe_par_nom(nom, onglet_leto);
	if (groupe_local == NULL)
	{
		return 0;
	}

	select_group(groupe_local); /* groupe selectionne pour modif */

	sc->xmin = (int) gtk_adjustment_get_value(onglet_leto->adj_h);
	sc->xmax = sc->xmin + (int) ((onglet_leto->adj_h)->page_size);
	sc->ymin = (int) gtk_adjustment_get_value(onglet_leto->adj_v);
	sc->ymax = sc->ymin + (int) (onglet_leto->adj_v->page_size);
	taillex = sc->xmax - sc->xmin;
	tailley = sc->ymax - sc->ymin;

	gtk_adjustment_set_value(onglet_leto->adj_h, (gdouble) (groupe_local->posx - taillex / 2));
	gtk_adjustment_set_value(onglet_leto->adj_v, (gdouble) (groupe_local->posy - tailley / 2));

	/*    printf("--- %d %d %d %d \n",sc->xmin,sc->ymin,sc->xmax,sc->ymax);*/

	regenerer_test(onglet_leto);
	centre.x = groupe_local->posx;
	centre.y = groupe_local->posy + 10;
	/* TODO: probleme => le cercle est ecrase par le callback "resize" de la fenetre principale */
	TxDessinerCercle(onglet_leto, rouge, TxVide, centre, 50, 3);
	return 1;
}

/*----------------------------------------------------------------------------*/
/* Gestion du search nom groupe (ex nom de la fonction_algo utilisee)         */
/*----------------------------------------------------------------------------*/

/* Creates a tree model containing the completions for group no_name*/
GtkTreeModel *create_completion_no_group(void)
{
	GtkListStore *store;
	GtkTreeIter iter;
	type_groupe *groupe;

	store = gtk_list_store_new(1, G_TYPE_STRING);

	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, groupe->no_name, -1);
		groupe = groupe->s;
	}

	return GTK_TREE_MODEL (store);
}

/* Creates a tree model containing the completions for group no_name*/
GtkTreeModel *create_completion_name_group(void)
{
	GtkListStore *store;
	GtkTreeIter iter;
	type_groupe *groupe;

	store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

	groupe = sc->deb_groupe;
	while (groupe != NULL)
	{
		gtk_list_store_append(store, &iter);
		/*  (*iter).get_value(store,iter,2,&val);*/
		gtk_list_store_set(store, &iter, 0, groupe->nom, 1, groupe->no_name, 2, groupe->no, -1);
		groupe = groupe->s;
	}

	return GTK_TREE_MODEL (store);
}

/* explore la liste */
void explore_list(GtkTreeModel *list_store, TxDonneesFenetre *onglet_leto)
{
	GtkTreeIter iter;
	gboolean valid;
	int ligne = 0;
	char *str_data1, *str_data2;
	int int_data;

	/* Get the first iter in the list */
	valid = gtk_tree_model_get_iter_first(list_store, &iter);

	while (valid)
	{
		gtk_tree_model_get(list_store, &iter, 0, &str_data1, 1, &str_data2, 2, &int_data, -1);

		/* Do something with the data */
		highlight_group_selected(str_data2, onglet_leto);
		g_free(str_data1);
		g_free(str_data2);

		ligne++;
		valid = gtk_tree_model_iter_next(list_store, &iter);
	}
}

/*Callbacks explore_list_by_name find_group_name commentes. Plus besoin de les appeler tout est fait dans do_find_completion
 * void explore_list_by_name(GtkTreeModel *list_store, char *nom, TxDonneesFenetre *onglet_leto)
{
	GtkTreeIter iter;
	gboolean valid;
	int ligne = 0;
	char *str_data1, *str_data2;
	int int_data;

	// Get the first iter in the list
	valid = gtk_tree_model_get_iter_first(list_store, &iter);
	while (valid)
	{
		gtk_tree_model_get(list_store, &iter, 0, &str_data1, 1, &str_data2, 2, &int_data, -1);

		// Do something with the data
		if (strcmp(nom, str_data1) == 0)
		{
			highlight_group_selected(str_data2, onglet_leto);
		}
		g_free(str_data1);
		g_free(str_data2);

		ligne++;
		valid = gtk_tree_model_iter_next(list_store, &iter);
	}
}*/
/*void find_group_name(GtkWidget * entry, gpointer data)
{
	char nom[255];
	const gchar *entry_text;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	memcpy(nom, entry_text, (strlen(entry_text) + 1) * sizeof(char));
	explore_list_by_name(sc->fm_group_list_store, nom, ((t_gennet_script *) data)->onglet_leto);

}*/

GtkWidget *do_find_completion(GtkWidget *do_widget, gpointer data)
{
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *entry= NULL;
	GtkEntryCompletion *completion;
	GtkTreeModel *completion_model;
	int result;
	char nom[255];
	const gchar *entry_text = NULL;
	GtkTreeIter iter;
	gboolean valid;
	char *str_data1, *str_data2;
	int int_data;
	GtkTreeModel *list_store;
	TxDonneesFenetre *onglet_leto;
	(void) do_widget;
	/*  printf("do_find_completion \n"); */

	/*GTK_WINDOW (do_widget)*/
	if (!sc->fm_window)
	{
		sc->fm_window = gtk_dialog_new_with_buttons("GtkEntryCompletion", NULL/*GTK_WINDOW(((t_gennet_script *) data)->onglet_leto)*/, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_STOCK_CLOSE, GTK_RESPONSE_NONE,GTK_STOCK_MEDIA_NEXT,REPONSE_SUIVANT,NULL);
		gtk_window_set_resizable(GTK_WINDOW (sc->fm_window), FALSE);

		/*	Commentaire des deux lignes pour Pouvoir ajouter d'autre bouton actif sinon n'importe quel evenemenement fermait la fentre*/
		/*g_signal_connect(sc->fm_window, "response", G_CALLBACK (gtk_widget_destroy), NULL);*/
		/*g_signal_connect(sc->fm_window, "destroy", G_CALLBACK (gtk_widget_destroyed), &sc->fm_window);*/

		vbox = gtk_vbox_new(FALSE, 5);
		gtk_box_pack_start(GTK_BOX (GTK_DIALOG (sc->fm_window)->vbox), vbox, TRUE, TRUE, 0);
		gtk_container_set_border_width(GTK_CONTAINER (vbox), 5);

		label = gtk_label_new(NULL);
		gtk_label_set_markup(GTK_LABEL (label), "Completion FIND");
		gtk_box_pack_start(GTK_BOX (vbox), label, FALSE, FALSE, 0);

		/* Create our entry */
		entry = gtk_entry_new();
		gtk_box_pack_start(GTK_BOX (vbox), entry, FALSE, FALSE, 0);

		/* Create the completion object */
		completion = gtk_entry_completion_new();

		/* Assign the completion to the entry */
		gtk_entry_set_completion(GTK_ENTRY (entry), completion);
		g_object_unref(completion);

		/* Create a tree model and use it as the completion model */
		sc->fm_group_list_store = completion_model = create_completion_name_group();

		/*  explore_list(completion_model); */

		gtk_entry_completion_set_model(completion, completion_model);
		g_object_unref(completion_model);

		/* Use model column 0 as the text column */
		gtk_entry_completion_set_text_column(completion, 0);
	}

	if (!GTK_WIDGET_VISIBLE (sc->fm_window)) gtk_widget_show_all(sc->fm_window);
	else gtk_widget_destroy(sc->fm_window);

	gtk_dialog_set_default_response(GTK_DIALOG(sc->fm_window), GTK_RESPONSE_ACCEPT);
	gtk_widget_show_all(sc->fm_window);
	list_store=sc->fm_group_list_store;
	onglet_leto = ((t_gennet_script *) data)->onglet_leto;
	valid = gtk_tree_model_get_iter_first(list_store, &iter);
	/*Recherche suivant uniquement. Les iterateur dans gtk 2.0 sont forward uniquement avec GTK3 les iterateurs sont bidirectionnels.*/
	do
	{
		result = gtk_dialog_run(GTK_DIALOG(sc->fm_window));
		/*dprints("result = %d\n",result);*/
		switch (result)
		{
		case REPONSE_SUIVANT:
			dprints("reponse : SUIVANT valid : %d\n",valid);
			entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
			strcpy(nom, entry_text);
			/*Recuperer le nom du script a la ligne iter si il y a qqchose a recuperer*/
			if (valid)
			{
				gtk_tree_model_get(list_store, &iter, 0, &str_data1, 1, &str_data2, 2, &int_data, -1);

				/* Tant que le nom du script n'est pas celui recherche on avance dans la liste */
				while (strcmp(nom, str_data1) != 0 && valid)
				{
					dprints("comparer a : %s \n",str_data1);
					g_free(str_data1);
					g_free(str_data2);
					gtk_tree_model_get(list_store, &iter, 0, &str_data1, 1, &str_data2, 2, &int_data, -1);
					valid = gtk_tree_model_iter_next(list_store, &iter);
				}
				dprints("valid :  %d\n",valid);
				if(valid)
				{
					highlight_group_selected(str_data2, onglet_leto);
					valid = gtk_tree_model_iter_next(list_store, &iter);
				}
				g_free(str_data1);
				g_free(str_data2);
			}
			if(!valid)
			{
				dprints("Reprise du debut \n");
				valid = gtk_tree_model_get_iter_first(list_store, &iter);
			}
			break;

		case GTK_RESPONSE_NONE:
			dprints("reponse : FERMER \n");
			//gtk_widget_hide(sc->fm_window);
			gtk_widget_destroy(sc->fm_window/*,&sc->fm_window*/);
			sc->fm_window = NULL;
			break;
		default:
			dprints("reponse : DEFAULT %d \n",result);
			//gtk_widget_hide(sc->fm_window);
			gtk_widget_destroy(sc->fm_window/*,&sc->fm_window*/);
			sc->fm_window = NULL;
			break;
		}

	} while (result == REPONSE_SUIVANT && sc->fm_window!= NULL);


	return sc->fm_window;
}

void callback_search_group_by_name(GtkWidget * widget, gpointer data)
{
#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if(tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto)==0) return;
#endif
	do_find_completion(widget, data);
}

/*----------------------------------------------------------------------------*/
/* Gestion du find no groupe symbolique                                       */
/*----------------------------------------------------------------------------*/

void callback_find_group(GtkWidget * widget, gpointer data)
{
	GtkWidget *find_dialog = NULL;
	int result;
	char nom[255];
	const gchar *entry_text;
	TxDonneesFenetre *donnees = ((t_gennet_script *) data)->onglet_leto;
	(void)widget;

#ifndef LETO
	/* controle si on est dans un onglet Leto
	 * ( l'onglet Metaleto etant le numero 0 )
	 */
	if(tab_is_Metaleto(donnees) == 0) return;
#endif

	find_dialog = ((t_gennet_script *) data)->fenetre_dialogue->window;
	if (find_dialog == NULL)
	{
		fprintf(stderr, "ERROR in callback_find_group: window is NULL\n");
		return;
	}

	gtk_dialog_set_default_response(GTK_DIALOG(find_dialog), GTK_RESPONSE_ACCEPT);
	gtk_widget_show_all(find_dialog);

	do
	{
		result = gtk_dialog_run(GTK_DIALOG(find_dialog));
		switch (result)
		{
		case GTK_RESPONSE_REJECT:
			gtk_widget_hide(find_dialog);
			show_status(donnees, "Group search cancelled");
			break;

		case GTK_RESPONSE_APPLY:
			entry_text = gtk_entry_get_text(GTK_ENTRY(((t_gennet_script *) data)->fenetre_dialogue->entry));
			memcpy(nom, entry_text, (strlen(entry_text) + 1) * sizeof(char));
			show_status(donnees, "Group search by symbolic name (%s)", nom);
			highlight_group_selected(nom, donnees);
			break;

		case GTK_RESPONSE_ACCEPT:
			entry_text = gtk_entry_get_text(GTK_ENTRY(((t_gennet_script *) data)->fenetre_dialogue->entry));
			memcpy(nom, entry_text, (strlen(entry_text) + 1) * sizeof(char));
			gtk_widget_hide(find_dialog);
			show_status(donnees, "Group search by symbolic name (%s)", nom);
			highlight_group_selected(nom, donnees);
			break;

		default:
			gtk_widget_hide(find_dialog);
			show_status(donnees, "");
			break;
		}
	} while (result == GTK_RESPONSE_APPLY);
}

void callback_hide_find_group(GtkWidget * widget, gpointer data)
{
	(void) data;
	gtk_widget_hide(widget);
}
