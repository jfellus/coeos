#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "basic_tools.h"
#include "bend.h"

float bound(float value, float min, float max)
{
  if (value < min)
  {
    return min;
  }
  else if (value > max)
  {
    return max;
  }

  return value;
}

int get_all_polyline_list_coordinates(t_polyline_list *polyline_list, int tab[][2])
{
  int nb_points = 0;
  t_polyline *polyline = NULL;

  if (polyline_list->first == NULL)
  {
    return 0;
  }
  else
  {
    polyline = polyline_list->first;

    tab[nb_points][0] = polyline->x_b;
    tab[nb_points][1] = polyline->y_b;
    nb_points++;

    while (polyline != NULL)
    {
      tab[nb_points][0] = polyline->x_e;
      tab[nb_points][1] = polyline->y_e;
      nb_points++;

      polyline = polyline->next;
    }
  }

  return nb_points;
}

void get_polyline_list_caption_coordinates(t_polyline_list *polyline_list, int *x, int *y)
{
  t_polyline * polyline = NULL;

  for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
  {
    switch (polyline->caption_mode)
    {
    case CAPTION_ON_BEND:
      *x = polyline->x_e;
      *y = polyline->y_e;
      return;

    case CAPTION_ON_LINE:
      *x = (polyline->x_e + polyline->x_b) / 2;
      *y = (polyline->y_e + polyline->y_b) / 2;
      return;

    case CAPTION_NO_CAPTION:
    default:
      break;
    }
  }

  *x = (polyline_list->first->x_e + polyline_list->first->x_b) / 2;
  *y = (polyline_list->first->y_e + polyline_list->first->y_b) / 2;
}

void get_polyline_rectangle(t_polyline *polyline, int *start_x, int *start_y, int *end_x, int *end_y)
{
  if (polyline->x_b < polyline->x_e)
  {
    *start_x = polyline->x_b;
    *end_x = polyline->x_e;
  }
  else
  {
    *start_x = polyline->x_e;
    *end_x = polyline->x_b;
  }

  if (polyline->y_b < polyline->y_e)
  {
    *start_y = polyline->y_b;
    *end_y = polyline->y_e;
  }
  else
  {
    *start_y = polyline->y_e;
    *end_y = polyline->y_b;
  }
}

void set_polyline_pointer(t_polyline * polyline, int x, int y)
{
  polyline->pointer_x = x - polyline->x_b;
  polyline->pointer_y = y - polyline->y_b;
}

void set_bend_pointer(t_polyline * polyline, int x, int y)
{
  polyline->pointer_x = x - polyline->x_e;
  polyline->pointer_y = y - polyline->y_e;
}

void set_bend_pointers_in_polyline_list(t_polyline_list *polyline_list, int x, int y)
{
  t_polyline *polyline = NULL;

  for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
  {
    set_bend_pointer(polyline, x, y);
  }
}

void set_polyline_begin_coordinates(t_polyline * polyline, int x, int y)
{
  polyline->x_b = x;
  polyline->y_b = y;
}

void set_polyline_end_coordinates(t_polyline * polyline, int x, int y)
{
  polyline->x_e = x;
  polyline->y_e = y;
}

int coordinates_on_polyline(t_polyline *polyline, int x, int y)
{
  float res = 0;

  if (fabs(polyline->a) < 1e-30 && fabs(polyline->b) < 1e-30)
  {
    if (x > polyline->x - LINE_CLICK_ZONE && x < polyline->x + LINE_CLICK_ZONE)
    {
      return 1;
    }
  }
  else
  {
    res = fabs(polyline->a * x - y + polyline->b) / sqrt(1 + pow(polyline->a, 2));
    if (res < LINE_CLICK_ZONE)
    {
      return 1;
    }
  }

  return 0;
}

void update_polyline_slope_info(t_polyline * polyline)
{
  if (polyline->x_e == polyline->x_b)
  {
    polyline->a = 0;
    polyline->b = 0;
    polyline->x = polyline->x_b;
  }
  else
  {
    polyline->a = (polyline->y_e - polyline->y_b) / (float) (polyline->x_e - polyline->x_b);
    polyline->b = ((polyline->y_b - polyline->a * polyline->x_b) + (polyline->y_e - polyline->a * polyline->x_e)) / 2.0;
    polyline->x = 0;
  }
}

t_polyline * find_bend(t_polyline_list *polyline_list, int x, int y)
{
  t_polyline *polyline;
  int new_x_e;
  int new_y_e;

  for (polyline = polyline_list->first; (polyline != NULL) && (polyline->next != NULL); polyline = polyline->next)
  {
    new_x_e = polyline->x_e;
    new_y_e = polyline->y_e;

    if (((x >= (new_x_e - (BEND_SIZE_X / 2))) && (x <= (new_x_e + (BEND_SIZE_X / 2)))) && ((y >= (new_y_e - (BEND_SIZE_Y / 2))) && (y <= (new_y_e + (BEND_SIZE_Y / 2))))) return polyline;
  }
  return NULL;
}

t_polyline * find_polyline(t_polyline_list *polyline_list, int x, int y)
{
  t_polyline *polyline = NULL;
  int new_x_b;
  int new_y_b;
  int new_x_e;
  int new_y_e;

  for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
  {
    get_polyline_rectangle(polyline, &new_x_b, &new_y_b, &new_x_e, &new_y_e);

    if (x >= new_x_b - LINE_CLICK_ZONE && x <= new_x_e + LINE_CLICK_ZONE && y >= new_y_b - LINE_CLICK_ZONE && y <= new_y_e + LINE_CLICK_ZONE)
    {
      if ((coordinates_on_polyline(polyline, x, y)) == 1)
      {
        return polyline;
      }
    }
  }
  return NULL;
}

t_polyline * find_held_polyline(t_polyline_list *polyline_list)
{
  t_polyline *polyline = NULL;

  for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
  {
    if (polyline->seg_hold == 1) return polyline;
  }
  return NULL;
}

t_polyline * find_held_bend(t_polyline_list *polyline_list)
{
  t_polyline *polyline = NULL;

  for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
  {
    if (polyline->hold == 1) return polyline;
  }
  return NULL;
}

t_polyline_list * create_polyline_list()
{
  t_polyline_list *polyline_list = NULL;

  polyline_list = (t_polyline_list *) malloc(sizeof(t_polyline_list));

  if (polyline_list == NULL)
  {
    fprintf(stderr, "ERROR in create_polyline_list(coeos): malloc failed for polyline_list\n");
    exit(1);
  }

  memset(polyline_list, 0, sizeof(t_polyline_list));

  return polyline_list;
}

t_polyline * add_polyline_to_polyline_list(t_polyline_list *polyline_list)
{
  t_polyline *polyline = NULL;

  if ((polyline = malloc(sizeof(t_polyline))) == NULL)
  {
    fprintf(stderr, "WARNING in add_polyline_to_polyline_list : Memory error");
    return NULL;
  }
  memset(polyline, 0, sizeof(t_polyline));

  if (polyline_list->first == NULL) polyline_list->first = polyline;
  else
  {
    polyline_list->first->prev = polyline;
    polyline->next = polyline_list->first;
    polyline_list->first = polyline;
  }
  return polyline;
}

void delete_polyline_from_polyline_list(t_polyline_list *polyline_list, t_polyline *polyline)
{
  if (polyline != NULL)
  {
    if (polyline_list->first == polyline)
    {
      polyline_list->first = polyline->next;
      if (polyline_list->first != NULL)
      {
        polyline_list->first->prev = NULL;
      }
    }
    else
    {
      if (polyline->next != NULL)
      {
        polyline->next->prev = polyline->prev;
      }
      if (polyline->prev != NULL)
      {
        polyline->prev->next = polyline->next;
      }
    }

    memset(polyline, 0, sizeof(t_polyline));
    free(polyline);
    polyline = NULL;
  }
}

void delete_all_polylines_in_polyline_list(t_polyline_list *polyline_list)
{
  t_polyline *previous;
  t_polyline *polyline = polyline_list->first;

  while (polyline != NULL)
  {
    previous = polyline;
    polyline = polyline->next;
    delete_polyline_from_polyline_list(polyline_list, previous);
  }
}

void delete_extra_polylines_in_polyline_list(t_polyline_list *polyline_list)
{
  t_polyline *previous, *polyline;

  if (polyline_list->first != NULL)
  {
    polyline = polyline_list->first->next;
    while (polyline != NULL)
    {
      previous = polyline;
      polyline = polyline->next;
      remove_polyline_and_update(polyline_list, previous);
    }
  }
}

t_polyline * insert_polyline_before(t_polyline_list *polyline_list, t_polyline *polyline)
{
  t_polyline *new_polyline = NULL;

  if (polyline == NULL || polyline_list == NULL) return NULL;

  if ((new_polyline = malloc(sizeof(t_polyline))) == NULL)
  {
    fprintf(stderr, "WARNING in insert_polyline_before : Memory error");
    return NULL;
  }
  memset(new_polyline, 0, sizeof(t_polyline));

  if (polyline->prev != NULL)
  {
    polyline->prev->next = new_polyline;
  }

  new_polyline->prev = polyline->prev;
  polyline->prev = new_polyline;
  new_polyline->next = polyline;

  if (polyline_list->first == polyline)
  {
    polyline_list->first = new_polyline;
  }
  return new_polyline;
}

t_polyline * insert_polyline_before_at_coordinates(t_polyline_list *polyline_list, t_polyline *polyline, int x, int y)
{
  t_polyline *new_polyline = NULL;

  new_polyline = insert_polyline_before(polyline_list, polyline);

  if (new_polyline == NULL)
  {
    fprintf(stderr, "ERROR in add_polyline_at_coordinates: could not insert new polyline\n");
    exit(1);
  }

  set_polyline_begin_coordinates(new_polyline, polyline->x_b, polyline->y_b);
  set_polyline_end_coordinates(new_polyline, x, y);
  set_polyline_begin_coordinates(polyline, x, y);

  update_polyline_slope_info(new_polyline);
  update_polyline_slope_info(polyline);

  return new_polyline;
}

t_polyline * insert_polyline_after(t_polyline *polyline)
{
  t_polyline *new_polyline = NULL;

  if (polyline == NULL) return NULL;

  if ((new_polyline = malloc(sizeof(t_polyline))) == NULL)
  {
    fprintf(stderr, "WARNING in insert_polyline_after : Memory error");
    return NULL;
  }
  memset(new_polyline, 0, sizeof(t_polyline));

  if (polyline->next != NULL)
  {
    polyline->next->prev = new_polyline;
  }

  new_polyline->next = polyline->next;
  polyline->next = new_polyline;
  new_polyline->prev = polyline;
  return new_polyline;
}

t_polyline * insert_polyline_after_at_coordinates(t_polyline *polyline, int x, int y)
{
  t_polyline *new_polyline = NULL;

  new_polyline = insert_polyline_after(polyline);

  if (new_polyline == NULL)
  {
    fprintf(stderr, "ERROR in insert_polyline_at_coordinates: could not insert new polyline\n");
    exit(1);
  }

  set_polyline_begin_coordinates(new_polyline, x, y);
  set_polyline_end_coordinates(new_polyline, polyline->x_e, polyline->y_e);

  set_polyline_end_coordinates(polyline, x, y);

  update_polyline_slope_info(new_polyline);
  update_polyline_slope_info(polyline);

  return new_polyline;
}

void remove_polyline_and_update(t_polyline_list *polyline_list, t_polyline *polyline)
{
  if (polyline->next != NULL)
  {
    polyline->next->x_b = polyline->x_b;
    polyline->next->y_b = polyline->y_b;

    update_polyline_slope_info(polyline->next);
  }
  else if (polyline->prev != NULL)
  {
    polyline->prev->x_e = polyline->x_e;
    polyline->prev->y_e = polyline->y_e;

    update_polyline_slope_info(polyline->prev);
  }
  else
  {
    return;
  }

  delete_polyline_from_polyline_list(polyline_list, polyline);
}

void reset_pointers_in_polyline_list(t_polyline_list *polyline_list)
{
  t_polyline *polyline = NULL;

  for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
  {
    polyline->pointer_x = -1;
    polyline->pointer_y = -1;
  }
}

void move_polyline_list(t_polyline_list *polyline_list, int x, int y)
{
  t_polyline *polyline = NULL;

  for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
  {
    move_bend(polyline, x, y);
  }
}

void move_bend(t_polyline *polyline, int x, int y)
{
  if ((polyline->x_e = x - polyline->pointer_x) < 0)
  {
    polyline->x_e = 0;
  }

  if ((polyline->y_e = y - polyline->pointer_y) < 0)
  {
    polyline->y_e = 0;
  }

  update_polyline_slope_info(polyline);

  /* On suppose ici que le segment suivant est le next */
  if (polyline->next != NULL)
  {
    polyline->next->x_b = polyline->x_e;
    polyline->next->y_b = polyline->y_e;

    update_polyline_slope_info(polyline->next);
  }
}

void move_bend_coordinates(t_polyline *polyline, int x, int y)
{
  polyline->x_e = x;
  polyline->y_e = y;
  update_polyline_slope_info(polyline);

  if (polyline->next != NULL)
  {
    polyline->next->x_b = x;
    polyline->next->y_b = y;
    update_polyline_slope_info(polyline->next);
  }
}

void move_polyline(t_polyline *polyline, int x, int y)
{
  int old_x;
  int old_y;

  if (polyline->prev != NULL && polyline->next != NULL)
  {
    old_x = polyline->x_b;
    old_y = polyline->y_b;

    if ((polyline->x_b = x - polyline->pointer_x) < 0)
    {
      polyline->x_b = 0;
    }

    if ((polyline->y_b = y - polyline->pointer_y) < 0)
    {
      polyline->y_b = 0;
    }

    if ((polyline->x_e += polyline->x_b - old_x) < 0)
    {
      polyline->x_e = 0;
    }

    if ((polyline->y_e += polyline->y_b - old_y) < 0)
    {
      polyline->y_e = 0;
    }

    update_polyline_slope_info(polyline);

    /* On suppose ici que le segment precedent est le prev */
    polyline->prev->x_e = polyline->x_b;
    polyline->prev->y_e = polyline->y_b;

    update_polyline_slope_info(polyline->prev);

    /* On suppose ici que le segment suivant est le next */
    polyline->next->x_b = polyline->x_e;
    polyline->next->y_b = polyline->y_e;

    update_polyline_slope_info(polyline->next);
  }
}

int move_polyline_coordinates(t_polyline *polyline, int start_x, int start_y, int end_x, int end_y)
{
  int coordinates_changed = 0;

  if (polyline->x_b != start_x || polyline->y_b != start_y || polyline->x_e != end_x || polyline->y_e != end_y)
  {
    coordinates_changed = 1;
    polyline->x_b = start_x;
    polyline->y_b = start_y;
    polyline->x_e = end_x;
    polyline->y_e = end_y;
  }

  update_polyline_slope_info(polyline);

  return coordinates_changed;
}

void move_polyline_list_coordinates(t_polyline_list *polyline_list, int new_start_x, int new_start_y, int new_end_x, int new_end_y)
{
  t_polyline *polyline;
  int old_start_x, old_start_y, old_end_x, old_end_y;
  int start_x, start_y, end_x, end_y;
  int old_dx, old_dy, new_dx, new_dy;
  float d_min = 0.1, d_max = 0.9;

  if (polyline_list == NULL)
  {
    EXIT_ON_ERROR("ERROR in move_polyline_list_coordinates(coeos): polyline_list is NULL\n");
  }

  polyline = polyline_list->first;
  old_start_x = polyline->x_b;
  old_start_y = polyline->y_b;

  while (polyline->next != NULL)
  {
    polyline = polyline->next;
  }
  old_end_x = polyline->x_e;
  old_end_y = polyline->y_e;

  if (new_start_x - old_start_x == new_end_x - old_end_x && new_start_y - old_start_y == new_end_y - old_end_y)
  {
    shift_polyline_list(polyline_list, new_start_x - old_start_x, new_start_y - old_start_y);
  }
  else
  {

    old_dx = old_end_x - old_start_x;
    old_dy = old_end_y - old_start_y;
    new_dx = new_end_x - new_start_x;
    new_dy = new_end_y - new_start_y;

    for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
    {
      if (polyline == polyline_list->first)
      {
        start_x = new_start_x;
        start_y = new_start_y;
      }
      else
      {
        start_x = polyline->prev->x_e;
        start_y = polyline->prev->y_e;
      }

      if (polyline->next == NULL)
      {
        end_x = new_end_x;
        end_y = new_end_y;
      }
      else if (polyline->fixed == 0)
      {

        if (fmin(old_start_x, old_end_x) <= polyline->x_e && fmax(old_start_x, old_end_x) >= polyline->x_e && old_dx != 0)
        {
          end_x = new_start_x + bound((polyline->x_e - old_start_x) / ((float) old_dx), d_min, d_max) * new_dx;
        }
        else
        {
          end_x = polyline->x_e;
        }

        if (fmin(old_start_y, old_end_y) <= polyline->y_e && fmax(old_start_y, old_end_y) >= polyline->y_e && old_dy != 0)
        {
          end_y = new_start_y + bound((polyline->y_e - old_start_y) / ((float) old_dy), d_min, d_max) * new_dy;
        }
        else
        {
          end_y = polyline->y_e;
        }
      }
      else
      {
        end_x = polyline->x_e;
        end_y = polyline->y_e;
      }

      move_polyline_coordinates(polyline, start_x, start_y, end_x, end_y);
    }
  }
}

void shift_polyline(t_polyline *polyline, int x, int y)
{
  polyline->x_b += x;
  if (polyline->x_b < 0)
  {
    polyline->x_b = 0;
  }

  polyline->x_e += x;
  if (polyline->x_e < 0)
  {
    polyline->x_e = 0;
  }

  polyline->y_b += y;
  if (polyline->y_b < 0)
  {
    polyline->y_b = 0;
  }

  polyline->y_e += y;
  if (polyline->y_e < 0)
  {
    polyline->y_e = 0;
  }

  update_polyline_slope_info(polyline);
}

void shift_polyline_list(t_polyline_list *polyline_list, int x, int y)
{
  t_polyline *polyline;

  for (polyline = polyline_list->first; polyline != NULL; polyline = polyline->next)
  {
    shift_polyline(polyline, x, y);
  }
}
