/*#define DEBUG*/

#include "public_leto.h"
#include "gere_coudes.h"

selected_group * is_selected(type_groupe *group)
{
   selected_group *sel_group;

   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      if (sel_group->group == group)
      {
	 return sel_group;
      }
   }
   return NULL;
}


void selection_unselect_group(selected_group *sel_group)
{
   if (sel_group == sc->groupes_courants)
   {
      sc->groupes_courants = sel_group->next;
   }

   if (sel_group->prev != NULL)
   {
      sel_group->prev->next = sel_group->next;
   }

   if (sel_group->next != NULL)
   {
      sel_group->next->prev = sel_group->prev;
   }
   
   memset(sel_group, 0, sizeof(sel_group));
   free(sel_group);
}

void selection_select_group(type_groupe *group)
{
   selected_group *new_sel_group = NULL;
   
   new_sel_group = (selected_group *) malloc(sizeof(selected_group));
   if (new_sel_group == NULL)
   {
      fprintf(stderr, "ERROR in select_group: malloc failed\n");
      return;
   }
   memset(new_sel_group, 0, sizeof(new_sel_group));
   new_sel_group->group = group;
   new_sel_group->next = sc->groupes_courants;

   if (new_sel_group->next != NULL)
   {
      new_sel_group->next->prev = new_sel_group;
   }

   sc->groupes_courants = new_sel_group;
}


void update_selection_rectangle(int xmin, int ymin, int xmax, int ymax, int ctrl_mask)
{
   type_groupe *groupe;
   selected_group *sel_group, *prev_sel_group;
   int x, y;

   if (ctrl_mask == 0)
   {
      sel_group = sc->groupes_courants;
      while (sel_group != NULL)
      {
	 prev_sel_group = sel_group;
	 sel_group = sel_group->next;

	 x = prev_sel_group->group->posx;
	 y = prev_sel_group->group->posy;

	 if (x < xmin || x > xmax || y < ymin || y > ymax)
	 {
	    selection_unselect_group(prev_sel_group);
	 }
      }
   }

   groupe = sc->deb_groupe;
   while (groupe != NULL)
   {
      x = groupe->posx;
      y = groupe->posy;
     
      if (x >= xmin && x <= xmax && y >= ymin && y <= ymax)
      {
	 if (is_selected(groupe) == NULL)
	 {
	    selection_select_group(groupe);
	 }
      }
      groupe = groupe->s;	
   }
}


type_groupe *selection_groupe(TxPoint point)
{
   type_groupe *group = NULL;
   selected_group *sel_group = NULL;
   debug_printf("selection_groupe\n");
   
   if ((group = test_group_position(sc->point_courant_leto)) != NULL)
   {
      if ((sel_group = is_selected(group)) == NULL)
      {
	 selection_select_group(group);
      }
      else
      {	 
	 selection_unselect_group(sel_group);
      }
      return group;
   }
   return NULL;
}

/* deselectionne l'ensemble des groupes qui avaient pu etre selectionnes */
void reset_selection_groups()
{
   selected_group *sel_group, *prev_sel_group; 
   
   sel_group = sc->groupes_courants;
   
   while (sel_group != NULL)
   {
      prev_sel_group = sel_group;
      sel_group = sel_group->next;
      
      memset(prev_sel_group, 0, sizeof(prev_sel_group));
      free(prev_sel_group);
   }
   sc->groupes_courants = NULL;
}

/*----------------------------------------------------------*/

type_groupe * gere_groupe(TxPoint point)
{
   type_groupe *group = NULL;
   selected_group *sel_group = NULL;
   int x_center = 0, y_center = 0, nb_selected_groups = 0;

   debug_printf("gere_groupe\n");

   if ((group = test_group_position(sc->point_courant_leto)) != NULL)
   {
      if (is_selected(group) == NULL)
      {
	 select_group(group);
      }

      if (sc->flag_create_link == LINK_CREATE_NO_ACTION)
      {
	 sc->flag_drag_group = 1;
	 
	 for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
	 {
	    x_center += sel_group->group->posx;
	    y_center += sel_group->group->posy;
	    nb_selected_groups++;
	 }
	 
	 sc->pointer_selection_center.x = point.x - x_center / nb_selected_groups;
	 sc->pointer_selection_center.y = point.y - y_center / nb_selected_groups;
      }
      return group;
   }
   else
   {
      reset_selection_groups();
      sc->flag_drag_group = 0;
      return NULL;
   }
}

void select_group(type_groupe *group)
{
   reset_selection_groups();
   selection_select_group(group);
}


/**
 * Tests if the cursor is on a group.
 *
 * @param point  the position of the cursor
 */
type_groupe *test_group_position(TxPoint point)
{
    type_groupe *group;

    group = sc->deb_groupe;
    while (group != NULL)
    {
       if (abs(group->posx - point.x) <= deltax + 10 && abs(group->posy - point.y) <= deltay + 10)
       {
	  debug_printf("Le groupe selectionne est %s\n", group->no_name);
	  return group;
       }
       group = group->s;
    }
    return NULL;
}

/*--------------------------------------------------------------*/


void drag_selected_groups(TxPoint point, TxDonneesFenetre *data)
{
   int nb_selected_groups = 0;
   int x_center = 0., y_center = 0.;
   int target_center_x, target_center_y;
   selected_group *sel_group = NULL;

   target_center_x = point.x - sc->pointer_selection_center.x;
   target_center_y = point.y - sc->pointer_selection_center.y;

   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      x_center += sel_group->group->posx;
      y_center += sel_group->group->posy;
      nb_selected_groups++;
   }

   slide_groups(target_center_x - x_center / nb_selected_groups, target_center_y - y_center / nb_selected_groups, 1);
}


void move_selected_groups(TxPoint point, TxDonneesFenetre *data)
{
   int nb_selected_groups = 0;
   float x_center = 0., y_center = 0.;
   selected_group *sel_group = NULL;

   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      x_center += sel_group->group->posx;
      y_center += sel_group->group->posy;
      nb_selected_groups++;
   }
   
   slide_groups((int) point.x - x_center / nb_selected_groups, (int) point.y - y_center / nb_selected_groups, 1);
}

/* change la position d'un groupe et celle des coudes des liaisons associees */
void nouvelle_position_groupe(type_groupe *groupe, TxPoint point)
{
   type_liaison *liaison;

   groupe->posx = point.x;
   groupe->posy = point.y;

   liaison = sc->deb_liaison;
   while (liaison != NULL)
   {
      if (liaison->depart == groupe->no || liaison->arrivee == groupe->no)
      {
	 rescale_link(liaison);
      }
      liaison = liaison->s;
   }
}

void slide_groups(int x, int y, int selection)
{
    type_groupe *gpe;
    selected_group *sel_group = NULL;
    type_liaison *liaison;
    int selected_planes[nb_max_planes];
    int planes_to_select = 0;

    debug_printf("slide_groups: selection = %i\n", selection);

    memset(selected_planes, 0, sizeof(selected_planes));

    if (selection == 0)
    {
       gpe = sc->deb_groupe;
       while (gpe != NULL)
       {
	  gpe->posx += x;
	  gpe->posy += y;
	  gpe = gpe->s;
       }    
    }
    else
    {       
       for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
       {
	  sel_group->group->posx += x;
	  sel_group->group->posy += y;
	  
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
		gpe->posx += x;
		gpe->posy += y;		
	     }
	     gpe = gpe->s;
	  }    
       }
    }

    liaison = sc->deb_liaison;
    while (liaison != NULL)
    {
       rescale_link(liaison);
       liaison = liaison->s;
    }
}
