#ifndef BLOCK_CREATE_H
#define BLOCK_CREATE_H

#include "gennet.h"
#include "graphic_Tx.h"
#include "reseau.h"
#include <search.h>

extern  void transfo_groups_in_block(char *nom);
void rename_group_and_associated_links(type_groupe *group, char *nom, t_gennet_script *script_gui);
int rename_group(type_groupe *group, char *nom, struct hsearch_data *hastab);

#endif

