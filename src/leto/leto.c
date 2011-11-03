/*------------------------------------------------------------*/
/*      CREATION D'UN SYSTEME DE RESEAUX DE NEURONES          */
/*                                                            */
/*  Cree un graphe comportant des groupes de neurones         */
/*  et le traduit sous forme de script pour la fabrication    */
/*  reelle du reseau par un cree_reseau.c                     */
/*                                                            */
/*  (c) Copyrights GAUSSIER Philippe    @1991, 92, 93, 94     */
/* sept 2001                                                  */
/* janv 2003 introduction de gtk a la place de xview          */
/* fevrier 2008 multi-leto (disparition des var globales      */
/*                                                            */
/* janvier 2009 ajout de l'affichage des commentaires         */

/*  pour SUNstations avec graphisme , menus simplifies        */
/*  ---> leto.c                                               */
/*                                                            */
/* tient compte de la memoire des liaisons                    */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*  cree un reseau a une couche, toute info dans les groupes  */
/*  nbre_entree et nbre_sortie =0 puisque sans importance     */
/*  type liaison  1 a 1 se fait modulo le nbre neurones entree*/
/*  creation d'un type entree et sortie                       */
/*  le reseau cree contient le nom du fichier script qui lui  */
/*  est associe de facon a ce que lors de l'execution on      */
/*  puisse retrouver les groupes d'entrees et de sortie avec  */
/*  leurs noms symboliques                                    */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/* changement important, on enregistre aussi le seuil pour    */
/*  chaque neurone. Utile pour les ET et OU du CMAC           */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/* le seuil n'est en general pas demande a l'utilisateur      */
/* sauf pour Hebb_Seuil qui est utilise pour reconstruire     */
/* entree fixe et entree variable : lors reconnaissance       */
/* ne tenir compte que de l'entree varible, reste inv a seuil */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/* fabrique le reseau et le script. Met au debut du fichier   */
/* reseau le nom du script de facon a ce que le prog.         */
/* utilisateur puisse retrouver le script et associer les     */
/* entree et sorties avec les noms choisi par prog            */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/* nouveau champs pour savoir si liaison secondaire ou non    */
/* uniquement stocke dans le script                           */
/*------------------------------------------------------------*/

/*#define DEBUG*/

#include "public_leto.h"
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>

#include "block_create.h"
#include "leto_global_var.h"
#include "creation.h"
#include <search.h>
#include "gere_coudes.h"
#include "manage_comments.h"

#include "gennet.h"




/*------------------------------------------------------------------*/
/* init par defaut du type de liens reliant macro et micro neurones */
/* doit toujours etre appeler au lancement de leto si on ne lit pas */
/*------------------------------------------------------------------*/

void cree_lien_micro_macro()
{
    type_liaison *liaison;

    sc->deb_liaison = sc->fin_liaison = creer_liaison((type_liaison *) 0);
    liaison = sc->fin_liaison;
    liaison->type = No_l_1_a;
    liaison->depart = liaison->arrivee = -1;    /* n'est connecte a aucun groupe en particulier ! */
    memcpy(liaison->depart_name,"-1", (strlen("-1")+1) * sizeof(char));
    memcpy(liaison->arrivee_name,"-1", (strlen("-1")+1) * sizeof(char));
    liaison->mode = 0;
    MY_FloatAffect(liaison->norme, 0.);
    MY_IntAffect(liaison->nbre, 0);
    sprintf(liaison->nom, "lien micro/macro");
    sc->nbre_liaison++;
}




/*------------------------------------------------------------------*/

void fonction_reverse(GtkWidget * widget, gpointer data)
{
   TxPoint point;
   selected_group *sel_group;
  
#ifndef LETO
   /* controle si on est dans un onglet Leto
    * ( l'onglet Metaleto etant le numero 0 )
    */
   if(tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto)==0) return;
#endif

   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      if (sel_group->group->reverse == 0)
      {
	 sel_group->group->reverse = 1;
      }
      else
      {
	 sel_group->group->reverse = -sel_group->group->reverse;
      }
      
      point.x = sel_group->group->posx;
      point.y = sel_group->group->posy; /* pas de changement de pos mais changement orientation */
      nouvelle_position_groupe(sel_group->group, point);      
   }
   regenerer_test(((t_gennet_script *) data)->onglet_leto);
}



/*-----------------------------------------------------------*/
/* selection des groupes a afficher a l'ecran                */
/*-----------------------------------------------------------*/

void init_display_planes()
{
    int i;
    for (i = 0; i < nb_max_planes; i++)
        sc->display_plane[i] = 1;
}


void select_plane(int plane,TxDonneesFenetre *onglet_leto)
{
    if (plane < 0)
        init_display_planes();
    else
    {
        if (plane >= nb_max_planes)
        {
	   fprintf(stderr, "ERROR the max of planes is %d (cannot deal with plane no %d)\n", nb_max_planes, plane);
	   exit(1);
        }
        sc->display_plane[plane] = -sc->display_plane[plane];
    }
    regenerer_test(onglet_leto);

}



/*---------------------------------------------------------------*/
/*                EFFACEMENT   DES LIAISONS                      */
/*---------------------------------------------------------------*/

void detruit_liaison(type_liaison * liaison,TxDonneesFenetre *onglet_leto)
{
    type_liaison *liaison1;

    detruit_coudes(liaison);

    /* on cherche la liaison precedente */
    for (liaison1 = sc->deb_liaison; liaison1 != NULL && liaison1->s != liaison; liaison1 = liaison1->s);

    /* si c'est le debut */
    if (liaison == sc->deb_liaison)
    {
        sc->deb_liaison = liaison->s;
    }

    /* si c'est la fin */
    if (liaison == sc->fin_liaison)
      {
	sc->fin_liaison = liaison1;
      }

    /* si c'est au milieu */
    if(liaison1 != NULL)
    {
       liaison1->s = liaison->s;
    }

    sc->nbre_liaison--;
    sc->liaison_courante = NULL;
    show_status(onglet_leto,"Link from %s to %s deleted", liaison->depart_name, liaison->arrivee_name);
    free(liaison);

    if (sc->modified == 0)
    {
       sc->modified = 1;
       set_title(onglet_leto);
    }
}

void create_link_callback(GtkWidget * widget, gpointer data)
{
#ifndef LETO
    /* controle si on est dans un onglet Leto
    * ( l'onglet Metaleto etant le numero 0 )
    */
    if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

    if (sc->flag_create_link == LINK_CREATE_NO_ACTION)
    {
        show_status(((t_gennet_script *) data)->onglet_leto,"Create algo link -> Select start group");
	sc->type_create_link = LINK_ALGO;       
        sc->flag_create_link = LINK_CREATE_CHOOSE_START;
    }
    else
    {
        sc->flag_create_link = LINK_CREATE_NO_ACTION;
        show_status(((t_gennet_script *) data)->onglet_leto,"Cancelled link creation");
    }
}


void create_one_to_one_link_callback(GtkWidget * widget, gpointer data)
{
#ifndef LETO
    /* controle si on est dans un onglet Leto
    * ( l'onglet Metaleto etant le numero 0 )
    */
    if (tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

    if (sc->flag_create_link == LINK_CREATE_NO_ACTION)
    {
        show_status(((t_gennet_script *) data)->onglet_leto,"Create one-to-one link -> Select start group");
	sc->type_create_link = LINK_ONE_TO_ONE;       
        sc->flag_create_link = LINK_CREATE_CHOOSE_START;
    }
    else
    {
        sc->flag_create_link = LINK_CREATE_NO_ACTION;
        show_status(((t_gennet_script *) data)->onglet_leto,"Cancelled link creation");
    }

}

/*---------------------------------------------------------------*/
/*                  efface un groupe                             */
/*---------------------------------------------------------------*/

/* gere la renumerotation des groupes / maintenant surtout important pour promethe */
void renumeroter()
{
    type_groupe *groupe = NULL;
    type_liaison *liaison = NULL;
    int n1 = 0;
    int n = 0;
    debug_printf("Renumber\n");
    groupe = sc->deb_groupe;
    while (groupe != NULL)
    {
        n1 = groupe->no;
        groupe->no = n;
        liaison = sc->deb_liaison;
        while (liaison != NULL)
        {
	  if (liaison->depart == n1)  { liaison->depart  = n; memcpy(liaison->depart_name,groupe->no_name, (strlen(groupe->no_name)+1) * sizeof(char));}
	  if (liaison->arrivee == n1) { liaison->arrivee = n; memcpy(liaison->arrivee_name,groupe->no_name, (strlen(groupe->no_name)+1) * sizeof(char));}
	  liaison = liaison->s;
        }
        groupe = groupe->s;
        n++;
    }
}

void automatic_group_rename(TxDonneesFenetre *onglet_leto)
{
   selected_group *sel_group;
   void *x;
   char temp_name[255] = "";
   int index = 0;

   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      do 
      {
	 sprintf(temp_name, "TEMP[%i]", index);	 
	 index++;
      } while (rename_group(sel_group->group, temp_name, onglet_leto) == 0);
   }

   index = 1;
   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      do
      {
	 sprintf(temp_name, "%i", index);	 
	 index++;
	 x = lookup_hash_table((void **)&(onglet_leto->hashtab), temp_name);
      } while (x != NULL);
      rename_group_and_associated_links(sel_group->group, temp_name, onglet_leto);
   }
   
   if (sc->modified == 0)
   {
      sc->modified = 1;
      set_title(onglet_leto);
   }
}


/*------------------------------------------------------------------*/
/* destruction de l'ensemble des groupes associes a un macro groupe */
/* tous les groupes qui ont le meme reverse en val abs (tous sur le meme plan) */
/* On espere que l'utilisateur n'a pas mis un autre groupe sur le meme plan sinon dommage pour lui ! */
/* Normalement avec le test que c'est bien un groupe inclus (deja_active) il y a peu de risques. */

void detruit_macro(type_groupe *groupe_cible,TxDonneesFenetre *onglet_leto)
{
  type_groupe *groupe;
  int plan_cible;

  if(groupe_cible==NULL)
    {
       fprintf(stderr, "ERROR: detruit_macro: rien a detuire le pointeur du groupe est NULL! \n");
       fprintf(stderr, "Ca ne devrait pas arriver je m'arrete (groupe = %d : %s )\n",groupe_cible->no,groupe_cible->no_name);
      exit(1);
    }
  plan_cible=abs(groupe_cible->reverse);
  groupe = sc->deb_groupe;
  while (groupe != NULL)
    {
      if(groupe!=groupe_cible && abs(groupe->reverse)==plan_cible && (groupe->deja_active==1))
	{
	  debug_printf("On demande la destruction du groupe %s appartenant au groupe %s \n",groupe->no_name,groupe_cible->no_name);
	  detruit_groupe(groupe, onglet_leto);
	  groupe=sc->deb_groupe; /* on ne sait plus ou on en est. On repart du debut. */
	}
      else groupe=groupe->s;	 
    }
}

/*------------------------------------------------------------------*/
void detruit_groupe(type_groupe *group, TxDonneesFenetre *onglet_leto)
{
  type_groupe *groupe1, *groupe2;
  type_liaison *liaison, *liaison2;
 
  debug_printf("detruit_groupe %s \n",group->no_name);
  if (group == NULL)
  {
     fprintf(stderr, "ERROR in detruit_groupe: group is NULL\n");
     exit(1);
  }

  if (group->type == No_Sub_Network)
  {
     detruit_macro(group,onglet_leto); /* on detruit les sous groupes de la macro puis le groupe lui meme*/
  }
  
  groupe1 = sc->deb_groupe;
  while (groupe1 != NULL && groupe1->s != group)
  {
     groupe1 = groupe1->s;
  }

  if (groupe1 == NULL)
  {
     fprintf(stderr, "Le groupe precedent No %s n'existe pas. Je ne peux pas le lier !!! \n", group->no_name);  
  }
  
  liaison = sc->deb_liaison;
  while (liaison != NULL)
    {
      liaison2 = liaison->s;
      if (strcmp(liaison->depart_name, group->no_name) == 0 || strcmp(liaison->arrivee_name, group->no_name) == 0)
      {
	 detruit_liaison(liaison, onglet_leto);
      }
      liaison = liaison2;
    }

  if (group == sc->fin_groupe)
  {
     sc->fin_groupe = groupe1;
     if (groupe1 != NULL)
     {
	groupe1->s = NULL;
     }
  }
  
  if (group == sc->deb_groupe)
  {
     sc->deb_groupe = sc->deb_groupe->s;
  }

  if(sc->fin_groupe != NULL && sc->deb_groupe != NULL && groupe1 != NULL && groupe1->s != NULL) /* c'est un groupe au milieu de la liste */
  {
     groupe2 = (groupe1->s)->s;
     if (groupe1 == NULL)
     {
	printf("Cas impossible detruit_groupe %s \n", group->no_name);
     }
     groupe1->s = groupe2;
  }
  memset(group, 0, sizeof(type_groupe));

  /*       supprimer toutes les liaisons et mettre a jour         */
  
  debug_printf("destroy the links\n");
  
  /*  renumeroter et recompter                                    */

  renumeroter(); /* toujours necessaire pour creation .res (meme avec les no_groupes symboliques)*/
  sc->nbre_groupe--;
  
  show_status(onglet_leto,"Group %s deleted", group->no_name);
  sc->modified = 1;
  set_title(onglet_leto);
  regenerer_test(onglet_leto);
}

/* detruit l'ensemble des groupes ayant ete selectionnes au prealable */
int detruit_ensemble_groupes(TxDonneesFenetre *onglet_leto)
{
   selected_group *sel_group = NULL;
   int groups_deleted = 0;

   debug_printf("Destruction ensemble de groupes \n");

   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      detruit_groupe(sel_group->group, onglet_leto);
      groups_deleted++;
   }

   reset_selection_groups();

   return groups_deleted;
}


/*------------------------------------------------------------------*/

void zoom_groups(float scale, TxDonneesFenetre *onglet_leto)
{
    int x, y, nbre_gpes;
    int xmin, xmax, ymin, ymax;
    int xoffset, yoffset;
    type_groupe *gpe;
    type_liaison *liaison = NULL;
    TxPoint point;
    selected_group *sel_group = NULL;
    int selected_planes[nb_max_planes];
    int planes_to_select = 0;

#ifndef LETO
    /* controle si on est dans un onglet Leto
    * ( l'onglet Metaleto etant le numero 0 )
    */
    if(tab_is_Metaleto(onglet_leto) == 0) return;
#endif

    memset(selected_planes, 0, sizeof(selected_planes));

    x = 0;                      /* translation vector */
    y = 0;
    nbre_gpes = 0;

    xmax = ymax = 0;
    xmin = ymin = 3000;

    if (sc->groupes_courants == NULL)
    {
      gpe = sc->deb_groupe;
      while (gpe != NULL)
      {
	x += gpe->posx;
	y += gpe->posy;
	nbre_gpes++;

	gpe = gpe->s;
      }
    }
    else
    {       
       for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
       {
	 x += sel_group->group->posx;
	 y += sel_group->group->posy;
	 nbre_gpes++;

	 if (abs(sel_group->group->reverse) >= 100)
	 {
	   planes_to_select = 1;
	   selected_planes[abs(sel_group->group->reverse)] = 1;
	 }
       }

       if (planes_to_select == 1)
       {
	  gpe = sc->deb_groupe;
	  while (gpe != NULL)
	  {
	    if (is_selected(gpe) == NULL && selected_planes[abs(gpe->reverse)] == 1)
	    {
	      x += gpe->posx;
	      y += gpe->posy;
	      nbre_gpes++;
	    }
	    gpe = gpe->s;
	  }    
       }
    }

    x = x / nbre_gpes;          /* position of the gravity center */
    y = y / nbre_gpes;

    gpe = sc->deb_groupe;
    while (gpe != NULL)
    {
      if (sc->groupes_courants == NULL || is_selected(gpe) || selected_planes[abs(gpe->reverse)] == 1)
      {
	gpe->posx = x + (gpe->posx - x) * scale;
	gpe->posy = y + (gpe->posy - y) * scale;
	if (gpe->posx < xmin)
	  xmin = gpe->posx;
	else if (gpe->posx > xmax)
	  xmax = gpe->posx;
	if (gpe->posy < ymin)
	  ymin = gpe->posy;
	else if (gpe->posy > ymax)
	  ymax = gpe->posy;
      }
      gpe = gpe->s;
    }

    point.x = xmin;
    point.y = ymin;

    xoffset = yoffset = 0;
    if (xmin < 60)
        xoffset = 60 - xmin;
    if (ymin < 40)
        yoffset = 40 - ymin;
    if (xoffset > 0 || yoffset > 0)
    {
        gpe = sc->deb_groupe;
        while (gpe != NULL)
        {
	  if (sc->groupes_courants == NULL || is_selected(gpe) || selected_planes[abs(gpe->reverse)] == 1)
	  {
            gpe->posx += xoffset;
            gpe->posy += yoffset;
	  }
	  
	  gpe = gpe->s;
        }
    }

    for (liaison = sc->deb_liaison; liaison != NULL; liaison = liaison->s)
    {
       rescale_link(liaison);
    }

    if (sc->modified == 0)
    {
       sc->modified = 1;
       set_title(onglet_leto);
    }

    regenerer_test(onglet_leto);

}

void zoom_in(GtkWidget * widget, gpointer data)
{
    zoom_groups(1.1, ((t_gennet_script *) data)->onglet_leto);
}

void zoom_out(GtkWidget * widget, gpointer data)
{
   zoom_groups(0.9, ((t_gennet_script *) data)->onglet_leto);
}


/*---------------------------------------------------------*/

void zoom(GtkWidget * widget, gpointer data)
{
    float scale;
    printf("scaling factor = ");
    scanf("%f", &scale);

    zoom_groups(scale, ((t_gennet_script *) data)->onglet_leto);
}


/*--------------------------------------------------*/

void creation_lien(char *no_groupe_depart_name, char *no_groupe_arrivee_name, TxDonneesFenetre *onglet_leto, int link_type)
{
    type_liaison *liaison;
    type_groupe *groupe1, *groupe2;
    int ret;

    printf("sc->deb_liaison : %p\n",(void*)sc->deb_liaison);

    debug_printf("creation_lien %s-> %s  nbre_liaison=%d \n",no_groupe_depart_name, no_groupe_arrivee_name, sc->nbre_liaison);

    if (sc->deb_liaison == NULL)  /* (nbre_liaison == 0) modif PG*/
    {
       debug_printf("creation de la premiere liaison \n");
       sc->deb_liaison = sc->fin_liaison = creer_liaison(NULL);
    }
    else
    {
        sc->fin_liaison = creer_liaison(sc->fin_liaison);
    }
    sc->liaison_courante = liaison = sc->fin_liaison;
    debug_printf("creation_lien \n");

    groupe1 = trouver_groupe_par_nom(no_groupe_depart_name,onglet_leto);
    
    memcpy(liaison->depart_name,groupe1->no_name,(strlen(groupe1->no_name)+1) * sizeof(char));
    liaison->depart = groupe1->no;

    groupe2 = trouver_groupe_par_nom(no_groupe_arrivee_name,onglet_leto);

    /* si on clique n'importe ou alors on prend le premier groupe */
    if (groupe2 == NULL) groupe2 = groupe1;

    memcpy(liaison->arrivee_name,groupe2->no_name,(strlen(groupe2->no_name)+1) * sizeof(char));
    liaison->arrivee = groupe2->no;

    if (link_type == LINK_ONE_TO_ONE)
    {
       liaison->type = No_l_1_1_non_modif;
       MY_FloatAffect(liaison->norme, 1.0);
    }
    else
    {
       liaison->type = No_l_algorithmique;
       MY_FloatAffect(liaison->norme, 0.1);
    }
       
    liaison->posx1 = groupe1->posx;
    liaison->posy1 = groupe1->posy;
    liaison->posx2 = groupe2->posx;
    liaison->posy2 = groupe2->posy;

    MY_Data_Copy(liaison->nbre, groupe1->nbre);
    MY_FloatAffect(liaison->temps, 0.);
    MY_FloatAffect(liaison->stemps, 0.);
    MY_FloatAffect(liaison->proba, 1.);
    MY_FloatAffect(liaison->dv_x, 0.);
    MY_FloatAffect(liaison->dv_y, 0.);
    liaison->style = 0; /* des segments par defaut */
    memcpy(liaison->nom, "???", (strlen("???")+1) * sizeof(char));

    initialise_coudes_liaison(liaison);
    affiche_liaison(liaison, groupe1, groupe2, onglet_leto);
    sc->nbre_liaison++;

    ret = formulaire_lien(liaison, onglet_leto);

    if (ret == GTK_RESPONSE_REJECT)
    {
       detruit_liaison(liaison, onglet_leto);
    }
    regenerer_test(onglet_leto);

    debug_printf("fin afficher liaison\n");
}


/*
 * fonction gere_modification_lien permet de determiner comment creer une liaison, si oui
 * ou non une liaison est selectionnee et si on veut la modifier
 */

int gere_modification_lien(TxPoint point,TxDonneesFenetre *onglet_leto)
{
   type_groupe *group;
   
   debug_printf("link management flag = %i\n", sc->flag_create_link);
   switch (sc->flag_create_link)
   {
      case LINK_CREATE_CHOOSE_START: /* Select start group to modify/create link */
	 if ((group = test_group_position(sc->point_courant_leto)) != NULL)
	 {
	    memcpy(sc->groupe_depart_name, group->no_name, (strlen(group->no_name)+1) * sizeof(char));
	    select_group(group);
	    sc->flag_create_link = LINK_CREATE_CHOOSE_END;
	    return 1;
	 }
	 return 0;
	 
      case LINK_CREATE_CHOOSE_END: /* Select end group to modify/create link */
	 if ((group = test_group_position(sc->point_courant_leto)) != NULL)
	 {
	    memcpy(sc->groupe_arrivee_name, group->no_name, (strlen(group->no_name)+1) * sizeof(char));	    
	    creation_lien(sc->groupe_depart_name, sc->groupe_arrivee_name, onglet_leto, sc->type_create_link);
	    sc->flag_create_link = LINK_CREATE_NO_ACTION;
	    return 1;
	 }
	 return 0;
	 
      default:
	 sc->flag_create_link = LINK_CREATE_NO_ACTION;
	 return 0;
   }
}

/*-------------------------------------------------------------*/

void creation_groupe(GtkWidget * widget, gpointer data)
{
   type_groupe *groupe2 = NULL;
   int	res = -1, nbre, selected_plane, reverse, exists;
   TxDonneesFenetre *onglet_leto = NULL;
   char base_nom[256], base_nom_complet[256];
   void *x;

#ifndef LETO
   /* controle si on est dans un onglet Leto
    * ( l'onglet Metaleto etant le numero 0 )
    */
   if(tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto) == 0) return;
#endif

   onglet_leto = ((t_gennet_script *)data)->onglet_leto;

   groupe2 = creer_groupeb(NULL);
   if(sc->fin_groupe != NULL)
   {
      sc->fin_groupe->s = groupe2;
   }
   sc->fin_groupe = groupe2;
   
   if(sc->deb_groupe == NULL)
   {
      sc->deb_groupe = sc->fin_groupe;
   }
   sc->nbre_groupe++;

   do
   {
      sprintf(groupe2->no_name, "%d", groupe2->no); /* par defaut le nom symbolique est le numero ordinal de creation du gpe */
      x = lookup_hash_table((void **)&(onglet_leto->hashtab), groupe2->no_name);
      if (x != NULL)
	 groupe2->no = ++sc->last_groupe_number;
   } while (x != NULL);

   groupe2->posx = sc->point_courant_leto.x;
   groupe2->posy = sc->point_courant_leto.y;
   groupe2->debug = 1;
   select_group(groupe2);

   res = formulaire_groupe(((t_gennet_script *) data)->onglet_leto);

   if (res == GTK_RESPONSE_ACCEPT)
   {	      
      if (groupe2->type == No_Sub_Network)
      {
	 memcpy(base_nom, groupe2->nom, (strlen(groupe2->nom) + 1) * sizeof(char));

	 get_base_name(base_nom);
	 
	 sprintf(base_nom_complet, "%s[%d]", base_nom, sc->nbre_macro_lues);
	 memcpy(groupe2->no_name, base_nom_complet,(strlen(base_nom_complet) + 1) * sizeof(char));
	 
	 debug_printf("%s nouveau symbole \n", groupe2->no_name);
	 debug_printf("no provisoire = %d \n", groupe2->no);

	 sc->nbre_groupes_lus = sc->last_groupe_number + 1;
	 debug_printf("Lancement de la lecture de la macro %s \n", base_nom);
	 nbre = read_macro(groupe2->no_name, groupe2->nom, groupe2->posx, groupe2->posy, 1, 1, &selected_plane, onglet_leto);
	 /* le nbre de groupes a ete modifie dans read_macro */
	 
	 if (groupe2->reverse > 0)
	 {
	    reverse = 1;
	 }
	 else
	 {
	    reverse = -1;
	 }
	 groupe2->reverse = reverse * selected_plane;

	 debug_printf("nombre de groupes lus = %d\n", nbre);
	 
	 sc->last_groupe_number = sc->nbre_groupes_lus - 1;
	 
	 debug_printf("\n\nListe des groupes : \n\n"); 
	 affiche_liste_groupes();
      } /* fin gestion No_Sub_Network */
      else
      {
	 sc->flag_drag_group = 1;
      }


      if (onglet_leto != NULL)
      {
	 exists = create_new_entry_in_hash_table(groupe2, (void **)&onglet_leto->hashtab);
      }
      else
      {
	 exists = create_new_entry_in_hash_table(groupe2, NULL);
      }
      
      if(exists < 0)
      { 
	 fprintf(stderr, "Error: Ce groupe existe deja dans la table. \n"); 
	 exit(1);
      }
   }
   else if (res == GTK_RESPONSE_REJECT)
   {
      reset_selection_groups();
      detruit_groupe(groupe2, onglet_leto);  
   }
   regenerer_test(onglet_leto);    
}



static void target_drag_data_received(GtkWidget * widget, GdkDragContext * context,
                          gint x, gint y, GtkSelectionData * data, guint info, guint time)
{
    g_print("Got: %s\n", data->data);
}



static void source_drag_data_get(GtkWidget * widget, GdkDragContext * context,
                                 GtkSelectionData * selection_data, guint info, guint time, gpointer data)
{
    unsigned char string[] = "Some String!";
    gtk_selection_data_set(selection_data, selection_data->target, 8, string, sizeof(string));
}

static GtkTargetEntry target_table[1];


void fill_link_dialog(type_liaison * link, int type)
{
   int i,updated;
   int *editable;
   char *entry_text;
   type_groupe *groupe;
   mode_lien mode_link;

   if (type >=0)
   {
      editable = lien_no_nom_type_link[type].editable;
   }
   else
   {
      editable = lien_no_nom_type_link[link->type].editable;
   }

   for (i = 0; i < Number_of_link_items; i++)
   {
      switch (i)
      {
	 case No_item_link_depart:
	    entry_text = link->depart_name;
	    break;
	 case No_item_link_arrivee:
	    entry_text = link->arrivee_name;
	    break;
	 case No_item_link_temps:
	    entry_text = MY_Float2Str(link->temps);
	    break;
	 case No_item_link_stemps:
	    entry_text = MY_Float2Str(link->stemps);
	    break;
	 case No_item_link_nbre:
	    entry_text = MY_Int2Str(link->nbre);
	    break;
	 case No_item_link_secondaire:
	    entry_text = Int2Str(link->secondaire);
	    break;
	 case No_item_link_style:
	    entry_text = Int2Str(link->style);
	    break;
	 case No_item_link_mode:
	    entry_text = Int2Str(link->mode);
	    break;
	 case No_item_link_norme:
	    entry_text = MY_Float2Str(link->norme);
	    break;
	 case No_item_link_dvx:
	    entry_text = MY_Float2Str(link->dv_x);
	    break;
	 case No_item_link_dvy:
	    entry_text = MY_Float2Str(link->dv_y);
	    break;
	 case No_item_link_proba:
	    entry_text = MY_Float2Str(link->proba);
	    break;
	 case No_item_link_name:
	    entry_text = link->nom;
	    break;
      }

      if (type < 0 || 
	  (gtk_editable_get_editable(GTK_EDITABLE(formulaire_link[i].widget_entry)) == FALSE && editable[i] == 1) ||
	  (gtk_editable_get_editable(GTK_EDITABLE(formulaire_link[i].widget_entry)) == TRUE && editable[i] != 1))
      {
	 TxUpdateChampsdansFormulaire(&formulaire_link[i], entry_text, editable[i]);
      }
   }

   if (type < 0)
   {
      update_champs_comment(&formulaire_link[No_item_link_comment], link->comment);
      
      gtk_entry_set_text(GTK_ENTRY(combo_type_link_entry), lien_no_nom_type_link[link->type].nom);

      /** get groupe out type */
      groupe=get_groupOut(link);

      updated=0;
      for(i=0; i<group_mode_link_tab[groupe->type].nb_mode && !updated;i++) {
	 mode_link=group_mode_link_tab[groupe->type].mode_tab[i];
	 if (link->mode == mode_link.type_lien.no)
	 {
	    gtk_entry_set_text(GTK_ENTRY(combo_mode_link_entry), mode_link.type_lien.nom);
	    updated=1;
	 }
      }
      if(!updated)
      {
	 TxUpdateChampsdansFormulaire(&formulaire_link[No_item_link_mode], Int2Str(link->mode), editable[No_item_link_mode]);
	 gtk_entry_set_text(GTK_ENTRY(combo_mode_link_entry), "unknown for this group");
      }
   }
}

int formulaire_lien(type_liaison * link,TxDonneesFenetre *onglet_leto)
{
   static GtkWidget *link_dialog = NULL;
   gint result;

   link_dialog = create_read_link(onglet_leto,link);

   fill_link_dialog(link, -1);

   gtk_dialog_set_default_response(GTK_DIALOG(link_dialog), GTK_RESPONSE_ACCEPT);
   gtk_widget_show(link_dialog);

   result = gtk_dialog_run(GTK_DIALOG(link_dialog));
   switch (result)
   {
      case GTK_RESPONSE_ACCEPT:
	 link_validate_button_callback(link_dialog, onglet_leto);
	 show_status(onglet_leto,"Link between %s and %s modified", link->depart_name, link->arrivee_name);
	 gtk_widget_destroy(link_dialog);
	 return GTK_RESPONSE_ACCEPT;

      case GTK_RESPONSE_REJECT:
	 link_cancel_button_callback(link_dialog, onglet_leto);
	 show_status(onglet_leto,"Cancelled link modification");
	 gtk_widget_destroy(link_dialog);
	 return GTK_RESPONSE_REJECT;

      case GTK_RESPONSE_APPLY:
	 link_validate_button_callback(link_dialog, onglet_leto);
	 show_status(onglet_leto,"Link between %s and %s modified", link->depart_name, link->arrivee_name);
	 gtk_widget_destroy(link_dialog);
	 return formulaire_lien(link,onglet_leto);

      default:
	 show_status(onglet_leto,"");
	 gtk_widget_destroy(link_dialog);
	 return GTK_RESPONSE_ACCEPT;
   }
}


void fill_group_dialog(int type)
{
   int i;
   type_groupe *group;
   int *editable;
   char *entry_text;

   if (sc->groupes_courants == NULL)
   {
      fprintf(stderr, "WARNING in fill_group_dialog: no group selected\n");
      return;
   }
    
   if (sc->groupes_courants->next == NULL)
   {
      group = sc->groupes_courants->group;

      if (type >= 0)
      {
	 editable = lien_no_nom_type_groupe[type].editable;
      }
      else
      {
	 editable = lien_no_nom_type_groupe[group->type].editable;
      }

      for (i = 0; i < Number_of_items; i++)
      {
	 switch (i)
	 {
	    case No_item_groupe:
	       entry_text = group->no_name;
	       break;
	    case No_item_nom:
	       entry_text = group->nom;
	       break;
	    case No_item_taillex:
	       entry_text = MY_Int2Str(group->taillex);
	       break;
	    case No_item_tailley:
	       entry_text = MY_Int2Str(group->tailley);
	       break;
	    case No_item_nbre:
	       entry_text = MY_Int2Str(group->nbre);
	       break;
	    case No_item_seuil:
	       entry_text = MY_Float2Str(group->seuil);
	       break;
	    case No_item_learning_rate:
	       entry_text = MY_Float2Str(group->learning_rate);
	       break;
	    case No_item_simulation_speed:
	       entry_text = MY_Float2Str(group->simulation_speed);
	       break;
	    case No_item_type2:
	       entry_text = MY_Int2Str(group->type2);
	       break;
	    case No_item_ech_temps:
	       entry_text = Int2Str(group->ech_temps);
	       break;
	    case No_item_noise_level:
	       entry_text = MY_Float2Str(group->noise_level);
	       break;
	    case No_item_tolerance:
	       entry_text = MY_Float2Str(group->tolerance);
	       break;
	    case No_item_alpha:
	       entry_text = MY_Float2Str(group->alpha);
	       break;
	    case No_item_nbre_de_1:
	       entry_text = MY_Float2Str(group->nbre_de_1);
	       break;
	    case No_item_dvp:
	       entry_text = MY_Int2Str(group->dvp);
	       break;
	    case No_item_dvn:
	       entry_text = MY_Int2Str(group->dvn);
	       break;
	    case No_item_sigma_f:
	       entry_text = MY_Float2Str(group->sigma_f);
	       break;
	    case No_item_plan:
	       entry_text = Int2Str(abs(group->reverse));
	       break;
	 }

	 if (type < 0 || 
	     (gtk_editable_get_editable(GTK_EDITABLE(formulaire_group[i].widget_entry)) == FALSE && editable[i] == 1) ||
	     (gtk_editable_get_editable(GTK_EDITABLE(formulaire_group[i].widget_entry)) == TRUE && editable[i] != 1))
	 {	     
	    TxUpdateChampsdansFormulaire(&formulaire_group[i], entry_text, editable[i]);
	 }
      }

      if (type < 0)
      {
	 update_champs_comment(&formulaire_group[No_item_comment], group->comment);
       
	 gtk_entry_set_text(GTK_ENTRY(combo_type_groupe_entry), lien_no_nom_type_groupe[group->type].nom);
       
	 switch(group->debug)
	 {
	    case 0:
	       gtk_entry_set_text(GTK_ENTRY(combo_debug_groupe), "Debug group off");
	       break;
	    case 1: 
	       gtk_entry_set_text(GTK_ENTRY(combo_debug_groupe), "Debug group on"); 
	       break;
	    case 2:
	       gtk_entry_set_text(GTK_ENTRY(combo_debug_groupe), "Debug group normalized"); 
	       break;
	    default: 
	       fprintf(stderr, "WARNING : Debug number in group %s (%d) unknown : %d", group->nom, group->no, group->debug);
	       break;
	 }
       
	 if (group->reverse <= 0)
	 {
	    gtk_entry_set_text(GTK_ENTRY(combo_reverse_groupe), "Reverse group off"); 
	 }
	 else
	 {
	    gtk_entry_set_text(GTK_ENTRY(combo_reverse_groupe), "Reverse group on"); 
	 }
      }
   }
   else
   {
      selected_group *sel_group = NULL;
      int plane = -1, ech_temps = -1, type2 = -1;
      char plane_text[256], ech_temps_text[256], type2_text[256], taillex_text[256], tailley_text[256];
      int first = 1;
      int field_editable;

      for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
      {
	 group = sel_group->group;
	 if (first == 1)
	 {
	    plane = abs(group->reverse);
	    sprintf(plane_text, "%i", plane);
	    ech_temps = group->ech_temps;
	    sprintf(ech_temps_text, "%i", ech_temps);
	    type2 = MY_Int2Int(group->type2);
	    sprintf(type2_text, "%i", type2);
	    sprintf(taillex_text, "%s", MY_Int2Str(group->taillex));
	    sprintf(tailley_text, "%s", MY_Int2Str(group->tailley));
	    first = 0;
	 }
	 else
	 {
	    if (plane != abs(group->reverse))
	    {
	       strcpy(plane_text, "X");
	    }
	    if (ech_temps != group->ech_temps)
	    {
	       strcpy(ech_temps_text, "X");
	    }
	    if (type2 != MY_Int2Int(group->type2))
	    {
	       strcpy(type2_text, "X");
	    }
	    if (strcmp(taillex_text, MY_Int2Str(group->taillex)) != 0)
	    {
	       strcpy(taillex_text, "X");
	    }
	    if (strcmp(tailley_text, MY_Int2Str(group->tailley)) != 0)
	    {
	       strcpy(tailley_text, "X");
	    }
	 }
      }
      
      for (i = 0; i < Number_of_items; i++)
      {
	 field_editable = 1;
	 switch (i)
	 {
	    case No_item_type2:
	       entry_text = type2_text;
	       break;
	    case No_item_ech_temps:
	       entry_text = ech_temps_text;
	       break;
	    case No_item_plan:
	       entry_text = plane_text;
	       break;
	    case No_item_taillex:
	       entry_text = taillex_text;
	       break;
	    case No_item_tailley:
	       entry_text = tailley_text;
	       break;
	    default:
	       field_editable = 0;
	       entry_text = "X";
	       break;
	 }

	 TxUpdateChampsdansFormulaire(&formulaire_group[i], entry_text, field_editable);
      }

      gtk_text_view_set_editable(GTK_TEXT_VIEW(formulaire_group[No_item_comment].widget_entry), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(formulaire_group[No_item_comment].widget_entry), FALSE);
      
      gtk_entry_set_text(GTK_ENTRY(combo_type_groupe_entry), "X");
      gtk_editable_set_editable(GTK_EDITABLE(combo_type_groupe_entry), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(combo_type_groupe_entry), FALSE);

      gtk_entry_set_text(GTK_ENTRY(combo_debug_groupe), "X");
      gtk_editable_set_editable(GTK_EDITABLE(combo_debug_groupe), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(combo_debug_groupe), FALSE);

      gtk_entry_set_text(GTK_ENTRY(combo_reverse_groupe), "X");
      gtk_editable_set_editable(GTK_EDITABLE(combo_reverse_groupe), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(combo_reverse_groupe), FALSE);
   }
}

/* formulaire affiche pour modifier les parametres d'un groupe */

int formulaire_groupe(TxDonneesFenetre *onglet_leto)
{
    static GtkWidget *group_dialog = NULL;
    gint result;
    
    group_dialog = create_read_group(onglet_leto);
    fill_group_dialog(-1);
 
    gtk_dialog_set_default_response(GTK_DIALOG(group_dialog), GTK_RESPONSE_ACCEPT);
    gtk_widget_show(group_dialog);

    result = gtk_dialog_run(GTK_DIALOG(group_dialog));
    switch (result)
    {
    case GTK_RESPONSE_REJECT:
      group_cancel_button_callback(group_dialog, onglet_leto);
      show_status(onglet_leto,"Cancelled group modification");
      gtk_widget_destroy(group_dialog);
      return GTK_RESPONSE_REJECT;

    case GTK_RESPONSE_APPLY:
      group_validate_button_callback(group_dialog, onglet_leto);
      gtk_widget_destroy(group_dialog);
      show_status(onglet_leto,"Group(s) modified");
      return formulaire_groupe(onglet_leto);

    case GTK_RESPONSE_ACCEPT:    
      group_validate_button_callback(group_dialog, onglet_leto);
      show_status(onglet_leto,"Group(s) modified");
      gtk_widget_destroy(group_dialog);
      return GTK_RESPONSE_ACCEPT;

    default:
      show_status(onglet_leto,"");
      gtk_widget_destroy(group_dialog);
      return GTK_RESPONSE_ACCEPT;
    }
}


void init_target_table()
{
    target_table[0].target = "plain";
    target_table[0].flags = 0;
    target_table[0].info = 0;
}

/* fonction de lancement de leto via metaleto */
void run_leto(char *fichier_script, char *fichier_draw, char *fichier_res, char *fichier_var, TxDonneesFenetre *onglet_leto,int idx,int numPage, long seed)
{
#ifndef LETO
  char *ext;
#endif

  les_scripts[idx].num_onglet=numPage;
  sc=&les_scripts[idx];

/****************** INITIALISATION ***********************************************/

    if (seed < 0)
    {
       seed = time(NULL);
    }
    srand48(seed);
    sc->seed = seed;
    
    if (sc->draw == NULL || strlen(sc->draw) == 0)
           sc->draw[0]='\0';

    init_global_var_un_script(sc);
    cree_lien_micro_macro();    /* pour pouvoir avoir dans le script les info
                                 * sur les liens dans le macro neurone */
    init_display_planes();
    init_target_table();

    /* initialise la table de hachage d'un onglet */
    remove_hash_table((void **)&onglet_leto->hashtab);
    init_hash_table((void **)&onglet_leto->hashtab);

/********************************************************************************/

#ifndef LETO
    /* load the script file */
    ext = get_file_extension(fichier_script);
    if (strcmp(ext, "script") != 0 && strcmp(ext, "symb") != 0 )
      fprintf(stderr, "WARNING: file name extension "
	      "should be '.script' or '.symb'\n");
    if (!strcmp(ext, ".symb"))
      sc->flag_symb = 1;
    else if (!strcmp(ext, ".script"))
      sc->flag_symb = 0;

    memcpy(sc->nomfich1, fichier_script, (strlen(fichier_script) + 1) * sizeof(char));
    
	/* load the draw file */
    ext = get_file_extension(fichier_draw);
    if (strcmp(ext, "draw") != 0)
      fprintf(stderr, "WARNING: file name extension "
	      "should be '.draw'\n");
    memcpy(sc->draw, fichier_draw, (strlen(fichier_draw) + 1) * sizeof(char));

    /* load the res file */
    if (fichier_res != NULL && strlen(fichier_res) > 0)
    {
       ext = get_file_extension(fichier_res);
       if (strcmp(ext, "res") != 0)
	  fprintf(stderr, "WARNING: file name extension should be '.res'\n");
       memcpy(sc->freseau, fichier_res, (strlen(fichier_res) + 1) * sizeof(char));
    }

    /* load the var file */
    memcpy(sc->fvar, fichier_var, (strlen(fichier_var) + 1) * sizeof(char));
#endif
    /* lecture recursive du script par defaut, definie dans script.c */
    lecture(1,onglet_leto);

    debug_printf("res file     = %s\n", sc->freseau);
    debug_printf("draw file    = %s\n", sc->draw);

    gtk_drag_dest_set(onglet_leto->window, GTK_DEST_DEFAULT_ALL,
                      target_table, 1, GDK_ACTION_COPY);
    
    gtk_signal_connect(GTK_OBJECT(onglet_leto->window), "drag_data_received",
                       GTK_SIGNAL_FUNC(target_drag_data_received), onglet_leto);

    gtk_signal_connect(GTK_OBJECT(onglet_leto->window), "drag_data_get",
                       GTK_SIGNAL_FUNC(source_drag_data_get), onglet_leto);

    onglet_leto->cr=NULL;


}

/* fonction pour creer le repertoire a la racine .leto s'il n'existe pas */
/* ce repertoire sert pour echanger des fichier copier/coller entre different letos */
/* Pour l'instant le fichier copy_buffer.script */
/* On pourrait l'utiliser pour stocker des preferences liees a l'IHM */

void create_dir_point_leto()
{
   char global_dir_leto[1024];
   int error;

   sprintf(global_dir_leto, "%s/.leto", getenv("HOME"));

   error = mkdir(global_dir_leto, S_IRWXU);
   if (error != 0)
   {
      if (errno == EEXIST)
      {
	 debug_printf(".leto existe deja. Rien a faire. \n");
      }
      else
      {
	 fprintf(stderr, "Error, impossible to create %s dir \n", global_dir_leto);
	 exit(1);
      }
   }
}

#ifdef LETO
/*-----------------MAIN LETO---------------------*/
int main(int argc, char *argv[])
{
	t_gennet_script	*pscript;
	GtkWidget	*parent = NULL;
	GtkWidget	*vbox;
	GtkWidget	*vbox_menu;
	int             i;
	long		seed = -1, seed_tmp;
	char            *ext;
        char		*tailptr;

	create_dir_point_leto();

	gtk_set_locale();
	setlocale(LC_NUMERIC, "C");
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);



/********************************* Creation de la fenetre ***********************************************************/

	if ((pscript = malloc(sizeof(t_gennet_script))) == NULL)
	{
		g_critical("Leto : Memory error");
		/* fermer ce qu'il faut */
		return 0;			
	}
	memset(pscript, 0, sizeof(t_gennet_script));
	if ((pscript->onglet_leto = malloc(sizeof(TxDonneesFenetre))) == NULL)
	{
		g_critical("Leto : Memory error");
		/* fermer ce qu'il faut */
		return 0;			
	}
	memset(pscript->onglet_leto, 0, sizeof(TxDonneesFenetre));
	pscript->onglet_leto->window = gtk_window_new(GTK_WINDOW_TOPLEVEL); /* creation de la fenetre principale */

	/* dimensionnement de la fenetre */
	gtk_window_set_default_size(GTK_WINDOW(pscript->onglet_leto->window), 
                                gdk_screen_width() - 300,
                                gdk_screen_height() - 300);

	/* defini le titre de la fenetre */
	gtk_window_set_title(GTK_WINDOW(pscript->onglet_leto->window), "Leto");

	sc=&les_scripts[0];	
	sc->xmin = 0;
	sc->ymin = 0;
	gtk_window_get_size(GTK_WINDOW(pscript->onglet_leto->window), &sc->xmax, &sc->ymax);

	/* creation d'une box vertical */
	vbox = gtk_vbox_new(FALSE, 1);
	gtk_container_add(GTK_CONTAINER(pscript->onglet_leto->window), vbox);

	/* creation d'une box vertical pour le menu */
	vbox_menu = gtk_vbox_new(FALSE, 1);
	gtk_box_pack_start(GTK_BOX(vbox), vbox_menu, FALSE, TRUE, 0);

	create_menubar_leto(pscript, vbox_menu, 0);
	parent = create_scroll_leto(pscript->onglet_leto,parent,vbox);
	create_status_bar_leto(pscript->onglet_leto,vbox);
	create_drawingArea_leto(pscript->onglet_leto,parent);

	/* signaux de la fenetre principal */
	g_signal_connect(pscript->onglet_leto->window, "delete_event", G_CALLBACK(leto_quit), NULL);
	g_signal_connect(pscript->onglet_leto->window, "check-resize", G_CALLBACK(resized_cb), pscript);

	if ((pscript->fenetre_dialogue = malloc(sizeof(TxDonneesFenetre))) == NULL)
	{
	g_critical("Leto : Memory error");
		/* fermer ce qu'il faut */
		return 0;			
	}
	memset(pscript->fenetre_dialogue, 0, sizeof(TxDonneesFenetre));
	/* pour Find no */
	create_fenetre_dialogue(pscript->fenetre_dialogue, pscript->onglet_leto);

/********************************************************************************************************************/


    	pscript->onglet_leto->cr=NULL;


/******************************************************* Lancement de Leto ****************************************/
	for(i=1;i<argc;i++)
	{
	   
	   if (argv[i][0] == '-') 
	   {
	      
	      /* Use the next character to decide what to do. */
	  
	      switch (argv[i][1]) {

		 case 'G':	
		    if (i < argc - 1)
		    {
		       seed_tmp = strtol(argv[++i], &tailptr, 10);
		       if (strcmp(tailptr, "\0") == 0)
			  seed = seed_tmp;
		       else 
			  fprintf(stderr, "A number must be supplied after parameter -G (found %s)\n", argv[i]);
		    }
		    else 
		       fprintf(stderr, "A number must be supplied after parameter -G\n");
		    break;

		 default:	fprintf(stderr, "Unknown parameter %s\n", argv[i]);
		    break;
		    
	      }
	   }
	   else
	   {
	      ext = get_file_extension(argv[i]);
	      
#ifdef SYMBOLIQUE_VERSION
	      if (strcmp(ext, "symb") == 0)
		{
		  memcpy(sc->nomfich1, argv[i], (strlen(argv[i])+1) * sizeof(char));
		  sc->flag_symb = 1;
		}
	      else if (strcmp(ext, "script") == 0)
		{
		  memcpy(sc->nomfich1, argv[i], (strlen(argv[i])+1) * sizeof(char));
		  sc->flag_symb = 0;
		}
#else
	      if (strcmp(ext, "script") == 0)
		{
		  memcpy(sc->nomfich1, argv[i], (strlen(argv[i])+1) * sizeof(char));
		  sc->flag_symb = 0;
		}
#endif

#ifndef SYMBOLIQUE_VERSION
	      if (strcmp(ext, "res") == 0)
		 memcpy(sc->freseau, argv[i], (strlen(argv[i])+1) * sizeof(char));
#endif

	      if (strcmp(ext, "draw") == 0)
		 memcpy(sc->draw, argv[i], (strlen(argv[i])+1) * sizeof(char));
	   }
	}

	if (sc->nomfich1 == NULL || strlen(sc->nomfich1) == 0)
	{
	   fprintf(stderr, "Missing argument : path to the script file !\n");
#ifdef SYMBOLIQUE_VERSION
	   fprintf(stderr, "Usage: leto [file.symb [file.draw]]\n");
#else
	   fprintf(stderr, "Usage: leto [file.script [file.res] [file.draw]]\n");
#endif
	   exit(1);
	}

	/* lancement de leto avec les nom de fichier.script et . draw et l'indice de script disponible
	 * et l'indice de l'onglet,
	 * les .scripts et .draw sont deja definie avant donc inutile de les mettres en arguments,
	 * comme leto est un unique onglet, l'indice d'onglet et de script sont egal a zero */
	run_leto("", "", "", "", pscript->onglet_leto, 0, 0, seed);
	pscript->sc=sc;

	set_title(pscript->onglet_leto);

	/* affichage de la fenetre */
	gtk_widget_show_all(pscript->onglet_leto->window);

	/* initialise le gc car cela genere parfois des erreurs de segmentations au demarrage */	pscript->onglet_leto->gc = gdk_gc_new(GDK_DRAWABLE(pscript->onglet_leto->window->window));

    gtk_drag_dest_set(pscript->onglet_leto->window, GTK_DEST_DEFAULT_ALL,
                      target_table, 1, GDK_ACTION_COPY);

    gtk_signal_connect(GTK_OBJECT(pscript->onglet_leto->window), "drag_data_received",
                       GTK_SIGNAL_FUNC(target_drag_data_received), NULL);

    /* gtk_drag_source_set(fenetre3.window, GDK_BUTTON1_MASK|GDK_BUTTON3_MASK,
     * target_table, 1, GDK_ACTION_COPY); */

    gtk_signal_connect(GTK_OBJECT(pscript->onglet_leto->window), "drag_data_get",
                       GTK_SIGNAL_FUNC(source_drag_data_get), NULL);


    pscript->onglet_leto->cr=NULL;

    gdk_threads_enter();

    gtk_main();
    gdk_threads_leave();

    gdk_font_unref(pscript->onglet_leto->font);

    return 0;
}
#endif
