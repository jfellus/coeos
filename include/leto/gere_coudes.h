#ifndef GERE_COUDES_H
#define GERE_COUDES_H

#include "graphic_Tx.h"
#include "reseau.h"
#include "script.h"

extern int test_link_position(TxPoint point);

extern int get_link_points(type_liaison *liaison, TxPoint tab[]);

extern int dupliquer_coude();
extern void efface_coude();
extern void efface_coudes();
extern void detruit_coudes(type_liaison *liaison);

extern void gere_coude(TxPoint point,TxDonneesFenetre *onglet_leto);
extern void fill_coudes();

extern void initialise_coudes_liaison(type_liaison *liaison);
void get_link_caption_coordinates(type_liaison *link, int *x, int *y);

extern void mise_a_jour_coude_extremites(type_liaison *liaison);
int deplace_coude_courant(donnees_script *script,int x, int y);
extern void rescale_coudes(type_liaison *liaison, int new_start_x, int new_start_y, int new_end_x, int new_end_y);
extern void rescale_link(type_liaison *liaison);

extern void shift_link(type_liaison *link, int x, int y);
#endif
