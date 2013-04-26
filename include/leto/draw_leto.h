#ifndef DRAW_LETO_H
#define DRAW_LETO_H

#include "graphic_Tx.h"

#define deltax         10
#define deltay         10
#define distc          20

extern void affiche_liaison(type_liaison * liaison, type_groupe * groupe1, type_groupe * groupe2,TxDonneesFenetre *onglet_leto);
extern void invert_colors(GtkWidget * widget, gpointer data);

extern void regenerer_test(TxDonneesFenetre *onglet_leto);
extern void draw_selection(GtkWidget * widget, gdouble x, gdouble y,gpointer data, int ctrl_mask);

#endif
