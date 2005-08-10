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

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>


#include "teseocallbacks.h"
#include "teseointerface.h"
#include "teseosupport.h"

#include "teseo_path.h"
#include "teseo_io.h"
#include "teseo_bezier_fit.h"
#include "teseo_lock.h"
#include "teseo_resample.h"
#include "teseo_session.h"
#include "teseo_main.h"
#include "teseo_wmean.h"

GtkWidget * win_teseo;
GtkWidget * dlg_preferences;
GtkWidget * dlg_about;
GtkWidget * dlg_session;
GtkWidget * dlg_move_rotation;
GtkFileChooser * filechooser_import;
GtkFileChooser * filechooser_session;

GimpDrawable * private_drawable ;
gint32  teseo_image ;

char TODO_str[] = "Sorry, this function is not implemented yet!";
char new_session_name[] = "New session ...";

int
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

 int ret=0;
 GtkEntry * teseo_imagefile_entry      =  (GtkEntry *)   lookup_widget(GTK_WIDGET(dlg_session), "teseo_imagefile_entry");
 GtkEntry * teseo_imageresolution_entry=  (GtkEntry *)   lookup_widget(GTK_WIDGET(dlg_session), "teseo_imageresolution_entry");


 char * image_file = gimp_image_get_filename(teseo_image);
 gdouble xresolution,yresolution;
 char str_res[80];

 gtk_entry_set_text (teseo_imagefile_entry, image_file);

 gimp_image_get_resolution (teseo_image,  &xresolution,  &yresolution);

 sprintf(str_res,"%d",(gint) xresolution);
 strcat(str_res," dpi");
 gtk_entry_set_text (teseo_imageresolution_entry, str_res);

 gtk_window_set_title (GTK_WINDOW (dlg_session), new_session_name);

 gint result = gtk_dialog_run (GTK_DIALOG (dlg_session));

  switch (result)
    {
      case GTK_RESPONSE_OK:
          ret=1;
	  new_session(image_file, NULL);
         break;
      case GTK_RESPONSE_DELETE_EVENT:
         break;
      default:
         break;
    }
  gtk_widget_hide (dlg_session);
  gtk_window_set_title (GTK_WINDOW (dlg_session), "");
  return ret;
}


void
on_properties1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
 gint result = gtk_dialog_run (GTK_DIALOG (dlg_session));
  switch (result)
    {
      case GTK_RESPONSE_OK:
          if (!save_session(current_session))
             g_message("Unable to save current Session.");
         break;
      case GTK_RESPONSE_CANCEL:
      case GTK_RESPONSE_DELETE_EVENT:
         load_session(current_session);
         break;
      default:
         break;
    }
 gtk_widget_hide (dlg_session);
}

//return 1 on success, 2 on user refuse, 0 on error
int
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

  char old_current_session[FILENAMELEN];
  char filename[FILENAMELEN];
  char * path=NULL;

  int ret=0;
  int ls=0;

  strcpy(old_current_session, current_session);

  path=teseo_get_environment_path( SESSIONPATH );

  gtk_file_chooser_set_current_folder(filechooser_session, path );
  g_free(path);

  gint result = gtk_dialog_run (GTK_DIALOG (filechooser_session));

  switch (result)
    {
      case GTK_RESPONSE_OK:
	 strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_session)) );
         ret=load_session(filename);
         break;
      case GTK_RESPONSE_CANCEL:
	 ret=2;
         break;
      case GTK_RESPONSE_DELETE_EVENT:
	 ret=2;
         break;
      default:
         break;
    }

    gtk_widget_hide ((GtkWidget *) filechooser_session);

    if (ret==1) {
        result = gtk_dialog_run (GTK_DIALOG (dlg_session));
        switch (result)
          {
            case GTK_RESPONSE_OK:
	       //ret=1;
               break;
            case GTK_RESPONSE_CANCEL:
            case GTK_RESPONSE_DELETE_EVENT:
  	       gtk_window_set_title (GTK_WINDOW (dlg_session), "");
               ls=load_session(old_current_session);
	       ret=2;
               break;
            default:
               break;
          }
        gtk_widget_hide (dlg_session);
    }
    if (ret==0) {
           g_message("Unable to open session.");
    }
    return ret;
}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_preferences_2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_svg1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // Import SVG
    char filename[FILENAMELEN];
    char * path=NULL;

    path=teseo_get_environment_path( SVGPATH );
    gtk_window_set_title (GTK_WINDOW (filechooser_import), "Import bezier path from SVG file");
    gtk_file_chooser_set_current_folder(filechooser_import, path );
    g_free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

    switch (result)
    {
      case GTK_RESPONSE_OK:
	 strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) );
	 teseo_import_svg_vectors( teseo_image, filename );
	 break;
      case GTK_RESPONSE_CANCEL:

	 break;
      case GTK_RESPONSE_DELETE_EVENT:

	 break;
      default:
	 break;
    }
    gtk_widget_hide ((GtkWidget *) filechooser_import);

}


void
on_dxf2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

  char filename[FILENAMELEN];
  char * path=NULL;

  path=teseo_get_environment_path( DXFPATH );
  gtk_window_set_title (GTK_WINDOW (filechooser_import), "Open DXF file");
  gtk_file_chooser_set_current_folder(filechooser_import, path );
  g_free(path);

  gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

  switch (result)
    {
      case GTK_RESPONSE_OK:
         strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) );
         teseo_import_dxf( teseo_image, filename );
         break;
      case GTK_RESPONSE_CANCEL:
         g_message("Cancel pressed: don't do anything.");
         break;
      case GTK_RESPONSE_DELETE_EVENT:
         g_message("Delete event, same as Cancel pressed.");
         break;
      default:
         break;
    }
  gtk_widget_hide ((GtkWidget *) filechooser_import);
}


void
on_trace2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_sac2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_sac_xy2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_sisma2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_timemark2_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_path1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_dxf1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export DXF
    gchar *dxf_path = NULL;
    gchar *image_filename = NULL;
    gchar dxf_path_filename[FILENAMELEN];
    gchar *pathname = NULL;
    // TODO catch scale value
    gint scale = 0;

    dxf_path = teseo_get_environment_path( DXFPATH );
    image_filename = g_path_get_basename( gimp_image_get_filename(teseo_image) );
    pathname = gimp_path_get_current(teseo_image);

    // TODO check bad character in pathname ...
    g_sprintf(dxf_path_filename, "%s%s%s_%s%s", dxf_path, G_DIR_SEPARATOR_S, image_filename, pathname, DXF_EXT);

    // TODO check if dxf_path_filename exists

    if(teseo_path_semantic_type(teseo_image, gimp_path_get_current(teseo_image)) == PATH_SEMANTIC_POLYLINE) {
	// TODO check options in session windows before saving
	teseo_save_path_dxf(teseo_image, dxf_path_filename, scale);
	g_message("Path \n\"%s\"\n saved in file \"%s\".", pathname, dxf_path_filename);
    } else {
	g_message("Path \"%s\" need resampling before exporting in DXF format !", pathname);
    }
    
    g_free(dxf_path);
    g_free(image_filename);

}


void
on_track1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export Trace
    // teseo_save_path_traccia(teseo_image, filename_trace);
    g_message(TODO_str);
}


void
on_sac1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export SAC
    g_message(TODO_str);
}


void
on_sac_xy1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_sisma1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export SISMA
    // TODO teseo_save_path_sisma(teseo_image, filename_sisma);
    g_message(TODO_str);
}


void
on_timemark1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export Timemark
    // TODO teseo_save_path_timemarker(teseo_image, dxf_filename, 0);
    g_message(TODO_str);
}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_resample1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO catch parameter for resampling_bezier
    teseo_resampling_bezier(teseo_image, 1, 1);
}


void
on_align_all1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_align_all_path(teseo_image);
}


void
on_link_all1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_link_all_path(teseo_image);
}


void
on_move_and_rotation1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO catch x, y, rotate value
    gint x=0, y=0;
    gdouble rotation = 0.0;
    gint x_cur, y_cur;
    
    gint result;

    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;

    gimp_path_get_points (teseo_image, gimp_path_get_current(teseo_image), &path_closed, &num_path_point_details, &points_pairs);

    if(num_path_point_details < 1) {
	g_message("Path is empty !");
    }
    
    x_cur = (gint) points_pairs[0];
    y_cur = (gint) points_pairs[1];

    result = gtk_dialog_run (GTK_DIALOG (dlg_move_rotation));
    switch (result)
    {
	case GTK_RESPONSE_OK:
	    g_message(TODO_str);
	    teseo_path_move(teseo_image, x, y, rotation);
	    break;
	case GTK_RESPONSE_CANCEL:
	    break;
	case GTK_RESPONSE_DELETE_EVENT:
	    break;
	default:
	    break;
    }
    gtk_widget_hide (dlg_move_rotation);
}


void
on_fitting_polyline1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_fitting_bezier1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    glong num_strokes;
    gdouble *strokes = NULL;
    glong num_path;
    gdouble *path_inter = NULL;
    char pathname [255] ;
    char newpathname [255] ;

    gint num_paths=0;

    gimp_path_list (teseo_image, &num_paths);

    // if at least one path exists
    if (num_paths>0) {
	
	// catch the name ot the current path
	strcpy(pathname, gimp_path_get_current(teseo_image));
	
	// convert path in strokes
	strokes = teseo_open_path_to_strokes(teseo_image, &num_strokes,  pathname);

	// fitting strokes with bezier curves
	teseo_fitting_bezier(num_strokes, strokes, &num_path, &path_inter);

	sprintf(newpathname, "%s - Fit Bezier", pathname);

	// create new path 
	gimp_path_set_points(teseo_image, newpathname, 1, num_path * 3, path_inter);

	if(path_inter) {
		g_free(path_inter);
	}
	if(strokes) {
		g_free(strokes);
	}
    }
}


void
on_recover_last2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_evaluate_middle_tms1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_adjustment1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_info_window_1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    gint result = gtk_dialog_run (GTK_DIALOG (dlg_about));
    switch (result)
    {
	case GTK_RESPONSE_OK:
	    break;
	case GTK_RESPONSE_DELETE_EVENT:
	    break;
	default:
	    break;
    }
    gtk_widget_hide (dlg_about);
}




void
on_preferences_w_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

    gint result = gtk_dialog_run (GTK_DIALOG (dlg_preferences));

    switch (result)
    {
	case GTK_RESPONSE_OK:
	    g_message("OK pressed: save new preferences.");
	    g_message(TODO_str);
	    break;
	case GTK_RESPONSE_CANCEL:
	    g_message("Cancel pressed: don't save anything, restore the old preferences in the preferences dialog.");
	    g_message(TODO_str);
	    break;
	case GTK_RESPONSE_DELETE_EVENT:
	    g_message("Delete event, same as Cancel pressed.");
	    g_message(TODO_str);
	    break;
	default:
	    break;
    }
    gtk_widget_hide (dlg_preferences);

}


gboolean
on_win_teseo_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    teseo_unlock();
    gtk_main_quit();
    return TRUE;
}


gboolean
on_win_preferences_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}


void
on_dlg_preferences_response            (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{
  gtk_widget_hide(dlg_preferences);
  // return response_id;
}


void
on_dlg_preferences_close               (GtkDialog       *dialog,
                                        gpointer         user_data)
{
  gtk_widget_hide(dlg_preferences);
  // return FALSE;
}



void
on_ascii1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_svg2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export SVG
    g_message("%s\nYou can use GIMP Paths tool to make this operation.",TODO_str);
}


void
on_ascii2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export ASCII
    g_message(TODO_str);
}


void
on_bezier1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // Import Bezier path (Teseo 1 format)
    char filename[FILENAMELEN];
    char * path=NULL;

    path=teseo_get_environment_path( BEZIERPATH );
    gtk_window_set_title (GTK_WINDOW (filechooser_import), "Open Bezier Path");
    gtk_file_chooser_set_current_folder(filechooser_import, path );
    g_free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

    switch (result)
    {
      case GTK_RESPONSE_OK:
	 strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) );
	 teseo_import_bzr( teseo_image, filename );
	 break;
      case GTK_RESPONSE_CANCEL:
	 g_message("Cancel pressed: don't do anything.");
	 break;
      case GTK_RESPONSE_DELETE_EVENT:
	 g_message("Delete event, same as Cancel pressed.");
	 break;
      default:
	 break;
    }
    gtk_widget_hide ((GtkWidget *) filechooser_import);
}

void
on_teseo_alg_back_toolbutton_clicked   (GtkButton   *button,
                                        gpointer         user_data)
{
    g_message("%s\nPress Back button.", TODO_str);
}


void
on_teseo_alg_go_toolbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
   int iter;
   GtkSpinButton * tfss = (GtkSpinButton *)   lookup_widget(GTK_WIDGET(win_teseo), "teseo_forward_step_spinbutton");
   iter=gtk_spin_button_get_value_as_int (tfss) ;
   gint32 drawable_ID=  gimp_image_get_active_drawable  (teseo_image);

   teseo_main_loop(iter, drawable_ID );
}


void
on_alg_wmean_radiotoolbutton_clicked   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{

  GtkButton *teseo_alg_go_toolbutton   = (GtkButton *)   lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_go_toolbutton");
  GtkButton *teseo_alg_back_toolbutton = (GtkButton *)   lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_back_toolbutton");

  wmeanParams s;

  s.colour=1;
  s.step   = 1 ;
  s.width  = 5;
  s.height = 50;

  GtkSpinButton * twhs = (GtkSpinButton *)   lookup_widget(GTK_WIDGET(win_teseo), "teseo_wm_height_spinbutton");
  GtkSpinButton * twws = (GtkSpinButton *)   lookup_widget(GTK_WIDGET(win_teseo), "teseo_wm_width_spinbutton");
  GtkRadioButton * twcbr = (GtkRadioButton *) lookup_widget(GTK_WIDGET(win_teseo), "teseo_wm_colour_black_radiobutton");

  if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(twcbr))  == TRUE ){
    s.colour=0;
    }
  else {
    s.colour=255;
    }

  s.width  = gtk_spin_button_get_value_as_int (twws);
  s.height = gtk_spin_button_get_value_as_int (twhs);

 teseo_main_init(
			(*teseo_wmean),
			(*teseo_wmean_init),
			&s,
			(*teseo_wmean_getinput),
			(*teseo_wmean_terminate),
			(*teseo_wmean_accumulate),
			(*teseo_wmean_starting_os),
			(*teseo_wmean_new_is),
			(*teseo_wmean_release)
		);

  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_go_toolbutton, TRUE);
  GTK_WIDGET_SET_FLAGS (teseo_alg_go_toolbutton, GTK_CAN_FOCUS);
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_back_toolbutton, TRUE);
  GTK_WIDGET_SET_FLAGS (teseo_alg_back_toolbutton, GTK_CAN_FOCUS);

}


/*
 Changes in the algorithms params disable go and back buttons, so the user reinitialise after changes
*/

static inline void disable_buttons(){
  GtkButton *teseo_alg_go_toolbutton   = (GtkButton *)   lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_go_toolbutton");
  GtkButton *teseo_alg_back_toolbutton = (GtkButton *)   lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_back_toolbutton");
  GtkRadioToolButton *ghost_radiotoolbutton = (GtkRadioToolButton *) lookup_widget(GTK_WIDGET(win_teseo), "ghost_radiotoolbutton");

  //toggle the alghoritms buttons as side effect
  gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (ghost_radiotoolbutton), TRUE);
  //set go and back buttons insensitive
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_go_toolbutton, FALSE);
  GTK_WIDGET_UNSET_FLAGS (teseo_alg_go_toolbutton, GTK_CAN_FOCUS);
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_back_toolbutton, FALSE);
  GTK_WIDGET_UNSET_FLAGS (teseo_alg_back_toolbutton, GTK_CAN_FOCUS);
}


void
on_teseo_wm_colour_black_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
 disable_buttons();
}


gint
on_teseo_wm_height_spinbutton_input    (GtkSpinButton   *spinbutton,
                                        gdouble *new_value,
                                        gpointer         user_data)
{
  //input signal once for every press on arrows
  disable_buttons();
  return TRUE;
}


gint
on_teseo_wm_width_spinbutton_input     (GtkSpinButton   *spinbutton,
                                        gdouble *new_value,
                                        gpointer         user_data)
{
  disable_buttons();
  return TRUE;
}


void
on_dlg_session_show                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
 static char session_name[FILENAMELEN];
 gchar *g_session_name = g_path_get_basename(current_session);
 strcpy(session_name, g_session_name);
 g_free(g_session_name);
 if(strcmp(gtk_window_get_title (GTK_WINDOW (dlg_session)), new_session_name) != 0) {
  gtk_window_set_title (GTK_WINDOW (dlg_session), session_name);
 }
}


void
on_win_teseo_show                (GtkWidget       *widget,
                                        gpointer         user_data)
{
  GtkMenuItem     *menu_open1 = (GtkMenuItem *)   lookup_widget(GTK_WIDGET(win_teseo), "open1");
  GtkMenuItem     *menu_new1 = (GtkMenuItem *)    lookup_widget(GTK_WIDGET(win_teseo), "new1");
  if (on_open1_activate (menu_open1, NULL) != 1)
  {
    //g_message("Open canceled");
    if (on_new1_activate (menu_new1, NULL)==0) {
      //g_message("New canceled");
      on_win_teseo_show(win_teseo,NULL);
    }
  }
}

void
on_dlg_about_show_label_ver            (GtkWidget       *widget,
                                        gpointer         user_data)
{
    gchar teseo_caption_markup[200];
    sprintf(teseo_caption_markup, "<small>Open source software &#169; %s\n%s</small>", TESEO_YEAR, TESEO_CAPTION_DEV);
    gtk_label_set_label((GtkLabel *) widget, teseo_caption_markup);
}


void
on_dlg_session_show_teseo_eventpathname
                                        (GtkWidget       *widget,
                                        gpointer         user_data)
{
    GtkComboBoxEntry * combo_box = (GtkComboBoxEntry *) widget;
    gchar **path_list;
    gint n_path, i;

    // TODO insert path in teseo_eventpathname combobox and select right one....
    path_list = gimp_path_list(teseo_image, &n_path);
    for(i=0; i < n_path; i++) {
	gtk_combo_box_append_text(combo_box, path_list[i]);
    }
    

}
