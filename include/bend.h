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
#ifndef _BEND_H_
#define _BEND_H_

#define	BEND_SIZE_X		10
#define	BEND_SIZE_Y		10
#define BEND_SPACE_SIZE		15

#define	LINE_CLICK_ZONE	5

#define CAPTION_NO_CAPTION 	0
#define CAPTION_ON_LINE 	1
#define CAPTION_ON_BEND 	2

struct s_polyline_list;

/* le coude se trouve a la fin de chaque segment (polyline) sauf pour le dernier */
typedef	struct			s_polyline
{
  int				x_b;	/* Abscisse du point de depart */
  int				y_b;	/* Ordonnee du point de depart */
  int				x_e;	/* Abscisse du point de fin */
  int				y_e;	/* Ordonnee du point de fin */
  float				a;	/* Pente du segment */
  float				b;	/* Decalage du segment */
  float				x;
  int				pointer_x;
  int				pointer_y;
  int				hold;		/* on reste clique sur le coude */
  int				seg_hold;	/* on reste clique sur le segment */
  int				caption_mode;
  int				fixed;
  struct s_polyline		*prev;
  struct s_polyline		*next;
}				t_polyline;

typedef	struct				s_polyline_list
{
  struct s_polyline *first;
}					t_polyline_list;

int get_all_polyline_list_coordinates(t_polyline_list *polyline_list, int tab[][2]);
void get_polyline_rectangle(t_polyline *polyline, int *start_x, int *start_y, int *end_x, int *end_y);
void get_polyline_list_caption_coordinates(t_polyline_list *polyline_list, int *x, int *y);

void set_polyline_begin_coordinates(t_polyline * polyline, int x, int y);
void set_polyline_end_coordinates(t_polyline * polyline, int x, int y);

void reset_pointers_in_polyline_list(t_polyline_list *polyline_list);
void set_bend_pointers_in_polyline_list(t_polyline_list *polyline_list, int x, int y);
void set_bend_pointer(t_polyline * polyline, int x, int y);
void set_polyline_pointer(t_polyline * polyline, int x, int y);

t_polyline * find_bend(t_polyline_list *data, int x, int y);
t_polyline * find_polyline(t_polyline_list *data, int x, int y);
t_polyline * find_held_bend(t_polyline_list *data);
t_polyline * find_held_polyline(t_polyline_list *data);

t_polyline_list * create_polyline_list();

t_polyline * insert_polyline_before(t_polyline_list * polyline_list, t_polyline *polyline);
t_polyline * insert_polyline_before_at_coordinates(t_polyline_list * polyline_list, t_polyline *polyline, int x, int y);
t_polyline * insert_polyline_after(t_polyline *polyline);
t_polyline * insert_polyline_after_at_coordinates(t_polyline *polyline, int x, int y);
t_polyline * add_polyline_to_polyline_list(t_polyline_list *polyline_list);

void delete_polyline_from_polyline_list(t_polyline_list *polyline_list, t_polyline *polyline);
void delete_all_polylines_in_polyline_list(t_polyline_list *polyline_list);
void delete_extra_polylines_in_polyline_list(t_polyline_list *polyline_list);
void remove_polyline_and_update(t_polyline_list *polyline_list, t_polyline *polyline);

void move_polyline_list(t_polyline_list *polyline_list, int x, int y);
void move_polyline_list_coordinates(t_polyline_list *polyline_list, int new_start_x, int new_start_y, int new_end_x, int new_end_y);
void move_polyline(t_polyline *polyline, int x, int y);
int move_polyline_coordinates(t_polyline *polyline, int start_x, int start_y, int end_x, int end_y);
void move_bend(t_polyline *polyline, int x, int y);
void move_bend_coordinates(t_polyline *polyline, int x, int y);

void shift_polyline(t_polyline *polyline, int x, int y);
void shift_polyline_list(t_polyline_list *polyline_list, int x, int y);
#endif
