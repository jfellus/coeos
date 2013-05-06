
/* gere l'affichage des commentaires, leur saisie et leur modification */


/*#define DEBUG 1*/

/* creation de la zone (sous fenetre avec ascenseur) pour afficher les commentaire a la fin du formulaire*/
#include "graphic_Tx.h"
#include "reseau.h"
#include "basic_tools.h"
#include "string.h"
#include "outils_script.h"
#include "group_selection.h"
#include "script.h"
#include "gennet.h"

GtkWidget *InsereCommentdansFormulaire(GtkWidget * fenetre, GtkWidget * table, TxChampsFormulaire * champs)
{
  GtkTextBuffer *buffer;
  GtkWidget *view1,*sw;
  GtkTextIter iter;
 
  champs->fenetre = fenetre;
  champs->table = table;
  champs->print_widget_entry = gtk_label_new(champs->nom_champs);

  gtk_widget_ref(champs->print_widget_entry);
  gtk_object_set_data_full(GTK_OBJECT(fenetre), champs->nom_champs, champs->print_widget_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(champs->print_widget_entry);

  gtk_table_attach(GTK_TABLE(table), champs->print_widget_entry, champs->x,
                     champs->x + 1, champs->y, champs->y + 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(champs->print_widget_entry), 0, 0.5);

  view1 = gtk_text_view_new ();
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (view1));
  /*champs->widget_entry =(GtkWidget *) buffer;*/
  champs->widget_entry =(GtkWidget *) view1;

      /* attention il faudrait reutiliser l'ancien buffer ou liberer le buffer ! */
  sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
				      GTK_POLICY_AUTOMATIC,
				      GTK_POLICY_AUTOMATIC);

  gtk_text_buffer_get_start_iter (buffer, &iter);
  gtk_text_buffer_insert (buffer, &iter, "Les commentaires doivent etre mis ici.\n", -1);

  gtk_widget_show(view1);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), view1);
  gtk_widget_show(sw);
  return sw ;

}


/* affichage des commentaires dans la sous fenetre buffer text*/
void affiche_comment(GtkTextBuffer *buffer, type_noeud_comment *comment)
{
   GtkTextIter iter;

   if (comment == NULL)
   {
      return;
   }
   
   gtk_text_buffer_get_line_count(buffer);

   gtk_text_buffer_set_text(buffer, comment->chaine, -1);

   gtk_text_buffer_get_end_iter(buffer, &iter);
   comment = comment->suiv;

   while (comment != NULL)
   {
      gtk_text_buffer_insert(buffer, &iter, comment->chaine, -1);
      comment = comment->suiv;
   }
}


/* Callback appele lors de la mise a jour de la fenetre */
void update_champs_comment(TxChampsFormulaire * champs, type_noeud_comment *comment)
{
   GtkTextBuffer *buffer;

   if (comment != NULL)
   {
      buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(champs->widget_entry));
      affiche_comment(buffer, comment);
   }
}




/* lors de la validation ou fermeture de la fenetre, il faut recopier le contenu du buffer dans
les champs commentaires (une ligne= 1 commentaire).
Un saut de ligne est rajoute a la fin si les commentaires ne se terminent pas par un saut de ligne.
Il faut tester que le texte commence bien par un caractere special associe aux commentaires sinon
on aura des pbs de relecture.
On libere les structures de commentaires devenues inutiles si on a globalement supprime des lignes
de commentaires.*/

type_noeud_comment *validate_comments(type_noeud_comment *anchor_comment, GtkWidget * entry)
{
  GtkTextIter start, end, iter;
  GtkTextBuffer *buffer;
  char *text;
  gboolean is_not_the_end;
  type_noeud_comment *pt_comment;
  int longueur;
  type_noeud_comment *pt_prec = NULL, *pt_fin = NULL;

  pt_comment = anchor_comment;

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry));

  gtk_text_buffer_get_line_count(buffer);

  gtk_text_buffer_get_bounds(buffer, &start, &end);

  iter = start;
  do
  {
     is_not_the_end = gtk_text_iter_forward_line(&iter);
     text = gtk_text_iter_get_text(&start, &iter);
     start = iter;

     if (pt_comment == NULL)
     {
	pt_comment = ALLOCATION(type_noeud_comment);
	pt_comment->suiv = NULL;
	if (anchor_comment == NULL)
	{
	   anchor_comment = pt_comment;
	}
	else
	{
	   pt_prec->suiv = pt_comment;
	}
     }

     longueur = strlen(text) + 1;
     if (longueur > TAILLE_CHAINE) 
     {
	printf("WARNING: la chaine de char: %s a ete tronquee dans validate_comments car sa taille est superieure a %d\n", text, TAILLE_CHAINE);
	longueur = TAILLE_CHAINE;
     }

     /* recopie dans le champs comment en rajoutant un % au debut s'il a ete oublie */
     /* on ne fait rien pour les lignes vides */
     if (test_comment(text) == 1 || empty_line(text) == 1)
     {
	memcpy(pt_comment->chaine, text, longueur * sizeof(char)); 
     }
     else
     {
	pt_comment->chaine[0] = '%';
	memcpy(&(pt_comment->chaine[1]), text, longueur * sizeof(char)); 
     }
     pt_prec = pt_comment;
     pt_comment = pt_comment->suiv;
 
     g_free (text);
  }
  while (is_not_the_end == TRUE);
  if (strlen(pt_prec->chaine) > 0 && pt_prec->chaine[strlen(pt_prec->chaine)-1] != '\n')
  {
     strcat(pt_prec->chaine, "\n");
  }

  /* Suppression des donnees du buffer */
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_delete(buffer, &start, &end);
  
  /* Suppression du reste de la liste chainee des commentaire ==> utile si on a supprime des commentaires*/
  pt_fin = pt_prec;
  while (pt_comment != NULL)
  {
     pt_prec = pt_comment;
     pt_comment = pt_comment->suiv;
     free(pt_prec);    
  }
  pt_fin->suiv = NULL;
  return anchor_comment;
}


void entry_group_callback_comment(GtkWidget * widget, GtkWidget * entry,  void* user_data)
{

  selected_group *sel_group;

  (void)widget;
  (void)user_data;


   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      sel_group->group->comment = validate_comments(sel_group->group->comment, entry);
   }
}


void entry_link_callback_comment(GtkWidget * widget, GtkWidget * entry, TxDonneesFenetre *onglet_leto)
{
  (void)widget;
  (void)onglet_leto;

   sc->liaison_courante->comment = validate_comments(sc->liaison_courante->comment, entry);
}
