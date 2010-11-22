#ifndef	_LIBPROMNET_H_
#define	_LIBPROMNET_H_

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libparserxml_MetaLeto.h>
#include "script_parserxml.h"

#define	MAX_NAME	256
#define	MAX_ADDRESS	256
#define	MAX_LOGIN	256
#define	MAX_CPU		256
#define	MAX_PATH	256
#define	MAX_INFORMATION	256

/*
** Structure of computer
*/

typedef	struct		s_computer
{
  char			name[MAX_NAME];
  char			address[MAX_ADDRESS];
  char			login[MAX_LOGIN];
  char			cpu[MAX_CPU];
  char			path[MAX_PATH];
  char			information[MAX_INFORMATION];

  computer		*data;

  struct s_computer	*prev;
  struct s_computer	*next;
}			t_computer;



#define	MAX_LOGICAL_NAME	48

#define	MAX_PATH_PROM_BINARY	256

#define	MAX_ALL_PATH		208

#define	MAX_PATH_FILE_SCRIPT	256
#define	MAX_PATH_FILE_SCRIPT_NON_SYMB	256
#define	MAX_PATH_FILE_DRAW	256
#define	MAX_PATH_FILE_RES	256
#define	MAX_PATH_FILE_CONFIG	256
#define	MAX_PATH_FILE_BUS	256
#define	MAX_PATH_FILE_VAR	256
#define	MAX_PATH_FILE_DEV	256
#define	MAX_PATH_FILE_PRT	256
#define	MAX_PATH_FILE_GCD	256
#define	MAX_PATH_PROM_DEPLOY	256

#define	EXT_SCRIPT_SCRIPT	".script"
#define	EXT_SCRIPT_NON_SYMB	".script_o"
#define	EXT_SCRIPT_SYMB		".symb"
#define	EXT_SCRIPT_DRAW		".draw"
#define	EXT_SCRIPT_RES		".res"
#define	EXT_SCRIPT_CONFIG	".config"
#define	EXT_SCRIPT_BUS		".bus"
#define	EXT_SCRIPT_VAR		".var"
#define	EXT_SCRIPT_DEV		".dev"
#define	EXT_SCRIPT_GCD		".gcd"
#define	EXT_SCRIPT_PRT		".prt"

#define	MAX_PROM_ARGS_LINE	2048

#define	MAX_COMPUTER		256

/*
** Structure of Promethe Script
*/
typedef	struct		s_prom_script
{
  char			logical_name[MAX_LOGICAL_NAME];

  char			path_prom_binary[MAX_PATH_PROM_BINARY];
  char			path_prom_deploy[MAX_PATH_PROM_DEPLOY];

  char			path_file_script[MAX_PATH_FILE_SCRIPT];
  char			path_file_script_non_symb[MAX_PATH_FILE_SCRIPT_NON_SYMB];
  char			path_file_draw[MAX_PATH_FILE_SCRIPT];
  char			path_file_res[MAX_PATH_FILE_RES];
  char			path_file_config[MAX_PATH_FILE_CONFIG];
  char			path_file_bus[MAX_PATH_FILE_BUS];
  char			path_file_var[MAX_PATH_FILE_VAR];
  char			path_file_dev[MAX_PATH_FILE_DEV];
  char			path_file_prt[MAX_PATH_FILE_PRT];
  char			path_file_gcd[MAX_PATH_FILE_GCD];

  char			prom_args_line[MAX_PROM_ARGS_LINE];

  char			computer[MAX_COMPUTER];

  node			*data;

  struct s_prom_script	*prev;
  struct s_prom_script	*next;
}			t_prom_script;


#define	MAX_LINK_NAME	256

/*
** Structure of Link
*/
typedef	struct		s_prom_link
{
  char			name[MAX_LINK_NAME];
  t_prom_script		*in;
  t_prom_script		*out;

  link_metaleto		*data;

  struct s_prom_link	*prev;
  struct s_prom_link	*next;
}			t_prom_link;


#define	MAX_NET_PATH	256
#define	MAX_CPT_PATH	256
#define	MAX_IVYBUS_ENVVAR 256

/*
** Main sctructure
*/
typedef	struct		s_promnet
{
  t_computer		*computer;
  computer		*tree_computer;

  t_prom_script		*prom_script;
  node			*tree_script;

  t_prom_link		*prom_link;
  link_metaleto		*tree_link;

  mxml_node_t		*net_tree;
  mxml_node_t		*cpt_tree;

  char			net_path[MAX_NET_PATH];
  char			cpt_path[MAX_CPT_PATH];
  char 			ivybus_envvar[MAX_IVYBUS_ENVVAR];
}			t_promnet;


/*
** Prototypes
*/
t_promnet	*promnet_init();

t_computer	*promnet_add_new_computer(t_promnet *promnet);
t_computer	*promnet_add_computer(t_promnet *promnet);
int		promnet_computer_set_name(t_computer *computer, char *name);
int		promnet_computer_set_address(t_computer *computer, char *address);
int		promnet_computer_set_login(t_computer *computer, char *login);
int		promnet_computer_set_cpu(t_computer *computer, char *cpu);
int		promnet_computer_set_path(t_computer *computer, char *path);
int		promnet_computer_set_information(t_computer *computer, char *information);
char		*promnet_computer_get_name(t_computer *computer);
char		*promnet_computer_get_address(t_computer *computer);
char		*promnet_computer_get_login(t_computer *computer);
char		*promnet_computer_get_cpu(t_computer *computer);
char		*promnet_computer_get_path(t_computer *computer);
char		*promnet_computer_get_information(t_computer *computer);
int		promnet_del_computer(t_promnet *promnet, t_computer *computer);
void		promnet_computer_copy(t_computer *src, t_computer *dst);
t_computer	*promnet_computer_get_next(t_promnet *promnet, t_computer *computer);

t_prom_script	*promnet_add_new_prom_script(t_promnet *promnet);
t_prom_script	*promnet_add_prom_script(t_promnet *promnet);
int		promnet_prom_script_set_logical_name(t_prom_script *prom_script, char *logical_name);
int		promnet_prom_script_set_path_prom_binary(t_prom_script *prom_script, char *path_prom_binary);
int		promnet_prom_script_set_path_prom_deploy(t_prom_script *prom_script, char *path_prom_binary);
void		promnet_prom_script_set_all_path(t_prom_script *prom_script, char *path, char *logical_name, char *extension);
int		promnet_prom_script_set_path_file_script(t_prom_script *prom_script, char *path_file_script);
int		promnet_prom_script_set_path_file_script_non_symb(t_prom_script *prom_script, char *path_file_script);
int		promnet_prom_script_set_path_file_draw(t_prom_script *prom_script, char *path_file_draw);
int		promnet_prom_script_set_path_file_res(t_prom_script *prom_script, char *path_file_res);
int		promnet_prom_script_set_path_file_config(t_prom_script *prom_script, char *path_file_config);
int		promnet_prom_script_set_path_file_bus(t_prom_script *prom_script, char *path_file_bus);
int		promnet_prom_script_set_path_file_var(t_prom_script *prom_script, char *path_file_var);
int		promnet_prom_script_set_path_file_dev(t_prom_script *prom_script, char *path_file_dev);
int		promnet_prom_script_set_path_file_prt(t_prom_script *prom_script, char *path_file_prt);
int		promnet_prom_script_set_path_file_gcd(t_prom_script *prom_script, char *path_file_gcd);
int		promnet_prom_script_set_prom_args_line(t_prom_script *prom_script, char *prom_args_line);
int		promnet_prom_script_set_computer(t_prom_script *prom_script, char *computer);
char		*promnet_prom_script_get_logical_name(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_prom_binary(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_prom_deploy(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_script(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_script_non_symb(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_draw(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_res(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_config(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_bus(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_var(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_dev(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_prt(t_prom_script *prom_script);
char		*promnet_prom_script_get_path_file_gcd(t_prom_script *prom_script);
char		*promnet_prom_script_get_prom_args_line(t_prom_script *prom_script);
char		*promnet_prom_script_get_computer(t_prom_script *prom_script);
int		promnet_del_prom_script(t_promnet *promnet, t_prom_script *prom_script);
t_prom_script	*promnet_prom_script_get_next(t_promnet *promnet, t_prom_script *prom_script);
t_prom_script	*promnet_prom_script_find_by_name(t_prom_script *prom_script, char *logical_name);


t_prom_link	*promnet_add_new_prom_link(t_promnet *promnet);
t_prom_link	*promnet_add_prom_link(t_promnet *promnet);
int		promnet_prom_link_set_name(t_prom_link *prom_link, char *name);
int		promnet_prom_link_set_input(t_prom_link *prom_link, t_prom_script *input);
int		promnet_prom_link_set_output(t_prom_link *prom_link, t_prom_script *output);
char		*promnet_prom_link_get_name(t_prom_link *prom_link);
t_prom_script	*promnet_prom_link_get_input(t_prom_link *prom_link);
t_prom_script	*promnet_prom_link_get_output(t_prom_link *prom_link);
int		promnet_del_prom_link(t_promnet *promnet, t_prom_link *prom_link);
t_prom_link	*promnet_prom_link_get_next(t_promnet *promnet, t_prom_link *prom_link);

int		promnet_load_prom_script(t_promnet *promnet);
int		promnet_load_prom_link(t_promnet *promnet);
int		promnet_load_file_net(t_promnet *promnet, char *path);

int		promnet_load_computer(t_promnet *promnet);
int		promnet_load_file_cpt(t_promnet *promnet, char *path);

int		promnet_save_file_net(t_promnet *promnet, char *path,char * ivybus_envvar);
int		promnet_save_file_cpt(t_promnet *promnet, char *path);

int		promnet_set_net_path(t_promnet *promnet, char *path);
int		promnet_set_cpt_path(t_promnet *promnet, char *path);
int		promnet_set_ivybus_envvar(t_promnet *promnet, char *envvar);

#endif /* _LIBPROMNET_H_ */
