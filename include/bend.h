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
