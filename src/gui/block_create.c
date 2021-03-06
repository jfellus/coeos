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
/* Transforme un ensemble de groupes selectionnes en une macro.     */
/* Les groupes ont deja ete sauvegarde dans le fichier macro / nom. */
/* Cette fonction doit renommer les groupes et liens consideres */
/* + creer une boite pour l'insertion de la macro */

/* no_name groupe inclus: nom_macro[No_inclusion]_no_name_groupe*/

/*#define DEBUG 1*/

#include "graphic_Tx.h"
#include "outils.h"
#include "outils_script.h"
#include "group_selection.h"
#include "gennet.h"
#include "draw_leto.h"

/**
 * Renomme un groupe et change le nom pour les liaisons associees 
 */

/* essaie de renomer un groupe en declarant le nouveau nom dans la table de hashcoding */
/* si succes renvoie 1 sinon renvoie 0                                                 */

int rename_group(type_groupe *group, char *nom, struct hsearch_data *hashtab)
{
  type_groupe *group_temp;

  group_temp = lookup_hash_table(hashtab, nom);
  if (group_temp == NULL)
  {
    strcpy(group->no_name, nom);
    insert_hash_table(hashtab, group->no_name, group);
    return 1;
  }
  else
  {
    fprintf(stderr, "WARNING: Group name %s is already used, cannot change the name of group %s\n", nom, group->no_name);
    return 0;
  }
}

void rename_group_and_associated_links(type_groupe *group, char *nom, t_gennet_script *script_gui)
{
  type_liaison *link;
  int n, succes;

  if (strcmp(group->no_name, nom) == 0)
  {
    return;
  }

  n = group->no;

  succes = rename_group(group, nom, script_gui->onglet_leto->hashtab);
  if (succes == 0)
  {
    fprintf(stderr, "Echec rename_group (rename_group_and_associated_links): no link information to change \n");
    return;
  }

  link = script_gui->sc->deb_liaison;
  while (link != NULL)
  {
    if (link->depart == n)
    {
      memcpy(link->depart_name, nom, (strlen(nom) + 1) * sizeof(char));
    }

    if (link->arrivee == n)
    {
      memcpy(link->arrivee_name, nom, (strlen(nom) + 1) * sizeof(char));
    }
    link = link->s;
  }
}

void transfo_groups_in_block(char *nom, t_gennet_script *script_gui)
{
  selected_group *sel_group = NULL;
  type_groupe *groupe_local, *new_group;
  int succes;
  int no_macro_used;
  int selected_plane;
  char base_nom[SIZE_NO_NAME], base_nom_complet[SIZE_NO_NAME];
  char new_name[SIZE_NO_NAME];
  char no_name[SIZE_NO_NAME];
  int px, py, mx, my, nbre_groupe_macro;
  float zoom;
  int reverse;
  type_liaison *liaison;

  printf("debut transfo_groups_in_block %s\n", nom);

  px = script_gui->sc->point_courant_leto.x;
  py = script_gui->sc->point_courant_leto.y; /* point central de la macro */

  /* creation d'un macro groupe: */
  script_gui->sc->fin_groupe = new_group = (type_groupe *) creer_groupeb(sc->fin_groupe);
  if (script_gui->sc->deb_groupe == NULL) script_gui->sc->deb_groupe = new_group;

  new_group->type = No_Sub_Network;
  script_gui->sc->nbre_neurone++;

  selected_plane = find_new_plane_for_new_macro(&no_macro_used);

  memcpy(base_nom, nom, (strlen(nom) + 1) * sizeof(char));
  get_base_name(base_nom);

  sprintf(base_nom_complet, "%s[%d]", base_nom, no_macro_used);

  mx = my = 0; /* centre de gravite des groupes lus */
  nbre_groupe_macro = 0;

  /* pour chaque groupe selectionne*/

  for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
  {
    groupe_local = sel_group->group;
    if (groupe_local != new_group)
    {
      groupe_local->deja_active = 1; /* c'est un groupe insere (a ne pas resauvegarder) */
      if (groupe_local->reverse > 0) reverse = 1;
      else reverse = -1;
      groupe_local->reverse = reverse * selected_plane; /* plan >100 pour tout bouger ensemble */
      mx = mx + groupe_local->posx;
      my = my + groupe_local->posy;
      nbre_groupe_macro++;
    }
  }

  new_group->deja_active = 0; /* le macro groupe devra etre sauvegarde */
  if (new_group->reverse > 0) reverse = 1;
  else reverse = -1;
  new_group->reverse = reverse * selected_plane; /* plan >100 pour tout bouger ensemble */
  new_group->posx = px;
  new_group->posy = py;
  select_group(new_group);
  memcpy(new_group->nom, nom, (strlen(nom) + 1) * sizeof(char));

  succes = rename_group(new_group, base_nom_complet, script_gui->onglet_leto->hashtab);
  if (succes == 0)
  {
    EXIT_ON_ERROR("Echec rename_group (transfo_groups_in_block): group %s already used, cannot continue \n", no_name);
  }

  zoom = 0.1;
  mx = mx / nbre_groupe_macro;
  my = my / nbre_groupe_macro;

  for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
  {
    groupe_local = sel_group->group;
    if (groupe_local != new_group)
    {
      groupe_local->posx = 2 + px + (int) (((float) (groupe_local->posx - mx)) * zoom);
      groupe_local->posy = 2 + py + (int) (((float) (groupe_local->posy - my)) * zoom);
      sprintf(new_name, "%s_%s", base_nom_complet, groupe_local->no_name);
      rename_group_and_associated_links(groupe_local, new_name, script_gui);
    }
  }

  regenerer_test(script_gui->onglet_leto);

  liaison = sc->deb_liaison;
  while (liaison != NULL)
  {
    if (selected_link(liaison, script_gui->onglet_leto) == 1)
    {
      liaison->deja_active = 1;
    }
    liaison = liaison->s;
  }
}

