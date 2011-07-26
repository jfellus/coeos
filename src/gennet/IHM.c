#include <stdlib.h>
#include <math.h>
#include <locale.h>

#include "public_leto.h"
#include "gennet.h"
#include "creation.h"
#include <outils_script.h>
#include "leto_global_var.h"
#include <search.h>
#include "gere_coudes.h"

static gboolean	WinMain_delete(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  return FALSE;
}

static void	WinMain_destroy(GtkWidget *widget, gpointer data)
{
  /* Faire la fonction qui libere toute la memoire (structures)*/
  t_gennet_script	*pscript;
  GtkWidget		*Notebook;

  /* on ferme tout les onglets */
  for (pscript = ((t_gennet *)data)->scripts; pscript != NULL && pscript->onglet_leto != NULL; pscript = pscript->next)
    {
      Notebook = lookup_widget (pscript->onglet_leto->window ,"Notebook");
      gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),pscript->sc->num_onglet);
      tabLeto_quit(NULL,pscript);
    }

  gtk_main_quit();
}

static void	Tools_SelectFolder_browse_clicked_callback(GtkWidget *widget, gpointer data)
{
  GtkWidget	*dialog = NULL;
  gint		result = -1;

  dialog = gtk_file_chooser_dialog_new("Select folder",
				       NULL,
				       GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
				       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
				       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
  result = gtk_dialog_run(GTK_DIALOG(dialog));

  switch (result)
    {
    case GTK_RESPONSE_ACCEPT :
      gtk_entry_set_text(GTK_ENTRY(data), gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog)));
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(dialog);
}

static void	Tools_OpenFile_browse_clicked_callback(GtkWidget *widget, gpointer data)
{
  GtkWidget	*dialog = NULL;
  gint		result = -1;
/*   GtkFileFilter *filter; */

  dialog = gtk_file_chooser_dialog_new("Open file",
				       NULL,
				       GTK_FILE_CHOOSER_ACTION_OPEN,
				       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
				       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

/*   filter = gtk_file_filter_new(); */
/*   gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), "*.net"); */
/*   gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), "*.cpt"); */
/*   gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), */
/* 			      GTK_FILE_FILTER(filter)); */

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  switch (result)
    {
    case GTK_RESPONSE_ACCEPT :
      gtk_entry_set_text(GTK_ENTRY(data), gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog)));
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(dialog);
}

static void	SaveFile_browse_clicked_callback(GtkWidget *widget, gpointer data)
{
  GtkWidget	*dialog = NULL;
  gint		result = -1;

  dialog = gtk_file_chooser_dialog_new("Save file",
				       NULL,
				       GTK_FILE_CHOOSER_ACTION_SAVE,
				       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
				       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
  result = gtk_dialog_run(GTK_DIALOG(dialog));

  switch (result)
    {
    case GTK_RESPONSE_ACCEPT :
      gtk_entry_set_text(GTK_ENTRY(data), gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog)));
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(dialog);
}

void		new_control(GtkWidget *dialog, GtkWidget *control, char *label)
{
  GtkWidget	*hBox = NULL;
  GtkWidget	*glabel = NULL;
  
  hBox = gtk_hbox_new(FALSE, 5);

  glabel = gtk_label_new(label);
  gtk_box_pack_start(GTK_BOX(hBox), glabel, FALSE, TRUE, 0);
  gtk_widget_show(glabel);

  gtk_box_pack_start(GTK_BOX(hBox), control, FALSE, TRUE, 0);
  gtk_widget_show(control);

  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), hBox);
  gtk_widget_show(hBox);
}

void		OpenFile_control_browse(GtkWidget *dialog, GtkWidget *control, char *label)
{
  GtkWidget	*hBox = NULL;
  GtkWidget	*browse = NULL;
  
  hBox = gtk_hbox_new(FALSE, 5);

  gtk_box_pack_start(GTK_BOX(hBox), control, FALSE, TRUE, 0);
  gtk_widget_show(control);

  browse = gtk_button_new_with_label("Browse");
  g_signal_connect(G_OBJECT(browse), "clicked", G_CALLBACK(Tools_OpenFile_browse_clicked_callback), control);
  gtk_box_pack_start(GTK_BOX(hBox), browse, FALSE, TRUE, 0);
  gtk_widget_show(browse);

  gtk_widget_show(hBox);

  new_control(dialog, hBox, label);
}

void		edit_with_leto_event(GtkSocket *socket, gpointer user_data)
{
  printf("added\n");
}

int		Write_WindowID(unsigned long wID)
{
  char		*home_path = "\0";
  char		dotgennetpath[MAX_PATH];
  char		widfile[MAX_PATH];
  int		fd;
  FILE		*metaleto_fd = NULL;

  if ((home_path = getenv(HOME_ENV)) == NULL)
    {
      fprintf(stderr, "Write_WindowID : path to user home directory is not found\n");
      return 1;
    }

  sprintf(dotgennetpath, "%s/%s", home_path, ".meta_leto");
  if ((fd = mkdir(dotgennetpath, 0755)) == -1)
    {
      if (errno != EEXIST)
	{
	  perror("Write_WindowID : mkdir");
	  return 1;
	}
    }

  sprintf(widfile, "%s/%s", dotgennetpath, "metaleto.wid");
  if ((metaleto_fd = fopen(widfile,"w+")) == NULL)
    {
      perror("Write_WindowID : fopen");
      return 1;
    }

  fprintf(metaleto_fd, "%lu\n", wID);
  fclose(metaleto_fd);
  return 0;
}

/* Cherche si il y a une place disponible pour un nouveau script
 * la fonction renvoi l'indice correspondant a un emplacement disponible
 * -1 si il n'y a plu de place */
int tab_is_Free()
{
	int i;
	for(i=0;i<NB_MAX_SCRIPTS;i++)
	  {
	    /* un script vide est equivalent a� ce que
	     * le numero d'onglet soit egal a zero */
	    if(les_scripts[i].num_onglet == 0)
	      {
		return i;
	      }
	  }
	return -1;
}

/* On regarde si l'onglet courant est celui de Metaleto
 * et non d'un onglet Leto, ceci afin d'implementer
 * les raccourcis clavier qui s'appliquent sur tout les onglets
 * et donc d'eviter que cela affecte l'onglet Metaleto
 */
int tab_is_Metaleto(TxDonneesFenetre *onglet_leto)
{
  GtkWidget	*pNotebook;
  int		num_page;
  
  if(onglet_leto == NULL)
    {
      fprintf(stderr,"Erreur : tab_is_Metaleto\n");
      return 0;
    }
  
  pNotebook = lookup_widget ( onglet_leto->window ,"Notebook");
  
  num_page = gtk_notebook_get_current_page( GTK_NOTEBOOK(pNotebook));
  
  /* retourne le numero de page, le 0 etant celui de Metaleto */
  return num_page;
}

void		Edit_Properties(t_gennet *gen, t_gennet_script *pscript)
{
  GtkWidget		*e_name = NULL;
  GtkWidget		*e_pbinary = NULL;
  GtkWidget		*e_pscript = NULL;
  GtkWidget		*e_pscript_non_symb = NULL;
  GtkWidget		*e_pdraw = NULL;
  GtkWidget		*e_pres = NULL;
  GtkWidget		*e_pconfig = NULL;
  GtkWidget		*e_pbus = NULL;
  GtkWidget		*e_pvar = NULL;
  GtkWidget		*e_pdev = NULL;
  GtkWidget		*e_pprt = NULL;
  GtkWidget		*e_pgcd = NULL;
  GtkWidget		*e_pdeploypath = NULL;
  GtkWidget		*e_argline = NULL;

  GtkWidget		*separator = NULL;
  GtkWidget		*label = NULL;
  gint			result = -1;
  char			*val = NULL;

  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gen->gui->NewComlinkItem), FALSE);
  
  gen->gui->NewScriptDialog = gtk_dialog_new_with_buttons("Script properties",
							  GTK_WINDOW(gen->gui->WinMain),
							  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							  GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
							  GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

  e_name = gtk_entry_new_with_max_length(MAX_LOGICAL_NAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_logical_name(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_name), val);
      if (val != NULL)
	free(val);
    }
  new_control(gen->gui->NewScriptDialog, e_name, "Script name :");

  label = gtk_label_new("");    
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewScriptDialog)->vbox), label);
  gtk_widget_show(label);

  separator = gtk_hseparator_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewScriptDialog)->vbox), separator);
  gtk_widget_show(separator);

  label = gtk_label_new("Development");    
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewScriptDialog)->vbox), label);
  gtk_widget_show(label);

  separator = gtk_hseparator_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewScriptDialog)->vbox), separator);
  gtk_widget_show(separator);

  e_pscript = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_script(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pscript), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pscript, "Path to .script :");

  e_pscript_non_symb = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_script_non_symb(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pscript_non_symb), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pscript_non_symb, "Path to .script_o :");

  e_pdraw = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_draw(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pdraw), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pdraw, "Path to .draw :");

  e_pres = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_res(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pres), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pres, "Path to .res :");

  e_pconfig = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_config(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pconfig), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pconfig, "Path to .config :");

  e_pbus = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_bus(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pbus), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pbus, "Path to .bus :");

  e_pvar = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_var(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pvar), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pvar, "Path to .var :");

  e_pdev = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_dev(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pdev), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pdev, "Path to .dev :");

  e_pprt = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_prt(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pprt), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pprt, "Path to .prt :");

  e_pgcd = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_file_gcd(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pgcd), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pgcd, "Path to .gcd :");

  label = gtk_label_new("");    
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewScriptDialog)->vbox), label);
  gtk_widget_show(label);

  separator = gtk_hseparator_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewScriptDialog)->vbox), separator);
  gtk_widget_show(separator);

  label = gtk_label_new("Deployment and execution");    
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewScriptDialog)->vbox), label);
  gtk_widget_show(label);

  separator = gtk_hseparator_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewScriptDialog)->vbox), separator);
  gtk_widget_show(separator);


  e_pdeploypath = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_prom_deploy(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pdeploypath), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pdeploypath, "Path to deploy the application:");

  e_pbinary = gtk_entry_new_with_max_length(SIZE_OF_PATHNAME - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_path_prom_binary(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_pbinary), val);
      if (val != NULL)
	free(val);
    }
  OpenFile_control_browse(gen->gui->NewScriptDialog, e_pbinary, "Path to promethe binary :");

  e_argline = gtk_entry_new_with_max_length(MAX_PROM_ARGS_LINE - 1);
  if (pscript != NULL)
    {
      val = promnet_prom_script_get_prom_args_line(pscript->prom_script);
      gtk_entry_set_text(GTK_ENTRY(e_argline), val);
      if (val != NULL)
	free(val);
    }
  new_control(gen->gui->NewScriptDialog, e_argline, "promethe arguments");

  result = gtk_dialog_run(GTK_DIALOG(gen->gui->NewScriptDialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      {
	t_gennet_script		*script = NULL;
	char			*name = NULL;
	
	if (pscript != NULL)
	  script = pscript;
	else
	  {
	    script = add_gennet_script(gen);
	    script->prom_script = promnet_add_new_prom_script(gen->promnet);
	  }
	name = (char *)gtk_entry_get_text(GTK_ENTRY(e_name));
	promnet_prom_script_set_logical_name(script->prom_script, name);
	promnet_prom_script_set_path_prom_deploy(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pdeploypath)));
	promnet_prom_script_set_path_prom_binary(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pbinary)));
	promnet_prom_script_set_path_file_script(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pscript)));
	promnet_prom_script_set_path_file_script_non_symb(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pscript_non_symb)));
	promnet_prom_script_set_path_file_draw(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pdraw)));
	promnet_prom_script_set_path_file_res(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pres)));
	promnet_prom_script_set_path_file_config(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pconfig)));
	promnet_prom_script_set_path_file_bus(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pbus)));
	promnet_prom_script_set_path_file_var(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pvar)));
	promnet_prom_script_set_path_file_dev(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pdev)));
	promnet_prom_script_set_path_file_prt(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pprt)));
	promnet_prom_script_set_path_file_gcd(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_pgcd)));
	promnet_prom_script_set_prom_args_line(script->prom_script, (char *)gtk_entry_get_text(GTK_ENTRY(e_argline)));

	script->pango = gtk_widget_create_pango_layout(GTK_WIDGET(gen->gui->DrawingArea), name);
	refresh_all(gen);
      }
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }

  gtk_widget_destroy(gen->gui->NewScriptDialog);

}

gboolean	NewComputer_selected_computer(GtkTreeSelection *selection, GtkTreeModel *model, GtkTreePath *path,
					      gboolean path_currently_selected, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  GtkTreeIter		iter;

  if (gtk_tree_model_get_iter(model, &iter, path))
    {
      t_gennet_computer	*computer = NULL;
      
      gtk_tree_model_get(model, &iter, COL_COMPUTER_DATA, &computer, -1);

      if (computer != NULL)
	{
	  if (computer->pango != NULL)
	    {
	      char	*name = NULL;
	      
	      name = promnet_computer_get_name(computer->computer);
	      gtk_entry_set_text(GTK_ENTRY(gen->gui->gname), name);
	      if (name != NULL)
		free(name);
	    }
	  else
	    {
	      gtk_entry_set_text(GTK_ENTRY(gen->gui->gname), "");
	    }
	}
    }
  return TRUE;
}

void		SelectFolder_control_browse(GtkWidget *dialog, GtkWidget *control, char *label)
{
  GtkWidget	*hBox = NULL;
  GtkWidget	*browse = NULL;
  
  hBox = gtk_hbox_new(FALSE, 5);

  gtk_box_pack_start(GTK_BOX(hBox), control, FALSE, TRUE, 0);
  gtk_widget_show(control);

  browse = gtk_button_new_with_label("Browse");
  g_signal_connect(G_OBJECT(browse), "clicked", G_CALLBACK(Tools_SelectFolder_browse_clicked_callback), control);
  gtk_box_pack_start(GTK_BOX(hBox), browse, FALSE, TRUE, 0);
  gtk_widget_show(browse);

  gtk_widget_show(hBox);

  new_control(dialog, hBox, label);
}

void		Computer_Edit_Computer(t_gennet *gen, t_gennet_computer *pcomputer)
{
  t_gennet_computer	*computer = NULL;
  GtkWidget		*gaddress = NULL;
  GtkWidget		*glogin = NULL;
  GtkWidget		*gpath = NULL;
  GtkWidget		*gcpu = NULL;
  GtkWidget		*ginfos = NULL;
  gint			result = -1;
  char			*val = NULL;

  gen->gui->AddComputerDialog = gtk_dialog_new_with_buttons("New computer",
				       GTK_WINDOW(gen->gui->NewComputerDialog),
				       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
				       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);


  gaddress = gtk_entry_new_with_max_length(MAX_ADDRESS - 1);
  if (pcomputer != NULL)
    {
      val = promnet_computer_get_address(pcomputer->computer);
      gtk_entry_set_text(GTK_ENTRY(gaddress), val);
      if (val != NULL)
	free(val);
    }
  new_control(gen->gui->AddComputerDialog, gaddress, "Address : ");

  glogin = gtk_entry_new_with_max_length(MAX_LOGIN - 1);
  if (pcomputer != NULL)
    {
      val = promnet_computer_get_login(pcomputer->computer);
      gtk_entry_set_text(GTK_ENTRY(glogin), val);
      if (val != NULL)
	free(val);
    }
  new_control(gen->gui->AddComputerDialog, glogin, "Login : ");

  gpath = gtk_entry_new_with_max_length(MAX_PATH - 1);
  if (pcomputer != NULL)
    {
      val = promnet_computer_get_path(pcomputer->computer);
      gtk_entry_set_text(GTK_ENTRY(gpath), val);
      if (val != NULL)
	free(val);
    }
  SelectFolder_control_browse(gen->gui->AddComputerDialog, gpath, "Path to simulator: ");

  gcpu = gtk_entry_new_with_max_length(MAX_CPU - 1);
  if (pcomputer != NULL)
    {
      val = promnet_computer_get_cpu(pcomputer->computer);
      gtk_entry_set_text(GTK_ENTRY(gcpu), val);
      if (val != NULL)
	free(val);
    }
  new_control(gen->gui->AddComputerDialog, gcpu, "CPU type : ");

  ginfos = gtk_entry_new_with_max_length(MAX_INFORMATION - 1);
  if (pcomputer != NULL)
    {
      val = promnet_computer_get_information(pcomputer->computer);
      gtk_entry_set_text(GTK_ENTRY(ginfos), val);
      if (val != NULL)
	free(val);
    }
  new_control(gen->gui->AddComputerDialog, ginfos, "Informations : ");

  result = gtk_dialog_run(GTK_DIALOG(gen->gui->AddComputerDialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      {
	GtkTreeIter   iter;
	GtkTreeSelection	*selection;
	
	computer = Tools_NewComputer_OK(gen, 
					gtk_entry_get_text(GTK_ENTRY(gaddress)),
					gtk_entry_get_text(GTK_ENTRY(glogin)),
					gtk_entry_get_text(GTK_ENTRY(gpath)),
					gtk_entry_get_text(GTK_ENTRY(gcpu)),
					gtk_entry_get_text(GTK_ENTRY(ginfos)),
					pcomputer);

	


	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gen->gui->tree_view));

	if (pcomputer != NULL)
	  {
	    GtkTreeModel		*model;

	    gtk_tree_selection_get_selected(selection, &model, &iter);
	    gtk_list_store_remove(gen->gui->list_computer, &iter);
	  }
	gtk_list_store_append(gen->gui->list_computer, &iter);
	gtk_list_store_set(gen->gui->list_computer, &iter, 
			   COL_COMPUTER_ADDRESS, gtk_entry_get_text(GTK_ENTRY(gaddress)),
			   COL_COMPUTER_LOGIN, gtk_entry_get_text(GTK_ENTRY(glogin)),
			   COL_COMPUTER_PATH, gtk_entry_get_text(GTK_ENTRY(gpath)),
			   COL_COMPUTER_CPU, gtk_entry_get_text(GTK_ENTRY(gcpu)),
			   COL_COMPUTER_INFORMATION, gtk_entry_get_text(GTK_ENTRY(ginfos)),
			   COL_COMPUTER_DATA, computer,
			   -1);
	gtk_tree_selection_select_iter(selection, &iter);
      }
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(gen->gui->AddComputerDialog);
}

static void	Tools_NewComputer_edit_clicked_callback(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  GtkTreeSelection	*selection;
  GtkTreeModel		*model;
  GtkTreeIter		iter;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gen->gui->tree_view));

  if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
      t_gennet_computer	*computer = NULL;
      
      gtk_tree_model_get(model, &iter, COL_COMPUTER_DATA, &computer, -1);
      Computer_Edit_Computer((t_gennet *)data, computer);
    }
}

static void	Tools_NewComputer_del_clicked_callback(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  GtkTreeSelection	*selection;
  GtkTreeModel		*model;
  GtkTreeIter		iter;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gen->gui->tree_view));

  if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
      t_gennet_computer	*computer = NULL;
      
      gtk_tree_model_get(model, &iter, COL_COMPUTER_DATA, &computer, -1);
      del_gennet_computer(gen, computer);
      gtk_list_store_remove(gen->gui->list_computer, &iter);
    }
}

static void	Tools_NewComputer_add_clicked_callback(GtkWidget *widget, gpointer data)
{
  Computer_Edit_Computer((t_gennet *)data, NULL);
}

void		Edit_Computer(t_gennet *gen, t_gennet_computer *pcomputer)
{
  gint			result = -1;
  GtkWidget		*button = NULL;
  GtkWidget		*glabel = NULL;
  GtkWidget		*hBox = NULL;
  GtkWidget		*Box = NULL;
  GtkCellRenderer	*renderer = NULL;
  t_gennet_computer	*computer = NULL;
  GtkTreeSelection	*selection = NULL;
  char			*val = NULL;

  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gen->gui->NewComlinkItem), FALSE);

  gen->gui->NewComputerDialog = gtk_dialog_new_with_buttons("New computer",
				       GTK_WINDOW(gen->gui->WinMain),
				       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
				       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

  hBox = gtk_hbox_new(FALSE, 5);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gen->gui->NewComputerDialog)->vbox), hBox);
  gtk_widget_show(hBox);

  glabel = gtk_label_new("logical name : ");
  gtk_box_pack_start(GTK_BOX(hBox), glabel, FALSE, TRUE, 0);
  gtk_widget_show(glabel);

  gen->gui->gname = gtk_entry_new_with_max_length(SIZE_OF_COMPUTER - 1);
  if (pcomputer != NULL)
    {
      val = promnet_computer_get_name(pcomputer->computer);
      gtk_entry_set_text(GTK_ENTRY(gen->gui->gname), val);
      if (val != NULL)
	free(val);
    }
  gtk_box_pack_start(GTK_BOX(hBox), gen->gui->gname, FALSE, TRUE, 0);
  gtk_widget_show(gen->gui->gname);

  Box = gtk_vbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hBox), Box, FALSE, TRUE, 0);
  gtk_widget_show(Box);

  gen->gui->tree_view = gtk_tree_view_new();
  gtk_box_pack_start(GTK_BOX(Box), GTK_WIDGET(gen->gui->tree_view), FALSE, TRUE, 0);
  gtk_widget_show(gen->gui->tree_view);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(	GTK_TREE_VIEW(gen->gui->tree_view),
						COL_COMPUTER_ADDRESS,
						"Address",
						renderer,
						"text",
						COL_COMPUTER_ADDRESS,
						NULL
						);

  gtk_tree_view_insert_column_with_attributes(	GTK_TREE_VIEW(gen->gui->tree_view),
						COL_COMPUTER_LOGIN,
						"Login",
						renderer,
						"text",
						COL_COMPUTER_LOGIN,
						NULL
						);

  gtk_tree_view_insert_column_with_attributes(	GTK_TREE_VIEW(gen->gui->tree_view),
						COL_COMPUTER_PATH,
						"Path",
						renderer,
						"text",
						COL_COMPUTER_PATH,
						NULL
						);

  gtk_tree_view_insert_column_with_attributes(	GTK_TREE_VIEW(gen->gui->tree_view),
						COL_COMPUTER_CPU,
						"CPU",
						renderer,
						"text",
						COL_COMPUTER_CPU,
						NULL
						);

  gtk_tree_view_insert_column_with_attributes(	GTK_TREE_VIEW(gen->gui->tree_view),
						COL_COMPUTER_INFORMATION,
						"Informations",
						renderer,
						"text",
						COL_COMPUTER_INFORMATION,
						NULL
						);

  gen->gui->list_computer = gtk_list_store_new(COL_COMPUTER_NB, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_tree_view_set_model(GTK_TREE_VIEW(gen->gui->tree_view), GTK_TREE_MODEL(gen->gui->list_computer));

  hBox = gtk_hbox_new(FALSE, 5);
  gtk_container_add(GTK_CONTAINER(Box), hBox);
  gtk_widget_show(hBox);

  fill_widget_list_computer(gen , pcomputer);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gen->gui->tree_view));
  gtk_tree_selection_set_select_function(selection, NewComputer_selected_computer, gen, NULL);
  
  button = gtk_button_new_with_label("Add");
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(Tools_NewComputer_add_clicked_callback), gen);
  gtk_box_pack_start(GTK_BOX(hBox), GTK_WIDGET(button), FALSE, TRUE, 0);
  gtk_widget_show(button);

  button = gtk_button_new_with_label("Edit");
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(Tools_NewComputer_edit_clicked_callback), gen);
  gtk_box_pack_start(GTK_BOX(hBox), GTK_WIDGET(button), FALSE, TRUE, 0);
  gtk_widget_show(button);

  button = gtk_button_new_with_label("Delete");
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(Tools_NewComputer_del_clicked_callback), gen);
  gtk_box_pack_start(GTK_BOX(hBox), GTK_WIDGET(button), FALSE, TRUE, 0);
  gtk_widget_show(button);

  result = gtk_dialog_run(GTK_DIALOG(gen->gui->NewComputerDialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      {
	if ((computer = find_selected_computer(gen)) != NULL)
  	  {
	    char			*name = NULL;
	  
	    if ((name = (char *)gtk_entry_get_text(GTK_ENTRY(gen->gui->gname))) != NULL)
	      {
		if (strlen(name) > 0)
		  {
		    if ((pcomputer == NULL) && (computer->pango != NULL))
		      {
			computer = copy_gennet_computer(gen, computer);
		      }
		    promnet_computer_set_name(computer->computer, name);
		    computer->pango = gtk_widget_create_pango_layout(GTK_WIDGET(gen->gui->DrawingArea), name);
		    refresh_computer(gen, computer, computer->pos_x, computer->pos_y);
		  }
	      }
	  }
      }
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(gen->gui->NewComputerDialog);
}

static void	menuComputerEdit(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_computer	*computer = NULL;

  if ((computer = find_widget_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  Edit_Computer(gen, computer);
  refresh_all(gen);
}

static void	menuComputerDelete(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_computer	*computer = NULL;

  if ((computer = find_widget_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  del_all_scriptlist(gen, computer);
  del_gennet_computer(gen, computer);
  refresh_all(gen);
}

static void	menuComputerCompileScripts(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_computer	*computer = NULL;
  t_gennet_script_list	*script_list = NULL;
  t_gennet_script	*script = NULL;
  GtkWidget		*Notebook = NULL;

  if ((computer = find_widget_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }

  Notebook = gen->gui->tabbedWin;
  for (script_list = computer->scriptlist; script_list != NULL; script_list = script_list->next)
    {
      script = script_list->script;

      if (script->sc == NULL)
	{
	  Edit_Script_With_Leto(gen, script);
	}
      gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),script->sc->num_onglet); 
      /* widget mis a NULL car il y en a pas ici ! de toute maniere creation_cb ne l'utilise pas */
      creation_cb(NULL, script);
    }

  /* Retour a l onglet coeos */
  gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),0); 
}

static void	menuScriptEdit(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script	*script = NULL;

  if ((script = find_widget_script(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  Edit_Script_With_Leto(gen, script);
  refresh_all(gen);
}

static void	menuScriptProperties(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script	*script = NULL;

  if ((script = find_widget_script(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  Edit_Properties(gen, script);
  refresh_all(gen);
}

static void	menuScriptDelete(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script	*script = NULL;

  if ((script = find_widget_script(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }

  /* on supprime tout le(s) onglet(s) rattache(s) au(x) script(s) */
  if (script->sc != NULL && script->sc->modified != 0)
    {
      if ((erase_without_saving(script)) == 0)
	return ;
    }
  if (script->sc != NULL)
    gtk_notebook_remove_page (GTK_NOTEBOOK(((t_gennet *)data)->gui->tabbedWin),script->sc->num_onglet);
  del_all_comlinklist(data, script);
  del_gennet_script(gen, script);
  refresh_all(gen);
}

static void	menuScriptCompile(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script	*script = NULL;
  GtkWidget		*Notebook = NULL;

  if ((script = find_widget_script(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }

  Notebook = gen->gui->tabbedWin;

  if (script->sc == NULL)
    {
      Edit_Script_With_Leto(gen, script);
    }
  gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),script->sc->num_onglet); 
  /* widget mis a NULL car il y en a pas ici ! de toute maniere creation_cb ne l'utilise pas */
  creation_cb(NULL, script);

  /* Retour a l onglet coeos */
  gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),0); 

}

static void	menuScriptInComputerEdit(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script_list	*scriptlist = NULL;

  if ((scriptlist = find_widget_script_in_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  Edit_Script_With_Leto(gen, scriptlist->script);
  refresh_all(gen);
}

static void	menuScriptInComputerProperties(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script_list	*scriptlist = NULL;

  if ((scriptlist = find_widget_script_in_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  Edit_Properties(gen, scriptlist->script);
  refresh_all(gen);
}

static void	menuScriptInComputerDelete(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_computer	*computer = NULL;
  t_gennet_script_list	*scriptlist = NULL;

  if ((scriptlist = find_widget_script_in_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  computer = find_widget_computer_by_script_list(gen, scriptlist);

  /* on supprime tout le(s) onglet(s) leto rattache(s) au(x) script(s) */
  if (scriptlist->script->sc != NULL && scriptlist->script->sc->modified != 0)
    {
      if ((erase_without_saving(scriptlist->script)) == 0)
	return ;
    }
  if (scriptlist->script->sc != NULL)
    gtk_notebook_remove_page (GTK_NOTEBOOK(((t_gennet *)data)->gui->tabbedWin),scriptlist->script->sc->num_onglet);

  del_all_comlinklist(data, scriptlist->script);
  del_gennet_script(gen, scriptlist->script);
  del_gennet_scriptlist(computer, scriptlist);
  refresh_all(gen);
}

static void	menuScriptInComputerCompile(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script_list	*scriptlist = NULL;
  t_gennet_script	*script = NULL;
  GtkWidget		*Notebook = NULL;

 if ((scriptlist = find_widget_script_in_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }

 script = scriptlist->script;
  Notebook = gen->gui->tabbedWin;

  if (script->sc == NULL)
    {
      Edit_Script_With_Leto(gen, script);
    }
  gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),script->sc->num_onglet); 
  /* widget mis a NULL car il y en a pas ici ! de toute maniere creation_cb ne l'utilise pas */
  creation_cb(NULL, script);

  /* Retour a l onglet coeos */
  gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),0); 

}

int		NewComlink_getName(t_gennet *data, t_gennet_comlink *comlink)
{
  GtkWidget	*dialog = NULL;
  GtkWidget	*e_name = NULL;
  gint		result = -1;
  char		*val = NULL;

  dialog = gtk_dialog_new_with_buttons("Link name",
				       GTK_WINDOW(data->gui->WinMain),
				       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
				       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);


  e_name = gtk_entry_new_with_max_length(MAX_LINK_NAME - 1);
  if (comlink != NULL)
  {
    val = promnet_prom_link_get_name(comlink->prom_link);
    gtk_entry_set_text(GTK_ENTRY(e_name), val);
    if (val != NULL)
      free(val);
  }
  new_control(dialog, e_name, "Link name :");

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      {
	char		*name = NULL;

	name = (char *)gtk_entry_get_text(GTK_ENTRY(e_name));
	promnet_prom_link_set_name(comlink->prom_link, name);
	comlink->pango = gtk_widget_create_pango_layout(data->gui->DrawingArea, name);
	memcpy(comlink->name, name, LETO_LIAISON_NAME_SIZE * sizeof(char));
	gtk_widget_destroy(dialog);
	return 1;
      }
      break;
    case GTK_RESPONSE_REJECT :
      {
	gtk_widget_destroy(dialog);
	return 0;
      }
      break;
    default :
      break;
    }
  gtk_widget_destroy(dialog);
  return 0;
}

static void	menuLinkEdit(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_comlink	*comlink = NULL;

  if ((comlink = find_widget_comlink(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  NewComlink_getName(gen, comlink);
  refresh_all(gen);
}

void	menuLinkDelete(gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_comlink	*comlink = NULL;
  t_gennet_comlink_list	*comlinklist = NULL;
 
  if ((comlink = find_widget_comlink(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  if (comlink->in)
    {
      if ((comlinklist = find_comlink_list_by_comlink(comlink->in->comlinklist_out, comlink)) != NULL)
	{
	  del_gennet_comlinklist_out(comlinklist->comlink->in, comlinklist);
	}
    }
  if (comlink->out)
    {
      if ((comlinklist = find_comlink_list_by_comlink(comlink->out->comlinklist_in, comlink)) != NULL)
	{
	  del_gennet_comlinklist_in(comlinklist->comlink->out, comlinklist);
	}
    }
  del_gennet_comlink(gen, comlink);
  refresh_all(gen);
}

void	    popup_menu_computer(t_gennet *data, GtkMenu *menu)
{
  GtkWidget	*menu_items;

  /* Menu Computer */

  /* menu Computer -> Edit */
  menu_items = gtk_menu_item_new_with_label("Edit");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuComputerEdit), data);
  gtk_widget_show(menu_items);

  /* menu Computer -> Delete */
  menu_items = gtk_menu_item_new_with_label("Delete");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuComputerDelete), data);
  gtk_widget_show(menu_items);

  /* menu Computer -> Compile script */
  menu_items = gtk_menu_item_new_with_label("Compile scripts");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuComputerCompileScripts), data);
  gtk_widget_show(menu_items);
}

void	    popup_menu_script(t_gennet *data, GtkMenu *menu)
{
  GtkWidget	*menu_items;

  /* Menu Script */

  /* menu Script -> Edit */
  menu_items = gtk_menu_item_new_with_label("Edit with Leto");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuScriptEdit), data);
  gtk_widget_show(menu_items);

  /* menu Script -> Properties */
  menu_items = gtk_menu_item_new_with_label("Propeties");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuScriptProperties), data);
  gtk_widget_show(menu_items);

  /* menu Script -> Delete */
  menu_items = gtk_menu_item_new_with_label("Delete");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuScriptDelete), data);
  gtk_widget_show(menu_items);

  /* menu Script -> Compile */
  menu_items = gtk_menu_item_new_with_label("Compile");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuScriptCompile), data);
  gtk_widget_show(menu_items);

}

void	    popup_menu_script_in_computer(t_gennet *data, GtkMenu *menu)
{
  GtkWidget	*menu_items;

  /* Menu ScriptInComputer */

  /* menu Script -> Edit */
  menu_items = gtk_menu_item_new_with_label("Edit with leto");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuScriptInComputerEdit), data);
  gtk_widget_show(menu_items);

  /* menu ScriptInComputer -> Properties */
  menu_items = gtk_menu_item_new_with_label("Properties");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuScriptInComputerProperties), data);
  gtk_widget_show(menu_items);

  /* menu ScriptInComputer -> Delete */
  menu_items = gtk_menu_item_new_with_label("Delete");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuScriptInComputerDelete), data);
  gtk_widget_show(menu_items);

  /* menu ScriptInComputer -> Compile */
  menu_items = gtk_menu_item_new_with_label("Compile");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuScriptInComputerCompile), data);
  gtk_widget_show(menu_items);

}

void	    popup_menu_link(t_gennet *data, GtkMenu *menu)
{
  GtkWidget	*menu_items;

  /* Menu Link */

  /* menu Link -> Edit link*/
  menu_items = gtk_menu_item_new_with_label("Edit link");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuLinkEdit), data);
  gtk_widget_show(menu_items);

  /* menu Link -> Delete link*/
  menu_items = gtk_menu_item_new_with_label("Delete link");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuLinkDelete), data);
  gtk_widget_show(menu_items);
}

void	    popup_menu_link_polyline(t_gennet *data, GtkMenu *menu)
{
  GtkWidget	*menu_items;

  /* Menu Polyline */

  /* menu Link - Polyline -> Add bend */
  menu_items = gtk_menu_item_new_with_label("Add bend");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuLinkAddBend), data);
  gtk_widget_show(menu_items);

  /* menu Link - Polyline -> Delete segment */
  menu_items = gtk_menu_item_new_with_label("Delete segment");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuLinkDeletePolyline), data);
  gtk_widget_show(menu_items);

}

void	    popup_menu_link_bend(t_gennet *data, GtkMenu *menu)
{
  GtkWidget	*menu_items;

  /* Menu Bend */

  /* menu Link - Bend -> Delete bend */
  menu_items = gtk_menu_item_new_with_label("Delete bend");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
  g_signal_connect_swapped(G_OBJECT(menu_items), "activate", G_CALLBACK(menuLinkDeleteBend), data);
  gtk_widget_show(menu_items);
}


void        popup_menu_handler(GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer data)
{
  t_gennet				*gen = (t_gennet *)data;
  t_gennet_computer			*computer = NULL;
  t_gennet_script			*script = NULL;
  t_gennet_script_list			*script_list = NULL;
  t_gennet_comlink			*comlink = NULL;
  t_polyline				*polyline = NULL;

  if ((script = find_widget_script(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
    {
      popup_menu_script((t_gennet *)data, menu);
    }
  else if ((script_list = find_widget_script_in_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
    {
      popup_menu_script_in_computer((t_gennet *)data, menu);
    }
  else if ((comlink = find_widget_comlink(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
    {
      popup_menu_link((t_gennet *)data, menu);
      if ((polyline = find_widget_comlink_bend(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
	{
	  popup_menu_link_bend((t_gennet *)data, menu);
	}
      else if ((polyline = find_widget_comlink_polyline(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
	{
	  popup_menu_link_polyline((t_gennet *)data, menu);
	}
    }
  else if ((computer = find_widget_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
    {
      popup_menu_computer((t_gennet *)data, menu);
    }

  gtk_widget_show(GTK_WIDGET(menu));
}


void		init_gui_winmain(t_gennet *data)
{
  GtkWidget	*label = NULL;

  data->gui->WinMain = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(data->gui->WinMain), "delete_event", G_CALLBACK(WinMain_delete), data);
  g_signal_connect(G_OBJECT(data->gui->WinMain), "destroy", G_CALLBACK(WinMain_destroy), data);
  /* dimensionnement de la fenetre */
  gtk_window_set_default_size(GTK_WINDOW(data->gui->WinMain), 
			      gdk_screen_width() - 450,
			      gdk_screen_height() - 450);
  gtk_widget_show(data->gui->WinMain);

  data->gui->tabbedWin = gtk_notebook_new();
  g_signal_connect(G_OBJECT(data->gui->tabbedWin), "switch-page", G_CALLBACK(changed_tab), data);
  g_signal_connect(G_OBJECT(data->gui->tabbedWin), "page-removed", G_CALLBACK(tabLeto_removed), data);
  g_signal_connect(G_OBJECT(data->gui->WinMain), "check-resize", G_CALLBACK(resized_cb), data);

  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(data->gui->tabbedWin), GTK_POS_TOP);
  gtk_container_add(GTK_CONTAINER(data->gui->WinMain), data->gui->tabbedWin);
  gtk_widget_show(data->gui->tabbedWin);

  data->gui->vBoxMain = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(data->gui->vBoxMain);

  label = gtk_label_new("Coeos");
  gtk_notebook_append_page(GTK_NOTEBOOK(data->gui->tabbedWin), data->gui->vBoxMain, label);

}

static void	OpenNetwork(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  GtkWidget		*dialog = NULL;
  GtkWidget		*e_netfile = NULL;
  GtkWidget		*e_cptfile = NULL;
  gint			result = -1;

  printf("OpenNetwork \n");

  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gen->gui->NewComlinkItem), FALSE);
  
  dialog = gtk_dialog_new_with_buttons("Open network",
				       GTK_WINDOW(gen->gui->WinMain),
				       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
				       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

  e_netfile = gtk_entry_new_with_max_length(MAX_NET_PATH - 1);
  OpenFile_control_browse(dialog, e_netfile, "Path to net file :");

  e_cptfile = gtk_entry_new_with_max_length(MAX_CPT_PATH - 1);
  OpenFile_control_browse(dialog, e_cptfile, "Path to cpt file :");

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      /* on enleve l'application courante avant d'en ouvrir une autre */
      NewNetwork(widget,data);
      loadNetwork(gen, (char *)gtk_entry_get_text(GTK_ENTRY(e_netfile)),
		  (char *)gtk_entry_get_text(GTK_ENTRY(e_cptfile)));
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(dialog);
}

void		SaveFile_control_browse(GtkWidget *dialog, GtkWidget *control, char *label)
{
  GtkWidget	*hBox = NULL;
  GtkWidget	*browse = NULL;
  
  hBox = gtk_hbox_new(FALSE, 5);

  gtk_box_pack_start(GTK_BOX(hBox), control, FALSE, TRUE, 0);
  gtk_widget_show(control);

  browse = gtk_button_new_with_label("Browse");
  g_signal_connect(G_OBJECT(browse), "clicked", G_CALLBACK(SaveFile_browse_clicked_callback), control);
  gtk_box_pack_start(GTK_BOX(hBox), browse, FALSE, TRUE, 0);
  gtk_widget_show(browse);

  gtk_widget_show(hBox);

  new_control(dialog, hBox, label);
}

static void	SaveNetwork(GtkWidget *widget, gpointer data)
{
  t_gennet	*gen = (t_gennet *)data;
  GtkWidget	*e_netfile = NULL;
  GtkWidget	*e_cptfile = NULL;
  GtkWidget	*e_ivybus_envvar = NULL;
  
  gint		result = -1;

  gen->gui->SaveNetworkDialog = gtk_dialog_new_with_buttons("Save Network",
							    GTK_WINDOW(gen->gui->WinMain),
							    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
							    GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);


  e_netfile = gtk_entry_new_with_max_length(MAX_NET_PATH - 1);
  if (gen->promnet != NULL && gen->promnet->net_path != NULL)
     gtk_entry_set_text((GtkEntry *)e_netfile, gen->promnet->net_path);
  SaveFile_control_browse(gen->gui->SaveNetworkDialog, e_netfile, "Path to net file :");

  e_cptfile = gtk_entry_new_with_max_length(MAX_CPT_PATH - 1);
  if (gen->promnet != NULL && gen->promnet->cpt_path != NULL)
     gtk_entry_set_text((GtkEntry *)e_cptfile, gen->promnet->cpt_path);
  SaveFile_control_browse(gen->gui->SaveNetworkDialog, e_cptfile, "Path to cpt file :");

  e_ivybus_envvar = gtk_entry_new_with_max_length(MAX_IVYBUS_ENVVAR - 1);
  if (gen->promnet != NULL && gen->promnet->ivybus_envvar != NULL)
     gtk_entry_set_text((GtkEntry *)e_ivybus_envvar, gen->promnet->ivybus_envvar);
  
  printf("IVYBUS: %s \n",gen->promnet->ivybus_envvar);
  new_control(gen->gui->SaveNetworkDialog, e_ivybus_envvar, "IVYBUS (IP:PORT) :");

  result = gtk_dialog_run(GTK_DIALOG(gen->gui->SaveNetworkDialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      promnet_save_file_net(gen->promnet, (char *)gtk_entry_get_text(GTK_ENTRY(e_netfile)),(char *)gtk_entry_get_text(GTK_ENTRY(e_ivybus_envvar)));
      promnet_save_file_cpt(gen->promnet, (char *)gtk_entry_get_text(GTK_ENTRY(e_cptfile)));
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(gen->gui->SaveNetworkDialog);
}


void		init_gui_toolbar(t_gennet *data)
{
  GtkWidget		*button;
  GtkWidget		*ToolBar;

  ToolBar = gtk_toolbar_new();

  gtk_toolbar_set_orientation(GTK_TOOLBAR(ToolBar), GTK_ORIENTATION_HORIZONTAL);
  gtk_toolbar_set_style(GTK_TOOLBAR(ToolBar), GTK_TOOLBAR_BOTH);
  gtk_container_set_border_width(GTK_CONTAINER(ToolBar), 5);
  gtk_box_pack_start(GTK_BOX(data->gui->vBoxMain), ToolBar, FALSE, TRUE, 0);
  gtk_widget_show(ToolBar);

  /* Bouton New */
  button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "New", "New network", "Private", NULL, GTK_SIGNAL_FUNC(NewNetwork), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));

  /* Bouton Open */
  button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "Open", "Open network", "Private", NULL, GTK_SIGNAL_FUNC(OpenNetwork), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));

  /* Bouton Save */
  button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "Save", "Save network", "Private", NULL, GTK_SIGNAL_FUNC(SaveNetwork), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));

  /* Bouton AutoCreateComlink */
  button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "Auto create links", "Automatically create network links from scripts", "Private", NULL, GTK_SIGNAL_FUNC(AutoCreateComlink), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));

  /* Bouton GenerateNetwork */
  button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "Generate .prt", "Generate network file config", "Private", NULL, GTK_SIGNAL_FUNC(GenerateNetwork), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));

  /* Bouton Compile tous les scripts */
  button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "Compile all", "Compile all neural scripts", "Private", NULL, GTK_SIGNAL_FUNC(CompileAllScripts), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));

  /* Bouton GenerateDeploy */
  button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "Generate deployment", "Generate deployment script", "Private", NULL, GTK_SIGNAL_FUNC(GenerateDeploySh), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));

  /* Bouton GenerateRun */
  /*button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "Generate launcher", "Generate launch script", "Private", NULL, GTK_SIGNAL_FUNC(GenerateRunSh), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));*/

  /* bouton quit */
  button = gtk_toolbar_append_item(GTK_TOOLBAR(ToolBar), "Quit", "Quit", "Private", NULL, GTK_SIGNAL_FUNC(WinMain_destroy), data);
  gtk_widget_show(button);
  gtk_toolbar_append_space(GTK_TOOLBAR(ToolBar));

}

void	Tools_NewComputer(GtkWidget *widget, gpointer data)
{
  Edit_Computer((t_gennet *)data, NULL);
}

void	Tools_NewComlink(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_comlink	*comlink = NULL;

  if (gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget)) == TRUE)
    {
      gen->gui->NewComlink = 1;
      comlink = add_gennet_comlink(gen);
      comlink->prom_link = promnet_add_new_prom_link(gen->promnet);
      if (NewComlink_getName(gen, comlink) == 0)
	{
	  del_gennet_comlink(gen, comlink);
	  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(((t_gennet *)data)->gui->NewComlinkItem), FALSE);
	  ((t_gennet *)data)->gui->NewComlink = 0;
	}
    }
  else
    {
      if (gen->gui->NewComlink == 1)
	{
	  if (gen->comlinks)
	    {
	      del_gennet_comlink(gen, gen->comlinks);
	    }
	}
      gen->gui->NewComlink = 0;
    }
}

void	Tools_NewScript(GtkWidget *widget, gpointer data)
{
  Edit_Properties((t_gennet *)data, NULL);
}
