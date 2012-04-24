#ifndef GROUP_SELECTION_H
#define GROUP_SELECTION_H

#ifndef AVEUGLE
#include "libx.h"

typedef struct selected_group
{
      struct selected_group *prev;
      struct selected_group *next;
      type_groupe *group;
} selected_group;

selected_group * is_selected(type_groupe *group);

void update_selection_rectangle(int xmin, int ymin, int xmax, int ymax, int ctrl_mask);
type_groupe * selection_groupe(TxPoint point);
void reset_selection_groups();

type_groupe * gere_groupe(TxPoint point);
type_groupe *test_group_position(TxPoint point);

void move_selected_groups(TxPoint point, TxDonneesFenetre *data);
void drag_selected_groups(TxPoint point, TxDonneesFenetre *data);

void nouvelle_position_groupe(type_groupe *groupe, TxPoint point);
void slide_groups(int x, int y, int selection);
void select_group(type_groupe *group);
#endif
#endif
