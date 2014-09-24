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
/*#define DEBUG*/

#include "leto.h"
#include "gere_coudes.h"
#include <math.h>
#include "draw_leto.h"
#include "outils.h"

void start_group_get_link_pos(type_groupe *group, type_liaison *link, int *x, int *y)
{
  int sens;

  if (group->reverse <= 0)
  {
    sens = 1;
  }
  else
  {
    sens = -1;
  }

  *x = group->posx + sens * (deltax + distc);
  if (link->mode < 2)
  {
    *y = group->posy;
  }
  else
  {
    *y = group->posy + 2 * deltay;
  }
}

void end_group_get_link_pos(type_groupe *group, type_liaison *link, int *x, int *y)
{
  int sens;
  (void) link;

  if (group->reverse <= 0)
  {
    sens = 1;
  }
  else
  {
    sens = -1;
  }

  *x = group->posx - sens * (deltax + distc);
  *y = group->posy + deltay;
}

void rescale_link(type_liaison *liaison)
{
  type_groupe *groupe1, *groupe2;
  int start_x, start_y, end_x, end_y;

  if (liaison->depart == -1 && liaison->arrivee == -1) return;

  groupe1 = trouver_groupe(liaison->depart);
  groupe2 = trouver_groupe(liaison->arrivee);

  start_group_get_link_pos(groupe1, liaison, &start_x, &start_y);
  end_group_get_link_pos(groupe2, liaison, &end_x, &end_y);

  rescale_coudes(liaison, start_x, start_y, end_x, end_y);
}

int deplace_coude_courant(donnees_script *script, int x, int y)
{
  if (script->coude_courant->next != NULL)
  {
    move_bend_coordinates(script->coude_courant, x, y);

    return 1;
  }
  return 0;

}

void get_link_caption_coordinates(type_liaison *link, int *x, int *y)
{
  get_polyline_list_caption_coordinates(link->polyline_list, x, y);
}

void initialise_coudes_liaison(type_liaison *liaison)
{
  if (liaison->polyline_list != NULL || liaison->depart < 0 || liaison->arrivee < 0)
  {
    return;
  }

  liaison->polyline_list = create_polyline_list();
  add_polyline_to_polyline_list(liaison->polyline_list);

  rescale_link(liaison);
}

int get_link_points(type_liaison *liaison, TxPoint tab[])
{
  int points[nbPoint_max][2];
  int i, nb_points;

  nb_points = get_all_polyline_list_coordinates(liaison->polyline_list, points);

  for (i = 0; i < nb_points; i++)
  {
    tab[i].x = points[i][0];
    tab[i].y = points[i][1];
  }

  return nb_points;
}

void detruit_coudes(type_liaison *liaison)
{
  if (liaison->polyline_list != NULL)
  {
    delete_all_polylines_in_polyline_list(liaison->polyline_list);
    free(liaison->polyline_list);
    liaison->polyline_list = NULL;
  }
}

void rescale_coudes(type_liaison *liaison, int new_start_x, int new_start_y, int new_end_x, int new_end_y)
{
  move_polyline_list_coordinates(liaison->polyline_list, new_start_x, new_start_y, new_end_x, new_end_y);

  liaison->posx1 = new_start_x;
  liaison->posy1 = new_start_y;
  liaison->posx2 = new_end_x;
  liaison->posy2 = new_end_y;
}

void gere_coude(TxPoint point, TxDonneesFenetre *onglet_leto)
{
  t_polyline *polyline;
  (void) onglet_leto;

  if (sc->liaison_courante != NULL)
  {
    polyline = find_bend(sc->liaison_courante->polyline_list, point.x, point.y);

    if (polyline != NULL)
    {
      sc->coude_courant = polyline;
      sc->flag_drag_coude = 1; /* le coude peut etre deplace */
    }
    else
    {
      sc->coude_courant = NULL;
      sc->flag_drag_coude = 0; /* le coude n'est plus selectionne, on a clicke trop loin */
    }
  }
}

void fill_coudes(TxDonneesFenetre *onglet_leto)
{
  t_polyline *polyline = NULL;
  TxPoint point;

  if (sc->liaison_courante == NULL)
  {
    return;
  }

  for (polyline = sc->liaison_courante->polyline_list->first; polyline->next != NULL; polyline = polyline->next)
  {
    point.x = polyline->x_e - BEND_SIZE_X / 2;
    point.y = polyline->y_e - BEND_SIZE_Y / 2;

    if (polyline == sc->coude_courant)
    {
      TxDessinerRectangle(onglet_leto, noir, TxPlein, point, BEND_SIZE_X + 3, BEND_SIZE_Y + 3, 1);
    }
    else
    {
      TxDessinerRectangle(onglet_leto, lut_g[sc->liaison_courante->type % 30], TxPlein, point, BEND_SIZE_X, BEND_SIZE_Y, 1);
    }
  }
}

void efface_coude()
{
  if (sc->coude_courant != NULL)
  {
    remove_polyline_and_update(sc->liaison_courante->polyline_list, sc->coude_courant);
    sc->coude_courant = NULL;
    sc->flag_drag_coude = 0;

    if (sc->liaison_courante->polyline_list->first != NULL && sc->liaison_courante->polyline_list->first->next == NULL)
    {
      sc->liaison_courante->polyline_list->first->caption_mode = CAPTION_NO_CAPTION;
    }
  }
}

int dupliquer_coude()
{
  t_polyline *polyline = NULL;
  t_polyline *tmp = NULL;

  if (sc->coude_courant != NULL) /* coude a dupliquer */
  {
    polyline = insert_polyline_before_at_coordinates(sc->liaison_courante->polyline_list, sc->coude_courant, sc->coude_courant->x_e - 15, sc->coude_courant->y_e);

    sc->coude_courant = polyline;
    return 1;
  }
  else if (sc->liaison_courante != NULL)
  {
    tmp = sc->liaison_courante->polyline_list->first;

    polyline = insert_polyline_before_at_coordinates(sc->liaison_courante->polyline_list, tmp, (tmp->x_b + tmp->x_e) / 2, (tmp->y_b + tmp->y_e) / 2);

    if (polyline->next->next == NULL)
    {
      polyline->caption_mode = CAPTION_ON_BEND;
    }

    sc->coude_courant = polyline;
    return 1;
  }

  printf("WARNING: selectioner d'abord une liaison ! \n");
  return 0;
}

void efface_coudes()
{
  if (sc->liaison_courante != NULL)
  {
    delete_extra_polylines_in_polyline_list(sc->liaison_courante->polyline_list);

    sc->coude_courant = NULL;
    sc->flag_drag_coude = 0;

    if (sc->liaison_courante->polyline_list->first != NULL)
    {
      sc->liaison_courante->polyline_list->first->caption_mode = CAPTION_NO_CAPTION;
    }
  }
}

void shift_link(type_liaison *link, int x, int y)
{
  if (link->polyline_list != NULL)
  {
    shift_polyline_list(link->polyline_list, x, y);
  }
}

int test_link_position(TxPoint point)
{
  t_polyline *polyline = NULL;
  type_liaison *liaison = NULL;

  liaison = sc->deb_liaison;
  while (liaison != NULL)
  {
    if (liaison->polyline_list != NULL)
    {
      polyline = find_polyline(liaison->polyline_list, point.x, point.y);

      if (polyline != NULL)
      {
        sc->liaison_courante = liaison;
        return 1;
      }
    }

    liaison = liaison->s;
  }

  sc->liaison_courante = NULL;
  sc->coude_courant = NULL;
  return 0;
}
