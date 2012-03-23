#include <stdlib.h>
#include <math.h>
#include <locale.h>

#include "icones.h"
#include "public_leto.h"
#include "gennet.h"
#include "creation.h"
#include "leto_global_var.h"
#include <search.h>
#include "gere_coudes.h"

int get_filename_from_path(char *val)
{
  int i,tmp=0;
  for(i=0;i<strlen(val);i++)
  {
    if(val[i]=='/')
      tmp=i+1;
  }
  return tmp;
}

void		fill_widget_list_computer(t_gennet *data, t_gennet_computer *pcomputer)
{
  t_gennet_computer	*computer = NULL;
  GtkTreeIter		iter;
  GtkTreeSelection	*selection;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->gui->tree_view));
  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
      gtk_list_store_append(data->gui->list_computer, &iter);
      gtk_list_store_set(data->gui->list_computer, &iter, 
			 COL_COMPUTER_ADDRESS, promnet_computer_get_address(computer->computer),
			 COL_COMPUTER_LOGIN, promnet_computer_get_login(computer->computer),
			 COL_COMPUTER_PATH, promnet_computer_get_path(computer->computer),
			 COL_COMPUTER_CPU, promnet_computer_get_cpu(computer->computer),
			 COL_COMPUTER_INFORMATION, promnet_computer_get_information(computer->computer),
			 COL_COMPUTER_DATA, computer,
			 -1);
      if (pcomputer == computer)
	gtk_tree_selection_select_iter(selection, &iter);
    }
}


t_gennet_computer	*find_widget_computer_by_script(t_gennet *data, t_gennet_script *script)
{
  t_gennet_computer	*computer = NULL;
  t_gennet_script_list	*script_list = NULL;

  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
      for (script_list = computer->scriptlist; script_list != NULL; script_list = script_list->next)
	{
	  if (script == script_list->script)
	    return computer;
	}
    }
  return NULL;
}

t_gennet_computer	*find_widget_computer_by_script_list(t_gennet *data, t_gennet_script_list *scriptlist)
{
  t_gennet_computer	*computer = NULL;
  t_gennet_script_list	*script_list = NULL;

  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
      for (script_list = computer->scriptlist; script_list != NULL; script_list = script_list->next)
	{
	  if (scriptlist == script_list)
	    return computer;
	}
    }
  return NULL;
}

t_gennet_computer	*find_unassigned_computer(t_gennet *data)
{
  t_gennet_computer	*computer = NULL;
  char			*name;

  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
      name = promnet_computer_get_name(computer->computer);
      if ((name == NULL) || (strlen(name) == 0))
	{
	  if (name != NULL)
	    free(name);
	  return computer;
	}
      if (name != NULL)
	free(name);
    }
  return NULL;
}


void		move_script_out_computer(t_gennet *data, t_gennet_computer *computer, t_gennet_script_list *script_list)
{
  script_list->script->in_computer = GENNET_OUT;
  computer->nb_script -= 1;
  promnet_prom_script_set_computer(script_list->script->prom_script, "");
  if (computer->scriptlist == script_list)
    {
      computer->scriptlist = script_list->next;
      if (computer->scriptlist != NULL)
	computer->scriptlist->prev = NULL;
    }
  else
    {
      if (script_list->next)
	script_list->next->prev = script_list->prev;
      if (script_list->prev)
	script_list->prev->next = script_list->next;
    }
  if (script_list != NULL)
    free(script_list);
}

void		move_script_in_computer(t_gennet *data, t_gennet_computer *computer, t_gennet_script *script)
{
  t_gennet_script_list	*scriptlist = NULL;
  char			*com_name = NULL;

  if ((scriptlist = malloc(sizeof(t_gennet_script_list))) == NULL)
    {
      g_critical("move_script_in_computer : Memory error");
      return ;
    }
  memset(scriptlist, 0, sizeof(t_gennet_script_list));
  if (computer->scriptlist == NULL)
    computer->scriptlist = scriptlist;
  else
    {
      computer->scriptlist->prev = scriptlist;
      scriptlist->next = computer->scriptlist;
      computer->scriptlist = scriptlist;
    }
  scriptlist->script = script;
  scriptlist->pango = script->pango;
  computer->nb_script += 1;
  script->in_computer = GENNET_IN_COMPUTER;
  com_name = promnet_computer_get_name(computer->computer);
  promnet_prom_script_set_computer(script->prom_script, com_name);
  if (com_name != NULL)
    free(com_name);
}



void		add_comlink_in_to_script(t_gennet_script *script, t_gennet_comlink *comlink)
{
  t_gennet_comlink_list	*comlinklist = NULL;

  if ((comlinklist = malloc(sizeof(t_gennet_comlink_list))) == NULL)
    {
      g_critical("add_comlink_in_to_script : Memory error");
      return ;
    }
  memset(comlinklist, 0, sizeof(t_gennet_comlink_list));
  if (script->comlinklist_out == NULL)
    script->comlinklist_out = comlinklist;
  else
    {
      script->comlinklist_out->prev = comlinklist;
      comlinklist->next = script->comlinklist_out;
      script->comlinklist_out = comlinklist;
    }
  comlinklist->comlink = comlink;
  promnet_prom_link_set_input(comlink->prom_link, script->prom_script);
}

void		add_comlink_out_to_script(t_gennet_script *script, t_gennet_comlink *comlink)
{
  t_gennet_comlink_list	*comlinklist = NULL;

  if ((comlinklist = malloc(sizeof(t_gennet_comlink_list))) == NULL)
    {
      g_critical("add_comlink_in_to_script : Memory error");
      return ;
    }
  memset(comlinklist, 0, sizeof(t_gennet_comlink_list));
  if (script->comlinklist_in == NULL)
    script->comlinklist_in = comlinklist;
  else
    {
      script->comlinklist_in->prev = comlinklist;
      comlinklist->next = script->comlinklist_in;
      script->comlinklist_in = comlinklist;
    }
  comlinklist->comlink = comlink;
  promnet_prom_link_set_output(comlink->prom_link, script->prom_script);
}


void		add_script_to_link(t_gennet *data, t_gennet_comlink *comlink, t_gennet_script *script)
{
  if (comlink->in == NULL)
    {
      comlink->in = script;
      add_polyline_to_polyline_list(&(comlink->polyline_list));
    }
  else if (comlink->out == NULL)
    {
      comlink->out = script;
      
      add_comlink_in_to_script(comlink->in, comlink);
      add_comlink_out_to_script(script, comlink);
      data->gui->NewComlink = 0;
      gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(data->gui->NewComlinkItem), FALSE);
      refresh_link(data, comlink);
    }
}


void		del_gennet_computer(t_gennet *data, t_gennet_computer *computer)
{
  if (data->computers == NULL)
    {
      return ;
    }

  if (data->computers == computer)
    {
      data->computers = computer->next;
      if (data->computers != NULL)
	data->computers->prev = NULL;
    }
  else
    {
      if (computer->next)
	computer->next->prev = computer->prev;
      if (computer->prev)
	computer->prev->next = computer->next;
    }
  promnet_del_computer(data->promnet, computer->computer);

  if (computer != NULL)
    free(computer);
}

void		del_gennet_script(t_gennet *data, t_gennet_script *script)
{
  if (data->scripts == NULL)
    {
      return ;
    }

  if (data->scripts == script)
    {
      data->scripts = script->next;
      if (data->scripts != NULL)
	data->scripts->prev = NULL;
    }
  else
    {
      if (script->next)
	script->next->prev = script->prev;
      if (script->prev)
	script->prev->next = script->next;
    }
  promnet_del_prom_script(data->promnet, script->prom_script);
  if (script != NULL)
      free(script);
}

void		del_gennet_scriptlist(t_gennet_computer *computer, t_gennet_script_list *scriptlist)
{
  if (computer->scriptlist == NULL)
    {
      return ;
    }

  if (computer->scriptlist == scriptlist)
    {
      computer->scriptlist = scriptlist->next;
      if (computer->scriptlist != NULL)
	computer->scriptlist->prev = NULL;
    }
  else
    {
      if (scriptlist->next)
	scriptlist->next->prev = scriptlist->prev;
      if (scriptlist->prev)
	scriptlist->prev->next = scriptlist->next;
    }
  if (scriptlist != NULL)
      free(scriptlist);
}

t_gennet_comlink_list	*find_comlink_list_by_comlink(t_gennet_comlink_list *comlinklist, t_gennet_comlink *comlink)
{
  if (comlinklist == NULL)
    return NULL;
  for (; comlinklist->prev != NULL; comlinklist = comlinklist->prev);
  for (; comlinklist != NULL; comlinklist = comlinklist->next)
    if (comlinklist->comlink == comlink)
      return comlinklist;
  return NULL;
}

void		del_gennet_comlinklist_in(t_gennet_script *script, t_gennet_comlink_list *comlinklist)
{
  if (script->comlinklist_in == NULL)
    {
      return ;
    }

  if (script->comlinklist_in == comlinklist)
    {
      script->comlinklist_in = comlinklist->next;
      if (script->comlinklist_in != NULL)
	script->comlinklist_in->prev = NULL;
    }
  else
    {
      if (comlinklist->next)
	comlinklist->next->prev = comlinklist->prev;
      if (comlinklist->prev)
	comlinklist->prev->next = comlinklist->next;
    }
  if (comlinklist != NULL)
      free(comlinklist);
}

void		del_gennet_comlinklist_out(t_gennet_script *script, t_gennet_comlink_list *comlinklist)
{
  if (script->comlinklist_out == NULL)
    {
      return ;
    }

  if (script->comlinklist_out == comlinklist)
    {
      script->comlinklist_out = comlinklist->next;
      if (script->comlinklist_out != NULL)
	script->comlinklist_out->prev = NULL;
    }
  else
    {
      if (comlinklist->next)
	comlinklist->next->prev = comlinklist->prev;
      if (comlinklist->prev)
	comlinklist->prev->next = comlinklist->next;
    }
  if (comlinklist != NULL)    
      free(comlinklist);
}


void		del_gennet_comlink(t_gennet *data, t_gennet_comlink *comlink)
{
  if (data->comlinks == NULL)
    {
      return ;
    }

  if (data->comlinks == comlink)
    {
      data->comlinks = comlink->next;
      if (data->comlinks != NULL)
	data->comlinks->prev = NULL;
    }
  else
    {
      if (comlink->next)
	comlink->next->prev = comlink->prev;
      if (comlink->prev)
	comlink->prev->next = comlink->next;
    }

  promnet_del_prom_link(data->promnet, comlink->prom_link);  

  delete_all_polylines_in_polyline_list(&(comlink->polyline_list));

  if (comlink != NULL)
    {
      memset(comlink,0,sizeof(t_gennet_comlink));
      free(comlink);
      comlink = NULL;
    }
}

void		del_all_comlinklist_in(t_gennet *data, t_gennet_script *script)
{
  t_gennet_comlink_list	*comlinklist = NULL;
  t_gennet_comlink_list	*next = NULL;
  t_gennet_comlink_list	*pcomlinklist = NULL;

  if (script == NULL)
    return;

  comlinklist = script->comlinklist_in;
  while (comlinklist != NULL)
    {
      next = comlinklist->next;
      if (comlinklist->comlink != NULL)
	{
	  if ((pcomlinklist = find_comlink_list_by_comlink(comlinklist->comlink->in->comlinklist_out, comlinklist->comlink)) != NULL)
	    {
	      del_gennet_comlinklist_out(comlinklist->comlink->in, pcomlinklist);
	    }
	  del_gennet_comlink(data, comlinklist->comlink);
	}
      del_gennet_comlinklist_in(script, comlinklist);
      comlinklist = next;
    }
}

void		del_all_comlinklist_out(t_gennet *data, t_gennet_script *script)
{
  t_gennet_comlink_list	*comlinklist = NULL;
  t_gennet_comlink_list	*next = NULL;
  t_gennet_comlink_list	*pcomlinklist = NULL;

  if (script == NULL)
    return;
  comlinklist = script->comlinklist_out;
  while (comlinklist != NULL)
    {
      next = comlinklist->next;
      if (comlinklist->comlink != NULL)
	{
	  if ((pcomlinklist = find_comlink_list_by_comlink(comlinklist->comlink->out->comlinklist_in, comlinklist->comlink)) != NULL)
	    {
	      del_gennet_comlinklist_in(comlinklist->comlink->out, pcomlinklist);
	    }
	  del_gennet_comlink(data, comlinklist->comlink);
	}
      del_gennet_comlinklist_out(script, comlinklist);
      comlinklist = next;
    }
}

void		del_all_comlinklist(t_gennet *data, t_gennet_script *script)
{
  if (script == NULL)
    return;
  if (script->comlinklist_in != NULL)
    del_all_comlinklist_in(data, script);
  if (script->comlinklist_out != NULL)
    del_all_comlinklist_out(data, script);
}


t_gennet_script	*add_gennet_script(t_gennet *data)
{
  t_gennet_script		*script = NULL;

  if ((script = malloc(sizeof(t_gennet_script))) == NULL)
    {
      g_critical("add_gennet_script : Memory error");
      return NULL;
    }
  memset(script, 0, sizeof(t_gennet_script));

  if (data->scripts == NULL)
  {
    data->scripts = script;
  }
  else
  {
     data->scripts->prev = script;
     script->next = data->scripts;
     data->scripts = script;
  }

  init_draw_script(data, script);
  
  return script;
}


char		*get_leto_binary()
{
  char		*home_path = "\0";
  char		*promethe_path;
  char		*leto_path = NULL;

  if ((promethe_path = getenv(PROMETHE_ENV)) == NULL)
    {
      if ((home_path = getenv(HOME_ENV)) == NULL)
	{
	  fprintf(stderr, "get_leto_binary : path to leto binary is not found\n");
	  return NULL;
	}
      promethe_path = PROMETHE_DIR;
    }
  if ((leto_path = malloc(PATH_MAX * sizeof(char))) == NULL)
    {
      perror("get_leto_binary : malloc");
      return NULL;
    }
  memset(leto_path, 0, PATH_MAX * sizeof(char));
  sprintf(leto_path, "%s/%s/%s", home_path, promethe_path, "bin/Linux/leto");
  return leto_path;
}


/**************************************************************************************/

/* Permet d'editer un script leto dans un nouvel onglet */
void	Edit_Script_With_Leto(t_gennet *gen, t_gennet_script *pscript)
{
  int 			i,nPage,idx;
  char			*nom_script = NULL;
  char			*nom_script_draw = NULL;
  char			*nom_script_var = NULL;
  char 			*nom_script_res = NULL;
  char			*nom_script_script = NULL;
  char			*copie_script_script = NULL;
  char			*pt = NULL;
  const char			*tab_name = NULL;
  gchar			*sMenuLabel = NULL;
  GtkWidget		*NomOnglet = NULL;
  GtkWidget 		*pMenuLabel = NULL;
  GtkWidget		*parent = NULL;
  GtkWidget 		*page = NULL;
  GtkWidget		*vbox = NULL;
  GtkNotebook 		*pNotebook = NULL;

  if (pscript->sc != NULL)
    {
      return ;
    }

  /* Tout d'abord on regarde a� quel indice peut-on creer l'onglet, si il reste encore de la place */
  idx=tab_is_Free();
  if(idx == -1)
    {
      fprintf(stderr,"Pas de place disponible pour un nouvel onglet de script leto ");
      return;
    }
  
  /* On memorise le nom du script */
  nom_script = promnet_prom_script_get_logical_name(pscript->prom_script);
  
  /* On memorise le chemin du script */
  nom_script_script = promnet_prom_script_get_path_file_script(pscript->prom_script);
  
  /* cas ou il n'y a pas de chemin de definis */
  if(nom_script_script == NULL || strlen(nom_script_script) == 0)
    {
      if ((nom_script_script = malloc (sizeof(char)*(strlen(nom_script)+8))) == NULL)
	{
	  g_critical("Edit_Script_With_Leto : Memory error");
	  /* fermer ce qu'il faut */
	  free(nom_script);
	  pscript->onglet_leto = NULL;
	  pscript->fenetre_dialogue = NULL;
	  return ;			
	}
      memset(nom_script_script, 0, sizeof(char)*(strlen(nom_script)+8));
      sprintf(nom_script_script,"%s.script",nom_script);
    }
  else if(nom_script == NULL || strlen(nom_script) == 0) /* cas ou il y a un chemin mais pas de nom logique */
    {
      if(nom_script != NULL || strlen(nom_script) == 0)
	free(nom_script);
      
      /* si le chemin existe et que le nom_script est null alors le nom_script sera
       * le nom du script fournit */
      
      /* on alloue suffisament de memoire */
      if((nom_script = (char *) malloc((strlen(nom_script_script)+1) * sizeof(char))) == NULL)
	{
	  g_critical("Edit_Script_With_Leto : Memory error");
	  /* fermer ce qu'il faut */
	  pscript->onglet_leto = NULL;
	  pscript->fenetre_dialogue = NULL;
	  return ;			
	}
      memset(nom_script,0,(strlen(nom_script_script)+1) * sizeof(char));
      /* on copie le chemin dans le nom logique */
      memcpy(nom_script,nom_script_script,(strlen(nom_script_script)+1) * sizeof(char));
      /* on recupere uniquement le nom du script pour le nom logique */
      get_base_name(nom_script);
      /* mise a jour du pscript */
      promnet_prom_script_set_logical_name(pscript->prom_script, nom_script);
    }

  /* On memorise le chemin du script */
  nom_script_draw = promnet_prom_script_get_path_file_draw(pscript->prom_script);

  if(nom_script_draw == NULL || strlen(nom_script_draw) == 0)
    {
       if ((nom_script_draw = malloc (sizeof(char)*(strlen(nom_script_script)+1))) == NULL)
	{
	  g_critical("Edit_Script_With_Leto : Memory error");
	  /* fermer ce qu'il faut */
	  free(nom_script);
	  free(nom_script_script);
	  pscript->onglet_leto = NULL;
	  pscript->fenetre_dialogue = NULL;
	  return ;			
	}
      memset(nom_script_draw, 0, sizeof(char)*strlen(nom_script_script));

      /* On copie le chemin du script */
      if ((copie_script_script = strdup(nom_script_script)) == NULL)
	{
	  g_critical("Edit_Script_With_Leto : Memory error");
	  /* fermer ce qu'il faut */
	  free(nom_script);
	  free(nom_script_script);
	  free(nom_script_draw);
	  pscript->onglet_leto = NULL;
	  pscript->fenetre_dialogue = NULL;
	  return ;			
	}
      
      /* On met a jour le chemin du script.draw */
      pt = rindex(copie_script_script, '.');
      if(pt!=NULL)
	{
	  *pt='\0'; /* on oublie l'extension du fichier: on met fin de chaine au debut de l'extension */
	  sprintf(nom_script_draw,"%s.draw",copie_script_script);
	}
      if(copie_script_script != NULL)
	free(copie_script_script);
    }

  /* On memorise le chemin du reseau */
  nom_script_res = promnet_prom_script_get_path_file_res(pscript->prom_script);

  /* On memorise le chemin du .var */
  nom_script_var = promnet_prom_script_get_path_file_var(pscript->prom_script);
  
  /*** Controle si le script n'est pas deja ouvert, cela est utile uniquement 
   *** si le nombre d'onglet genere par gtk est superieur a 1 
   *** (NOTE : le premier onglet "metaleto" ne compte pas )***/
  pNotebook = GTK_NOTEBOOK(gen->gui->tabbedWin);
  nPage=(int)gtk_notebook_get_n_pages (pNotebook);
  for (i = 1; i < nPage; i++)
    {
      page = NULL;
      page = gtk_notebook_get_nth_page(pNotebook, i);
      /* comparaison du nom du label de l'onglet parcouru 
       * avec le nom du label que l'utilisateur souhaite creer */
      tab_name = gtk_notebook_get_tab_label_text(pNotebook, page);
      if(strncmp(tab_name, nom_script, strlen(nom_script) * sizeof(char))==0 && (tab_name[strlen(nom_script)] == '*' || tab_name[strlen(nom_script)] == '\0'))
	{
	   fprintf(stderr, "WARNING: the script already has a Leto tab\n");
	   break;
	}
    }
  
  /* si le script n'est pas deja� ouvert alors on creer un nouvel onglet */
  if(i >= nPage)
    {
      /********************************************** Creation de l'onglet ***********************************************/
      
      if ((pscript->onglet_leto = malloc(sizeof(TxDonneesFenetre))) == NULL)
	{
	  g_critical("Edit_Script_With_Leto : Memory error");
	  free(nom_script);
	  /* fermer ce qu'il faut */
	  return ;			
	}
      memset(pscript->onglet_leto, 0, sizeof(TxDonneesFenetre));
      
      sMenuLabel = g_strdup_printf("Menu -> Page %d", nPage);
      pMenuLabel = gtk_label_new(sMenuLabel);
      
      NomOnglet = gtk_label_new(nom_script);
      pscript->onglet_leto->window = gtk_vbox_new(FALSE,1);
      
      /********************************* Creation de la fenetre ***********************************************************/
      
      /* Pour le leto (non metaleto) faire la meme chose pour la fenetre principal de Leto avec le meme nom (Winmain) */
      g_object_ref(gen->gui->WinMain);
      g_object_set_data_full(G_OBJECT(pscript->onglet_leto->window),
			     "Winmain", gen->gui->WinMain,
			     (GDestroyNotify) g_object_unref);
      
      /* save du notebook */
      g_object_ref(pNotebook);
      g_object_set_data_full(G_OBJECT(pscript->onglet_leto->window),
			     "Notebook", pNotebook,
			     (GDestroyNotify) g_object_unref);
      
      /* creation d'une box vertical pour le menu */
      vbox = gtk_vbox_new(FALSE, 1);
      gtk_box_pack_start(GTK_BOX(pscript->onglet_leto->window), vbox, FALSE, FALSE, 0);
      
      /* on l'attache a une vbox pour que le menu soit en haut de la fenetre */
      create_menubar_leto(pscript, vbox, nPage);
      /* le dernier parametre est null car seulement utile pour la version Leto seul */
      parent = create_scroll_leto(pscript->onglet_leto,parent, NULL);
      create_drawingArea_leto(pscript->onglet_leto,parent);
      create_status_bar_leto(pscript->onglet_leto, NULL);
      
      if ((pscript->fenetre_dialogue = malloc(sizeof(TxDonneesFenetre))) == NULL)
	{
	  g_critical("Edit_Script_With_Leto : Memory error");
	  /* fermer ce qu'il faut */
	  free(nom_script);
	  free(pscript->onglet_leto);
	  pscript->onglet_leto = NULL;
	  return ;			
	}
      memset(pscript->fenetre_dialogue, 0, sizeof(TxDonneesFenetre));
      
      create_fenetre_dialogue(pscript->fenetre_dialogue,pscript->onglet_leto); /* pour Find no */
      
      /********************************************************************************************************************/
      

      pscript->onglet_leto->cr=NULL;

      
      /* insere un onglet dans pNotebook, qui a pour nom NomOnglet, 
       * contient pscript->onglet_leto->window et a comme menu pMenuLabel */
      gtk_notebook_append_page_menu(pNotebook, pscript->onglet_leto->window, NomOnglet, pMenuLabel);
      
      /* on libere */
      g_free(sMenuLabel);
      
      /* Activation du menu popup */
      gtk_notebook_popup_enable(pNotebook);
      
      /* Affichage */
      gtk_widget_show_all(gen->gui->tabbedWin);
      
      /******************************************************* Lancement de Leto ****************************************/
      
      /* initialise le gc car cela genere parfois des erreurs de segmentations au demarrage */
      pscript->onglet_leto->gc = gdk_gc_new(pscript->onglet_leto->window->window);
      
      /* lancement de leto avec les nom de fichier.script et . draw et l'indice de script disponible et l'indice de l'onglet */
      run_leto( nom_script_script, nom_script_draw, nom_script_res, nom_script_var, pscript->onglet_leto, idx, nPage, gen->seed);
      pscript->sc=sc;
      sc = NULL;

      if (nom_script_script != NULL)
	free(nom_script_script);

      if (nom_script_draw != NULL)
	free(nom_script_draw);
    }
  if (nom_script != NULL)
    free(nom_script);
}


t_gennet_computer	*add_gennet_computer(t_gennet *data)
{
  t_gennet_computer	*computer = NULL;

  if ((computer = malloc(sizeof(t_gennet_computer))) == NULL)
    {
      g_critical("add_gennet_computer : Memory error");
      return NULL;
    }
  memset(computer, 0, sizeof(t_gennet_computer));
  if (data->computers == NULL)
    data->computers = computer;
  else
    {
      data->computers->prev = computer;
      computer->next = data->computers;
      data->computers = computer;
    }

  init_draw_computer(data, computer);

  return computer;
}

t_gennet_computer	*copy_gennet_computer(t_gennet *data, t_gennet_computer *src)
{
  t_gennet_computer	*computer = NULL;

  if ((computer = add_gennet_computer(data)) != NULL)
    {
      computer->computer = promnet_add_new_computer(data->promnet);
      promnet_computer_copy(src->computer, computer->computer);
      return computer;
    }
  return NULL;
}

t_gennet_computer	*find_selected_computer(t_gennet *data)
{
  GtkTreeSelection	*selection;
  GtkTreeModel		*model;
  GtkTreeIter		iter;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->gui->tree_view));
  if (gtk_tree_selection_get_selected(selection, &model, &iter))
  {
    t_gennet_computer	*computer = NULL;

    gtk_tree_model_get(model, &iter, COL_COMPUTER_DATA, &computer, -1);
    return computer;
  }
  return NULL;
}

void			del_all_gennet_comlink(t_gennet *data)
{
  t_gennet_comlink	*current = NULL;
  t_gennet_comlink_list	*comlinklist = NULL;

  for (current = data->comlinks; current != NULL; current = current->next)
    {
      if (current->in)
	{
	  if ((comlinklist = find_comlink_list_by_comlink(current->in->comlinklist_out, current)) != NULL)
	    {
	      del_gennet_comlinklist_out(comlinklist->comlink->in, comlinklist);
	    }
	}
      if (current->out)
	{
	  if ((comlinklist = find_comlink_list_by_comlink(current->out->comlinklist_in, current)) != NULL)
	    {
	      del_gennet_comlinklist_in(comlinklist->comlink->out, comlinklist);
	    }
	}

      del_gennet_comlink(data, current);
    }
}

void		del_all_scriptlist(t_gennet *data, t_gennet_computer *computer)
{
  t_gennet_script_list	*current = NULL;
  t_gennet_script_list	*next = NULL;

  if (computer == NULL)
    return;
  current = computer->scriptlist;
  while (current != NULL)
    {
      next = current->next;
      if (current->script != NULL)
	{
	  del_all_comlinklist(data, current->script);

	  /* on supprime tout le(s) onglet(s) leto rattache(s) au(x) script(s) */
	  if (current->script->sc != NULL && current->script->sc->modified != 0)
	    {
	      if ((erase_without_saving(current->script)) == 0)
		return ;
	    }
	  if (current->script->sc != NULL)
	    gtk_notebook_remove_page (GTK_NOTEBOOK(((t_gennet *)data)->gui->tabbedWin),current->script->sc->num_onglet);

	  del_gennet_script(data, current->script);
	}
      del_gennet_scriptlist(computer, current);
      current = next;
    }
}

void		del_all_computer(t_gennet *data)
{
  t_gennet_computer	*current = NULL;
  t_gennet_computer	*next = NULL;

  current = data->computers;
  while (current != NULL)
    {
      next = current->next;
      if (current->scriptlist != NULL)
	{
	  del_all_scriptlist(data, current);
	}
      del_gennet_computer(data, current);
      current = next;
    }
}

void		del_all_script(t_gennet *data)
{
  t_gennet_script	*current = NULL;
  t_gennet_script	*next = NULL;

  current = data->scripts;
  while (current != NULL)
    {
      next = current->next;
      del_all_comlinklist(data, current);

      /* on supprime tout le(s) onglet(s) leto rattache(s) au(x) script(s) */
      if (current->sc != NULL && current->sc->modified != 0)
	{
	  if ((erase_without_saving(current)) == 0)
	    return ;
	}
      if (current->sc != NULL)
	gtk_notebook_remove_page (GTK_NOTEBOOK(((t_gennet *)data)->gui->tabbedWin),current->sc->num_onglet);

      del_gennet_script(data, current);
      current = next;
    }
}

void		del_all_gennet(t_gennet *data)
{
  /* nettoie les computer + les script qui leurs sont attaches */
  if (data->computers != NULL)
    del_all_computer(data);

  /* nettoie tous les scripts restant : donc ceux qui ne sont pas attaches */
  if (data->scripts != NULL)
    del_all_script(data);

  /* SI ON PASSE DANS CE IF, C EST QU IL Y A UN PROBLEME GRAVE ! */
  if (data->comlinks != NULL)
    del_all_gennet_comlink(data);

  if (data->promnet != NULL)
  {
     promnet_set_net_path(data->promnet,"");
     promnet_set_cpt_path(data->promnet, "");
  }
}

/* creer une nouvelle application vierge */
void	NewNetwork(GtkWidget *widget, gpointer data)
{
  t_gennet_script	*pscript;
  GtkWidget		*Notebook;

  /* on ferme tout les onglets */
  for (pscript = ((t_gennet *)data)->scripts; pscript != NULL && pscript->onglet_leto != NULL; pscript = pscript->next)
    {
      Notebook = lookup_widget (pscript->onglet_leto->window ,"Notebook");
      gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),pscript->sc->num_onglet);
      tabLeto_quit(NULL,pscript);
    }

  /* on detruit tout */
  del_all_gennet((t_gennet *)data);

  /* on rafraichit */
  refresh_all((t_gennet *)data);
}

t_gennet_computer	*Tools_NewComputer_OK(t_gennet *data, const char *address, const char *login,
				     const char *path, const char *cpu, const char *infos, t_gennet_computer *pcomputer)
{
  t_gennet_computer	*computer = NULL;

  if (pcomputer != NULL)
    {
      computer = pcomputer;
    }
  else
    {
      computer = add_gennet_computer(data);
      computer->computer = promnet_add_new_computer(data->promnet);
    }
  promnet_computer_set_address(computer->computer, (char *)address);
  promnet_computer_set_login(computer->computer, (char *)login);
  promnet_computer_set_path(computer->computer, (char *)path);
  promnet_computer_set_cpu(computer->computer, (char *)cpu);
  promnet_computer_set_information(computer->computer, (char *)infos);
  return computer;
}


void	menuLinkDeletePolyline(gpointer data)
{
  t_gennet				*gen = (t_gennet *)data;
  t_gennet_comlink			*comlink = NULL;
  t_polyline				*polyline = NULL;
 
  if ((comlink = find_widget_comlink(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  if ((polyline = find_widget_comlink_polyline(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  if ((polyline->prev == NULL) && (polyline->next == NULL))
    {
      menuLinkDelete(data);
    }
  else
  {
     remove_polyline_and_update(&(comlink->polyline_list), polyline);
  }
  refresh_all(gen);
}

void	menuLinkDeleteBend(gpointer data)
{
  t_gennet				*gen = (t_gennet *)data;
  t_gennet_comlink			*comlink = NULL;
  t_polyline				*polyline = NULL;
 
  if ((comlink = find_widget_comlink(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  if ((polyline = find_widget_comlink_bend(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
    {
      return;
    }
  if ((polyline->prev == NULL) && (polyline->next == NULL))
    {
      menuLinkDelete(data);
    }
  else
  {
     remove_polyline_and_update(&(comlink->polyline_list), polyline);
  }
  refresh_all(gen);
}

void	menuLinkAddBend(gpointer data)
{
  t_gennet				*gen = (t_gennet *)data;
  t_gennet_comlink			*comlink = NULL;
  t_polyline				*polyline = NULL;
  t_polyline				*new_polyline = NULL;
 
  if ((comlink = find_widget_comlink(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
  {
     return;
  }

  if ((polyline = find_widget_comlink_polyline(gen, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
  {
     return;
  }

  if ((new_polyline = insert_polyline_after_at_coordinates(polyline, gen->gui->mouse_x, gen->gui->mouse_y)) == NULL)
  {
     return;
  }
  refresh_all(gen);
}


t_gennet_computer	*find_widget_computer_by_name(t_gennet *data, char *name)
{
  t_gennet_computer	*computer = NULL;

  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
      char	*cname = NULL;

      cname = promnet_computer_get_name(computer->computer);
      if (!strcmp(name, cname))
	{
	  if (cname != NULL)
	    free(cname);
	  return computer;
	}
      if (cname != NULL)
	free(cname);
    }
  return NULL;
}

t_gennet_script	*find_widget_script_in_computer_by_name(t_gennet *data, char *name)
{
  t_gennet_computer	*computer = NULL;
  t_gennet_script_list	*script = NULL;

  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
      for (script = computer->scriptlist; script != NULL; script = script->next)
	{
	  char	*cname = NULL;
	  
	  cname = promnet_prom_script_get_logical_name(script->script->prom_script);
	  if (!strcmp(name, cname))
	    {
	      if (cname != NULL)
		free(cname);
	      return script->script;
	    }
	  if (cname != NULL)
	    free(cname);
	}
    }
  return NULL;
}

t_gennet_script	*find_widget_script_by_name(t_gennet *data, char *name)
{
  t_gennet_script	*script = NULL;

  for (script = data->scripts; script != NULL; script = script->next)
    {
      char	*cname = NULL;

      cname = promnet_prom_script_get_logical_name(script->prom_script);
      if (!strcmp(name, cname))
	{
	  if (cname != NULL)
	    free(cname);
	  return script;
	}
      if (cname != NULL)
	free(cname);
    }
  return NULL;
}

/* void			aff_comlinklist(t_gennet_script *pscript) */
/* { */
/*   t_gennet_comlink_list	*com = NULL; */

/*   fprintf(stdout, "### ### COMLINKLIST ### ###\n"); */
/*   for (com = pscript->comlinklist_in; com != NULL; com = com->next) */
/*     { */
/*       fprintf(stdout, "IN ####\n%p %p %p\nin : %p out : %p comlink : %p\n####\n", com->prev, com, com->next, com->comlink->in, com->comlink->out, com->comlink); */
/*     } */
/*   for (com = pscript->comlinklist_out; com != NULL; com = com->next) */
/*     { */
/*       fprintf(stdout, "OUT ####\n%p %p %p\nin : %p out : %p comlink : %p\n####\n", com->prev, com, com->next, com->comlink->in, com->comlink->out, com->comlink); */
/*     } */

/* } */

/* void			aff_comlinks(t_gennet *data) */
/* { */
/*   t_gennet_comlink	*com = NULL; */

/*   fprintf(stdout, "### ### COMLINK ### ###\n"); */
/*   for (com = data->comlinks; com != NULL; com = com->next) */
/*     { */
/*       fprintf(stdout, "####\n%p %p %p\n####\n", com->prev, com, com->next); */
/*     } */
/* } */

t_gennet_comlink	*add_gennet_comlink(t_gennet *data)
{
  t_gennet_comlink	*comlink = NULL;
  
  if ((comlink = malloc(sizeof(t_gennet_comlink))) == NULL)
    {
      g_critical("add_gennet_comlink : Memory error");
      return NULL;
    }
  memset(comlink, 0, sizeof(t_gennet_comlink));
  if (data->comlinks == NULL)
    data->comlinks = comlink;
  else
    {
      data->comlinks->prev = comlink;
      comlink->next = data->comlinks;
      data->comlinks = comlink;
    }
  return comlink;
}

void		separate_superimposed_comlink_by_script_out(t_gennet *gen, t_gennet_script *script_in, t_gennet_script *script_out)
{
  int				cpt = 0;
  int				x = 0;
  int				y = 0;
  t_gennet_comlink_list		*comlinklist_out = NULL;
  t_polyline 			*polyline = NULL;

  for (comlinklist_out = script_in->comlinklist_out; comlinklist_out != NULL; comlinklist_out = comlinklist_out->next)
    {
      if (comlinklist_out->comlink->out == script_out)
	{
	   cpt++;
	   if (cpt > 1)
	   {
	      if (script_in->pos_x < script_out->pos_x)
	      {
		 x = (script_in->pos_x + IMG_SCRIPT_WIDTH) + ((script_out->pos_x - (script_in->pos_x + IMG_SCRIPT_WIDTH)) / 2);
	      }
	      else
	      {
		 x = (script_out->pos_x + IMG_SCRIPT_WIDTH) + ((script_in->pos_x - (script_out->pos_x + IMG_SCRIPT_WIDTH)) / 2);
	      }
	      
	      if (script_in->pos_y < script_out->pos_y)
	      {
		 y = (script_in->pos_y + IMG_SCRIPT_HEIGHT) + ((script_out->pos_y - (script_in->pos_y + IMG_SCRIPT_HEIGHT)) / 2);
	      }
	      else
	      {
		 y = (script_out->pos_y + IMG_SCRIPT_HEIGHT) + ((script_in->pos_y - (script_out->pos_y + IMG_SCRIPT_HEIGHT)) / 2);
	      }
	      polyline = comlinklist_out->comlink->polyline_list.first;
	      
	      if (fabs(polyline->a) < 1)
	      {
		 insert_polyline_after_at_coordinates(polyline, x, y + BEND_SPACE_SIZE * cpt);
	      }
	      else
	      {
		 insert_polyline_after_at_coordinates(polyline, x + BEND_SPACE_SIZE * cpt, y);
	      }
	      refresh_link(gen, comlinklist_out->comlink);
	   }
	}
    }
}

void		loadComlink(t_gennet *data)
{
  t_prom_link		*current = NULL;
  t_gennet_comlink	*comlink = NULL;
  t_prom_script		*prom_script = NULL;
  t_gennet_script	*script = NULL;

  while ((current = promnet_prom_link_get_next(data->promnet, current)) != NULL)
    {
      char	*name = NULL;

      comlink = add_gennet_comlink(data);
      comlink->prom_link = current;
      name = promnet_prom_link_get_name(current);
      comlink->pango = gtk_widget_create_pango_layout(data->gui->DrawingArea, name);
      memcpy(comlink->name, name, LETO_LIAISON_NAME_SIZE * sizeof(char));
      if (name != NULL)
	free(name);

      prom_script = promnet_prom_link_get_input(current);
      if (prom_script != NULL)
	{
	  name = promnet_prom_script_get_logical_name(prom_script);
	  if ((script = find_widget_script_by_name(data, name)))
	    {
	      add_script_to_link(data, comlink, script);
	    }
	  else if ((script = find_widget_script_in_computer_by_name(data, name)))
	    {
	      add_script_to_link(data, comlink, script);
	    }
	  if (name != NULL)
	    free(name);
	}

      prom_script = promnet_prom_link_get_output(current);
      if (prom_script != NULL)
	{
	  name = promnet_prom_script_get_logical_name(prom_script);
	  if ((script = find_widget_script_by_name(data, name)))
	    {
	      add_script_to_link(data, comlink, script);
	    }
	  else if ((script = find_widget_script_in_computer_by_name(data, name)))
	    {
	      add_script_to_link(data, comlink, script);
	    }
	  if (name != NULL)
	    free(name);
	}
      refresh_link(data, comlink);
    }
}

void		loadScript(t_gennet *data)
{
  t_prom_script		*current = NULL;
  t_gennet_script	*script = NULL;
  t_gennet_computer	*computer = NULL;



  debug_printf("LECTURE D'UN SCRIPT \n");

  while ((current = promnet_prom_script_get_next(data->promnet, current)) != NULL)
    {
      char	*name = NULL;

      script = add_gennet_script(data);
      script->prom_script = current;

      name = promnet_prom_script_get_logical_name(current);
      script->pango = gtk_widget_create_pango_layout(GTK_WIDGET(data->gui->DrawingArea), name);

      if (name != NULL)
	free(name);

      name = promnet_prom_script_get_computer(current);
      if (name != NULL)
	{
	  if ((computer = find_widget_computer_by_name(data, name)))
	    {
	      move_script_in_computer(data, computer, script);
	      draw_script_in_computer(data, computer, script);
	      debug_printf(" ---------- script in computer\n");
	    }
	}
      if (name != NULL)
	free(name);

      /* on ouvre l'onglet */
      Edit_Script_With_Leto(data,script);
    }
}

void		loadComputer(t_gennet *data)
{
  t_computer	*current = NULL;
  t_gennet_computer	*computer = NULL;

  while ((current = promnet_computer_get_next(data->promnet, current)) != NULL)
    {
      char	*name = NULL;

      computer = add_gennet_computer(data);
      computer->computer = current;
      name = promnet_computer_get_name(current);
      computer->pango = gtk_widget_create_pango_layout(data->gui->DrawingArea, name);
      if (name != NULL)
	free(name);
    }
}

void loadNetwork_cpt(t_gennet *data, char *cptfile)
{
  if (promnet_load_file_cpt(data->promnet, cptfile) == -1)
    {
      g_critical("loadNetwork : loading cpt file failed");
      return ;
    }
  /*
  ** Charger les computer en 1er
  */
  loadComputer(data);
}

void		separate_superimposed_comlink(t_gennet *gen)
{
  t_gennet_script	*script_in = NULL;
  t_gennet_script	*script_out = NULL;

  for (script_in = gen->scripts; script_in != NULL; script_in = script_in->next)
    {
      for (script_out = gen->scripts; script_out != NULL; script_out = script_out->next)
	{
	  separate_superimposed_comlink_by_script_out(gen, script_in, script_out);
	}
    }
}

void loadNetwork_net(t_gennet *data, char *netfile)
{

  debug_printf("execute loadNetwork_net \n");
  if (promnet_load_file_net(data->promnet, netfile) == -1)
    {
      g_critical("loadNetwork : loading net file failed");
      return ;
    }
  /*
  ** Puis les scripts
  */
  debug_printf("avant loadScript\n");
  loadScript(data);
  debug_printf("scripts reading done\n");  

  /*
  ** Puis les links
  */
  loadComlink(data);
  debug_printf("links reading done\n");  

  separate_superimposed_comlink(data);
  debug_printf("separate_superimposed_comlink done\n");  
}

void		loadNetwork(t_gennet *data, char *netfile, char *cptfile)
{
  debug_printf("LoadNetwork \n");
  loadNetwork_cpt(data,cptfile);
  loadNetwork_net(data,netfile);
  refresh_all(data);
}


void		writeNetworkSection(t_gennet *data, FILE *fprt)
{
  t_gennet_computer	*computer = NULL;
  t_gennet_script	*script = NULL;
  t_gennet_script_list	*scriptlist = NULL;
  int			port = 50000;
  char			*name = NULL;
  char			*address = NULL;

  fprintf(fprt, "begin network\n");

  if (data->computers == NULL)
    {
      computer = Tools_NewComputer_OK(data, 
				      "127.0.0.1",
				      "",
				      "",
				      "",
				      "automatically added by coeos",
				      NULL);
      promnet_computer_set_name(computer->computer, "localhost");
      computer->pango = gtk_widget_create_pango_layout(GTK_WIDGET(data->gui->DrawingArea), "localhost");

      for (script = data->scripts; script != NULL; script = script->next)
	{
	  script->pos_x = computer->pos_x + IMG_COMPUTER_WIDTH;
	  script->pos_y = computer->pos_y + IMG_COMPUTER_HEIGHT;
	  move_script_in_computer(data, computer, script);
	}
      refresh_all(data);
    }

  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
      if (computer->scriptlist == NULL)
	continue;
      for (scriptlist = computer->scriptlist; scriptlist != NULL; scriptlist = scriptlist->next)
	{
	  if (scriptlist->script == NULL)
	    continue;
	  if ((scriptlist->script->comlinklist_in != NULL) || (scriptlist->script->comlinklist_out != NULL))
	    {
	      name = promnet_prom_script_get_logical_name(scriptlist->script->prom_script);
	      address = promnet_computer_get_address(computer->computer);
	      fprintf(fprt, "%s %s %i\n", name, address, port);
	      port += 10;
	    }
	}
    }
  fprintf(fprt, "end\n");
}

t_gennet_computer	*find_computer_by_script(t_gennet_computer *computer, t_gennet_script *script)
{
  t_gennet_script_list	*scriptlist = NULL;

  for (; computer != NULL; computer = computer->next)
    {
      for (scriptlist = computer->scriptlist; scriptlist != NULL; scriptlist = scriptlist->next)
	{
	  if (scriptlist->script == script)
	    return computer;
	}
    }
  return NULL;
}

void		writeLinkSection(t_gennet *data, FILE *fprt)
{
  t_gennet_computer	*computer = NULL;
  t_gennet_comlink	*comlink = NULL;
  char			*name_link = NULL;
  char			*name_in = NULL;
  char			*name_out = NULL;

  fprintf(fprt, "begin link\n");
  for (comlink = data->comlinks; comlink != NULL; comlink = comlink->next)
    {
      if ((computer = find_computer_by_script(data->computers, comlink->in)) == NULL)
	continue;
      if ((computer = find_computer_by_script(data->computers, comlink->out)) == NULL)
	continue;
      name_link = promnet_prom_link_get_name(comlink->prom_link);
      name_in = promnet_prom_script_get_logical_name(comlink->in->prom_script);
      name_out = promnet_prom_script_get_logical_name(comlink->out->prom_script);
      fprintf(fprt, "%s %s %s\n", name_link, name_in, name_out);
      if (name_link != NULL)
	free(name_link);
      if (name_in != NULL)
	free(name_in);
      if (name_out != NULL)
	free(name_out);
    }

  fprintf(fprt, "end\n");
}

void		writePrt(t_gennet *data, FILE *fprt)
{
  printf("\nWriting prt file ...");
  writeNetworkSection(data, fprt);
  writeLinkSection(data, fprt);
  printf(".................... completed !!\n\n");
}

type_groupe	*find_next_network_send_group(type_groupe *groupe)
{
  for(; groupe != NULL; groupe = groupe->s)
    {
      if (strncmp(groupe->nom, LETO_NETWORK_GROUP_SEND, strlen(LETO_NETWORK_GROUP_SEND)) == 0)
	{
	  return groupe;
	}
    }
  return NULL;
}

type_groupe	*find_next_network_receive_group(type_groupe *groupe)
{
  for(; groupe != NULL; groupe = groupe->s)
    {
      if (strncmp(groupe->nom, LETO_NETWORK_GROUP_RECEIVE, strlen(LETO_NETWORK_GROUP_RECEIVE)) == 0)
	{
	  return groupe;
	}
      else if (strncmp(groupe->nom, LETO_NETWORK_GROUP_RECV, strlen(LETO_NETWORK_GROUP_RECV)) == 0)
	{
	  return groupe;
	}

    }
  return NULL;
}

type_liaison	*find_liaison_by_send_group(type_liaison *liaison, type_groupe *groupe)
{
  int		i = 0;
  type_liaison	*save_liaison = NULL;

  if (groupe == NULL)
    return NULL;
  for(; liaison != NULL; liaison = liaison->s)
    {
      if (liaison->arrivee == groupe->no)
	{
	  /* si le nom de la liaison ne comence pas par sync */
	  if ((strncmp(liaison->nom, "sync", 4)) != 0)
	    {
	      save_liaison = liaison;
	      i++;
	    }
	}
    }
  if (i == 0)
    {
      fprintf(stderr, "find_liaison_by_send_group : no input link from f_send no = %i no_name = %s\n", groupe->no, groupe->no_name);
    }
  else if (i == 1)
    return save_liaison;
  else if (i > 1)
    {
      fprintf(stderr, "find_liaison_by_send_group : %i input link for f_send no = %i no_name = %s\n", i, groupe->no, groupe->no_name);
      return NULL;
    }
  return NULL;
}

type_liaison	*find_liaison_by_receive_group(type_liaison *liaison, type_groupe *groupe)
{
  int		i = 0;
  type_liaison	*save_liaison = NULL;

  if (groupe == NULL)
    return NULL;
  for(; liaison != NULL; liaison = liaison->s)
    {
      if (liaison->arrivee == groupe->no)
	{
	  if ((strncmp(liaison->nom, "sync", 4)) != 0)
	    {
	      save_liaison = liaison;
	      i++;
	    }
	}
    }
  if (i == 0)
    {
      fprintf(stderr, "find_liaison_by_receive_group : no input link from f_receive no = %i no_name = %s\n", groupe->no, groupe->no_name);
    }
  else if (i == 1)
    return save_liaison;
  else if (i > 1)
    {
      fprintf(stderr, "find_liaison_by_receive_group : %i input link for f_receive no = %i no_name = %s\n", i, groupe->no, groupe->no_name);
      return NULL;
    }
  return NULL;
}

type_liaison	*find_liaison_by_name(type_liaison *liaison, char *name)
{
  int		i = 0;
  type_liaison	*save_liaison = NULL;
  char			*end_name_liaison = NULL;
  char			lname[LETO_LIAISON_NAME_SIZE];


  if (name == NULL)
    return NULL;
  for(; liaison != NULL; liaison = liaison->s)
    {
  
	memset(lname, 0, LETO_LIAISON_NAME_SIZE * sizeof(char));
	end_name_liaison = strstr(liaison->nom, LETO_NETWORK_OPTION_SEPARATOR);
	if (end_name_liaison != NULL)
	  {
	    memcpy(lname, liaison->nom, (end_name_liaison - liaison->nom) * sizeof(char));
	  }
	else
	  {
	    memcpy(lname, liaison->nom, LETO_LIAISON_NAME_SIZE * sizeof(char));
	  }

      if (strcmp(lname, name) == 0)
	{
	  save_liaison = liaison;
	  i++;
	}
    }
  if (i == 1)
    return save_liaison;
  return NULL;  
}

type_groupe	*find_groupe_by_no(type_groupe *groupe, int no)
{
  int		i = 0;
  type_groupe	*save_groupe = NULL;

  for(; groupe != NULL; groupe = groupe->s)
    {
      if (no == groupe->no)
	{
	  save_groupe = groupe;
	  i++;
	}
    }
  if (i == 0)
    {
      fprintf(stderr, "find_groupe_by_no : no group found : no = %i\n", no);
    }
  else if (i == 1)
    return save_groupe;
  else if (i > 1)
    {
      fprintf(stderr, "find_groupe_by_no : %i group found : no = %i\n", i, no);
    }
  return NULL;
}

t_gennet_script	*find_receive_script_by_network_liaison_name(t_gennet *gen, char *liaison_name)
{
  int			i = 0;
  char			*name = NULL;
  char			lname[LETO_LIAISON_NAME_SIZE];
  t_gennet_script	*script = NULL;
  t_gennet_script	*script_save = NULL;
  type_liaison		*liaison_recv = NULL;
  type_groupe		*groupe_recv = NULL;

  memset(lname, 0, LETO_LIAISON_NAME_SIZE * sizeof(char));
  name = strstr(liaison_name, LETO_NETWORK_OPTION_SEPARATOR);
  if (name != NULL)
    {
      memcpy(lname, liaison_name, (name - liaison_name) * sizeof(char));
    }
  else
    {
      memcpy(lname, liaison_name, LETO_LIAISON_NAME_SIZE * sizeof(char));
    }

  for (script = gen->scripts; script != NULL; script = script->next)
    {
      if (script->sc == NULL)
	{
	  Edit_Script_With_Leto(gen, script);
	}
      liaison_recv = script->sc->deb_liaison;
      liaison_recv = find_liaison_by_name(liaison_recv, lname);
      if (liaison_recv == NULL)
	{
	  continue;
	}
      else
	{
	  groupe_recv = script->sc->deb_groupe;
	  groupe_recv = find_groupe_by_no(groupe_recv, liaison_recv->arrivee);
	  if (groupe_recv == NULL)
	    {
	      continue;
	    }
	  if (strncmp(groupe_recv->nom, LETO_NETWORK_GROUP_RECEIVE, strlen(LETO_NETWORK_GROUP_RECEIVE)) == 0)
	    {
	      i++;
	      script_save = script;
	    }
	  else if (strncmp(groupe_recv->nom, LETO_NETWORK_GROUP_RECV, strlen(LETO_NETWORK_GROUP_RECV)) == 0)
	    {
	      i++;
	      script_save = script;
	    }
	}
    }
  if (i == 0)
    {
      fprintf(stdout, "find_receive_script_by_network_liaison_name : no input network link %s found\n", lname);
    }
  else if (i == 1)
    return script_save;
  else if (i > 1)
    {
      fprintf(stdout, "find_receive_script_by_network_liaison_name : %i input network link %s found\n", i, lname);
    }
  return NULL;
}

t_gennet_script	*find_send_script_by_network_liaison_name(t_gennet *gen, char *liaison_name)
{
  int			i = 0;
  char			*name = NULL;
  char			lname[LETO_LIAISON_NAME_SIZE];
  t_gennet_script	*script = NULL;
  t_gennet_script	*script_save = NULL;
  type_liaison		*liaison_send = NULL;
  type_groupe		*groupe_send = NULL;

  memset(lname, 0, LETO_LIAISON_NAME_SIZE * sizeof(char));
  name = strstr(liaison_name, LETO_NETWORK_OPTION_SEPARATOR);
  if (name != NULL)
    {
      memcpy(lname, liaison_name, (name - liaison_name) * sizeof(char));
    }
  else
    {
      memcpy(lname, liaison_name, LETO_LIAISON_NAME_SIZE * sizeof(char));
    }

  for (script = gen->scripts; script != NULL; script = script->next)
    {
      if (script->sc == NULL)
	{
	  Edit_Script_With_Leto(gen, script);
	}
      liaison_send = script->sc->deb_liaison;
      liaison_send = find_liaison_by_name(liaison_send, lname);
      if (liaison_send == NULL)
	{
	  continue;
	}
      else
	{
	  groupe_send = script->sc->deb_groupe;
	  groupe_send = find_groupe_by_no(groupe_send, liaison_send->arrivee);
	  if (groupe_send == NULL)
	    {
	      continue;
	    }
	  if (strncmp(groupe_send->nom, LETO_NETWORK_GROUP_SEND, strlen(LETO_NETWORK_GROUP_SEND)) == 0)
	    {
	      i++;
	      script_save = script;
	    }
	}
    }
  if (i == 0)
    {
      fprintf(stdout, "find_send_script_by_network_liaison_name : no output network link %s found\n", lname);
    }
  else if (i == 1)
    return script_save;
  else if (i > 1)
    {
      fprintf(stdout, "find_send_script_by_network_liaison_name : %i output network link %s found\n", i, lname);
    }
  return NULL;
}

int	is_comlink_exist_by_name(t_gennet *gen, char *name)
{
  t_gennet_comlink	*comlink = NULL;

  for (comlink = gen->comlinks; comlink != NULL; comlink = comlink->next)
    {
      if (strcmp(comlink->name, name) == 0)
	return 1;
    }
  return 0;
}

void	AutoCreateComlink_by_receive_group(t_gennet *gen, t_gennet_script *script)
{
  t_gennet_script	*script_send = NULL;
  t_gennet_comlink	*comlink = NULL;
  type_groupe		*groupe = NULL;
  type_liaison		*liaison_recv = NULL;
  char			*name = NULL;
  char			lname[LETO_LIAISON_NAME_SIZE];

  for (groupe = script->sc->deb_groupe; groupe != NULL; groupe = groupe->s)
    {
      groupe = find_next_network_receive_group(groupe);
      if (groupe == NULL)
	break;
      
      liaison_recv = script->sc->deb_liaison;
      liaison_recv = find_liaison_by_receive_group(liaison_recv, groupe);
      if (liaison_recv == NULL)
	{
	  continue;
	}
      script_send = find_send_script_by_network_liaison_name(gen, liaison_recv->nom);
      if (script_send == NULL)
	{
	  continue;
	}
      
      memset(lname, 0, LETO_LIAISON_NAME_SIZE * sizeof(char));
      name = strstr(liaison_recv->nom, LETO_NETWORK_OPTION_SEPARATOR);
      if (name != NULL)
	{
	  memcpy(lname, liaison_recv->nom, (name - liaison_recv->nom) * sizeof(char));
	}
      else
	{
	  memcpy(lname, liaison_recv->nom, LETO_LIAISON_NAME_SIZE * sizeof(char));
	}
      name = lname;
/*       name = strstr(lname, LETO_NETWORK_ID_SEPARATOR); */
/*       if (name != NULL) */
/* 	{ */
/* 	  *name = '\0'; */
/* 	  memset(name + 1, 0, (LETO_LIAISON_NAME_SIZE - strlen(lname)) * sizeof(char)); */
/* 	} */

      if (is_comlink_exist_by_name(gen, lname) == 0)
	{
	  comlink = add_gennet_comlink(gen);
	  comlink->prom_link = promnet_add_new_prom_link(gen->promnet);
	  memcpy(comlink->name, lname, LETO_LIAISON_NAME_SIZE * sizeof(char));
	  comlink->pango = gtk_widget_create_pango_layout(gen->gui->DrawingArea, comlink->name);
	  promnet_prom_link_set_name(comlink->prom_link, comlink->name);
	  add_script_to_link(gen, comlink, script_send);
	  add_script_to_link(gen, comlink, script);
	}
    }
}

void	AutoCreateComlink_by_send_group(t_gennet *gen, t_gennet_script *script)
{
  t_gennet_script	*script_recv = NULL;
  t_gennet_comlink	*comlink = NULL;
  type_groupe		*groupe = NULL;
  type_liaison		*liaison_send = NULL;
  char			*name = NULL;
  char			lname[LETO_LIAISON_NAME_SIZE];

  for (groupe = script->sc->deb_groupe; groupe != NULL; groupe = groupe->s)
    {
      groupe = find_next_network_send_group(groupe);
      if (groupe == NULL)
	break;
      
      liaison_send = script->sc->deb_liaison;
      liaison_send = find_liaison_by_send_group(liaison_send, groupe);
      if (liaison_send == NULL)
	{
	  continue;
	}
      script_recv = find_receive_script_by_network_liaison_name(gen, liaison_send->nom);
      if (script_recv == NULL)
	{
	  continue;
	}
      
      memset(lname, 0, LETO_LIAISON_NAME_SIZE * sizeof(char));
      name = strstr(liaison_send->nom, LETO_NETWORK_OPTION_SEPARATOR);
      if (name != NULL)
	{
	  memcpy(lname, liaison_send->nom, (name - liaison_send->nom) * sizeof(char));
	}
      else
	{
	  memcpy(lname, liaison_send->nom, LETO_LIAISON_NAME_SIZE * sizeof(char));
	}
      name = lname;
/*       name = strstr(lname, LETO_NETWORK_ID_SEPARATOR); */
/*       if (name != NULL) */
/* 	{ */
/* 	  *name = '\0'; */
/* 	  memset(name + 1, 0, (LETO_LIAISON_NAME_SIZE - strlen(lname)) * sizeof(char)); */
/* 	} */

      if (is_comlink_exist_by_name(gen, lname) == 0)
	{
	  comlink = add_gennet_comlink(gen);
	  comlink->prom_link = promnet_add_new_prom_link(gen->promnet);
	  memcpy(comlink->name, lname, LETO_LIAISON_NAME_SIZE * sizeof(char));
	  comlink->pango = gtk_widget_create_pango_layout(gen->gui->DrawingArea, comlink->name);
	  promnet_prom_link_set_name(comlink->prom_link, comlink->name);
	  add_script_to_link(gen, comlink, script);
	  add_script_to_link(gen, comlink, script_recv);
	}
    }
}

void	AutoCreateComlink(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script	*script = NULL;

  for (script = gen->scripts; script != NULL; script = script->next)
    {
      del_all_comlinklist(gen, script);
      if (script->sc == NULL)
	{
	  Edit_Script_With_Leto(gen, script);
	}
      AutoCreateComlink_by_send_group(gen, script);
      AutoCreateComlink_by_receive_group(gen, script);
    }
}

void	CompileAllScripts(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  t_gennet_script	*script = NULL;
  GtkWidget		*Notebook = NULL;

  Notebook = gen->gui->tabbedWin;

  for (script = gen->scripts; script != NULL; script = script->next)
    {
      if (script->sc == NULL)
	{
	  Edit_Script_With_Leto(gen, script);
	}
      gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),script->sc->num_onglet); 
      creation_cb(widget, script);
    }

  /* Retour a l onglet coeos */
  gtk_notebook_set_current_page(GTK_NOTEBOOK(Notebook),0); 
}

void	GenerateNetwork(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  GtkWidget		*e_path = NULL;
  gint			result = -1;

  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gen->gui->NewComlinkItem), FALSE);
  
  gen->gui->PathDialog = gtk_dialog_new_with_buttons("Generate PRT file",
								GTK_WINDOW(gen->gui->WinMain),
								GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
								GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
								GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

  e_path = gtk_entry_new_with_max_length(PATH_MAX - 1);
  SaveFile_control_browse(gen->gui->PathDialog, e_path, "path :");

  result = gtk_dialog_run(GTK_DIALOG(gen->gui->PathDialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      {
	char	*path = NULL;

	if ((path = (char *)gtk_entry_get_text(GTK_ENTRY(e_path))) != NULL)
	  {
	    if (strlen(path) > 0)
	      {
		FILE	*fprt = NULL;

		memcpy(gen->prt_path, path, strlen(path) + 1);
		if ((fprt = fopen(path, "w")) == NULL)
		  {
		    fprintf(stderr, "GenerateNetwork : Open file error\n");
		    break;
		  }
		writePrt(gen, fprt);
		fclose(fprt);
	      }
	  }
      }
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(gen->gui->PathDialog);
}

/*void	GenerateRunSh(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  GtkWidget		*e_path = NULL;
  gint			result = -1;
  char chaine[255];

  sprintf(chaine,"Generate run.sh file");

  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gen->gui->NewComlinkItem), FALSE);
  gen->gui->PathDialog = gtk_dialog_new_with_buttons(chaine,
								GTK_WINDOW(gen->gui->WinMain),
								GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
								GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
								GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

  e_path = gtk_entry_new_with_max_length(MAX_PATH_FILE_PRT - 1);
  SaveFile_control_browse(gen->gui->PathDialog, e_path, "path :");

  result = gtk_dialog_run(GTK_DIALOG(gen->gui->PathDialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      {
	char	*path = NULL;

	if ((path = (char *)gtk_entry_get_text(GTK_ENTRY(e_path))) != NULL)
	  {
	    if (strlen(path) > 0)
	      {
		FILE	*fprt = NULL;
		char cmd[255];
		memcpy(gen->prt_path, path, strlen(path) + 1);
		if ((fprt = fopen(path, "w+")) == NULL)
		  {
		    fprintf(stderr, "GenerateNetwork : Open file error\n");
		    break;
		  }
		writeRun(gen, fprt);
		fclose(fprt);
		sprintf(cmd,"chmod +x %s\n",path);
		system(cmd);
	      }
	  }
      }
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(gen->gui->PathDialog);
}*/

void	GenerateDeploySh(GtkWidget *widget, gpointer data)
{
  t_gennet		*gen = (t_gennet *)data;
  GtkWidget		*e_path = NULL;
  gint			result = -1;
  char chaine[255];

  sprintf(chaine,"Generate deploy.sh file");

  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gen->gui->NewComlinkItem), FALSE);
  gen->gui->PathDialog = gtk_dialog_new_with_buttons(chaine,
								GTK_WINDOW(gen->gui->WinMain),
								GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
								GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
								GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

  e_path = gtk_entry_new_with_max_length(PATH_MAX - 1);
  SaveFile_control_browse(gen->gui->PathDialog, e_path, "path :");

  result = gtk_dialog_run(GTK_DIALOG(gen->gui->PathDialog));

  switch(result)
    {
    case GTK_RESPONSE_ACCEPT :
      {
	char	*path = NULL;

	if ((path = (char *)gtk_entry_get_text(GTK_ENTRY(e_path))) != NULL)
	  {
	    if (strlen(path) > 0)
	      {
		FILE	*fprt = NULL;
		char cmd[255];
		memcpy(gen->prt_path, path, strlen(path) + 1);
		if ((fprt = fopen(path, "w+")) == NULL)
		  {
		    fprintf(stderr, "GenerateNetwork : Open file error\n");
		    break;
		  }
		writeDeploy(gen, fprt);
		fclose(fprt);
		sprintf(cmd,"chmod +x %s\n",path);
		system(cmd);
	      }
	  }
      }
      break;
    case GTK_RESPONSE_REJECT :
      break;
    default :
      break;
    }
  gtk_widget_destroy(gen->gui->PathDialog);
}

/*void		writeRun(t_gennet *data, FILE *frun)
{
  t_gennet_computer	*computer = NULL;
  t_gennet_script_list	*scriptlist = NULL;
  char			*val = NULL;
  
  printf("\nWriting run file .....");

  fprintf(frun,"#!/bin/bash\n");  
  fprintf(frun,"export IVYBUS=%s && ivyprobe '(.*)' &\nsleep 5 \n",data->promnet->ivybus_envvar);  
  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
      for (scriptlist = computer->scriptlist; scriptlist != NULL; scriptlist = scriptlist->next)
	{
	  val = promnet_computer_get_login(computer->computer);
	  fprintf(frun, "xterm -hold -e ssh %s", val);
	  if (val != NULL)
	    free(val);

	  val = promnet_computer_get_address(computer->computer);
	  fprintf(frun, "@%s ", val);
	  if (val != NULL)
	    free(val);
	
          fprintf(frun,"%s/launch_%s.sh & \n",promnet_prom_script_get_path_prom_deploy(scriptlist->script->prom_script),promnet_prom_script_get_logical_name(scriptlist->script->prom_script));
	}
    }
   
    fprintf(frun, "exit\n");
    
    printf(".................... completed !!\n\n");
    

}*/

void		writeDeploy(t_gennet *data, FILE *frun)
{
  t_gennet_computer	*computer = NULL;
  t_gennet_script_list	*scriptlist = NULL;
  char			*val = NULL; 
  char filename[256],chmod_cmd[256];
  int tmpi;
  FILE * f_launch=NULL;
  FILE * f_synchro=NULL;
  FILE * f_synchro_bin=NULL;
  printf("Creating deployment file ....");
  
/* Creation des synchonizer de binaire */
  f_synchro_bin=fopen("synchronize_binaries.sh","w+");
  if(f_synchro_bin==NULL)
  {
      printf("ouverture de synchronize_binaries.sh impossible\n");
      exit(-1);
  }    
  fprintf(f_synchro_bin,"#!/bin/sh\n\n");
 
  sprintf(chmod_cmd,"chmod +x %s\n",filename);
  
   system("mkdir -p .deployment\n");


  for (computer = data->computers; computer != NULL; computer = computer->next)
    {
	/*synchro des bin vers le pc concerned*/

	fprintf(f_synchro_bin,"scp %s/bin_leto_prom/* %s@%s:./bin_leto_prom/ \n",getenv("HOME"),promnet_computer_get_login(computer->computer),promnet_computer_get_address(computer->computer));
	
	/* Creation des synchonizer de simulateur pour chaque PC pour la synchronization des code des ordinateur */
	sprintf(filename,"synchronize_simulator_to_%s.sh",promnet_computer_get_name(computer->computer));
	f_synchro=fopen(filename,"w+");
	if(f_synchro==NULL)
	{
	    printf("ouverture de %s impossible\n",filename);
	    exit(-1);
	}    
	fprintf(f_synchro,"#!/bin/sh\n\n");
	fprintf(f_synchro,"rep_emb=%s\nrep_local=%s\nip_emb=%s\n\n",promnet_computer_get_path(computer->computer),getenv("PROM_HOME"),promnet_computer_get_address(computer->computer));
	fprintf(f_synchro,"cd $rep_local/prom_user/prom_user/user/ \n");
	fprintf(f_synchro,"rsync -avR --exclude '*.o' --exclude '*akefile*' . $ip_emb:$rep_emb/prom_user/prom_user/user/\n");

	fprintf(f_synchro,"cd $rep_local/coeos/coeos/\n");
	fprintf(f_synchro,"rsync -avR --exclude '*.o' --exclude '*akefile*'  . $ip_emb:$rep_emb/coeos/coeos/\n");

	fprintf(f_synchro,"cd $rep_local/hardware/hardware/\n");
	fprintf(f_synchro,"rsync -avR --exclude '*.o' --exclude '*akefile*' . $ip_emb:$rep_emb/hardware/hardware/\n");

	fprintf(f_synchro,"cd $rep_local/graphique/graphique/\n");
	fprintf(f_synchro,"rsync -avR --exclude '*.o' --exclude '*akefile*' . $ip_emb:$rep_emb/graphique/graphique\n");

	fprintf(f_synchro,"cd $rep_local/prom_kernel/prom_kernel/\n");
	fprintf(f_synchro,"rsync -avR --exclude '*.o' --exclude '*akefile*' . $ip_emb:$rep_emb/prom_kernel/prom_kernel/\n");

	fprintf(f_synchro,"cd $rep_local/basic_parser/basic_parser/\n");
	fprintf(f_synchro,"rsync -avR --exclude '*.o' --exclude '*akefile*' . $ip_emb:$rep_emb/basic_parser/basic_parser/\n");

	fprintf(f_synchro,"cd $rep_local/libcomm/libcomm/\n");
	fprintf(f_synchro,"rsync -avR --exclude '*.o' --exclude '*akefile*' . $ip_emb:$rep_emb/libcomm/libcomm/\n");
	fclose(f_synchro);

	sprintf(chmod_cmd,"chmod +x %s\n",filename);
	system(chmod_cmd);

	/*Creation des launcher pour chaque script associed a un computer*/
	for (scriptlist = computer->scriptlist; scriptlist != NULL; scriptlist = scriptlist->next)
	{
          val = promnet_prom_script_get_path_prom_deploy(scriptlist->script->prom_script);
	  if(strcmp(val,"")!=0)
	  {
	    free(val);
  
	    sprintf(filename,".deployment/launch_%s.sh",promnet_prom_script_get_logical_name(scriptlist->script->prom_script));
	    f_launch=fopen(filename,"w+");
	    if(f_launch==NULL)
	    {
		printf("ouverture de %s impossible\n",filename);
		exit(-1);
	    }
	    
	    fprintf(f_launch,"cd %s\n",promnet_prom_script_get_path_prom_deploy(scriptlist->script->prom_script));
	  
	    val = promnet_prom_script_get_path_prom_binary(scriptlist->script->prom_script);
	    fprintf(f_launch, "%s ", val);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_prom_args_line(scriptlist->script->prom_script);
	    fprintf(f_launch, "%s ", val);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_path_file_script_non_symb(scriptlist->script->prom_script);
	    tmpi=get_filename_from_path(val);
	    fprintf(f_launch, "%s ", val+tmpi);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_path_file_res(scriptlist->script->prom_script);
	    tmpi=get_filename_from_path(val);
	    fprintf(f_launch, "%s ", val+tmpi);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_path_file_config(scriptlist->script->prom_script);
	    tmpi=get_filename_from_path(val);
	    fprintf(f_launch, "%s ", val+tmpi);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_path_file_bus(scriptlist->script->prom_script);
	    tmpi=get_filename_from_path(val);
	    fprintf(f_launch, "%s ", val+tmpi);
	    if (val != NULL)
	      free(val);

	    val = promnet_prom_script_get_path_file_dev(scriptlist->script->prom_script);
	    tmpi=get_filename_from_path(val);
	    fprintf(f_launch, "%s ", val+tmpi);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_path_file_gcd(scriptlist->script->prom_script);
	    tmpi=get_filename_from_path(val);
	    fprintf(f_launch, "%s ", val+tmpi);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_path_file_prt(scriptlist->script->prom_script);
	    tmpi=get_filename_from_path(val);
	    fprintf(f_launch, "%s ", val+tmpi);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_logical_name(scriptlist->script->prom_script);
	    fprintf(f_launch, "-n %s\n", val);
	    if (val != NULL)
	      free(val);
	    
	    fclose(f_launch);
	    
            sprintf(chmod_cmd,"chmod +x %s",filename);
            system(chmod_cmd);
		
/*deploy launcher */	  
	    fprintf(frun, "scp -r %s ",filename);						
/*deploy .script_o*/
	    val = promnet_prom_script_get_path_file_script_non_symb(scriptlist->script->prom_script);
	    if(strcmp(val,"")!=0)
	    {
	      fprintf(frun, "%s ", val);
	      if (val != NULL)
		free(val);
	    }
/*deploy .res*/
	    val = promnet_prom_script_get_path_file_res(scriptlist->script->prom_script);
	    if(strcmp(val,"")!=0)
	    {
	      fprintf(frun, "%s ", val);
	      if (val != NULL)
		  free(val);
	    }
/*deploy .draw*/
	    val = promnet_prom_script_get_path_file_draw(scriptlist->script->prom_script);
	    if(strcmp(val,"")!=0)
	    {
	      fprintf(frun, "%s ", val);
	      if (val != NULL)
		  free(val);
	    }
/*deploy .config*/
	  val = promnet_prom_script_get_path_file_config(scriptlist->script->prom_script);
	  if(strcmp(val,"")!=0)
	  {
	    fprintf(frun, "%s ", val);
	    if (val != NULL)
	      free(val);
	  }
/*deploy .bus*/
	  val = promnet_prom_script_get_path_file_bus(scriptlist->script->prom_script);
	  if(strcmp(val,"")!=0)
	  {
	    fprintf(frun, "%s ", val);
	    if (val != NULL)
	      free(val);
	  }
	  }
/*deploy .dev*/
	  val = promnet_prom_script_get_path_file_dev(scriptlist->script->prom_script);
	  if(strcmp(val,"")!=0)
	  {
	    fprintf(frun, "%s ", val);
	    if (val != NULL)
	      free(val);
	  }	 
/*deploy .gcd*/
	  val = promnet_prom_script_get_path_file_gcd(scriptlist->script->prom_script);
	  if(strcmp(val,"")!=0)
	  {
	    fprintf(frun, "%s ", val);
	    if (val != NULL)
	      free(val);
	  } 
/*deploy .prt*/
	  val = promnet_prom_script_get_path_file_prt(scriptlist->script->prom_script);
	  if(strcmp(val,"")!=0)
	  {
	    fprintf(frun, "%s ", val);
	    if (val != NULL)
	      free(val);
	  }

/*deploy des .hw* et des mask*/
	    val = promnet_prom_script_get_logical_name(scriptlist->script->prom_script);;
	    fprintf(frun, "%s/*.hw* %s/*.mask %s/mask/ ", val,val,val);
	    if (val != NULL)
	      free(val);

/*destination du scp*/
	    val = promnet_computer_get_login(computer->computer);
	    fprintf(frun, "%s@", val);
	    if (val != NULL)
	      free(val);

	    val = promnet_computer_get_address(computer->computer);
	    fprintf(frun, "%s:", val);
	    if (val != NULL)
	      free(val);
	    
	    val = promnet_prom_script_get_path_prom_deploy(scriptlist->script->prom_script);
	    fprintf(frun, "%s \n", val);
	    if (val != NULL)
	      free(val);
	    fprintf(frun, "\n");  
	  }
	}

        fclose(f_synchro_bin);
	sprintf(chmod_cmd,"chmod +x synchronize_binaries.sh\n");
	system(chmod_cmd);

	printf(".................... completed !!\n\n");

}

void	init_gui_arg_find_cpt(t_gennet *data)
{
  int			i = 0;
  char			*ext = NULL;

  for (i = 1; i < data->ac; i++)
  {
    ext = get_file_extension(data->av[i]);
    if (strcmp(ext, "cpt") == 0)
      {
	loadNetwork_cpt(data,data->av[i]);
	debug_printf("loadNetwork_cpt done \n");
      }
  }
}

void	init_gui_arg_find_net(t_gennet *data)
{
  int			i = 0;
  char			*ext = NULL;

  for (i = 1; i < data->ac; i++)
  {
    ext = get_file_extension(data->av[i]);
    if (strcmp(ext, "net") == 0)
      {
	loadNetwork_net(data,data->av[i]);
	debug_printf("loadNetwork_net done \n");
      }
  }
}

void	init_gui_arg_find_symb(t_gennet *data)
{
  int			i = 0;
  char			*ext = NULL;
  char			nom[1024];
  char			*end_path = NULL;
  char			basepath[MAX_ALL_PATH];
  t_gennet_script	*pscript = NULL;

  for (i = 1; i < data->ac; i++)
  {
    ext = get_file_extension(data->av[i]);
    if (strcmp(ext, "symb") == 0)
      {
	   /* c'est un script donc on creer le pscript
	    * et on ouvre l'onglet */
	   pscript = add_gennet_script(data);
	   pscript->prom_script = promnet_add_new_prom_script(data->promnet);
	   if (pscript->prom_script != promnet_prom_script_get_next(data->promnet,NULL))
	      fprintf(stderr, "Probleme d'ajout du script %s a la liste des scripts\n", data->av[i]);
	     
	   /* on ajoute le nom logique du script */
	   memset(nom,0,sizeof(char) * 1024);
	   memcpy(nom,data->av[i],(strlen(data->av[i])+1) * sizeof(char));
	   get_base_name(nom);
	   promnet_prom_script_set_logical_name(pscript->prom_script, nom);
	   if ((end_path = rindex(data->av[i], '/')) != NULL)
	   {
	      memset(basepath, 0, MAX_ALL_PATH);
	      memcpy(basepath, data->av[i], end_path - data->av[i]);
	      /* on ajoute le chemin du script */
	      promnet_prom_script_set_all_path(pscript->prom_script, basepath, nom, ext);
	   }
	   else
	   {
	      promnet_prom_script_set_all_path(pscript->prom_script, ".", nom, ext);
/* 	      promnet_prom_script_set_path_file_script(pscript->prom_script, data->av[i]); */
	   }
	   pscript->pango = gtk_widget_create_pango_layout(GTK_WIDGET(data->gui->DrawingArea), nom);

	   /* on ouvre l'onglet */
	   Edit_Script_With_Leto(data,pscript);
      }
  }
}

void	init_gui_arg_find_script(t_gennet *data)
{
  int			i = 0;
  char			*ext = NULL;
  char			nom[1024];
  char			*end_path = NULL;
  char			basepath[MAX_ALL_PATH];
  t_gennet_script	*pscript = NULL;

  for (i = 1; i < data->ac; i++)
  {
    ext = get_file_extension(data->av[i]);
    if (strcmp(ext, "script") == 0)
      {
	   /* c'est un script donc on creer le pscript
	    * et on ouvre l'onglet */
	   pscript = add_gennet_script(data);
	   pscript->prom_script = promnet_add_new_prom_script(data->promnet);
	   if (pscript->prom_script != promnet_prom_script_get_next(data->promnet,NULL))
	      fprintf(stderr, "Probleme d'ajout du script %s a la liste des scripts\n", data->av[i]);
	     
	   /* on ajoute le nom logique du script */
	   memset(nom,0,sizeof(char) * 1024);
	   memcpy(nom,data->av[i],(strlen(data->av[i])+1) * sizeof(char));
	   get_base_name(nom);
	   promnet_prom_script_set_logical_name(pscript->prom_script, nom);
	   if ((end_path = rindex(data->av[i], '/')) != NULL)
	   {
	      memset(basepath, 0, MAX_ALL_PATH);
	      memcpy(basepath, data->av[i], end_path - data->av[i]);
	      /* on ajoute le chemin du script */
	      promnet_prom_script_set_all_path(pscript->prom_script, basepath, nom, ext);
	   }
	   else
	   {
	      promnet_prom_script_set_all_path(pscript->prom_script, ".", nom, ext);
/* 	      promnet_prom_script_set_path_file_script(pscript->prom_script, data->av[i]); */
	   }
	   pscript->pango = gtk_widget_create_pango_layout(GTK_WIDGET(data->gui->DrawingArea), nom);

	   /* on ouvre l'onglet */
	   Edit_Script_With_Leto(data,pscript);
      }
  }
}

/* fonction qui initialise les onglets et leurs contenues
 * de Metaleto en fonction des arguments donnees
 * en ligne de commande */
void init_gui_arg(t_gennet *data)
{
  int			i = 1;
  long			seed = -1;
  char			*tailptr = NULL;

  /* normalement cela ne devrait jamais se produire
   * mais on ne sait jamais */
  if(data == NULL)
    return;

  /* pour chaque argument
   * ( l'argument 0 etant "metaleto" )
   * on regarde de quel extension est l'argument
   * puis on le traite */
  for(i=1;i<data->ac;i++)
  {
     
     if (data->av[i][0] == '-') 
     {	  
	/* Use the next character to decide what to do. */
	  
	switch (data->av[i][1]) {
	     
	   case 'G':	
	      if (i < data->ac - 1)
	      {
		 seed = strtol(data->av[++i], &tailptr, 10);
		 if (strcmp(tailptr, "\0") == 0)
		    data->seed = seed;
		 else 
		    fprintf(stderr, "A number must be supplied after parameter -G (found %s)\n", data->av[i]);
	      }
	      else 
		 fprintf(stderr, "A number must be supplied after parameter -G\n");
	      break;
		
	   default:	fprintf(stderr, "Unknown parameter %s\n", data->av[i]);
	      break;
		
	}
     }
  }
  init_gui_arg_find_cpt(data);
  init_gui_arg_find_net(data);
  init_gui_arg_find_symb(data);
  init_gui_arg_find_script(data);
  refresh_all(data);
}



void		init_gui(t_gennet *data)
{

  gtk_init(&data->ac, &data->av);
  debug_printf("gtk_init done\n");
  init_gui_winmain(data);
  debug_printf("init_gui_winmain done\n");
  init_gui_toolbar(data);
  debug_printf("init_gui_toolbar done\n");
  init_gui_drawing(data);
  debug_printf("init_gui_drawing done\n");
  init_gui_arg(data);
  debug_printf("init_gui_arg done\n");

}

void		init_data(t_gennet *data)
{
  if ((data->gui = malloc(sizeof(t_gui))) == NULL)
    {
      perror("init_data : malloc");
      exit(-1);
    }
  memset(data->gui, 0, sizeof(t_gui));
  data->promnet = promnet_init();
  data->seed = -1;
}

#ifndef LETO
int	main(int ac, char **av)
{
  t_gennet	data;

  create_dir_point_leto(); /* creation du repertoire .leto s'il n'existe pas */
  memset(&data, 0, sizeof(t_gennet));

  data.ac = ac;
  data.av = av;

  init_data(&data);
  debug_printf("init data done\n");
  gtk_set_locale();
  setlocale(LC_NUMERIC, "C");
  gtk_disable_setlocale();  
  
  init_gui(&data);
  debug_printf("init gui done\n");


  gdk_threads_enter();

  gtk_main();

  gdk_threads_leave();
  
  free(data.promnet);

  return 0;
}
#endif
