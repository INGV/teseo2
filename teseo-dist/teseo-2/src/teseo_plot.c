/* Teseo-2 Plug-in
 * Copyright (C) 2005  Stefano Pintore, Matteo Quintiliani (the "Authors").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE Authors BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Authors of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Authors.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>
#include <stdio.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include <glib.h>
#include <glib/gprintf.h>

#include <pango/pango.h>

#include <gtkextra/gtkplot.h>
#include <gtkextra/gtkplotdata.h>
#include <gtkextra/gtkplotcanvas.h>
#include <gtkextra/gtkplotps.h>
#include <gtkextra/gtkplotcanvastext.h>
#include <gtkextra/gtkplotcanvasline.h>
#include <gtkextra/gtkplotcanvasellipse.h>
#include <gtkextra/gtkplotcanvasrectangle.h>
#include <gtkextra/gtkplotcanvasplot.h>
#include <gtkextra/gtkplotcanvaspixmap.h>
#include <gtkextra/gtkplotprint.h>
#include <gtkextra/gtkplotbar.h>

#include <libgimp/gimp.h>

#include "teseo_plot.h"

GdkPixmap *pixmap;
GtkWidget **plots;
GtkPlotData *dataset[5];

gint nlayers = 0;
GtkWidget *active_plot;


gint select_item(GtkWidget *widget, GdkEvent *event, GtkPlotCanvasChild *child, gpointer data)
{
	GtkWidget **widget_list = NULL;
	GtkWidget *active_widget = NULL;
	gint n = 0;
	gdouble *x = NULL, *y = NULL;

	if(GTK_IS_PLOT_CANVAS_TEXT(child))
		printf("Item selected: TEXT\n");
	if(GTK_IS_PLOT_CANVAS_PIXMAP(child))
		printf("Item selected: PIXMAP\n");
	if(GTK_IS_PLOT_CANVAS_RECTANGLE(child))
		printf("Item selected: RECTANGLE\n");
	if(GTK_IS_PLOT_CANVAS_ELLIPSE(child))
		printf("Item selected: ELLIPSE\n");
	if(GTK_IS_PLOT_CANVAS_LINE(child))
		printf("Item selected: LINE\n");
	if(GTK_IS_PLOT_CANVAS_PLOT(child)){
		switch(GTK_PLOT_CANVAS_PLOT(child)->pos){
			case GTK_PLOT_CANVAS_PLOT_IN_TITLE:
				printf("Item selected: TITLE\n");
				break;
			case GTK_PLOT_CANVAS_PLOT_IN_LEGENDS:
				printf("Item selected: LEGENDS\n");
				break;
			case GTK_PLOT_CANVAS_PLOT_IN_PLOT:
				printf("Item selected: PLOT\n");
				break;
			case GTK_PLOT_CANVAS_PLOT_IN_AXIS:
				printf("Item selected: AXIS\n");
				break;
			case GTK_PLOT_CANVAS_PLOT_IN_MARKER:
				printf("Item selected: MARKER\n");
				break;
			case GTK_PLOT_CANVAS_PLOT_IN_GRADIENT:
				printf("Item selected: GRADIENT\n");
				break;
			case GTK_PLOT_CANVAS_PLOT_IN_DATA:
				x = gtk_plot_data_get_x(GTK_PLOT_CANVAS_PLOT(child)->data, &n);
				y = gtk_plot_data_get_y(GTK_PLOT_CANVAS_PLOT(child)->data, &n);
				n = GTK_PLOT_CANVAS_PLOT(child)->datapoint;
				printf("Item selected: DATA\n");
				printf("Active point: %d -> %f %f\n",
					GTK_PLOT_CANVAS_PLOT(child)->datapoint, x[n], y[n]);
				break;
			default:
				break;
		}
		widget_list = plots;
		active_widget = GTK_WIDGET(GTK_PLOT_CANVAS_PLOT(child)->plot);
	}

	return TRUE;
}


GtkWidget * new_layer(GtkWidget *canvas)
{
	//gchar label[10];
	nlayers++;

	plots = (GtkWidget **)g_realloc(plots, nlayers * sizeof(GtkWidget *));

	//sprintf(label, "%d", nlayers);
	plots[nlayers-1] = gtk_plot_new_with_size(NULL, .5, .25);
	gtk_widget_show(plots[nlayers-1]);

	return plots[nlayers-1];
}


void
build_data(GtkWidget *plot, gdouble *ret_b, gdouble *ret_e, gulong ntries, const gchar * Xtitle, const gchar * Ytitle )
{
	GdkColor color;
	GtkPlotAxis *axis;

	static gdouble px1[800]={};
	static gdouble py1[800]={};


	static gdouble dx1[800]={};
	static gdouble dy1[800]={};

	//TODO
	//gdouble * dx1= (gdouble *) g_malloc(sizeof (gdouble) * ntries );
	//gdouble * dy1= (gdouble *) g_malloc(sizeof (gdouble) * ntries );

	gulong i;

	for (i=0;i<ntries; i++){
		px1[i]=ret_b[i];
		py1[i]=ret_e[i];

		dx1[i]=0.2;
		dy1[i]=0.1;
	}

	gdouble emin= ret_e[0] ;
	gdouble emax= ret_e[0] ;

	for (i=1; i<ntries;i++){
		emin = (emin<ret_e[i]) ? emin:ret_e[i];
		emax = (emax>ret_e[i]) ? emax:ret_e[i];
	}


	gtk_plot_set_range(GTK_PLOT(active_plot), ret_b[0], ret_b[ntries-1], 0., emax);

	axis = gtk_plot_get_axis(GTK_PLOT(plot), GTK_PLOT_AXIS_TOP);
	gtk_plot_axis_set_title (axis, Xtitle);

	axis = gtk_plot_get_axis(GTK_PLOT(plot), GTK_PLOT_AXIS_BOTTOM);
	gtk_plot_axis_set_title (axis, Xtitle);

	axis = gtk_plot_get_axis(GTK_PLOT(plot), GTK_PLOT_AXIS_RIGHT);
	gtk_plot_axis_set_title (axis, Ytitle);

	axis = gtk_plot_get_axis(GTK_PLOT(plot), GTK_PLOT_AXIS_LEFT);
	gtk_plot_axis_set_title (axis, Ytitle);

	dataset[0] = GTK_PLOT_DATA(gtk_plot_data_new());
	gtk_plot_add_data(GTK_PLOT(plot), dataset[0]);
	gtk_widget_show(GTK_WIDGET(dataset[0]));

	gtk_plot_data_set_points(dataset[0], ret_b, ret_e, dx1, dy1, ntries);

	gdk_color_parse("red", &color);
	//gdk_color_alloc(gdk_colormap_get_system(), &color); Deprecated
	gdk_colormap_alloc_color(gdk_colormap_get_system(), &color, FALSE, TRUE);

	gtk_plot_data_set_symbol(dataset[0],
				GTK_PLOT_SYMBOL_DIAMOND,
				GTK_PLOT_SYMBOL_EMPTY,
				4, 2, &color, &color);

	gtk_plot_data_set_line_attributes(dataset[0],
					GTK_PLOT_LINE_SOLID,
					0, 0, 1, &color);

	gtk_plot_data_set_connector(dataset[0], GTK_PLOT_CONNECT_STRAIGHT);

	gdk_color_parse("blue", &color);
	gdk_colormap_alloc_color(gdk_colormap_get_system(), &color, FALSE, TRUE);

}


GtkWidget * teseo_plot_new(gdouble *ret_b, gdouble *ret_e, gulong ntries, const gchar * main_title , const gchar * Xtitle, const gchar * Ytitle){

	GtkWidget *window1;
	GtkWidget *vbox1;
	GtkWidget *scrollw1;
	GtkWidget *canvas;
	GtkPlotCanvasChild *child;

	gint page_width, page_height;
	gfloat scale = 4.;

	page_width = GTK_PLOT_LETTER_W * scale;
	page_height = GTK_PLOT_LETTER_H * scale;

	window1=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(window1), main_title);
	//gtk_widget_set_usize(window1,650,350); deprecated
	//set modal
	gtk_window_set_modal(GTK_WINDOW(window1),TRUE);
	gtk_window_set_default_size(GTK_WINDOW(window1),550,300);
	gtk_container_set_border_width(GTK_CONTAINER(window1),0);

	vbox1=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window1),vbox1);
	gtk_widget_show(vbox1);

	scrollw1=gtk_scrolled_window_new(NULL, NULL);
	gtk_container_set_border_width(GTK_CONTAINER(scrollw1),0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollw1), GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);

	gtk_box_pack_start(GTK_BOX(vbox1),scrollw1, TRUE, TRUE,0);
	gtk_widget_show(scrollw1);

	canvas = gtk_plot_canvas_new(page_width, page_height, 1.0);
	GTK_PLOT_CANVAS_SET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_DND_FLAGS);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollw1), canvas);

	//gdk_color_parse("light blue", &color);
	//gdk_color_alloc(gtk_widget_get_colormap(canvas), &color);
	//gtk_plot_canvas_set_background(GTK_PLOT_CANVAS(canvas), &color);

	gtk_widget_show(canvas);
	active_plot = new_layer(canvas);

	//gtk_plot_clip_data(GTK_PLOT(active_plot), TRUE);
	//gtk_plot_set_range(GTK_PLOT(active_plot), -1., 1., -1., 1.4);
	//gtk_plot_legends_move(GTK_PLOT(active_plot), 0.500, 0.05);
	//gtk_plot_set_legends_border(GTK_PLOT(active_plot), 0, 0);

	gtk_plot_hide_legends(GTK_PLOT(active_plot));
	gtk_plot_axis_hide_title(gtk_plot_get_axis(GTK_PLOT(active_plot), GTK_PLOT_AXIS_TOP));
	gtk_plot_axis_show_ticks(gtk_plot_get_axis(GTK_PLOT(active_plot), GTK_PLOT_AXIS_BOTTOM), 15, 3);
	gtk_plot_set_ticks(GTK_PLOT(active_plot), GTK_PLOT_AXIS_X, 5., 1);
	gtk_plot_set_ticks(GTK_PLOT(active_plot), GTK_PLOT_AXIS_Y, 20., 1);
	gtk_plot_axis_set_visible(gtk_plot_get_axis(GTK_PLOT(active_plot), GTK_PLOT_AXIS_TOP), FALSE);
	gtk_plot_axis_set_visible(gtk_plot_get_axis(GTK_PLOT(active_plot), GTK_PLOT_AXIS_RIGHT), FALSE);
	gtk_plot_x0_set_visible(GTK_PLOT(active_plot), TRUE);
	gtk_plot_y0_set_visible(GTK_PLOT(active_plot), TRUE);
	//gtk_plot_axis_set_labels_suffix(gtk_plot_get_axis(GTK_PLOT(active_plot), GTK_PLOT_AXIS_LEFT), "%");
	gtk_plot_axis_title_set_attributes(gtk_plot_get_axis(GTK_PLOT(active_plot), GTK_PLOT_AXIS_LEFT),
								NULL, 0, 0, NULL, NULL, FALSE,GTK_JUSTIFY_CENTER);
	child = gtk_plot_canvas_plot_new(GTK_PLOT(active_plot));
	gtk_plot_canvas_put_child(GTK_PLOT_CANVAS(canvas), child, .15, .06, .65, .31);
	gtk_widget_show(active_plot);
	GTK_PLOT_CANVAS_PLOT(child)->flags |= GTK_PLOT_CANVAS_PLOT_SELECT_POINT;
	GTK_PLOT_CANVAS_PLOT(child)->flags |= GTK_PLOT_CANVAS_PLOT_DND_POINT;

	g_signal_connect(GTK_OBJECT(canvas), "select_item",
			(GtkSignalFunc) select_item, NULL);

	child = gtk_plot_canvas_text_new("Times-BoldItalic", 16, 0, NULL, NULL, TRUE,
				GTK_JUSTIFY_CENTER,
				main_title);

	gtk_plot_canvas_put_child(GTK_PLOT_CANVAS(canvas), child, .40, .020, .0, .0);

	gtk_plot_text_set_border(&GTK_PLOT_CANVAS_TEXT(child)->text, GTK_PLOT_BORDER_SHADOW, 2, 0, 2);


	build_data(active_plot, ret_b, ret_e, ntries, Xtitle, Ytitle);

	//gtk_plot_canvas_export_ps(GTK_PLOT_CANVAS(canvas), "demoplot.ps", GTK_PLOT_PORTRAIT, FALSE, GTK_PLOT_LETTER);
	//OG
	//gtk_plot_canvas_export_ps_with_size(GTK_PLOT_CANVAS(canvas), "demoplot.ps", GTK_PLOT_PORTRAIT, TRUE, GTK_PLOT_PSPOINTS, 300, 400);

	return window1;
}

