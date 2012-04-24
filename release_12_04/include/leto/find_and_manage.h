#ifndef FIND_AND_MANAGE_H
#define FIND_AND_MANAGE_H

#include <gtk/gtk.h>

extern void callback_find_group(GtkWidget * widget, gpointer data);
extern void callback_hide_find_group(GtkWidget * widget, gpointer data);
extern void callback_search_group_by_name(GtkWidget * widget, gpointer data);
extern GtkWidget * do_find_completion (GtkWidget *do_widget, gpointer data);

#endif
