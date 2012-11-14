
/* Transforme un ensemble de groupes selectionnes en une macro.     */
/* Les groupes ont deja ete sauvegarde dans le fichier macro / nom. */
/* Cette fonction doit renommer les groupes et liens consideres */
/* + creer une boite pour l'insertion de la macro */

/* no_name groupe inclus: nom_macro[No_inclusion]_no_name_groupe*/


/*#define DEBUG 1*/

#include "public_leto.h"
#include "outils_script.h"

/**
 * Renomme un groupe et change le nom pour les liaisons associees 
 */
 
/* essaie de renomer un groupe en declarant le nouveau nom dans la table de hashcoding */
/* si succes renvoie 1 sinon renvoie 0                                                 */

int rename_group(type_groupe *group, char *nom, TxDonneesFenetre *onglet_leto)
{
  type_groupe *group_temp;

  group_temp = lookup_hash_table((void **)&onglet_leto->hashtab, nom);
  if (group_temp == NULL)
  {
     debug_printf("rename_group: group %s changed to %s\n", group->no_name, nom);
     memcpy(group->no_name,nom, (strlen(nom)+1) * sizeof(char));
     insert_hash_table((void **)&onglet_leto->hashtab, group->no_name, group);
     return 1;
  }
  else
  {
     fprintf(stderr, "WARNING: Group name %s is already used, cannot change the name of group %s\n", nom, group->no_name);
     return 0;
  }
}    

void rename_group_and_associated_links(type_groupe *group, char *nom, TxDonneesFenetre *onglet_leto)
{
    type_liaison *link;
    int n,succes;
    
    if (strcmp(group->no_name, nom) == 0)
    {
       debug_printf("rename_group_and_associated_links: no_name is already %s => No change\n", nom);
       return;
    }

    n = group->no;

    succes = rename_group(group, nom, onglet_leto);
    if (succes == 0)
    {
       fprintf(stderr, "Echec rename_group (rename_group_and_associated_links): no link information to change \n");
       return;
    }

    link = sc->deb_liaison;
    while (link != NULL)
    {
       if (link->depart == n)
       {
	  memcpy(link->depart_name, nom, (strlen(nom)+1) * sizeof(char));
       }

       if (link->arrivee == n)
       {
	  memcpy(link->arrivee_name, nom, (strlen(nom)+1) * sizeof(char));
       }
       link = link->s;
    }
}

void transfo_groups_in_block(char *nom,TxDonneesFenetre *onglet_leto)
{
  selected_group *sel_group = NULL;
  type_groupe *groupe_local,*new_group;
  int succes;
  int no_macro_used;
  int selected_plane;
  char base_nom[SIZE_NO_NAME],base_nom_complet[SIZE_NO_NAME];
  char new_name[SIZE_NO_NAME];
  char no_name[SIZE_NO_NAME];
  int px,py,mx,my,nbre_groupe_macro;
  float zoom;
  int reverse;
  type_liaison *liaison;
	
  printf("debut transfo_groups_in_block %s\n",nom);
  
  px=sc->point_courant_leto.x;py=sc->point_courant_leto.y;  /* point central de la macro */

/* creation d'un macro groupe: */
  sc->fin_groupe =new_group = (type_groupe *) creer_groupeb(sc->fin_groupe);
  if (sc->deb_groupe == NULL)
     sc->deb_groupe = new_group;

  new_group->type=No_Sub_Network;
  sc->nbre_neurone++;

  selected_plane=find_new_plane_for_new_macro(&no_macro_used);
 
  memcpy(base_nom,nom, (strlen(nom)+1) * sizeof(char));
  get_base_name(base_nom);

  sprintf(base_nom_complet,"%s[%d]",base_nom,no_macro_used);
  debug_printf("base = %s \n",base_nom_complet);


  mx=my=0; /* centre de gravite des groupes lus */
  nbre_groupe_macro=0;

  /* pour chaque groupe selectionne*/
  debug_printf("Changement de statut des groupes selectionnes base_nom_complet = %s\n",base_nom_complet);
	 
  for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
  {
     groupe_local = sel_group->group;
     if (groupe_local != new_group) 
     {
	groupe_local->deja_active = 1; /* c'est un groupe insere (a ne pas resauvegarder) */
	if(groupe_local->reverse>0) reverse=1; else reverse=-1;
	groupe_local->reverse=reverse*selected_plane; /* plan >100 pour tout bouger ensemble */
	mx=mx+groupe_local->posx;
	my=my+groupe_local->posy;
	nbre_groupe_macro++;
     }
  }

  new_group->deja_active=0; /* le macro groupe devra etre sauvegarde */  
  if(new_group->reverse>0) reverse=1; else reverse=-1;
    new_group->reverse=reverse*selected_plane; /* plan >100 pour tout bouger ensemble */
  new_group->posx=px; new_group->posy=py;
  select_group(new_group);
  memcpy(new_group->nom,nom, (strlen(nom)+1) * sizeof(char));

  succes=rename_group(new_group,base_nom_complet, onglet_leto);
  if(succes==0)
      {
	printf("Echec rename_group (transfo_groups_in_block): group %s already used, cannot continue \n",no_name);
	exit(0);
      }

  debug_printf("Repositionnement des groupes \n");

  zoom=0.1; 
  mx=mx/nbre_groupe_macro;  my=my/nbre_groupe_macro ;

  for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
  {
     groupe_local = sel_group->group;
     if (groupe_local != new_group) 
     {
	groupe_local->posx = 2+px+(int)(((float)(groupe_local->posx-mx))*zoom);
	groupe_local->posy = 2+py+(int)(((float)(groupe_local->posy-my))*zoom);
	sprintf(new_name,"%s_%s",base_nom_complet,groupe_local->no_name);
	rename_group_and_associated_links(groupe_local, new_name, onglet_leto);
     }
  }

  regenerer_test(onglet_leto);
  debug_printf("Fin de modification des groupes \n");
  
  debug_printf("Debut modification des liaisons \n");
  liaison = sc->deb_liaison;
  while (liaison != NULL)
  {
     if (selected_link(liaison, onglet_leto) == 1)
     {
	liaison->deja_active=1;
     }
     liaison=liaison->s;
  }
 debug_printf("Fin block_create \n"); 
}

	
