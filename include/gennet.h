#ifndef _GENNET_H_
#define _GENNET_H_

#include <errno.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Conflit de declaration sur link avec libarserxml.h */
/* #include <unistd.h> */

#include "libpromnet.h"
#include "bend.h"

#define	PROMETHE_DIR	"/simulateur"
#define	PROMETHE_ENV	"PROMETHE_DIR"
#define	HOME_ENV	"HOME"

#define	TABMAIN_ROWS	3
#define	TABMAIN_COLS	1

#define	WINDOW_WIDTH	32760
#define	WINDOW_HEIGHT	32760

#define	TOOLTIPS_MAX_LENGTH	2048

#define	GENNET_CONF_DIR		"~/.meta_leto"
#define	GENNET_WINDOWID_FILE	GENNET_CONF_DIR"/meta_leto.wid"

#define	GENNET_IN_COMPUTER	1
#define	GENNET_OUT		0

#define	GENNET_COMPUTER_PARK_WIDTH	250
#define	GENNET_COMPUTER_PARK_HEIGHT	250

#define	LETO_NETWORK_GROUP_SEND		"f_send"
#define	LETO_NETWORK_GROUP_RECEIVE	"f_receive"
#define	LETO_NETWORK_GROUP_RECV		"f_recv"

#define	LETO_NETWORK_OPTION_SEPARATOR		"-"
#define	LETO_NETWORK_ID_SEPARATOR		"_"
#define	LETO_LIAISON_NAME_SIZE		100

enum
  {
    COL_COMPUTER_ADDRESS = 0,
    COL_COMPUTER_LOGIN,
    COL_COMPUTER_PATH,
    COL_COMPUTER_CPU,
    COL_COMPUTER_INFORMATION,
    COL_COMPUTER_DATA,
    COL_COMPUTER_NB
  };

enum
  {
    COL_SCRIPT_NAME = 0,
    COL_SCRIPT_DATA,
    COL_SCRIPT_NB
  };

typedef	struct		s_gui
{
  GtkWidget		*WinMain;
  GtkWidget		*vBoxMain;
  GtkWidget		*tabbedWin;
  GtkWidget		*DrawingArea;
  GtkWidget		*NewScriptDialog;
  GtkWidget		*NewComputerDialog;
  GtkWidget		*AddComputerDialog;
  GtkWidget		*SaveNetworkDialog;
  GtkWidget		*PathDialog;
  GtkListStore		*list_computer;
  GtkWidget		*tree_view;
  GtkWidget		*gname;
  GtkToolItem		*NewComlinkItem;
  int			mouse_x;
  int			mouse_y;
  char			NewComlink;
  int			font_width;
  int			font_height;
}			t_gui;

typedef	struct			s_gennet_computer
{
  t_computer			*computer;
  PangoLayout			*pango;
  GdkPixmap			*pixmap;
  int				pos_x;
  int				pos_y;
  char				hold;
  int				pointer_x;
  int				pointer_y;
  int				nb_script;
  struct s_gennet_script_list	*scriptlist;
  struct s_gennet_computer	*prev;
  struct s_gennet_computer	*next;
}				t_gennet_computer;

typedef	struct			s_gennet_script
{
  t_prom_script			*prom_script;
  PangoLayout			*pango;
  GdkPixmap			*pixmap;
  GtkActionGroup		*action_group;
  int				color_index;
  int				in_computer;
  int				pos_x;
  int				pos_y;
  char				hold;
  int				pointer_x;
  int				pointer_y;
  donnees_script		*sc; /* donnees du script Leto */
  struct TxDonneesFenetre	*onglet_leto; /* donnees graphiques de leto */
  struct TxDonneesFenetre	*fenetre_dialogue; /* pour le find no */
  struct s_gennet_comlink_list	*comlinklist_in;
  struct s_gennet_comlink_list	*comlinklist_out;
  struct s_gennet_script	*prev;
  struct s_gennet_script	*next;
}				t_gennet_script;

typedef	struct			s_gennet_script_list
{
  t_gennet_script		*script;
  char				hold;
  PangoLayout			*pango;
  struct s_gennet_script_list	*prev;
  struct s_gennet_script_list	*next;
}				t_gennet_script_list;

typedef	struct				s_gennet_comlink
{
  t_prom_link				*prom_link;
  PangoLayout				*pango;
  char					name[LETO_LIAISON_NAME_SIZE];
  char					hold;
#ifdef USE_CAIRO
  cairo_pattern_t 			*color;
#endif /* USE_CAIRO */
  t_gennet_script			*in;
  t_gennet_script			*out;
  t_polyline_list			polyline_list;
  struct s_gennet_comlink		*prev;
  struct s_gennet_comlink		*next;
}					t_gennet_comlink;

typedef	struct			s_gennet_comlink_list
{
  t_gennet_comlink		*comlink;
  struct s_gennet_comlink_list	*prev;
  struct s_gennet_comlink_list	*next;
}				t_gennet_comlink_list;

typedef	struct		s_gennet
{
  int			ac;
  char			**av;
  char			prt_path[MAX_PATH];
  long			seed;
  t_gui			*gui;
  t_promnet		*promnet;
  t_gennet_computer	*computers;
  t_gennet_script	*scripts;
  t_gennet_comlink	*comlinks;
}			t_gennet;

/* draw */
void refresh_link(t_gennet *data, t_gennet_comlink *comlink);
void refresh_all(t_gennet *data);
void init_gui_drawing(t_gennet *data);
void refresh_script(t_gennet *data, t_gennet_script *script, int x, int y);
void refresh_computer(t_gennet *data, t_gennet_computer *computer, int x, int y);
void init_draw_script(t_gennet *data, t_gennet_script *script);
void init_draw_computer(t_gennet *data, t_gennet_computer *script);
void draw_script_in_computer(t_gennet *data, t_gennet_computer *computer, t_gennet_script *script);

/* IHM */
extern int tab_is_Metaleto(TxDonneesFenetre *onglet_leto);
extern int tab_is_Free();
void menuLinkDelete(gpointer data);
void SaveFile_control_browse(GtkWidget *dialog, GtkWidget *control, char *label);
void init_gui_toolbar(t_gennet *data);
void init_gui_winmain(t_gennet *data);
void popup_menu_handler(GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer data);
void Tools_NewComputer(GtkWidget *widget, gpointer data);
void Tools_NewComlink(GtkWidget *widget, gpointer data);
void Tools_NewScript(GtkWidget *widget, gpointer data);

/* gennet */
void NewNetwork(GtkWidget *widget, gpointer data);
void init_gui_arg(t_gennet *data);
void loadNetwork_net(t_gennet *data, char *netfile);
void loadNetwork_cpt(t_gennet *data, char *cptfile);
void AutoCreateComlink(GtkWidget *widget, gpointer data);
void GenerateNetwork(GtkWidget *widget, gpointer data);
void GenerateRunSh(GtkWidget *widget, gpointer data);
void GenerateDeploySh(GtkWidget *widget, gpointer data);
t_gennet_script	* add_gennet_script(t_gennet *data);
t_gennet_computer * Tools_NewComputer_OK(t_gennet *data, const char *address, const char *login,
					 const char *path, const char *cpu, const char *infos, t_gennet_computer *pcomputer);
void del_gennet_computer(t_gennet *data, t_gennet_computer *computer);
void del_gennet_scriptlist(t_gennet_computer *computer, t_gennet_script_list *scriptlist);
void del_gennet_comlinklist_in(t_gennet_script *script, t_gennet_comlink_list *comlinklist);
void del_gennet_comlink(t_gennet *data, t_gennet_comlink *comlink);
void fill_widget_list_computer(t_gennet *data, t_gennet_computer *pcomputer);

t_gennet_computer * find_widget_computer_by_script(t_gennet *data, t_gennet_script *script);
t_gennet_computer * find_widget_computer(t_gennet *data, int x, int y);
t_gennet_computer * find_widget_computer_by_script_list(t_gennet *data, t_gennet_script_list *scriptlist);
t_gennet_computer * find_selected_computer(t_gennet *data);
t_gennet_computer * copy_gennet_computer(t_gennet *data, t_gennet_computer *src);
t_gennet_script * find_widget_script(t_gennet *data, int x, int y);
t_gennet_script_list * find_widget_script_in_computer(t_gennet *data, int x, int y);
t_gennet_comlink * find_widget_comlink(t_gennet *data, int x, int y);
t_gennet_comlink_list * find_comlink_list_by_comlink(t_gennet_comlink_list *comlinklist, t_gennet_comlink *comlink);

t_polyline * find_widget_comlink_bend(t_gennet *data, int x, int y);
t_polyline * find_widget_comlink_polyline(t_gennet *data, int x, int y);

void del_all_scriptlist(t_gennet *data, t_gennet_computer *computer);
void Edit_Script_With_Leto(t_gennet *gen, t_gennet_script *pscript);
void del_all_comlinklist(t_gennet *data, t_gennet_script *script);
void del_gennet_script(t_gennet *data, t_gennet_script *script);
void del_gennet_comlinklist_out(t_gennet_script *script, t_gennet_comlink_list *comlinklist);
void loadNetwork(t_gennet *data, char *netfile, char *cptfile);
void writeRun(t_gennet *data, FILE *frun);
void writeDeploy(t_gennet *data, FILE *frun);
t_gennet_comlink * add_gennet_comlink(t_gennet *data);
void add_script_to_link(t_gennet *data, t_gennet_comlink *comlink, t_gennet_script *script);
t_gennet_computer * find_widget_held_computer(t_gennet *data, int x, int y);
t_gennet_script_list * find_widget_held_script_in_computer(t_gennet *data, int x, int y, t_gennet_computer **p_computer);
t_gennet_script	* find_widget_held_script(t_gennet *data, int x, int y);
t_polyline * find_widget_held_comlink_bend(t_gennet *data, int x, int y);
t_polyline * find_widget_held_comlink_polyline_list(t_gennet *data, int x, int y);
void move_script_in_computer(t_gennet *data, t_gennet_computer *computer, t_gennet_script *script);
void move_script_out_computer(t_gennet *data, t_gennet_computer *computer, t_gennet_script_list *script_list);

void menuLinkAddBend(gpointer data);
void menuLinkDeletePolyline(gpointer data);
void menuLinkDeleteBend(gpointer data);

#endif /* _GENNET_H_ */
