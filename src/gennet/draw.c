#include <stdlib.h>
#include <math.h>
#include <locale.h>

/*#define DEBUG*/

#include "public_leto.h"
#include "icones.h"
#include "gennet.h"
#include "creation.h"
#include <outils_script.h>
#include "leto_global_var.h"
#include <search.h>
#include "bend.h"

void reset_pointer_bend_script_in_computer_park(t_gennet *gen, t_gennet_computer *computer, t_gennet_script_list *scriptlist, GdkEventButton *event)
{
	t_gennet_comlink_list *comlinklist_out = NULL;
	t_gennet_computer *cur_computer = NULL;

	for (comlinklist_out = scriptlist->script->comlinklist_out; comlinklist_out != NULL; comlinklist_out = comlinklist_out->next)
	{
		if ((cur_computer = find_widget_computer_by_script(gen, comlinklist_out->comlink->out)) == NULL) continue;
		if (cur_computer == computer)
		{
			reset_pointers_in_polyline_list(&(comlinklist_out->comlink->polyline_list));
		}
	}
}

void update_position_bend_script_in_computer_park(t_gennet *gen, t_gennet_computer *computer, t_gennet_script_list *scriptlist, GdkEventMotion *event)
{
	t_gennet_comlink_list *comlinklist_out = NULL;
	t_gennet_computer *cur_computer = NULL;

	for (comlinklist_out = scriptlist->script->comlinklist_out; comlinklist_out != NULL; comlinklist_out = comlinklist_out->next)
	{
		if ((cur_computer = find_widget_computer_by_script(gen, comlinklist_out->comlink->out)) == NULL) continue;
		if (cur_computer == computer)
		{
			move_polyline_list(&(comlinklist_out->comlink->polyline_list), (int) event->x, (int) event->y);
		}
	}
}

void update_pointer_bend_script_in_computer_park(t_gennet *gen, t_gennet_computer *computer, t_gennet_script_list *scriptlist, GdkEventButton *event)
{
	t_gennet_comlink_list *comlinklist_out = NULL;
	t_gennet_computer *cur_computer = NULL;

	for (comlinklist_out = scriptlist->script->comlinklist_out; comlinklist_out != NULL; comlinklist_out = comlinklist_out->next)
	{
		if ((cur_computer = find_widget_computer_by_script(gen, comlinklist_out->comlink->out)) == NULL) continue;
		if (cur_computer == computer)
		{
			set_bend_pointers_in_polyline_list(&(comlinklist_out->comlink->polyline_list), (int) event->x, (int) event->y);
		}
	}
}

void update_pointer_script_in_computer_park(t_gennet *gen, t_gennet_computer *computer, GdkEventButton *event)
{
	t_gennet_script_list *script_list = NULL;

	for (script_list = computer->scriptlist; script_list != NULL; script_list = script_list->next)
	{
		script_list->script->pointer_x = (((int) event->x) - script_list->script->pos_x);
		script_list->script->pointer_y = (((int) event->y) - script_list->script->pos_y);
		update_pointer_bend_script_in_computer_park(gen, computer, script_list, event);
	}
}

void update_position_script_in_computer_park(t_gennet *gen, t_gennet_computer *computer, GdkEventMotion *event)
{
	t_gennet_script_list *script_list = NULL;

	for (script_list = computer->scriptlist; script_list != NULL; script_list = script_list->next)
	{
		if ((script_list->script->pos_x = (((int) event->x) - script_list->script->pointer_x)) < 0) script_list->script->pos_x = 0;
		if ((script_list->script->pos_y = (((int) event->y) - script_list->script->pointer_y)) < 0) script_list->script->pos_y = 0;
		update_position_bend_script_in_computer_park(gen, computer, script_list, event);
	}
}

void reset_pointer_script_in_computer_park(t_gennet *gen, t_gennet_computer *computer, GdkEventButton *event)
{
	t_gennet_script_list *script_list = NULL;

	for (script_list = computer->scriptlist; script_list != NULL; script_list = script_list->next)
	{
		script_list->script->pointer_x = -1;
		script_list->script->pointer_y = -1;
		reset_pointer_bend_script_in_computer_park(gen, computer, script_list, event);
	}
}

GdkPixmap *Get_Pixmap_From_File(t_gennet *data, gchar **img, GdkBitmap **mask, GdkColor *trans)
{
	GdkPixmap *pixmap = NULL;

	if ((img == NULL) || (*img == NULL)) return NULL;

	pixmap = gdk_pixmap_create_from_xpm_d(data->gui->WinMain->window, mask, trans, img);
	return (pixmap);
}

void refresh_all(t_gennet *data)
{

	debug_printf("refresh_all\n");
	gtk_widget_queue_draw_area(data->gui->DrawingArea, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void refresh_zone(t_gennet *data, int x, int y, int size_x, int size_y)
{
	debug_printf("refresh_zone\n");
	gtk_widget_queue_draw_area(data->gui->DrawingArea, x - 5, y - 5, size_x + 10, size_y + 10);
}

void refresh_computer(t_gennet *data, t_gennet_computer *computer, int x, int y)
{
	debug_printf("refresh_computer\n");
	if (!computer) return;

	gtk_widget_queue_draw_area(data->gui->DrawingArea, x - 5, y - 5, computer->width + 10, computer->height + 10);
}

void refresh_script(t_gennet *data, t_gennet_script *script, int x, int y)
{
	int height = 0;

	debug_printf("refresh_script\n");

	if ((!script) || (script->in_computer == GENNET_OUT)) return;

	height = IMG_SCRIPT_HEIGHT + VSPACE_COMPUTER_SCRIPT + data->gui->font_height;

	gtk_widget_queue_draw_area(data->gui->DrawingArea, x - 5, y - 5, IMG_SCRIPT_WIDTH + (data->gui->font_width * LOGICAL_NAME_MAX) + 10, height + 10);
}

void refresh_link(t_gennet *data, t_gennet_comlink *comlink)
{
	t_polyline *polyline = NULL;
	int new_x_b;
	int new_y_b;
	int new_x_e;
	int new_y_e;
	int start_x;
	int start_y;
	int end_x;
	int end_y;
	int do_refresh;

	debug_printf("refresh_link\n");

	if (!comlink) return;

	if (comlink->in == NULL || comlink->out == NULL) return;

	for (polyline = comlink->polyline_list.first; polyline != NULL; polyline = polyline->next)
	{
		do_refresh = 0;

		if (polyline->prev == NULL)
		{
			if (comlink->in->pos_x < polyline->x_e)
			{
				start_x = comlink->in->pos_x + IMG_SCRIPT_WIDTH;
			}
			else
			{
				start_x = comlink->in->pos_x;
			}

			if (comlink->in->pos_y < polyline->y_e)
			{
				start_y = comlink->in->pos_y + IMG_SCRIPT_HEIGHT;
			}
			else
			{
				start_y = comlink->in->pos_y;
			}
		}
		else
		{
			start_x = polyline->prev->x_e;
			start_y = polyline->prev->y_e;
		}

		if (polyline->next == NULL)
		{
			if (polyline->x_b < comlink->out->pos_x)
			{
				end_x = comlink->out->pos_x;
			}
			else
			{
				end_x = comlink->out->pos_x + IMG_SCRIPT_WIDTH;
			}

			if (polyline->y_b < comlink->out->pos_y)
			{
				end_y = comlink->out->pos_y;
			}
			else
			{
				end_y = comlink->out->pos_y + IMG_SCRIPT_HEIGHT;
			}
		}
		else
		{
			end_x = polyline->next->x_b;
			end_y = polyline->next->y_b;
		}

		do_refresh = move_polyline_coordinates(polyline, start_x, start_y, end_x, end_y);

		if (do_refresh == 1)
		{
			get_polyline_rectangle(polyline, &new_x_b, &new_y_b, &new_x_e, &new_y_e);
			gtk_widget_queue_draw_area(data->gui->DrawingArea, new_x_b - 5, new_y_b - 5, new_x_e - new_x_b + 5, new_y_e - new_y_b + 5);
		}
	}
}

void DrawingArea_expose_event_script_in_computer(GtkWidget *widget, t_gennet_computer *computer, t_gennet *data)
{
	t_gennet_script_list *scriptlist = NULL;
	t_gennet_comlink_list *comlinklist = NULL;
	int nr = 0;

	debug_printf("expose_event_script_in_computer\n");

	for (scriptlist = computer->scriptlist; scriptlist != NULL; scriptlist = scriptlist->next, nr++)
	{
		gdk_draw_drawable(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], scriptlist->script->pixmap, 0, 0, scriptlist->script->pos_x, scriptlist->script->pos_y, IMG_SCRIPT_WIDTH, IMG_SCRIPT_HEIGHT);

		if (scriptlist->script->pango) gdk_draw_layout(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], scriptlist->script->pos_x, scriptlist->script->pos_y + VSPACE_COMPUTER_SCRIPT + IMG_SCRIPT_HEIGHT, scriptlist->script->pango);

		for (comlinklist = scriptlist->script->comlinklist_in; comlinklist != NULL; comlinklist = comlinklist->next)
			refresh_link(data, comlinklist->comlink);
		for (comlinklist = scriptlist->script->comlinklist_out; comlinklist != NULL; comlinklist = comlinklist->next)
			refresh_link(data, comlinklist->comlink);
	}
}

void DrawingArea_expose_event_computer(GtkWidget *widget, GdkEventExpose *event, t_gennet *data)
{
	t_gennet *gen = (t_gennet *) data;
	t_gennet_computer *computer = NULL;

	debug_printf("expose_event_computer\n");

	for (computer = gen->computers; computer != NULL; computer = computer->next)
	{
		if (computer->pango)
		{
			gdk_draw_drawable(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], computer->pixmap, 0, 0, computer->pos_x, computer->pos_y, IMG_COMPUTER_WIDTH, IMG_COMPUTER_HEIGHT);

			gdk_draw_rectangle(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], FALSE, computer->pos_x, computer->pos_y, computer->width, computer->height);

			gdk_draw_rectangle(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], TRUE, computer->pos_x + computer->width - 10, computer->pos_y + computer->height - 10, 10, 10);

			gdk_draw_layout(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], computer->pos_x, computer->pos_y + VSPACE_COMPUTER_SCRIPT + IMG_COMPUTER_HEIGHT, computer->pango);

			if (computer->scriptlist != NULL) DrawingArea_expose_event_script_in_computer(widget, computer, gen);
		}
	}
}

void DrawingArea_expose_event_comlink(GtkWidget *widget, GdkEventExpose *event, t_gennet *data)
{
	t_gennet *gen = (t_gennet *) data;
	t_gennet_comlink *comlink = NULL;
	t_polyline *polyline = NULL;

	GdkColor *color = NULL;
	cairo_t *cr = NULL;

	double angle, dy;

	debug_printf("expose_event_comlink\n");

	cr = gdk_cairo_create(GTK_LAYOUT(widget)->bin_window);

	for (comlink = gen->comlinks; comlink != NULL; comlink = comlink->next)
	{

		if ((comlink->in == NULL) || (comlink->out == NULL)) continue;

		comlink->color = cairo_pattern_create_linear(comlink->in->pos_x, comlink->in->pos_y, comlink->out->pos_x, comlink->out->pos_y);

		color = &couleurs[comlink->in->color_index];
		cairo_pattern_add_color_stop_rgb(comlink->color, 0., color->red / 65535., color->green / 65535., color->blue / 65535.);
		color = &couleurs[comlink->out->color_index];
		cairo_pattern_add_color_stop_rgb(comlink->color, 1., color->red / 65535., color->green / 65535., color->blue / 65535.);

		cairo_set_source(cr, comlink->color);

		for (polyline = comlink->polyline_list.first; polyline != NULL; polyline = polyline->next)
		{

			cairo_move_to(cr, (double) polyline->x_b, (double) polyline->y_b);
			cairo_line_to(cr, (double) polyline->x_e, (double) polyline->y_e);
			cairo_stroke(cr);

			if (polyline->next != NULL)
			{

				cairo_rectangle(cr, polyline->x_e - (BEND_SIZE_X / 2), polyline->y_e - (BEND_SIZE_Y / 2), BEND_SIZE_X, BEND_SIZE_Y);
				cairo_fill(cr);

			}
			else
			{

				cairo_move_to(cr, (double) polyline->x_e, (double) polyline->y_e);
				if (polyline->x > 0)
				{
					if (polyline->y_b < polyline->y_e) dy = -20.;
					else dy = 20.;

					cairo_line_to(cr, (double) polyline->x_e + 20 * cos(7 * M_PI / 12), (double) polyline->y_e + dy);
					cairo_line_to(cr, (double) polyline->x_e + 20 * cos(5 * M_PI / 12), (double) polyline->y_e + dy);
				}
				else
				{
					angle = atan(polyline->a);
					if (polyline->x_b > polyline->x_e) angle += M_PI;
					cairo_line_to(cr, (double) polyline->x_e + 20 * cos(11 * M_PI / 12 + angle), (double) polyline->y_e + 20 * sin(11 * M_PI / 12 + angle));
					cairo_line_to(cr, (double) polyline->x_e + 20 * cos(13 * M_PI / 12 + angle), (double) polyline->y_e + 20 * sin(13 * M_PI / 12 + angle));
				}

				cairo_close_path(cr);
				cairo_fill(cr);

			}
		}
	}

	cairo_destroy(cr);

}

void DrawingArea_expose_event_script(GtkWidget *widget, GdkEventExpose *event, t_gennet *data)
{
	t_gennet *gen = (t_gennet *) data;
	t_gennet_script *script = NULL;
	t_gennet_comlink_list *comlinklist = NULL;

	debug_printf("expose_event_script\n");

	for (script = gen->scripts; script != NULL; script = script->next)
	{
		if (script->in_computer == GENNET_OUT)
		{
			gdk_draw_drawable(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], script->pixmap, 0, 0, script->pos_x, script->pos_y, IMG_SCRIPT_WIDTH, IMG_SCRIPT_HEIGHT);

			if (script->pango) gdk_draw_layout(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], script->pos_x, script->pos_y + VSPACE_COMPUTER_SCRIPT + IMG_SCRIPT_HEIGHT, script->pango);

			for (comlinklist = script->comlinklist_in; comlinklist != NULL; comlinklist = comlinklist->next)
				refresh_link(gen, comlinklist->comlink);
			for (comlinklist = script->comlinklist_out; comlinklist != NULL; comlinklist = comlinklist->next)
				refresh_link(gen, comlinklist->comlink);
		}
	}
}

void DrawingArea_clear(GtkWidget *widget, GdkEventExpose *event)
{
	debug_printf("clear\n");
	gdk_draw_rectangle(GTK_LAYOUT(widget)->bin_window, widget->style->white_gc, TRUE, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void draw_argument(t_prom_script *prom_script, void *argument, char *label, char **cur)
{
	char *val, *val_name;

	val = promnet_prom_script_get_argument(prom_script, argument);
	if ((val_name = rindex(val, '/')) != NULL) sprintf(*cur, "%s : %s\n", label, val_name + 1);
	else sprintf(*cur, "%s : %s\n", label, val);
	*cur += strlen(*cur);
	if (val != NULL) free(val);

}

void DrawingArea_tooltips_event_callback(GtkWidget *widget, gpointer data)
{
	t_gennet *gen = (t_gennet *) data;
	t_gennet_computer *computer = NULL;
	t_gennet_script *script = NULL;
	t_gennet_script_list *script_list = NULL;
	t_gennet_comlink *comlink = NULL;
	char tooltext[TOOLTIPS_MAX_LENGTH];
	char *cur = NULL;
	char *val = NULL;
	PangoLayout *pango = NULL;
	t_prom_script *prom_script;

	debug_printf("tooltips_event_callback\n");

	cur = tooltext;
	memset(tooltext, 0, TOOLTIPS_MAX_LENGTH);
	if ((script_list = find_widget_script_in_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
	{
		if (script_list->script->in_computer != GENNET_IN_COMPUTER) return;
		script = script_list->script;
		prom_script = script->prom_script;

		draw_argument(prom_script, prom_script->logical_name, "logical_name", &cur);
		draw_argument(prom_script, prom_script->path_prom_binary, "executable", &cur);
		draw_argument(prom_script, prom_script->prom_args_line, "arguments", &cur);
		draw_argument(prom_script, prom_script->path_file_symb, "symb", &cur);
		draw_argument(prom_script, prom_script->path_file_script, "script", &cur);
		draw_argument(prom_script, prom_script->path_file_draw, "draw", &cur);
		draw_argument(prom_script, prom_script->path_file_res, "res", &cur);
		draw_argument(prom_script, prom_script->path_file_config, "config", &cur);
		draw_argument(prom_script, prom_script->path_file_dev, "dev", &cur);
		draw_argument(prom_script, prom_script->path_file_prt, "prt", &cur);
		draw_argument(prom_script, prom_script->path_file_gcd, "gcd", &cur);

		pango = gtk_widget_create_pango_layout(gen->gui->DrawingArea, tooltext);

		gdk_draw_layout(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], gen->gui->mouse_x, gen->gui->mouse_y, pango);
	}
	else if ((script = find_widget_script(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
	{
		if (script->in_computer != GENNET_OUT) return;

		prom_script = script->prom_script;

		draw_argument(prom_script, prom_script->logical_name, "logical_name", &cur);
		draw_argument(prom_script, prom_script->path_prom_binary, "executable", &cur);
		draw_argument(prom_script, prom_script->prom_args_line, "arguments", &cur);
		draw_argument(prom_script, prom_script->path_file_symb, "symb", &cur);
		draw_argument(prom_script, prom_script->path_file_script, "script", &cur);
		draw_argument(prom_script, prom_script->path_file_draw, "draw", &cur);
		draw_argument(prom_script, prom_script->path_file_res, "res", &cur);
		draw_argument(prom_script, prom_script->path_file_config, "config", &cur);
		draw_argument(prom_script, prom_script->path_file_dev, "dev", &cur);
		draw_argument(prom_script, prom_script->path_file_prt, "prt", &cur);
		draw_argument(prom_script, prom_script->path_file_gcd, "gcd", &cur);

		pango = gtk_widget_create_pango_layout(gen->gui->DrawingArea, tooltext);

		gdk_draw_layout(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], gen->gui->mouse_x, gen->gui->mouse_y, pango);
	}
	else if ((comlink = find_widget_comlink(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
	{
		t_prom_script *prom_script = NULL;

		val = promnet_prom_link_get_name(comlink->prom_link);
		sprintf(cur, "name : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);

		prom_script = promnet_prom_link_get_input(comlink->prom_link);
		val = promnet_prom_script_get_argument(prom_script, prom_script->logical_name);
		sprintf(cur, "from : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);

		prom_script = promnet_prom_link_get_output(comlink->prom_link);
		val = promnet_prom_script_get_argument(prom_script, prom_script->logical_name);
		sprintf(cur, "to : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);

		pango = gtk_widget_create_pango_layout(gen->gui->DrawingArea, tooltext);

		gdk_draw_layout(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], gen->gui->mouse_x, gen->gui->mouse_y, pango);
	}
	else if ((computer = find_widget_computer(gen, gen->gui->mouse_x, gen->gui->mouse_y)) != NULL)
	{
		val = promnet_computer_get_name(computer->computer);
		sprintf(cur, "name : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);
		val = promnet_computer_get_address(computer->computer);
		sprintf(cur, "address : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);
		val = promnet_computer_get_login(computer->computer);
		sprintf(cur, "login : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);
		val = promnet_computer_get_path(computer->computer);
		sprintf(cur, "path to binary folder : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);
		val = promnet_computer_get_cpu(computer->computer);
		sprintf(cur, "CPU : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);
		val = promnet_computer_get_information(computer->computer);
		sprintf(cur, "Informations : %s\n", val);
		cur += strlen(cur);
		if (val != NULL) free(val);

		pango = gtk_widget_create_pango_layout(gen->gui->DrawingArea, tooltext);

		gdk_draw_layout(GTK_LAYOUT(widget)->bin_window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], gen->gui->mouse_x, gen->gui->mouse_y, pango);
	}
}

gboolean DrawingArea_expose_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	debug_printf("expose_event_callback\n");
	DrawingArea_clear(widget, event);
	DrawingArea_expose_event_computer(widget, event, (t_gennet *) data);
	DrawingArea_expose_event_script(widget, event, (t_gennet *) data);
	DrawingArea_expose_event_comlink(widget, event, (t_gennet *) data);
	DrawingArea_tooltips_event_callback(widget, data);
	return TRUE;
}

void DrawingArea_button1_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	t_gennet *gen = (t_gennet *) data;
	t_gennet_computer *computer = NULL;
	t_gennet_script *script = NULL;
	t_gennet_script_list *script_list = NULL;
	t_polyline *polyline = NULL;

	debug_printf("button1_press_event\n");

	if ((script_list = find_widget_script_in_computer(gen, event->x, event->y)) != NULL)
	{
		if (script_list->script->in_computer != GENNET_IN_COMPUTER) return;
		if (gen->gui->NewComlink == 0)
		{
			script_list->script->pointer_x = IMG_SCRIPT_WIDTH / 2;
			script_list->script->pointer_y = IMG_SCRIPT_HEIGHT / 2;
			script_list->hold = 1;
		}
		else
		{
			add_script_to_link(gen, gen->comlinks, script_list->script);
		}
	}
	else if ((script = find_widget_script(gen, event->x, event->y)) != NULL)
	{
		if (script->in_computer != GENNET_OUT) return;
		if (gen->gui->NewComlink == 0)
		{
			script->pointer_x = (((int) event->x) - script->pos_x);
			script->pointer_y = (((int) event->y) - script->pos_y);
			script->hold = 1;
		}
		else
		{
			add_script_to_link(gen, gen->comlinks, script);
		}
	}
	else if ((polyline = find_widget_comlink_bend(gen, event->x, event->y)) != NULL)
	{
		if (gen->gui->NewComlink == 0)
		{
			set_bend_pointer(polyline, (int) event->x, (int) event->y);
			polyline->hold = 1;
		}
	}
	else if ((polyline = find_widget_comlink_polyline(gen, event->x, event->y)) != NULL)
	{
		if (gen->gui->NewComlink == 0)
		{
			set_polyline_pointer(polyline, (int) event->x, (int) event->y);
			polyline->seg_hold = 1;
		}
	}
	else if ((computer = find_widget_computer(gen, event->x, event->y)) != NULL)
	{
		if (gen->gui->NewComlink == 0)
		{
			if (is_resize_computer(computer, event->x, event->y))
			{
				computer->pointer_x = (((int) event->x) - computer->width);
				computer->pointer_y = (((int) event->y) - computer->height);

				computer->resize = 1;
			}
			else
			{
				computer->pointer_x = (((int) event->x) - computer->pos_x);
				computer->pointer_y = (((int) event->y) - computer->pos_y);

				update_pointer_script_in_computer_park(gen, computer, event);
			}
			computer->hold = 1;
		}
	}
	else
	{
		if (gen->gui->NewComlink == 1)
		{
			del_gennet_comlink(gen, gen->comlinks);
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gen->gui->NewComlinkItem), FALSE);
			gen->gui->NewComlink = 0;
		}
	}
}

gboolean DrawingArea_button_press_event_callback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	debug_printf("button_press_event_callback\n");

	if (event->button == 1) DrawingArea_button1_press_event(widget, event, data);
	else if (event->button == 3)
	{
		GtkWidget *menu = NULL;

		menu = gtk_menu_new();
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, popup_menu_handler, data, event->button, event->time);
	}
	return TRUE;
}

void DrawingArea_button_motion_event_callback(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
	t_gennet *gen = (t_gennet *) data;
	t_gennet_computer *computer = NULL;
	t_gennet_script *script = NULL;
	t_gennet_script_list *script_list = NULL;
	t_polyline *polyline = NULL;

	debug_printf("button_motion_event_callback\n");

	if ((script = find_widget_held_script(gen, event->x, event->y)) != NULL)
	{
		if ((script->pos_x = (((int) event->x) - script->pointer_x)) < 0) script->pos_x = 0;
		if ((script->pos_y = (((int) event->y) - script->pointer_y)) < 0) script->pos_y = 0;
	}
	else if ((script_list = find_widget_held_script_in_computer(gen, event->x, event->y, &computer)) != NULL)
	{
		script = script_list->script;

		script->hold = 1;
		move_script_out_computer(gen, computer, script_list);

		if ((script->pos_x = (((int) event->x) - script->pointer_x)) < 0) script->pos_x = 0;
		if ((script->pos_y = (((int) event->y) - script->pointer_y)) < 0) script->pos_y = 0;
	}
	else if ((polyline = find_widget_held_comlink_bend(gen, event->x, event->y)) != NULL)
	{
		move_bend(polyline, (int) event->x, (int) event->y);
	}
	else if ((polyline = find_widget_held_comlink_polyline_list(gen, event->x, event->y)) != NULL)
	{
		move_polyline(polyline, (int) event->x, (int) event->y);
	}
	else if ((computer = find_widget_held_computer(gen, event->x, event->y)) != NULL)
	{
		if (computer->resize == 1)
		{
			if ((computer->width = (((int) event->x) - computer->pointer_x)) < 100) computer->width = 100;
			if ((computer->height = (((int) event->y) - computer->pointer_y)) < 100) computer->height = 100;
		}
		else
		{
			if ((computer->pos_x = (((int) event->x) - computer->pointer_x)) < 0) computer->pos_x = 0;
			if ((computer->pos_y = (((int) event->y) - computer->pointer_y)) < 0) computer->pos_y = 0;
			update_position_script_in_computer_park(gen, computer, event);
		}
	}
}

gboolean DrawingArea_motion_event_callback(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
	t_gennet *gen = (t_gennet *) data;

	debug_printf("motion_event_callback\n");

	refresh_zone(gen, gen->gui->mouse_x - 10, gen->gui->mouse_y - 10, 500, 500);
	gen->gui->mouse_x = event->x;
	gen->gui->mouse_y = event->y;
	refresh_zone(gen, gen->gui->mouse_x - 10, gen->gui->mouse_y - 10, 500, 500);
	if (event->state & GDK_BUTTON1_MASK)
	{
		DrawingArea_button_motion_event_callback(widget, event, data);
		refresh_all(gen);
	}
	return TRUE;
}

gboolean DrawingArea_button_release_event_callback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	t_gennet *gen = (t_gennet *) data;
	t_gennet_computer *computer = NULL;
	t_gennet_script *script = NULL;
	t_gennet_script_list *script_list = NULL;
	t_polyline *polyline = NULL;

	debug_printf("button_release_event_callback\n");

	if ((computer = find_widget_held_computer(gen, event->x, event->y)) != NULL)
	{
		computer->pointer_x = -1;
		computer->pointer_y = -1;
		computer->hold = 0;
		computer->resize = 0;
		reset_pointer_script_in_computer_park(gen, computer, event);
	}
	else if ((script = find_widget_held_script(gen, event->x, event->y)) != NULL)
	{
		if ((computer = find_widget_computer(gen, event->x, event->y)) != NULL)
		{
			move_script_in_computer(gen, computer, script);
			refresh_computer(gen, computer, computer->pos_x, computer->pos_y);
		}
		script->pointer_x = -1;
		script->pointer_y = -1;
		script->hold = 0;
	}
	else if ((script_list = find_widget_held_script_in_computer(gen, event->x, event->y, &computer)) != NULL)
	{
		script_list->script->pointer_x = -1;
		script_list->script->pointer_y = -1;
		script_list->hold = 0;
	}
	else if ((polyline = find_widget_held_comlink_bend(gen, event->x, event->y)) != NULL)
	{
		polyline->hold = 0;
		polyline->pointer_x = -1;
		polyline->pointer_y = -1;
	}
	else if ((polyline = find_widget_held_comlink_polyline_list(gen, event->x, event->y)) != NULL)
	{
		polyline->seg_hold = 0;
		polyline->pointer_x = -1;
		polyline->pointer_y = -1;
	}
	return TRUE;
}

void init_gui_drawing_tools(t_gennet *data, GtkWidget *hBox)
{
	GtkWidget *ToolBar = NULL;
	GtkToolItem *ToolItem = NULL;
	GtkWidget *image = NULL;
	GdkPixmap *pixmap = NULL;
	GdkBitmap *mask = NULL;

	ToolBar = gtk_toolbar_new();

	gtk_toolbar_set_orientation(GTK_TOOLBAR(ToolBar), GTK_ORIENTATION_VERTICAL);
	gtk_toolbar_set_style(GTK_TOOLBAR(ToolBar), GTK_TOOLBAR_BOTH);
	gtk_container_set_border_width(GTK_CONTAINER(ToolBar), 5);
	gtk_box_pack_start(GTK_BOX(hBox), ToolBar, FALSE, TRUE, 0);
	gtk_widget_show(ToolBar);

	/* Bouton NewComputer */
	pixmap = Get_Pixmap_From_File(data, IMG_COMPUTER, &mask, NULL);
	image = gtk_image_new_from_pixmap(pixmap, mask);
	gtk_widget_show(image);
	g_object_unref(mask);
	g_object_unref(pixmap);

	ToolItem = gtk_tool_button_new(image, "New computer");
	g_signal_connect(G_OBJECT(ToolItem), "clicked", G_CALLBACK(Tools_NewComputer), data);
	gtk_widget_show(GTK_WIDGET(ToolItem));
	gtk_toolbar_insert(GTK_TOOLBAR(ToolBar), ToolItem, -1);

	/* Bouton NewComLink */
	pixmap = Get_Pixmap_From_File(data, IMG_COMLINK, &mask, NULL);
	image = gtk_image_new_from_pixmap(pixmap, mask);
	gtk_widget_show(image);
	g_object_unref(mask);
	g_object_unref(pixmap);

	data->gui->NewComlinkItem = gtk_toggle_tool_button_new();
	g_signal_connect(G_OBJECT(data->gui->NewComlinkItem), "toggled", G_CALLBACK(Tools_NewComlink), data);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(data->gui->NewComlinkItem), image);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(data->gui->NewComlinkItem), "New link");
	gtk_widget_show(GTK_WIDGET(data->gui->NewComlinkItem));
	gtk_toolbar_insert(GTK_TOOLBAR(ToolBar), data->gui->NewComlinkItem, -1);

	/* Bouton NewScript */
	pixmap = Get_Pixmap_From_File(data, IMG_SCRIPT, &mask, NULL);
	image = gtk_pixmap_new(pixmap, mask);
	gtk_widget_show(image);
	g_object_unref(mask);
	g_object_unref(pixmap);

	ToolItem = gtk_tool_button_new(image, "New script");
	g_signal_connect(G_OBJECT(ToolItem), "clicked", G_CALLBACK(Tools_NewScript), data);
	gtk_widget_show(GTK_WIDGET(ToolItem));
	gtk_toolbar_insert(GTK_TOOLBAR(ToolBar), ToolItem, -1);
}

void init_gui_drawing_area(t_gennet *data, GtkWidget *hBox)
{
	GtkWidget *sWin = NULL;

	sWin = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sWin), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(hBox), sWin, TRUE, TRUE, 0);
	gtk_widget_show(sWin);

	data->gui->DrawingArea = gtk_layout_new(NULL, NULL);
	gtk_layout_set_size(GTK_LAYOUT(data->gui->DrawingArea), WINDOW_WIDTH, WINDOW_HEIGHT);
	g_signal_connect(G_OBJECT(data->gui->DrawingArea), "expose_event", G_CALLBACK(DrawingArea_expose_event_callback), data);
	g_signal_connect(G_OBJECT(data->gui->DrawingArea), "button_press_event", G_CALLBACK(DrawingArea_button_press_event_callback), data);
	g_signal_connect(G_OBJECT(data->gui->DrawingArea), "motion_notify_event", G_CALLBACK(DrawingArea_motion_event_callback), data);
	g_signal_connect(G_OBJECT(data->gui->DrawingArea), "button_release_event", G_CALLBACK(DrawingArea_button_release_event_callback), data);

	gtk_widget_set_events(data->gui->DrawingArea, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK);
	gtk_container_add(GTK_CONTAINER(sWin), data->gui->DrawingArea);
	gtk_widget_show(data->gui->DrawingArea);
}

void init_gui_drawing(t_gennet *data)
{
	GtkWidget *hBox;
	PangoContext *context;
	PangoFontMetrics *metrics;

	hBox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(data->gui->vBoxMain), hBox, TRUE, TRUE, 0);
	gtk_widget_show(hBox);

	init_gui_drawing_tools(data, hBox);
	init_gui_drawing_area(data, hBox);

	context = gtk_widget_get_pango_context(GTK_WIDGET(data->gui->DrawingArea));
	metrics = pango_context_get_metrics(context, GTK_WIDGET(data->gui->DrawingArea)->style->font_desc, pango_context_get_language(context));

	data->gui->font_width = PANGO_PIXELS(pango_font_metrics_get_approximate_char_width(metrics));
	data->gui->font_height = PANGO_PIXELS(pango_font_metrics_get_ascent(metrics)) + PANGO_PIXELS(pango_font_metrics_get_descent(metrics));
}

t_polyline *find_widget_comlink_bend(t_gennet *data, int x, int y)
{
	t_gennet_comlink *comlink = NULL;
	t_polyline *result = NULL;

	for (comlink = data->comlinks; comlink != NULL; comlink = comlink->next)
	{
		if ((result = find_bend(&(comlink->polyline_list), x, y)) != NULL) return result;
	}
	return NULL;
}

t_polyline *find_widget_comlink_polyline(t_gennet *data, int x, int y)
{
	t_gennet_comlink *comlink = NULL;
	t_polyline *result = NULL;

	for (comlink = data->comlinks; comlink != NULL; comlink = comlink->next)
	{
		if ((result = find_polyline(&(comlink->polyline_list), x, y)) != NULL) return result;
	}
	return NULL;
}

t_gennet_comlink *find_widget_comlink(t_gennet *data, int x, int y)
{
	t_gennet_comlink *comlink = NULL;

	for (comlink = data->comlinks; comlink != NULL; comlink = comlink->next)
	{
		if (find_polyline(&(comlink->polyline_list), x, y) != NULL) return comlink;
	}
	return NULL;
}

t_gennet_computer *find_widget_computer(t_gennet *data, int x, int y)
{
	t_gennet_computer *computer = NULL;

	for (computer = data->computers; computer != NULL; computer = computer->next)
	{
		if ((x > (computer->pos_x - COMPUTER_CLICK_ZONE)) && (x < (computer->pos_x + computer->width + COMPUTER_CLICK_ZONE)) && (y > (computer->pos_y - COMPUTER_CLICK_ZONE)) && (y < (computer->pos_y + computer->height + COMPUTER_CLICK_ZONE))) return computer;
	}
	return NULL;
}

int is_resize_computer(t_gennet_computer *computer, int x, int y)
{
	if ((x > (computer->pos_x + computer->width - 10)) && (x < (computer->pos_x + computer->width)) && (y > (computer->pos_y + computer->height - 10)) && (y < (computer->pos_y + computer->height))) return 1;
	else return 0;
}

t_gennet_computer *find_widget_held_computer(t_gennet *data, int x, int y)
{
	t_gennet_computer *computer = NULL;

	for (computer = data->computers; computer != NULL; computer = computer->next)
		if (computer->hold == 1) return computer;
	return NULL;
}

t_polyline *find_widget_held_comlink_bend(t_gennet *data, int x, int y)
{
	t_gennet_comlink *comlink = NULL;
	t_polyline *result = NULL;

	for (comlink = data->comlinks; comlink != NULL; comlink = comlink->next)
	{
		if ((result = find_held_bend(&(comlink->polyline_list))) != NULL) return result;
	}
	return NULL;
}

t_polyline *find_widget_held_comlink_polyline_list(t_gennet *data, int x, int y)
{
	t_gennet_comlink *comlink = NULL;
	t_polyline *result = NULL;

	for (comlink = data->comlinks; comlink != NULL; comlink = comlink->next)
	{
		if ((result = find_held_polyline(&(comlink->polyline_list))) != NULL) return result;
	}
	return NULL;
}

t_gennet_script_list *find_widget_held_script_in_computer(t_gennet *data, int x, int y, t_gennet_computer **p_computer)
{
	t_gennet_computer *computer = NULL;
	t_gennet_script_list *script_list = NULL;

	if (p_computer == NULL) return NULL;

	for (computer = data->computers; computer != NULL; computer = computer->next)
	{
		for (script_list = computer->scriptlist; script_list != NULL; script_list = script_list->next)
		{
			if (script_list->hold == 1)
			{
				(*p_computer) = computer;
				return script_list;
			}
		}
	}
	(*p_computer) = NULL;
	return NULL;
}

t_gennet_script_list *find_widget_script_in_computer(t_gennet *data, int x, int y)
{
	t_gennet_computer *computer = NULL;
	t_gennet_script_list *script_list = NULL;

	for (computer = data->computers; computer != NULL; computer = computer->next)
	{
		for (script_list = computer->scriptlist; script_list != NULL; script_list = script_list->next)
		{
			if (script_list->script->in_computer == GENNET_IN_COMPUTER)
			{

				if ((x > (script_list->script->pos_x - SCRIPT_CLICK_ZONE)) && (x < (script_list->script->pos_x + IMG_SCRIPT_WIDTH + SCRIPT_CLICK_ZONE)) && (y > (script_list->script->pos_y - SCRIPT_CLICK_ZONE))
						&& (y < (script_list->script->pos_y + IMG_SCRIPT_HEIGHT + SCRIPT_CLICK_ZONE))) return script_list;
			}
		}
	}
	return NULL;
}

t_gennet_script *find_widget_script(t_gennet *data, int x, int y)
{
	t_gennet_script *script = NULL;

	for (script = data->scripts; script != NULL; script = script->next)
	{
		if (script->in_computer == GENNET_IN_COMPUTER) continue;

		if ((x > (script->pos_x - SCRIPT_CLICK_ZONE)) && (x < (script->pos_x + IMG_SCRIPT_WIDTH + SCRIPT_CLICK_ZONE)) && (y > (script->pos_y - SCRIPT_CLICK_ZONE)) && (y < (script->pos_y + IMG_SCRIPT_HEIGHT + SCRIPT_CLICK_ZONE))) return script;
	}
	return NULL;
}

t_gennet_script *find_widget_held_script(t_gennet *data, int x, int y)
{
	t_gennet_script *script = NULL;

	for (script = data->scripts; script != NULL; script = script->next)
		if ((script->hold == 1) && (script->in_computer == GENNET_OUT)) return script;
	return NULL;
}

void init_draw_script(t_gennet *data, t_gennet_script *script)
{
	int index = 0;
	t_gennet_script *tmp = NULL;

	tmp = data->scripts;

	while (tmp->next != NULL)
	{
		tmp = tmp->next;
		index++;
	}

	script->color_index = 17 + (index % (nb_max_couleurs - 16));
	script->pixmap = Get_Pixmap_From_File(data, IMG_SCRIPT, NULL, &couleurs[script->color_index]);
	script->pos_x = (IMG_SCRIPT_WIDTH + 10) * (index % 6);
	script->pos_y = GENNET_COMPUTER_PARK_HEIGHT + 20 + (IMG_SCRIPT_HEIGHT + 10) * (index / 6);
}

void init_draw_computer(t_gennet *data, t_gennet_computer *computer)
{
	int index = 0;
	t_gennet_computer *tmp = NULL;

	tmp = data->computers;

	while (tmp->next != NULL)
	{
		tmp = tmp->next;
		index++;
	}

	computer->width = GENNET_COMPUTER_PARK_WIDTH;
	computer->height = GENNET_COMPUTER_PARK_HEIGHT;
	computer->pixmap = Get_Pixmap_From_File(data, IMG_COMPUTER, NULL, NULL);
	computer->pos_x = (computer->width + 20) * index;
	computer->pos_y = 0;
}

void draw_script_in_computer(t_gennet *data, t_gennet_computer *computer, t_gennet_script *script)
{
	int index = 0;
	t_gennet_script_list *tmp = NULL;
	int nb_script_width = computer->width / (IMG_SCRIPT_WIDTH + 10);

	for (tmp = computer->scriptlist; tmp != NULL; tmp = tmp->next)
		index++;

	script->pos_x = computer->pos_x + (IMG_SCRIPT_WIDTH + 10) * ((index - 1) % nb_script_width);
	script->pos_y = computer->pos_y + IMG_COMPUTER_HEIGHT + 20 + (IMG_SCRIPT_HEIGHT + 10) * ((index - 1) / nb_script_width);
}
