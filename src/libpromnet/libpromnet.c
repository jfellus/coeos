#include "libpromnet.h"

t_promnet	*promnet_init()
{
  t_promnet	*promnet = NULL;
  
  if ((promnet = malloc(sizeof(t_promnet))) == NULL)
    {
      perror("promnet_init : malloc");
      return NULL;
    }
  memset(promnet, 0, sizeof(t_promnet));

  return promnet;
}

t_computer	*promnet_add_new_computer(t_promnet *promnet)
{
  t_computer	*computer = NULL;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_add_computer : invalid promnet pointer\n");
      return NULL;
    }

  computer = promnet->computer;

  for (; (computer != NULL) && (computer->next != NULL); computer = computer->next);


  if (computer == NULL)
    {
      if ((promnet->computer = malloc(sizeof(t_computer))) == NULL)
	{
	  perror("promnet_add_computer : malloc");
	  return NULL;
	}
      memset(promnet->computer, 0, sizeof(t_computer));
      computer = promnet->computer;
    }
  else if (computer->next == NULL)
    {
      if ((computer->next = malloc(sizeof(t_computer))) == NULL)
	{
	  perror("promnet_add_computer : malloc");
	  return NULL;
	}
      memset(computer->next, 0, sizeof(t_computer));
      computer->next->prev = computer;
      computer = computer->next;
    }
  else
    {
      fprintf(stderr, "promnet_add_computer : unknown error in computers list\n");
      return NULL;
    }
  promnet->tree_computer = addComputer(promnet->tree_computer);
  computer->data = promnet->tree_computer;
  return computer;
}

t_computer	*promnet_add_computer(t_promnet *promnet)
{
  t_computer	*computer = NULL;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_add_computer : invalid promnet pointer\n");
      return NULL;
    }

  computer = promnet->computer;

  for (; (computer != NULL) && (computer->next != NULL); computer = computer->next);


  if (computer == NULL)
    {
      if ((promnet->computer = malloc(sizeof(t_computer))) == NULL)
	{
	  perror("promnet_add_computer : malloc");
	  return NULL;
	}
      memset(promnet->computer, 0, sizeof(t_computer));
      computer = promnet->computer;
    }
  else if (computer->next == NULL)
    {
      if ((computer->next = malloc(sizeof(t_computer))) == NULL)
	{
	  perror("promnet_add_computer : malloc");
	  return NULL;
	}
      memset(computer->next, 0, sizeof(t_computer));
      computer->next->prev = computer;
      computer = computer->next;
    }
  else
    {
      fprintf(stderr, "promnet_add_computer : unknown error in computers list\n");
      return NULL;
    }
  return computer;
}

int		promnet_computer_set_name(t_computer *computer, char *name)
{
  int		len = -1;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_set_name : invalid computer pointer\n");
      return -1;
    }
  if (name == NULL)
    {
      fprintf(stderr, "promnet_computer_set_name : invalid name pointer\n");
      return -1;
    }
  len = strlen(name);
  if ((len >= 0) && (len < MAX_NAME))
    {
      memcpy(computer->name, name, len + 1);
      setNameComputer(computer->data, name);
      return 0;
    }
  return -1;
}

int		promnet_computer_set_address(t_computer *computer, char *address)
{
  int		len = -1;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_set_address : invalid computer pointer\n");
      return -1;
    }
  if (address == NULL)
    {
      fprintf(stderr, "promnet_computer_set_address : invalid address pointer\n");
      return -1;
    }
  len = strlen(address);
  if ((len >= 0) && (len < MAX_ADDRESS))
    {
      memcpy(computer->address, address, len + 1);
      setIp(computer->data, address);
      return 0;
    }
  return -1;
}

int		promnet_computer_set_login(t_computer *computer, char *login)
{
  int		len = -1;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_set_login : invalid computer pointer\n");
      return -1;
    }
  if (login == NULL)
    {
      fprintf(stderr, "promnet_computer_set_login : invalid login pointer\n");
      return -1;
    }
  len = strlen(login);
  if ((len >= 0) && (len < MAX_LOGIN))
    {
      memcpy(computer->login, login, len + 1);
      setLogin(computer->data, login);
      return 0;
    }
  return -1;
}

int		promnet_computer_set_cpu(t_computer *computer, char *cpu)
{
  int		len = -1;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_set_cpu : invalid computer pointer\n");
      return -1;
    }
  if (cpu == NULL)
    {
      fprintf(stderr, "promnet_computer_set_cpu : invalid cpu pointer\n");
      return -1;
    }
  len = strlen(cpu);
  if ((len >= 0) && (len < MAX_CPU))
    {
      memcpy(computer->cpu, cpu, len + 1);
      setCpu(computer->data, cpu);
      return 0;
    }
  return -1;
}

int		promnet_computer_set_path(t_computer *computer, char *path)
{
  int		len = -1;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_set_path : invalid computer pointer\n");
      return -1;
    }
  if (path == NULL)
    {
      fprintf(stderr, "promnet_computer_set_path : invalid path pointer\n");
      return -1;
    }
  len = strlen(path);
  if ((len >= 0) && (len < MAX_CPU))
    {
      memcpy(computer->path, path, len + 1);
      setPath(computer->data, path);
      return 0;
    }
  return -1;
}

int		promnet_computer_set_information(t_computer *computer, char *information)
{
  int		len = -1;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_set_information : invalid computer pointer\n");
      return -1;
    }
  if (information == NULL)
    {
      fprintf(stderr, "promnet_computer_set_information : invalid information pointer\n");
      return -1;
    }
  len = strlen(information);
  if ((len >= 0) && (len < MAX_INFORMATION))
    {
      memcpy(computer->information, information, len + 1);
      setInformations(computer->data, information);
      return 0;
    }
  return -1;
}

char		*promnet_computer_get_name(t_computer *computer)
{
  char		*name = NULL;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_get_name : invalid computer pointer\n");
      return NULL;
    }
  if ((name = strdup((void *)computer->name)) == NULL)
    {
      perror("promnet_computer_get_name : error");
      return NULL;
    }    
  return name;
}

char		*promnet_computer_get_address(t_computer *computer)
{
  char		*address = NULL;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_get_address : invalid computer pointer\n");
      return NULL;
    }
  if ((address = strdup((void *)computer->address)) == NULL)
    {
      perror("promnet_computer_get_address : error");
      return NULL;
    }    
  return address;
}

char		*promnet_computer_get_login(t_computer *computer)
{
  char		*login = NULL;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_get_login : invalid computer pointer\n");
      return NULL;
    }
  if ((login = strdup((void *)computer->login)) == NULL)
    {
      perror("promnet_computer_get_login : error");
      return NULL;
    }    
  return login;
}

char		*promnet_computer_get_cpu(t_computer *computer)
{
  char		*cpu= NULL;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_get_cpu : invalid computer pointer\n");
      return NULL;
    }
  if ((cpu = strdup((void *)computer->cpu)) == NULL)
    {
      perror("promnet_computer_get_cpu : error");
      return NULL;
    }    
  return cpu;
}

char		*promnet_computer_get_path(t_computer *computer)
{
  char		*path = NULL;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_get_path : invalid computer pointer\n");
      return NULL;
    }
  if ((path = strdup((void *)computer->path)) == NULL)
    {
      perror("promnet_computer_get_path : error");
      return NULL;
    }    
  return path;
}

char		*promnet_computer_get_information(t_computer *computer)
{
  char		*information = NULL;

  if (computer == NULL)
    {
      fprintf(stderr, "promnet_computer_get_information : invalid computer pointer\n");
      return NULL;
    }
  if ((information = strdup((void *)computer->information)) == NULL)
    {
      perror("promnet_computer_get_information : error");
      return NULL;
    }    
  return information;
}

int		promnet_del_computer(t_promnet *promnet, t_computer *computer)
{
  t_computer	*pcomputer = NULL;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_del_computer : invalid promnet pointer\n");
      return -1;
    }
  if (computer == NULL)
    {
      fprintf(stderr, "promnet_del_computer : invalid computer pointer\n");
      return -1;
    }

  pcomputer = promnet->computer;

  for (; ( pcomputer != NULL) && (pcomputer != computer); pcomputer = pcomputer->next);

  if (pcomputer == NULL)
    {
      fprintf(stderr, "promnet_del_computer : unknown computer\n");
      return -1;
    }

  /*
  ** Here pcomputer == computer
  */
  if (promnet->computer == pcomputer)
    {
      promnet->computer = pcomputer->next;
      if (promnet->computer)
	promnet->computer->prev = NULL;
    }
  else
    {
      if (computer->next != NULL)
	computer->next->prev = pcomputer->prev;
      if (computer->prev != NULL)
	computer->prev->next = pcomputer->next;
      if ((computer->next == NULL) && (computer->prev == NULL))
	promnet->computer = NULL;
    }
  promnet->tree_computer = delComputer(computer->data);
  free(computer);
  computer = pcomputer = NULL;
  return 0;
}

void		promnet_computer_copy(t_computer *src, t_computer *dst)
{
  char		*val = NULL;

  val = promnet_computer_get_name(src);
  promnet_computer_set_name(dst, val);
  free(val);
  val = promnet_computer_get_address(src);
  promnet_computer_set_address(dst, val);
  free(val);
  val = promnet_computer_get_login(src);
  promnet_computer_set_login(dst, val);
  free(val);
  val = promnet_computer_get_cpu(src);
  promnet_computer_set_cpu(dst, val);
  free(val);
  val = promnet_computer_get_path(src);
  promnet_computer_set_path(dst, val);
  free(val);
  val = promnet_computer_get_information(src);
  promnet_computer_set_information(dst, val);
  free(val);
}

t_computer	*promnet_computer_get_next(t_promnet *promnet, t_computer *computer)
{
  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_computer_get_next : invalid promnet pointer\n");
      return NULL;
    }
  if (computer == NULL)
    return promnet->computer;
  else
    return computer->next;
}

t_prom_script	*promnet_add_new_prom_script(t_promnet *promnet)
{
  t_prom_script	*prom_script = NULL;

   if (promnet == NULL)
    {
      fprintf(stderr, "promnet_add_prom_script : invalid promnet pointer\n");
      return NULL;
    }

  prom_script = promnet->prom_script;

  for (; (prom_script != NULL) && (prom_script->next != NULL); prom_script = prom_script->next);


  if (prom_script == NULL)
    {
      if ((promnet->prom_script = malloc(sizeof(t_prom_script))) == NULL)
	{
	  perror("promnet_add_prom_script : malloc");
	  return NULL;
	}
      memset(promnet->prom_script, 0, sizeof(t_prom_script));
      prom_script = promnet->prom_script;
    }
  else if (prom_script->next == NULL)
    {
      if ((prom_script->next = malloc(sizeof(t_prom_script))) == NULL)
	{
	  perror("promnet_add_prom_script : malloc");
	  return NULL;
	}
      memset(prom_script->next, 0, sizeof(t_prom_script));
      prom_script->next->prev = prom_script;
      prom_script = prom_script->next;
    }
  else
    {
      fprintf(stderr, "promnet_add_prom_script : unknown error in prom_scripts list\n");
      return NULL;
    }
  promnet->tree_script = addNode(promnet->tree_script);
  prom_script->data = promnet->tree_script;
  return prom_script;
}

t_prom_script	*promnet_add_prom_script(t_promnet *promnet)
{
  t_prom_script	*prom_script = NULL;

   if (promnet == NULL)
    {
      fprintf(stderr, "promnet_add_prom_script : invalid promnet pointer\n");
      return NULL;
    }

  prom_script = promnet->prom_script;

  for (; (prom_script != NULL) && (prom_script->next != NULL); prom_script = prom_script->next);


  if (prom_script == NULL)
    {
      if ((promnet->prom_script = malloc(sizeof(t_prom_script))) == NULL)
	{
	  perror("promnet_add_prom_script : malloc");
	  return NULL;
	}
      memset(promnet->prom_script, 0, sizeof(t_prom_script));
      prom_script = promnet->prom_script;
    }
  else if (prom_script->next == NULL)
    {
      if ((prom_script->next = malloc(sizeof(t_prom_script))) == NULL)
	{
	  perror("promnet_add_prom_script : malloc");
	  return NULL;
	}
      memset(prom_script->next, 0, sizeof(t_prom_script));
      prom_script->next->prev = prom_script;
      prom_script = prom_script->next;
    }
  else
    {
      fprintf(stderr, "promnet_add_prom_script : unknown error in prom_scripts list\n");
      return NULL;
    }
  return prom_script;
}

int		promnet_prom_script_set_logical_name(t_prom_script *prom_script, char *logical_name)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_logical_name : invalid prom_script pointer\n");
      return -1;
    }
  if (logical_name == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_logical_name : invalid logical_name pointer\n");
      return -1;
    }
  len = strlen(logical_name);
  if ((len >= 0) && (len < MAX_LOGICAL_NAME))
    {
      memcpy(prom_script->logical_name, logical_name, len + 1);
      setNameNode(prom_script->data, logical_name);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_prom_deploy(t_prom_script *prom_script, char *path_prom_deploy)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_prom_deploy : invalid prom_script pointer\n");
      return -1;
    }
  if (path_prom_deploy == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_prom_deploy : invalid path_prom_deploy pointer\n");
      return -1;
    }
  len = strlen(path_prom_deploy);
  if ((len >= 0) && (len < MAX_PATH_PROM_DEPLOY))
    {
      memcpy(prom_script->path_prom_deploy, path_prom_deploy, len + 1);
      setDeploy(prom_script->data, path_prom_deploy);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_prom_binary(t_prom_script *prom_script, char *path_prom_binary)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_prom_binary : invalid prom_script pointer\n");
      return -1;
    }
  if (path_prom_binary == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_prom_binary : invalid path_prom_binary pointer\n");
      return -1;
    }
  len = strlen(path_prom_binary);
  if ((len >= 0) && (len < MAX_PATH_PROM_BINARY))
    {
      memcpy(prom_script->path_prom_binary, path_prom_binary, len + 1);
      setCmd(prom_script->data, path_prom_binary);
      return 0;
    }
  return -1;
}

void		promnet_prom_script_set_all_path(t_prom_script *prom_script, char *path, char *logical_name, char *extension)
{
  char		all_path[MAX_ALL_PATH];

  if (strlen(prom_script->path_file_script) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s.%s", path, prom_script->logical_name, extension);
      promnet_prom_script_set_path_file_script(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_script_non_symb) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_NON_SYMB);
      promnet_prom_script_set_path_file_script_non_symb(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_draw) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_DRAW);
      promnet_prom_script_set_path_file_draw(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_res) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_RES);
      promnet_prom_script_set_path_file_res(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_config) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_CONFIG);
      promnet_prom_script_set_path_file_config(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_bus) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_BUS);
      promnet_prom_script_set_path_file_bus(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_var) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_VAR);
      promnet_prom_script_set_path_file_var(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_dev) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_DEV);
      promnet_prom_script_set_path_file_dev(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_gcd) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_GCD);
      promnet_prom_script_set_path_file_gcd(prom_script, all_path);
    }
  if (strlen(prom_script->path_file_prt) <= 0)
    {
      memset(all_path, 0, MAX_ALL_PATH * sizeof(char));
      sprintf(all_path, "%s/%s%s", path, prom_script->logical_name, EXT_SCRIPT_PRT);
      promnet_prom_script_set_path_file_prt(prom_script, all_path);
    }
}

int		promnet_prom_script_set_path_file_script(t_prom_script *prom_script, char *path_file_script)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_script : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_script : invalid path_file_script pointer\n");
      return -1;
    }
  len = strlen(path_file_script);
  if ((len >= 0) && (len < MAX_PATH_FILE_SCRIPT))
    {
      memcpy(prom_script->path_file_script, path_file_script, len + 1);
      setScript(prom_script->data, path_file_script);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_script_non_symb(t_prom_script *prom_script, char *path_file_script_non_symb)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_script_non_symb : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_script_non_symb == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_script_non_symb : invalid path_file_script pointer\n");
      return -1;
    }
  len = strlen(path_file_script_non_symb);
  if ((len >= 0) && (len < MAX_PATH_FILE_SCRIPT_NON_SYMB))
    {
      memcpy(prom_script->path_file_script_non_symb, path_file_script_non_symb, len + 1);
      setScriptNonSymb(prom_script->data, path_file_script_non_symb);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_draw(t_prom_script *prom_script, char *path_file_draw)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_draw : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_draw == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_draw : invalid path_file_draw pointer\n");
      return -1;
    }
  len = strlen(path_file_draw);
  if ((len >= 0) && (len < MAX_PATH_FILE_DRAW))
    {
      memcpy(prom_script->path_file_draw, path_file_draw, len + 1);
      setDraw(prom_script->data, path_file_draw);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_res(t_prom_script *prom_script, char *path_file_res)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_res : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_res == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_res : invalid path_file_res pointer\n");
      return -1;
    }
  len = strlen(path_file_res);
  if ((len >= 0) && (len < MAX_PATH_FILE_RES))
    {
      memcpy(prom_script->path_file_res, path_file_res, len + 1);
      setRes(prom_script->data, path_file_res);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_config(t_prom_script *prom_script, char *path_file_config)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_config : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_config == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_config : invalid path_file_config pointer\n");
      return -1;
    }
  len = strlen(path_file_config);
  if ((len >= 0) && (len < MAX_PATH_FILE_CONFIG))
    {
      memcpy(prom_script->path_file_config, path_file_config, len + 1);
      setConfig(prom_script->data, path_file_config);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_bus(t_prom_script *prom_script, char *path_file_bus)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_bus : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_bus == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_bus : invalid path_file_bus pointer\n");
      return -1;
    }
  len = strlen(path_file_bus);
  if ((len >= 0) && (len < MAX_PATH_FILE_BUS))
    {
      memcpy(prom_script->path_file_bus, path_file_bus, len + 1);
      setBus(prom_script->data, path_file_bus);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_var(t_prom_script *prom_script, char *path_file_var)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_var : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_var == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_var : invalid path_file_var pointer\n");
      return -1;
    }
  len = strlen(path_file_var);
  if ((len >= 0) && (len < MAX_PATH_FILE_VAR))
    {
      memcpy(prom_script->path_file_var, path_file_var, len + 1);
      setVar(prom_script->data, path_file_var);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_dev(t_prom_script *prom_script, char *path_file_dev)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_dev : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_dev == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_dev : invalid path_file_dev pointer\n");
      return -1;
    }
  len = strlen(path_file_dev);
  if ((len >= 0) && (len < MAX_PATH_FILE_DEV))
    {
      memcpy(prom_script->path_file_dev, path_file_dev, len + 1);
      setDev(prom_script->data, path_file_dev);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_prt(t_prom_script *prom_script, char *path_file_prt)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_prt : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_prt == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_prt : invalid path_file_prt pointer\n");
      return -1;
    }
  len = strlen(path_file_prt);
  if ((len >= 0) && (len < MAX_PATH_FILE_PRT))
    {
      memcpy(prom_script->path_file_prt, path_file_prt, len + 1);
      setPrt(prom_script->data, path_file_prt);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_path_file_gcd(t_prom_script *prom_script, char *path_file_gcd)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_gcd : invalid prom_script pointer\n");
      return -1;
    }
  if (path_file_gcd == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_path_file_gcd : invalid path_file_gcd pointer\n");
      return -1;
    }
  len = strlen(path_file_gcd);
  if ((len >= 0) && (len < MAX_PATH_FILE_GCD))
    {
      memcpy(prom_script->path_file_gcd, path_file_gcd, len + 1);
      setGcd(prom_script->data, path_file_gcd);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_prom_args_line(t_prom_script *prom_script, char *prom_args_line)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_prom_args_line : invalid prom_script pointer\n");
      return -1;
    }
  if (prom_args_line == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_prom_args_line : invalid prom_args_line pointer\n");
      return -1;
    }
  len = strlen(prom_args_line);
  if ((len >= 0) && (len < MAX_PROM_ARGS_LINE))
    {
      memcpy(prom_script->prom_args_line, prom_args_line, len + 1);
      setOptions(prom_script->data, prom_args_line);
      return 0;
    }
  return -1;
}

int		promnet_prom_script_set_computer(t_prom_script *prom_script, char *computer)
{
  int		len = -1;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_computer : invalid prom_script pointer\n");
      return -1;
    }
  if (computer == NULL)
    {
      fprintf(stderr, "promnet_prom_script_set_computer : invalid computer pointer\n");
      return -1;
    }
  len = strlen(computer);
  if ((len >= 0) && (len < MAX_COMPUTER))
    {
      memcpy(prom_script->computer, computer, len + 1);
      setComputer(prom_script->data, computer);
      return 0;
    }
  return -1;
}

char		*promnet_prom_script_get_logical_name(t_prom_script *prom_script)
{
  char		*logical_name = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_logical_name : invalid prom_script pointer\n");
      return NULL;
    }
  if ((logical_name = strdup((void *)prom_script->logical_name)) == NULL)
    {
      perror("promnet_prom_script_get_logical_name : error");
      return NULL;
    }    
  return logical_name;
}

char		*promnet_prom_script_get_path_prom_deploy(t_prom_script *prom_script)
{
  char		*path_prom_deploy = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_prom_deploy : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_prom_deploy = strdup((void *)prom_script->path_prom_deploy)) == NULL)
    {
      perror("promnet_prom_script_get_path_prom_deploy : error");
      return NULL;
    }    
  return path_prom_deploy;
}

char		*promnet_prom_script_get_path_prom_binary(t_prom_script *prom_script)
{
  char		*path_prom_binary = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_prom_binary : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_prom_binary = strdup((void *)prom_script->path_prom_binary)) == NULL)
    {
      perror("promnet_prom_script_get_path_prom_binary : error");
      return NULL;
    }    
  return path_prom_binary;
}

char		*promnet_prom_script_get_path_file_script(t_prom_script *prom_script)
{
  char		*path_file_script = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_script : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_script = strdup((void *)prom_script->path_file_script)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_script : error");
      return NULL;
    }    
  return path_file_script;
}

char		*promnet_prom_script_get_path_file_script_non_symb(t_prom_script *prom_script)
{
  char		*path_file_script_non_symb = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_script_non_symb : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_script_non_symb = strdup((void *)prom_script->path_file_script_non_symb)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_script_non_symb : error");
      return NULL;
    }    
  return path_file_script_non_symb;
}

char		*promnet_prom_script_get_path_file_draw(t_prom_script *prom_script)
{
  char		*path_file_draw = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_draw : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_draw = strdup((void *)prom_script->path_file_draw)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_draw : error");
      return NULL;
    }    
  return path_file_draw;
}

char		*promnet_prom_script_get_path_file_res(t_prom_script *prom_script)
{
  char		*path_file_res = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_res : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_res = strdup((void *)prom_script->path_file_res)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_res : error");
      return NULL;
    }    
  return path_file_res;
}

char		*promnet_prom_script_get_path_file_config(t_prom_script *prom_script)
{
  char		*path_file_config = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_config : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_config = strdup((void *)prom_script->path_file_config)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_config : error");
      return NULL;
    }    
  return path_file_config;
}

char		*promnet_prom_script_get_path_file_bus(t_prom_script *prom_script)
{
  char		*path_file_bus = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_bus : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_bus = strdup((void *)prom_script->path_file_bus)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_bus : error");
      return NULL;
    }    
  return path_file_bus;
}

char		*promnet_prom_script_get_path_file_var(t_prom_script *prom_script)
{
  char		*path_file_var = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_var : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_var = strdup((void *)prom_script->path_file_var)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_var : error");
      return NULL;
    }    
  return path_file_var;
}

char		*promnet_prom_script_get_path_file_dev(t_prom_script *prom_script)
{
  char		*path_file_dev = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_dev : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_dev = strdup((void *)prom_script->path_file_dev)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_dev : error");
      return NULL;
    }    
  return path_file_dev;
}

char		*promnet_prom_script_get_path_file_prt(t_prom_script *prom_script)
{
  char		*path_file_prt = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_prt : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_prt = strdup((void *)prom_script->path_file_prt)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_prt : error");
      return NULL;
    }    
  return path_file_prt;
}

char		*promnet_prom_script_get_path_file_gcd(t_prom_script *prom_script)
{
  char		*path_file_gcd = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_path_file_gcd : invalid prom_script pointer\n");
      return NULL;
    }
  if ((path_file_gcd = strdup((void *)prom_script->path_file_gcd)) == NULL)
    {
      perror("promnet_prom_script_get_path_file_gcd : error");
      return NULL;
    }    
  return path_file_gcd;
}

char		*promnet_prom_script_get_prom_args_line(t_prom_script *prom_script)
{
  char		*prom_args_line = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_prom_args_line : invalid prom_script pointer\n");
      return NULL;
    }
  if ((prom_args_line = strdup((void *)prom_script->prom_args_line)) == NULL)
    {
      perror("promnet_prom_script_get_prom_args_line : error");
      return NULL;
    }    
  return prom_args_line;
}

char		*promnet_prom_script_get_computer(t_prom_script *prom_script)
{
  char		*computer = NULL;

  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_computer : invalid prom_script pointer\n");
      return NULL;
    }
  if ((computer = strdup((void *)prom_script->computer)) == NULL)
    {
      perror("promnet_prom_script_get_computer : error");
      return NULL;
    }    
  return computer;
}

int		promnet_del_prom_script(t_promnet *promnet, t_prom_script *prom_script)
{
  t_prom_script	*pprom_script = NULL;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_del_prom_script : invalid promnet pointer\n");
      return -1;
    }
  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_del_prom_script : invalid prom_script pointer\n");
      return -1;
    }

  pprom_script = promnet->prom_script;

  for (; ( pprom_script != NULL) && (pprom_script != prom_script); pprom_script = pprom_script->next);

  if (pprom_script == NULL)
    {
      fprintf(stderr, "promnet_del_prom_script : unknown prom_script\n");
      return -1;
    }

  /*
  ** Here pprom_script == prom_script
  */
  if (promnet->prom_script == pprom_script)
    {
      promnet->prom_script = pprom_script->next;
      if (promnet->prom_script)
	promnet->prom_script->prev = NULL;
    }
  else
    {
      if (prom_script->next != NULL)
	prom_script->next->prev = pprom_script->prev;
      if (prom_script->prev != NULL)
	prom_script->prev->next = pprom_script->next;
      if ((prom_script->next == NULL) && (prom_script->prev == NULL))
	promnet->prom_script = NULL;
    }
  promnet->tree_script = delNode(prom_script->data);
  free(prom_script);
  prom_script = pprom_script = NULL;

  return 0;
}

t_prom_script	*promnet_prom_script_get_next(t_promnet *promnet, t_prom_script *prom_script)
{
   static t_prom_script *last_script = NULL;
  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_prom_script_get_next : invalid promnet pointer\n");
      return NULL;
    }
  if (prom_script == NULL)
     if (last_script == NULL)
     {
	last_script = promnet->prom_script;
	return promnet->prom_script;
     }
     else
     {
	if (last_script->next != NULL)
	{
	   last_script = last_script->next;
	   return last_script;
	}
	else
	   return last_script->next;
     }
  else
  {
     if (prom_script->next != NULL)
	last_script = prom_script->next;
    return prom_script->next;
  }
}

t_prom_script	*promnet_prom_script_find_by_name(t_prom_script *prom_script, char *logical_name)
{
  if (prom_script == NULL)
    {
      fprintf(stderr, "promnet_prom_script_find_by_name : invalid prom_script pointer\n");
      return NULL;
    }

  for (;prom_script != NULL; prom_script = prom_script->next)
    {
      if (!strncmp(logical_name, prom_script->logical_name, MAX_LOGICAL_NAME))
	return prom_script;
    }
  return NULL;
}

t_prom_link	*promnet_add_new_prom_link(t_promnet *promnet)
{
  t_prom_link	*prom_link = NULL;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_add_prom_link : invalid promnet pointer\n");
      return NULL;
    }

  prom_link = promnet->prom_link;

  for (; (prom_link != NULL) && (prom_link->next != NULL); prom_link = prom_link->next);

  if (prom_link == NULL)
    {
      if ((promnet->prom_link = malloc(sizeof(t_prom_link))) == NULL)
	{
	  perror("promnet_add_prom_link : malloc");
	  return NULL;
	}
      memset(promnet->prom_link, 0, sizeof(t_prom_link));
      prom_link = promnet->prom_link;
    }
  else if (prom_link->next == NULL)
    {
      if ((prom_link->next = malloc(sizeof(t_prom_link))) == NULL)
	{
	  perror("promnet_add_prom_link : malloc");
	  return NULL;
	}
      memset(prom_link->next, 0, sizeof(t_prom_link));
      prom_link->next->prev = prom_link;
      prom_link = prom_link->next;
    }
  else
    {
      fprintf(stderr, "promnet_add_prom_link : unknown error in prom_links list\n");
      return NULL;
    }
  promnet->tree_link = addLink(promnet->tree_link);
  prom_link->data = promnet->tree_link;
  return prom_link;
}

t_prom_link	*promnet_add_prom_link(t_promnet *promnet)
{
  t_prom_link	*prom_link = NULL;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_add_prom_link : invalid promnet pointer\n");
      return NULL;
    }

  prom_link = promnet->prom_link;

  for (; (prom_link != NULL) && (prom_link->next != NULL); prom_link = prom_link->next);

  if (prom_link == NULL)
    {
      if ((promnet->prom_link = malloc(sizeof(t_prom_link))) == NULL)
	{
	  perror("promnet_add_prom_link : malloc");
	  return NULL;
	}
      memset(promnet->prom_link, 0, sizeof(t_prom_link));
      prom_link = promnet->prom_link;
    }
  else if (prom_link->next == NULL)
    {
      if ((prom_link->next = malloc(sizeof(t_prom_link))) == NULL)
	{
	  perror("promnet_add_prom_link : malloc");
	  return NULL;
	}
      memset(prom_link->next, 0, sizeof(t_prom_link));
      prom_link->next->prev = prom_link;
      prom_link = prom_link->next;
    }
  else
    {
      fprintf(stderr, "promnet_add_prom_link : unknown error in prom_links list\n");
      return NULL;
    }
  return prom_link;
}

int		promnet_prom_link_set_name(t_prom_link *prom_link, char *name)
{
  int		len = -1;

  if (prom_link == NULL)
    {
      fprintf(stderr, "promnet_prom_link_set_name : invalid prom_link pointer\n");
      return -1;
    }
  if (name == NULL)
    {
      fprintf(stderr, "promnet_prom_link_set_name : invalid name pointer\n");
      return -1;
    }
  len = strlen(name);
  if ((len >= 0) && (len < MAX_LINK_NAME))
    {
      memcpy(prom_link->name, name, len + 1);
      setNameLink(prom_link->data, name);
      return 0;
    }
  return -1;
}

int		promnet_prom_link_set_input(t_prom_link *prom_link, t_prom_script *input)
{
  if (prom_link == NULL)
    {
      fprintf(stderr, "promnet_prom_link_set_input : invalid prom_link pointer\n");
      return -1;
    }
  if (input == NULL)
    {
      fprintf(stderr, "promnet_prom_link_set_input : invalid input script pointer\n");
      return -1;
    }
  prom_link->in = input;
  setInput(prom_link->data, input->logical_name);
  return 0;
}

int		promnet_prom_link_set_output(t_prom_link *prom_link, t_prom_script *output)
{
  if (prom_link == NULL)
    {
      fprintf(stderr, "promnet_prom_link_set_output : invalid prom_link pointer\n");
      return -1;
    }
  if (output == NULL)
    {
      fprintf(stderr, "promnet_prom_link_set_output : invalid output script pointer\n");
      return -1;
    }
  prom_link->out = output;
  setOutput(prom_link->data, output->logical_name);
  return 0;
}

char		*promnet_prom_link_get_name(t_prom_link *prom_link)
{
  char		*name = NULL;

  if (prom_link == NULL)
    {
      fprintf(stderr, "promnet_prom_link_get_name : invalid prom_link pointer\n");
      return NULL;
    }
  if ((name = strdup((void *)prom_link->name)) == NULL)
    {
      perror("promnet_prom_link_get_name : error");
      return NULL;
    }    
  return name;
}

t_prom_script	*promnet_prom_link_get_input(t_prom_link *prom_link)
{
  if (prom_link == NULL)
    {
      fprintf(stderr, "promnet_prom_link_get_input : invalid prom_link pointer\n");
      return NULL;
    }
  return prom_link->in;
}

t_prom_script	*promnet_prom_link_get_output(t_prom_link *prom_link)
{
  if (prom_link == NULL)
    {
      fprintf(stderr, "promnet_prom_link_get_output : invalid prom_link pointer\n");
      return NULL;
    }
  return prom_link->out;
}

int		promnet_del_prom_link(t_promnet *promnet, t_prom_link *prom_link)
{
  t_prom_link	*pprom_link = NULL;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_del_prom_link : invalid promnet pointer\n");
      return -1;
    }
  if (prom_link == NULL)
    {
      fprintf(stderr, "promnet_del_prom_link : invalid prom_link pointer\n");
      return -1;
    }

  pprom_link = promnet->prom_link;

  for (; ( pprom_link != NULL) && (pprom_link != prom_link); pprom_link = pprom_link->next);

  if (pprom_link == NULL)
    {
      fprintf(stderr, "promnet_del_prom_link : unknown prom_link\n");
      return -1;
    }

  /*
  ** Here pprom_link == prom_link
  */

  if (promnet->prom_link == pprom_link)
    {
      promnet->prom_link = pprom_link->next;
      if (promnet->prom_link)
	promnet->prom_link->prev = NULL;
    }
  else
    {
      if (prom_link->next != NULL)
	prom_link->next->prev = pprom_link->prev;
      if (prom_link->prev != NULL)
	prom_link->prev->next = pprom_link->next;
      if ((prom_link->next == NULL) && (prom_link->prev == NULL))
	promnet->prom_link = NULL;
    }
  promnet->tree_link = delLink(prom_link->data);
  free(prom_link);
  prom_link = pprom_link = NULL;

  return 0;
}

t_prom_link	*promnet_prom_link_get_next(t_promnet *promnet, t_prom_link *prom_link)
{
  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_prom_link_get_next : invalid promnet pointer\n");
      return NULL;
    }
  if (prom_link == NULL)
    return promnet->prom_link;
  else
    return prom_link->next;
}

int		promnet_load_prom_script(t_promnet *promnet)
{
  t_prom_script	*prom_script = NULL;
  node		*data = NULL;

  for (data = promnet->tree_script; data != NULL; data = getNextNode(data))
    {
      char		*val = NULL;

      if ((prom_script = promnet_add_prom_script(promnet)) == NULL)
	{
	  fprintf(stderr, "promnet_load_prom_script : invalid promnet pointer\n");
	  return -1;	  
	}

      prom_script->data = data;

      val = getNameNode(prom_script->data);
      promnet_prom_script_set_logical_name(prom_script, val);
      free(val);

      val = getCmd(prom_script->data);
      promnet_prom_script_set_path_prom_binary(prom_script, val);
      free(val);

      val = getDeploy(prom_script->data);
      promnet_prom_script_set_path_prom_deploy(prom_script, val);
      free(val);

      val = getScript(prom_script->data);
      promnet_prom_script_set_path_file_script(prom_script, val);
      free(val);

      val = getScriptNonSymb(prom_script->data);
      promnet_prom_script_set_path_file_script_non_symb(prom_script, val);
      free(val);

      val = getDraw(prom_script->data);
      promnet_prom_script_set_path_file_draw(prom_script, val);
      free(val);

      val = getRes(prom_script->data);
      promnet_prom_script_set_path_file_res(prom_script, val);
      free(val);

      val = getConfig(prom_script->data);
      promnet_prom_script_set_path_file_config(prom_script, val);
      free(val);

      val = getBus(prom_script->data);
      promnet_prom_script_set_path_file_bus(prom_script, val);
      free(val);

      val = getVar(prom_script->data);
      promnet_prom_script_set_path_file_var(prom_script, val);
      free(val);

      val = getDev(prom_script->data);
      promnet_prom_script_set_path_file_dev(prom_script, val);
      free(val);

      val = getPrt(prom_script->data);
      promnet_prom_script_set_path_file_prt(prom_script, val);
      free(val);

      val = getGcd(prom_script->data);
      promnet_prom_script_set_path_file_gcd(prom_script, val);
      free(val);

      val = getOptions(prom_script->data);
      promnet_prom_script_set_prom_args_line(prom_script, val);
      free(val);

      val = getComputer(prom_script->data);
      promnet_prom_script_set_computer(prom_script, val);
      free(val); 
    } 
  
  return 0;
}

int		promnet_load_prom_link(t_promnet *promnet)
{
  t_prom_link	*prom_link = NULL;
  t_prom_script	*prom_script = NULL;
  link_metaleto		*data = NULL;

  for (data = promnet->tree_link; data != NULL; data = getNextLink(data))
    {
      char		*val = NULL;

      if ((prom_link = promnet_add_prom_link(promnet)) == NULL)
	{
	  fprintf(stderr, "promnet_load_prom_link : invalid promnet pointer\n");
	  return -1;	  
	}

      prom_link->data = data;

      val = getNameLink(prom_link->data);
      promnet_prom_link_set_name(prom_link, val);
      free(val); 

      val = getInput(prom_link->data);
      prom_script = promnet_prom_script_find_by_name(promnet->prom_script, val);
      promnet_prom_link_set_input(prom_link, prom_script);
      free(val);

      val = getOutput(prom_link->data);
      prom_script = promnet_prom_script_find_by_name(promnet->prom_script, val);
      promnet_prom_link_set_output(prom_link, prom_script);
      free(val);
    } 
  
  return 0;
}

int		promnet_load_file_net(t_promnet *promnet, char *path)
{
  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_load_file_net : invalid promnet pointer\n");
      return 0;
    }
  


  if ((promnet->net_tree = loadFile(path)) == NULL)
    {
      fprintf(stderr, "promnet_load_file_net : loading script tree failed\n");
      return -1;
    }

  getIvy(promnet->ivybus_envvar,promnet->net_tree);
  
  if ((promnet->tree_script = getNetwork(promnet->net_tree)) == NULL)
    {
      fprintf(stderr, "promnet_load_file_net : no scripts in the net file %s\n", path);
    }
  else
    {
      if (promnet_load_prom_script(promnet) == -1)
	{
	  fprintf(stderr, "promnet_load_file_net : loading scripts failed\n");
	  return -1;
	}
    }

  if ((promnet->tree_link = getLinks(promnet->net_tree)) == NULL)
    {
      fprintf(stderr, "promnet_load_file_net : no links in the net file %s\n", path);
    }
  else
    {
      if (promnet_load_prom_link(promnet) == -1)
	{
	  fprintf(stderr, "promnet_load_file_net : loading links failed\n");
	  return -1;
	}
    }

  promnet_set_net_path(promnet, path);
  return 0;
}

int		promnet_load_computer(t_promnet *promnet)
{
  t_computer	*prom_computer = NULL;
  computer	*data = NULL;

  for (data = promnet->tree_computer; data != NULL; data = getNextComputer(data))
    {
      char		*val = NULL;

      if ((prom_computer = promnet_add_computer(promnet)) == NULL)
	{
	  fprintf(stderr, "promnet_load_computer : invalid promnet pointer\n");
	  return -1;	  
	}

      prom_computer->data = data;

      val = getNameComputer(prom_computer->data);
      promnet_computer_set_name(prom_computer, val);
      free(val); 

      val = getIp(prom_computer->data);
      promnet_computer_set_address(prom_computer, val);
      free(val); 

      val = getLogin(prom_computer->data);
      promnet_computer_set_login(prom_computer, val);
      free(val);

      val = getPath(prom_computer->data);
      promnet_computer_set_path(prom_computer, val);
      free(val);

      val = getCpu(prom_computer->data);
      promnet_computer_set_cpu(prom_computer, val);
      free(val);

      val = getInformations(prom_computer->data);
      promnet_computer_set_information(prom_computer, val);
      free(val);
    }
  
  return 0;
}

int		promnet_load_file_cpt(t_promnet *promnet, char *path)
{
  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_load_file_cpt : invalid promnet pointer\n");
      return -1;
    }

  if ((promnet->cpt_tree = loadFile(path)) == NULL)
    {
      fprintf(stderr, "promnet_load_file_cpt : loading cpt tree failed\n");
      return -1;
    }

  if ((promnet->tree_computer = getComputers(promnet->cpt_tree)) == NULL)
    {
      fprintf(stderr, "promnet_load_file_cpt : no computers in cpt file %s\n", path);
    }
  else
    {
      if (promnet_load_computer(promnet) == -1)
	{
	  fprintf(stderr, "promnet_load_file_cpt : loading computers failed\n");
	  return -1;
	}
    }
  promnet_set_cpt_path(promnet, path);
  return 0;
}

int		promnet_save_file_net(t_promnet *promnet, char *path, char * ivybus_envvar)
{
  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_save_file_net : invalid promnet pointer\n");
      return -1;
    }

  saveNetFile(promnet->tree_script, promnet->tree_link, path, ivybus_envvar);
  promnet_set_net_path(promnet, path);
  promnet_set_ivybus_envvar(promnet, ivybus_envvar);
  return 0;
}

int		promnet_save_file_cpt(t_promnet *promnet, char *path)
{
  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_save_file_cpt : invalid promnet pointer\n");
      return -1;
    }

  saveComputerFile(promnet->tree_computer, path);
  promnet_set_cpt_path(promnet, path);
  return 0;
}

int		promnet_set_net_path(t_promnet *promnet, char *path)
{
  int		len = -1;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_set_net_path : invalid promnet pointer\n");
      return -1;
    }
  if (path == NULL)
    {
      fprintf(stderr, "promnet_set_net_path : invalid path pointer\n");
      return -1;
    }
  len = strlen(path);
  if ((len >= 0) && (len < MAX_NET_PATH))
    {
      memcpy(promnet->net_path, path, len + 1);
      return 0;
    }
  return -1;
}

int		promnet_set_ivybus_envvar(t_promnet *promnet, char *ivybus_envvar)
{
  int		len = -1;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_set_net_path : invalid promnet pointer\n");
      return -1;
    }
  if (ivybus_envvar == NULL)
    {
      fprintf(stderr, "promnet_set_net_path : invalid ivybus_envar pointer\n");
      return -1;
    }
  len = strlen(ivybus_envvar);
  if ((len >= 0) && (len < MAX_IVYBUS_ENVVAR))
    {
      memcpy(promnet->ivybus_envvar, ivybus_envvar, len + 1);
      return 0;
    }
  return -1;
}


int		promnet_set_cpt_path(t_promnet *promnet, char *path)
{
  int		len = -1;

  if (promnet == NULL)
    {
      fprintf(stderr, "promnet_set_cpt_path : invalid promnet pointer\n");
      return -1;
    }
  if (path == NULL)
    {
      fprintf(stderr, "promnet_set_cpt_path : invalid path pointer\n");
      return -1;
    }
  len = strlen(path);
  if ((len >= 0) && (len < MAX_CPT_PATH))
    {
      memcpy(promnet->cpt_path, path, len + 1);
      return 0;
    }
  return -1;
}

