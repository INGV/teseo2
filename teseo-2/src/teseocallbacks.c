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
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

//#include <gtkextra/gtkplot.h>

#include "teseocallbacks.h"
#include "teseointerface.h"
#include "teseosupport.h"

#include "teseo_path.h"
#include "teseo_io.h"
#include "teseo_bezier_pfit.h"
#include "teseo_lock.h"
#include "teseo_resample.h"
#include "teseo_session.h"
#include "teseo_main.h"
#include "teseo_wmean.h"
#include "teseo_gimp_extends.h"
#include "teseo_snap.h"
#include "teseo_sac.h"
#include "teseo_timemark.h"
#include "teseo_filters.h"
#include "teseo_wiechert.h"

#include <gtkdatabox.h>
#include "teseo_databox.h"

GtkWidget * win_teseo;
GtkWidget * dlg_preferences;
GtkWidget * dlg_about;
GtkWidget * dlg_session;
GtkWidget * dlg_parasites;
GtkWidget * win_curvature;



GtkWidget *active_plot=NULL;
GtkWidget *active_slope_plot=NULL;

extern gdouble ret_b[N_TRIES];
extern gdouble ret_errors[N_TRIES];
extern gdouble hist_points[DEFAULT_B*2*180];
extern gdouble histogram[180];
gdouble dx1[800]={};
gdouble dy1[800]={};

//GtkWidget * real_gtk_plot=NULL;


GtkFileChooserDialog * filechooser_import;
GtkFileChooserDialog * filechooser_export;
GtkFileChooserDialog * filechooser_session;




/*Exception a global widget*/
GtkSpinButton * teseo_spbtn_arm_shift;



GimpDrawable * private_drawable ;
gint32  teseo_image ;

char TODO_str[] = "Sorry, this function is not implemented yet!";
char new_session_name[] = "New session ...";

// Private function to wrap teseo_lookup_widget
GtkWidget*
teseo_lookup_widget(GtkWidget *widget, const gchar *widget_name, gint default_value) {
    gchar msg[1024];
    GtkWidget * ret = lookup_widget(widget, widget_name);
    if(!ret) {
	g_sprintf(msg, "Teseo2-WARNING: widget \"%s\" not found in \"%s\". Set to %d (default).", gtk_widget_get_name(widget), widget_name, default_value);
	g_message(msg);
    }
    return ret;
}

// Private function to wrap gtk_statusbar_push
guint teseo_gtk_statusbar_push(const gchar *text) {
      return gtk_statusbar_push (GTK_STATUSBAR(teseo_lookup_widget(GTK_WIDGET(win_teseo), "statusbar", 0)), 0, text);
}

int
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

 int ret=0;
 GtkEntry * teseo_imagefile_entry      =  (GtkEntry *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_imagefile_entry", 0);
 GtkEntry * teseo_imageresolution_entry=  (GtkEntry *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_imageresolution_entry", 0);

 char * image_file = gimp_image_get_filename(teseo_image);
 gdouble xresolution,yresolution;
 gchar str_res[80];

 gtk_entry_set_text (teseo_imagefile_entry, image_file);

 gimp_image_get_resolution (teseo_image,  &xresolution,  &yresolution);

 g_sprintf(str_res,"%d",(gint) xresolution);
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
  gchar * tep = NULL;
  gchar filename_session [FILENAMELEN];

  gint result = gtk_dialog_run (GTK_DIALOG (dlg_session));
  switch (result)
    {
      case GTK_RESPONSE_OK:
         if (!save_session(current_session))
	 	g_message("Unable to save current Session.");
         break;
      case GTK_RESPONSE_CANCEL:
      case GTK_RESPONSE_DELETE_EVENT:
	// concatenate path session and filename
	tep = teseo_get_environment_path(SESSIONPATH);
	strcpy(filename_session,tep);
	strcat(filename_session,G_DIR_SEPARATOR_S);
	strcat(filename_session,current_session);
	g_free(tep);
	load_session(filename_session);
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
  //char filename[FILENAMELEN];
  gchar * filename =NULL;
  char * path=NULL;
  gchar * tep = NULL;
  gchar filename_session [FILENAMELEN];


  int ret=0;
  int ls=0;

  strcpy(old_current_session, current_session);

  path=teseo_get_environment_path( SESSIONPATH );

  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_session), path );
  //g_free(path);

  gint result = gtk_dialog_run (GTK_DIALOG (filechooser_session));

  switch (result)
    {
	case GTK_RESPONSE_OK:
		//strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_session)) );
		//g_message("before loading");
		//ret=load_session(filename);
		filename =  gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER (filechooser_session)) ;
		//g_message("Opening %s",filename);
		ret=load_session(filename);
		g_free(filename);
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
              if (!save_session(current_session))
                g_message("Unable to save current Session.");
              break;
            case GTK_RESPONSE_CANCEL:
            case GTK_RESPONSE_DELETE_EVENT:
  	       gtk_window_set_title (GTK_WINDOW (dlg_session), "");
	       // concatenate path session and filename
	       tep = teseo_get_environment_path(SESSIONPATH);
	       strcpy(filename_session,tep);
	       strcat(filename_session,G_DIR_SEPARATOR_S);
	       strcat(filename_session,old_current_session);
	       g_free(tep);
	       ls=load_session(filename_session);
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
    g_free(path);
    return ret;
}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
 g_message(TODO_str);
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
    //char filename[FILENAMELEN];
    gchar * filename = NULL;
    char * path=NULL;

    path=teseo_get_environment_path( SVGPATH );
    gtk_window_set_title (GTK_WINDOW (filechooser_import), "Import bezier path from SVG file");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_import), path );
    g_free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

    switch (result)
    {
	case GTK_RESPONSE_OK:
		//strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) );
		filename =  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) ;
		teseo_import_svg_vectors( teseo_image, filename );
		g_free(filename);
		teseo_gtk_statusbar_push("SVG file imported in current path.");
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
on_svg_combine1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // Import SVG combining more components
    //char filename[FILENAMELEN];
    gchar * filename = NULL;
    char * path=NULL;

    path=teseo_get_environment_path( SVGPATH );
    gtk_window_set_title (GTK_WINDOW (filechooser_import), "Import bezier path from SVG file combining more components");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_import), path );
    g_free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

    switch (result)
    {
	case GTK_RESPONSE_OK:
		filename =  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) ;
		teseo_import_svg_vectors_combining_more_components(teseo_image, filename );
		g_free(filename);
		teseo_gtk_statusbar_push("SVG file imported in current path combining more components.");
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
  // Import DXF
  //char filename[FILENAMELEN];
  gchar * filename= NULL;
  char * path=NULL;

  path=teseo_get_environment_path( DXFPATH );
  gtk_window_set_title (GTK_WINDOW (filechooser_import), "Open DXF file");
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_import), path );
  g_free(path);

  gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

  switch (result)
    {
      case GTK_RESPONSE_OK:
         //strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) );
         filename= gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import));
         teseo_import_dxf( teseo_image, filename );
	 g_free(filename);
         teseo_gtk_statusbar_push("DXF file imported in current path.");
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
    // Import Ascii timemarker path (Teseo 2 format)
    //char filename[FILENAMELEN];
    gchar * filename = NULL;
    char * path=NULL;

    path=teseo_get_environment_path( TMARKPATH );
    gtk_window_set_title (GTK_WINDOW (filechooser_import), "Open ASCII Timemarker path");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_import), path );
    g_free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

    switch (result)
    {
	case GTK_RESPONSE_OK:
		//strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) );
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import));
		teseo_import_path_ascii( teseo_image, filename, 1 );
		g_free(filename);
		teseo_gtk_statusbar_push("ASCII Timemarker file imported in current path.");
		break;
	case GTK_RESPONSE_CANCEL:
		//g_message("Cancel pressed: don't do anything.");
		break;
	case GTK_RESPONSE_DELETE_EVENT:
		//g_message("Delete event, same as Cancel pressed.");
		break;
	default:
		break;
    }
    gtk_widget_hide ((GtkWidget *) filechooser_import);

}


void
on_path1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}

/*

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



 gtk_file_chooser_set_current_name (GtkFileChooser *chooser, const gchar *name);

TODO correct gtk_file_chooser_get_filename lines gfree the result!!!!


*/


void
on_dxf1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // Export DXF
    gchar *dxf_path = NULL;
    gchar *image_filename = NULL;
    // gchar dxf_path_filename[FILENAMELEN];
    gchar dxf_filename[FILENAMELEN];
    gchar * filename = NULL;
    gchar *pathname = NULL;
    // TODO catch scale value
    gint result;
    gint scale = 0;
    GtkCheckButton * teseo_reloc = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_reloc_checkbutton", scale);

    if(teseo_reloc) {
        scale = gtk_toggle_button_get_active((GtkToggleButton *) teseo_reloc);
    }

    dxf_path = teseo_get_environment_path( DXFPATH );
    image_filename = g_path_get_basename( gimp_image_get_filename(teseo_image) );
    pathname = gimp_path_get_current(teseo_image);

    // TODO check bad character in pathname ...
    //    g_sprintf(dxf_path_filename, "%s%s%s_%s%s", dxf_path, G_DIR_SEPARATOR_S, image_filename, pathname, DXF_EXT);
    g_sprintf(dxf_filename, "%s_%s%s",  image_filename, pathname, DXF_EXT);

    gtk_window_set_title (GTK_WINDOW (filechooser_export), "Save as DXF");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_export), dxf_path );
    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(filechooser_export), dxf_filename);

    if(teseo_path_semantic_type(teseo_image, gimp_path_get_current(teseo_image)) == PATH_SEMANTIC_POLYLINE) {
        result = gtk_dialog_run (GTK_DIALOG (filechooser_export));

        switch (result)
        {
            case GTK_RESPONSE_OK:

                filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_export)) ;
                //g_sprintf(dxf_path_filename, "%s%s%s", dxf_path, G_DIR_SEPARATOR_S, filename);
                g_printf("\ndxf filename: %s\n", filename);

                // TODO check if dxf_path_filename exists
                if(teseo_if_file_exists_overwrite_dialog(filename)) {
                    // TODO check options in session windows before saving
                    teseo_save_path_dxf(teseo_image, filename, scale);
                }

		g_free(filename);

                // g_message("Path \n\"%s\"\n saved in file \"%s\".", pathname, filename);
                teseo_gtk_statusbar_push("Current path exported in DXF format.");

                break;
            case GTK_RESPONSE_CANCEL:

                break;
            case GTK_RESPONSE_DELETE_EVENT:

                break;
            default:
                break;
        }
        gtk_widget_hide ((GtkWidget *) filechooser_export);
    } else {
        g_message("Path \"%s\" needs resampling or forcing polyline before exporting in DXF format !", pathname);
    }


    g_free(dxf_path);
    g_free(image_filename);
    //g_free(filename);
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
    // Export SAC
    gchar *sac_path = NULL;
    gchar *image_filename = NULL;
    gchar sac_path_filename[FILENAMELEN];
    gchar *pathname = NULL;
    float paper_velocity = 1;
    gchar * filename = NULL;
    gint result;
    // TODO catch scale value

    sac_path = teseo_get_environment_path( SACPATH );
    image_filename = g_path_get_basename( gimp_image_get_filename(teseo_image) );
    pathname = gimp_path_get_current(teseo_image);

    // TODO check bad character in pathname ...
    // g_sprintf(sac_path_filename, "%s%s%s_%s%s", sac_path, G_DIR_SEPARATOR_S, image_filename, pathname, SAC_EXT);
    g_sprintf(sac_path_filename, "%s_%s%s",  image_filename, pathname, SAC_EXT);

    // TODO check if sac_path_filename exists

    gtk_window_set_title (GTK_WINDOW (filechooser_export), "Save as SAC");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_export), sac_path );
    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(filechooser_export), sac_path_filename);

    if(teseo_path_semantic_type(teseo_image, gimp_path_get_current(teseo_image)) == PATH_SEMANTIC_POLYLINE) {

        result = gtk_dialog_run (GTK_DIALOG (filechooser_export));
        switch (result)
        {
            case GTK_RESPONSE_OK:
                filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_export)) ;

                // TODO check options in session windows before saving
                // TODO when resampling catch delta value for SAC
                // TODO catch paper_velocity

                gchar KSTNM[8] = SACSTRUNDEF;
                float CMPAZ=0.0, CMPINC=0.0, STLA=0.0, STLO=0.0, STEL=0.0;

                GtkEntry *teseo_kstnm_entry      =  (GtkEntry *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_kstnm_entry", 0);

                GtkSpinButton *teseo_paper_speed_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_paper_speed_spinbutton", paper_velocity);
                GtkSpinButton *teseo_cmpaz_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_cmpaz_spinbutton", CMPAZ);
                GtkSpinButton *teseo_cmpinc_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_cmpinc_spinbutton", CMPINC);
                GtkSpinButton *teseo_stla_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_stla_spinbutton", STLA);
                GtkSpinButton *teseo_stlo_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_stlo_spinbutton", STLO);
                GtkSpinButton *teseo_stel_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_stel_spinbutton", STEL);

                if(teseo_paper_speed_spinbutton) {
                    paper_velocity = gtk_spin_button_get_value (teseo_paper_speed_spinbutton)/60; //Obtaining mm/sec
                }

                if(teseo_kstnm_entry) {
                    strcpy(KSTNM, gtk_entry_get_text(teseo_kstnm_entry));
                    if(strcmp(KSTNM, "") == 0) {
                        strcpy(KSTNM, SACSTRUNDEF);
                    }
                }

                if(teseo_cmpaz_spinbutton) {
                    CMPAZ = gtk_spin_button_get_value (teseo_cmpaz_spinbutton);
                }

                if(teseo_cmpinc_spinbutton) {
                    CMPINC = gtk_spin_button_get_value (teseo_cmpinc_spinbutton);
                }

                if(teseo_stla_spinbutton) {
                    STLA = gtk_spin_button_get_value (teseo_stla_spinbutton);
                }

                if(teseo_stlo_spinbutton) {
                    STLO = gtk_spin_button_get_value (teseo_stlo_spinbutton);
                }

                if(teseo_stel_spinbutton) {
                    STEL = gtk_spin_button_get_value (teseo_stel_spinbutton);
                }


                if(teseo_if_file_exists_overwrite_dialog(filename)) {
                    if(teseo_sac_path_export(teseo_image, filename, paper_velocity, KSTNM, CMPAZ, CMPINC, STLA, STLO, STEL)) {
                        // g_message("Path \n\"%s\"\n saved in file \"%s\".", pathname, filename);
                        teseo_gtk_statusbar_push("Current path exported in SAC format.");
                    } else {
                        g_message("Teseo2-CRITICAL: an error occurred saving path \n\"%s\"\n in file \"%s\".\nAre you sure path is evenly spaced?", pathname, filename);
                    }
                }

                break;
            case GTK_RESPONSE_CANCEL:

                break;
            case GTK_RESPONSE_DELETE_EVENT:

                break;
            default:
                break;
        }
        gtk_widget_hide ((GtkWidget *) filechooser_export);


        

    } else {
	g_message("Path \"%s\" needs resampling before exporting in SAC format !", pathname);
    }

    g_free(sac_path);
    g_free(image_filename);
    g_free(filename);
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
    gchar *timemark_path = NULL;
    gchar *image_filename = NULL;
    GString *timemark_path_filename=NULL;
    gchar *pathname = NULL;
    gchar * filename = NULL;
    gint result;

    timemark_path = teseo_get_environment_path( TMARKPATH );
    image_filename = g_path_get_basename( gimp_image_get_filename(teseo_image) );
    pathname = gimp_path_get_current(teseo_image);

    timemark_path_filename = g_string_new("Uname");

    // TODO check bad character in pathname ...
    // g_string_printf(timemark_path_filename, "%s%s%s_%s%s", timemark_path, G_DIR_SEPARATOR_S, image_filename, pathname, ASCII_EXT);
    g_string_printf(timemark_path_filename, "%s_%s%s", image_filename, pathname, ASCII_EXT);

    // TODO check if timemark_path_filename exists

    gtk_window_set_title (GTK_WINDOW (filechooser_export), "Save as TIMEMARK ASCII");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_export), timemark_path );
    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(filechooser_export), timemark_path_filename->str);

    if(teseo_path_semantic_type(teseo_image, gimp_path_get_current(teseo_image)) == PATH_SEMANTIC_POLYLINE) {

        result = gtk_dialog_run (GTK_DIALOG (filechooser_export));
        switch (result)
        {
            case GTK_RESPONSE_OK:
                filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_export)) ;

                // TODO check if timemark_path_filename exists
                if(teseo_if_file_exists_overwrite_dialog(filename)) {
                    // TODO check options in session windows before saving
                    teseo_save_path_ascii(teseo_image, filename);
                    // g_message("Path \n\"%s\"\n saved in file \"%s\".", pathname, filename);
                }
		g_free(filename);
                teseo_gtk_statusbar_push("Current path exported in TIMEMARK ASCII format.");

                break;
            case GTK_RESPONSE_CANCEL:

                break;
            case GTK_RESPONSE_DELETE_EVENT:

                break;
            default:
                break;
        }
        gtk_widget_hide ((GtkWidget *) filechooser_export);

    } else {
	g_message("Path \"%s\" needs forcing polyline before exporting in ASCII format !", pathname);
    }

    g_string_free(timemark_path_filename, TRUE);
    g_free(timemark_path);
    g_free(image_filename);

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
    // default is 1
    gint step_bezier = 1;
    // default is TRUE
    gboolean abscissa_asc = TRUE;

    GtkSpinButton * teseo_step_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_step_spinbutton", step_bezier);
    GtkCheckButton * teseo_abscissa_asc_checkbutton = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_abscissa_asc_checkbutton", abscissa_asc);

    if(teseo_step_spinbutton) {
	step_bezier = gtk_spin_button_get_value_as_int (teseo_step_spinbutton) ;
    }

    if(teseo_abscissa_asc_checkbutton) {
	abscissa_asc = gtk_toggle_button_get_active((GtkToggleButton *) teseo_abscissa_asc_checkbutton);
    }

    teseo_resampling_bezier(teseo_image, abscissa_asc, step_bezier);
}


void
on_align_all1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_align_all_path(teseo_image);
}

void
on_align_unlocked_paths1_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_align_all_path_unlocked(teseo_image, FALSE);
}


void
on_link_all1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_link_all_path(teseo_image);
}


void
on_link_unlocked_paths1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_link_all_path_unlocked(teseo_image, FALSE);
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
    gchar newpathname [255] ;

    gint num_paths=0;

    gimp_path_list (teseo_image, &num_paths);

    // if at least one path exists
    if (num_paths>0) {

        if(teseo_path_semantic_type(teseo_image, gimp_path_get_current(teseo_image)) == PATH_SEMANTIC_POLYLINE) {

            // catch the name ot the current path
            strcpy(pathname, gimp_path_get_current(teseo_image));

            // convert path in strokes
            strokes = teseo_open_path_to_strokes(teseo_image, &num_strokes,  pathname);

	    if(strokes) {
		    // fitting strokes with bezier curves
		    teseo_p_fitting_bezier(num_strokes, strokes, &num_path, &path_inter);

		    g_sprintf(newpathname, "%s_F", pathname);

		    // create new path
		    gimp_path_set_points(teseo_image, newpathname, 1, num_path, path_inter);

		    g_free(path_inter);
		    g_free(strokes);
	    } else {
		    g_message("Teseo2: Path fitting has not been executed!!");
	    }

        } else {
            g_message("Teseo2: Path fitting is enabled only on polylines!");
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
    GList *x_list, *iter_list;
    gint i;
    x_list = teseo_timemark_check_path(teseo_image);
    if(x_list) {
        for(iter_list = g_list_first(x_list), i=1; iter_list != NULL; iter_list = g_list_next(iter_list), i++ ) {
            g_printf("%d: %d\n", i, GPOINTER_TO_INT(iter_list->data));
            gimp_image_add_vguide(teseo_image, GPOINTER_TO_INT(iter_list->data));
        }
    } else {
        g_message("Path should have all timemarks.");
    }
    g_list_free(x_list);
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
    gimp_help(NULL, "teseo");
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
    // Import Ascii poliline path (Teseo 2 format)
    //char filename[FILENAMELEN];
    gchar* filename = NULL;
    char * path=NULL;

    path=teseo_get_environment_path( ASCIIPATH );
    gtk_window_set_title (GTK_WINDOW (filechooser_import), "Open ASCII Path");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_import), path );
    g_free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

    switch (result)
    {
	case GTK_RESPONSE_OK:
		//strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) );
		filename =  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import));
		teseo_import_path_ascii( teseo_image, filename, 0 );
		g_free(filename);
		teseo_gtk_statusbar_push("ASCII file imported in current path.");
		break;
	case GTK_RESPONSE_CANCEL:
		//g_message("Cancel pressed: don't do anything.");
		break;
	case GTK_RESPONSE_DELETE_EVENT:
		//g_message("Delete event, same as Cancel pressed.");
		break;
	default:
		break;
    }
    gtk_widget_hide ((GtkWidget *) filechooser_import);
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
    // Export ASCII

    gchar *ascii_path = NULL;
    gchar *image_filename = NULL;
    GString *ascii_path_filename=NULL;
    gchar *pathname = NULL;
    gchar * filename = NULL;
    gint result;

    ascii_path = teseo_get_environment_path( ASCIIPATH );
    image_filename = g_path_get_basename( gimp_image_get_filename(teseo_image) );
    pathname = gimp_path_get_current(teseo_image);

    ascii_path_filename = g_string_new("Uname");

    // TODO check bad character in pathname ...
    // g_string_printf(ascii_path_filename, "%s%s%s_%s%s", ascii_path, G_DIR_SEPARATOR_S, image_filename, pathname, ASCII_EXT);
    g_string_printf(ascii_path_filename, "%s_%s%s", image_filename, pathname, ASCII_EXT);

    // TODO check if ascii_path_filename exists

    gtk_window_set_title (GTK_WINDOW (filechooser_export), "Save as ASCII");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_export), ascii_path );
    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(filechooser_export), ascii_path_filename->str);

    if(teseo_path_semantic_type(teseo_image, gimp_path_get_current(teseo_image)) == PATH_SEMANTIC_POLYLINE) {

        result = gtk_dialog_run (GTK_DIALOG (filechooser_export));
        switch (result)
        {
            case GTK_RESPONSE_OK:
                filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_export)) ;
		// TODO check if ascii_path_filename exists
                if(teseo_if_file_exists_overwrite_dialog(filename)) {
                    // TODO check options in session windows before saving
                    teseo_save_path_ascii(teseo_image, filename);
                    // g_message("Path \n\"%s\"\n saved in file \"%s\".", pathname, filename);
                }
		g_free(filename);
                teseo_gtk_statusbar_push("Current path exported in ASCII format.");

                break;
            case GTK_RESPONSE_CANCEL:

                break;
            case GTK_RESPONSE_DELETE_EVENT:

                break;
            default:
                break;
        }
        gtk_widget_hide ((GtkWidget *) filechooser_export);

    } else {
	g_message("Path \"%s\" needs resampling or forcing polyline before exporting in ASCII format !", pathname);
    }

    g_string_free(ascii_path_filename, TRUE);
    g_free(ascii_path);
    g_free(image_filename);

}


void
on_bezier1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // Import Bezier path (Teseo 1 format)
    //char filename[FILENAMELEN];
    gchar * filename=NULL;
    char * path=NULL;

    path=teseo_get_environment_path( BEZIERPATH );
    gtk_window_set_title (GTK_WINDOW (filechooser_import), "Open Bezier Path");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_import), path );
    g_free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (filechooser_import));

    switch (result)
    {
      case GTK_RESPONSE_OK:
	 //strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import)) );
	 filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_import));
	 teseo_import_bzr( teseo_image, filename );
         teseo_gtk_statusbar_push("Bezier file imported in current path.");
	 g_free(filename);
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
on_teseo_alg_go_toolbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
   int iter;

   gboolean check_guide;
   gboolean use_guide=FALSE;
   gboolean use_bezier=FALSE;

   GtkSpinButton  * tfss  = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_forward_step_spinbutton", 0);
   GtkCheckButton * tcfgs = (GtkCheckButton*) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_checkbtn_first_guide_stop", 0);
   GtkCheckButton * tcdbc = (GtkCheckButton*) teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_direct_bez_checkbutton", 0);

   use_bezier = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tcdbc));
   check_guide = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tcfgs));


   iter = gtk_spin_button_get_value_as_int (tfss) ;

   gint32 drawable_ID=  gimp_image_get_active_drawable  (teseo_image);

   //if checked and exist one guide
    gint32 *guides=NULL, n_guides;

    if (check_guide){
      n_guides = teseo_gimp_image_find_guides_orientation(teseo_image, GIMP_ORIENTATION_VERTICAL, &guides);
      if(n_guides >= 1) {
	iter=guides[0];
//	g_message("Stopping at guide in %d",iter);
	use_guide=TRUE;
      }
      else {
	  g_message("No guide found");
      }
      g_free(guides);
    }
    teseo_wmean_set_dir(RIGHT);
    teseo_main_loop(iter, drawable_ID,use_guide, use_bezier);
}


void
on_alg_wmean_radiotoolbutton_clicked   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{

  GtkButton *teseo_alg_go_toolbutton   = (GtkButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_go_toolbutton", 0);
  GtkButton *teseo_alg_back_toolbutton = (GtkButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_back_toolbutton", 0);
  GtkButton *teseo_alg_up_toolbutton   = (GtkButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_up_toolbutton", 0);
  GtkButton *teseo_alg_down_toolbutton = (GtkButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_down_toolbutton", 0);


  wmeanParams s;

  s.colour=1;
  s.step   = 1 ;
  s.width  = 5;
  s.height = 50;

  GtkSpinButton * twhs = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_wm_height_spinbutton", 0);
  GtkSpinButton * twws = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_wm_width_spinbutton", 0);
  GtkRadioButton * twcbr = (GtkRadioButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_wm_colour_black_radiobutton", 0);

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
			(*teseo_wmean_get_x),
			(*teseo_wmean_release)
		);

  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_go_toolbutton, TRUE);
  GTK_WIDGET_SET_FLAGS (teseo_alg_go_toolbutton, GTK_CAN_FOCUS);
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_back_toolbutton, TRUE);
  GTK_WIDGET_SET_FLAGS (teseo_alg_back_toolbutton, GTK_CAN_FOCUS);

  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_up_toolbutton, TRUE);
  GTK_WIDGET_SET_FLAGS (teseo_alg_up_toolbutton, GTK_CAN_FOCUS);
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_down_toolbutton, TRUE);
  GTK_WIDGET_SET_FLAGS (teseo_alg_down_toolbutton, GTK_CAN_FOCUS);


}


/*
 Changes in the algorithms params disable go and back buttons, so the user reinitialise after changes
*/

static inline void disable_buttons(){
  GtkButton *teseo_alg_go_toolbutton   = (GtkButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_go_toolbutton", 0);
  GtkButton *teseo_alg_back_toolbutton = (GtkButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_back_toolbutton", 0);

  GtkButton *teseo_alg_up_toolbutton   = (GtkButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_up_toolbutton", 0);
  GtkButton *teseo_alg_down_toolbutton = (GtkButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_alg_down_toolbutton", 0);


  GtkRadioToolButton *ghost_radiotoolbutton = (GtkRadioToolButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "ghost_radiotoolbutton", 0);

  //toggle the alghoritms buttons as side effect
  gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (ghost_radiotoolbutton), TRUE);

  //set go and back buttons insensitive
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_go_toolbutton, FALSE);
  GTK_WIDGET_UNSET_FLAGS (teseo_alg_go_toolbutton, GTK_CAN_FOCUS);
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_back_toolbutton, FALSE);
  GTK_WIDGET_UNSET_FLAGS (teseo_alg_back_toolbutton, GTK_CAN_FOCUS);
  //set up and down buttons insensitive
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_up_toolbutton, FALSE);
  GTK_WIDGET_UNSET_FLAGS (teseo_alg_up_toolbutton, GTK_CAN_FOCUS);
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_down_toolbutton, FALSE);
  GTK_WIDGET_UNSET_FLAGS (teseo_alg_down_toolbutton, GTK_CAN_FOCUS);


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
    GtkMenuItem     *menu_open1 = (GtkMenuItem *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "open1", 0);
    GtkMenuItem     *menu_new1 = (GtkMenuItem *)    teseo_lookup_widget(GTK_WIDGET(win_teseo), "new1", 0);
    gchar *name_image=NULL;
    GString  *string_name_image=NULL;
    GtkLabel *label_image = (GtkLabel *)    teseo_lookup_widget(GTK_WIDGET(win_teseo), "label_image", 0);

    //Test if there are available sessions
    gchar * image_filename = gimp_image_get_filename(teseo_image);

    if (test_session(image_filename)){
      
      if (on_open1_activate (menu_open1, NULL) != 1)
      {
          //g_message("Open canceled");
          if (on_new1_activate (menu_new1, NULL)==0) {
              //g_message("New canceled");
              on_win_teseo_show(win_teseo,NULL);
          }
      }
    }
    else
    {
        if (on_new1_activate (menu_new1, NULL)==0) {
          on_win_teseo_show(win_teseo,NULL);
        }
   }

    string_name_image = g_string_new("Uname");
    name_image = g_path_get_basename (image_filename);

    if(name_image) {
        g_string_printf(string_name_image, "<small>%s</small>", name_image);
        g_free(name_image);
    }
    gtk_label_set_label(label_image, string_name_image->str);

    g_free(image_filename);
}

void
on_dlg_about_show_label_ver            (GtkWidget       *widget,
                                        gpointer         user_data)
{
    gchar teseo_caption_markup[200];
    g_sprintf(teseo_caption_markup, "<small>Open source software &#169; %s\n%s</small>", TESEO_YEAR, TESEO_CAPTION_DEV);
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
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), path_list[i]);
    }


}


void
on_print_for_debug1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    gint i;
    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;

    teseo_gimp_path_get_points (teseo_image, gimp_path_get_current(teseo_image), &path_closed, &num_path_point_details, &points_pairs);

    for(i=0; i<num_path_point_details; i++) {
	g_printf("\n%04d - %f", i, points_pairs[i]);
    }
    g_printf("\n\n");

    g_free(points_pairs);


    g_printf("gimp_directory() %s\n, gimp_data_directory() %s\n, gimp_locale_directory() %s\n, gimp_plug_in_directory() %s\n, gimp_sysconf_directory() %s\n", gimp_directory(), gimp_data_directory(), gimp_locale_directory(), gimp_plug_in_directory(), gimp_sysconf_directory());

}


void
on_snap1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // default is 1
    gint thickness_trace = 1;
    // default is 0 (Black)
    gint trace_colour=0;

    GtkSpinButton * teseo_thickness_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_thickness_trace", thickness_trace);
    GtkRadioButton * twcbr = (GtkRadioButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_wm_colour_black_radiobutton", trace_colour);

    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(twcbr))  == TRUE ){
        trace_colour=0;
    }
    else {
        trace_colour=255;
    }

    if(teseo_thickness_spinbutton) {
	thickness_trace = gtk_spin_button_get_value_as_int (teseo_thickness_spinbutton) ;
    }

    teseo_snap_path(teseo_image, trace_colour, thickness_trace);

}


void
on_salva1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  if (!save_session(current_session)) {
     g_message("Unable to save current Session.");
  } else {
      teseo_gtk_statusbar_push ("Session saved.");
  }
}


void
on_teseo_alg_down_toolbutton_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{
   int iter;

   gboolean check_guide;
   gboolean use_guide=FALSE;
   gboolean use_bezier=FALSE;

   GtkSpinButton  * tfss  = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_forward_step_spinbutton", 0);
   GtkCheckButton * tcfgs = (GtkCheckButton*) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_checkbtn_first_guide_stop", 0);
   GtkCheckButton * tcdbc = (GtkCheckButton*) teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_direct_bez_checkbutton", 0);

   use_bezier = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tcdbc));
   check_guide = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tcfgs));
   iter = gtk_spin_button_get_value_as_int (tfss) ;

   gint32 drawable_ID=  gimp_image_get_active_drawable  (teseo_image);

   //if checked and exist one guide
    gint32 *guides=NULL, n_guides;

    if (check_guide){
      n_guides = teseo_gimp_image_find_guides_orientation(teseo_image, GIMP_ORIENTATION_VERTICAL, &guides);
      if(n_guides >= 1) {
	iter=guides[0];
//	g_message("Stopping at guide in %d",iter);
	use_guide=TRUE;
      }
      else {
	  g_message("No guide found");
      }
      g_free(guides);
    }
    teseo_wmean_set_dir(DOWN);
    teseo_main_loop(iter, drawable_ID,use_guide, use_bezier);

}


void
on_teseo_alg_up_toolbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
   int iter;

   gboolean check_guide;
   gboolean use_guide=FALSE;
   gboolean use_bezier=FALSE;

   GtkSpinButton  * tfss  = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_forward_step_spinbutton", 0);
   GtkCheckButton * tcfgs = (GtkCheckButton*) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_checkbtn_first_guide_stop", 0);
   GtkCheckButton * tcdbc = (GtkCheckButton*) teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_direct_bez_checkbutton", 0);

   use_bezier = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tcdbc));
   check_guide = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tcfgs));
   iter = gtk_spin_button_get_value_as_int (tfss) ;

   gint32 drawable_ID=  gimp_image_get_active_drawable  (teseo_image);

   //if checked and exist one guide
    gint32 *guides=NULL, n_guides;

    if (check_guide){
      n_guides = teseo_gimp_image_find_guides_orientation(teseo_image, GIMP_ORIENTATION_VERTICAL, &guides);
      if(n_guides >= 1) {
	iter=guides[0];
//	g_message("Stopping at guide in %d",iter);
	use_guide=TRUE;
      }
      else {
	  g_message("No guide found");
      }
      g_free(guides);
    }
    teseo_wmean_set_dir(UP);
    teseo_main_loop(iter, drawable_ID,use_guide, use_bezier);

}


void
on_add_tms_from_guides1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_timemark_add_from_guides(teseo_image);
}


void
on_force_polyline1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_path_force_polyline(teseo_image);
}


void
on_teseo_alg_undo_toolbutton_clicked   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
    int iter;
    /*
    glong n_strokes;
    gdouble *strokes;
    */
    gint new_size;
    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;

    GtkSpinButton  * tbss  = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_back_step_spinbutton", 0);
    char * pathname = gimp_path_get_current(teseo_image);

    iter = gtk_spin_button_get_value_as_int (tbss) ;

    /*
    strokes = teseo_open_path_to_strokes(teseo_image,  &n_strokes,  pathname);
    teseo_strokes_to_open_path(teseo_image, n_strokes-iter, strokes, pathname);
    */

    teseo_gimp_path_get_points (teseo_image, pathname, &path_closed, &num_path_point_details, &points_pairs);

    new_size = num_path_point_details - (iter * 9);

    if(new_size > 0) {

        gimp_path_set_points(teseo_image, pathname, 1, new_size, points_pairs);
        //beware i'm deleting old path with pathname
        gimp_path_delete(teseo_image,pathname);

    } else {
        g_message("Too much points to delete !");
    }

    g_free(pathname);
    // g_free(strokes);
    g_free(points_pairs);
}


void
on_teseo_alg_left_toolbutton_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{
   int iter;

   gboolean check_guide;
   gboolean use_guide=FALSE;
   gboolean use_bezier=FALSE;

   GtkSpinButton  * tfss  = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_forward_step_spinbutton", 0);
   GtkCheckButton * tcfgs = (GtkCheckButton*) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_checkbtn_first_guide_stop", 0);
   GtkCheckButton * tcdbc = (GtkCheckButton*) teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_direct_bez_checkbutton", 0);

   use_bezier = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tcdbc));
   check_guide = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tcfgs));
   iter = gtk_spin_button_get_value_as_int (tfss) ;

   gint32 drawable_ID=  gimp_image_get_active_drawable  (teseo_image);

   //if checked and exist one guide
    gint32 *guides=NULL, n_guides;

    if (check_guide){
      n_guides = teseo_gimp_image_find_guides_orientation(teseo_image, GIMP_ORIENTATION_VERTICAL, &guides);
      if(n_guides >= 1) {
	iter=guides[0];
//	g_message("Stopping at guide in %d",iter);
	use_guide=TRUE;
      }
      else {
	  g_message("No guide found");
      }
      g_free(guides);
    }
    teseo_wmean_set_dir(LEFT);
    teseo_main_loop(iter, drawable_ID,use_guide,  use_bezier);
}



void
on_teseo_alg2_radiotoolbutton_clicked  (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	disable_buttons();
}


void
on_clean1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    gint32 clean_colour = 0;
    gint32 clean_threshold = 150;
    gint32 clean_fill_colour = 255;
    gboolean clean_greater = TRUE;
    gint32 clean_length = 35;
    gboolean clean_horizontal = TRUE;
    gboolean clean_newlayer = FALSE;
    gboolean clean_transparent = FALSE;
    gint32 teseo_active_layer, teseo_copy_layer;

    GtkRadioButton * teseo_clean_greather = (GtkRadioButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_clean_greater_radiobutton", 0);
    GtkSpinButton *teseo_clean_length = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_clean_length_spinbutton", clean_length);
    GtkRadioButton * teseo_clean_horizontal = (GtkRadioButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_clean_horizontal_radiobutton", 0);
    GtkRadioButton * teseo_clean_newlayer = (GtkRadioButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_clean_newlayer_checkbutton", 0);
    GtkCheckButton * teseo_clean_transparent = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_clean_transparent_checkbutton", 0);
    GtkScale* teseo_bc_hscale = (GtkScale *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_bc_hscale", 0);
    GtkScale* teseo_th_hscale = (GtkScale *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_th_hscale", 0);
    GtkScale* teseo_fc_hscale = (GtkScale *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "teseo_fc_hscale", 0);


	if(teseo_bc_hscale) {
		clean_colour = gtk_range_get_value( (GtkRange *) teseo_bc_hscale);
	}

	if(teseo_th_hscale) {
		clean_threshold = gtk_range_get_value( (GtkRange *) teseo_th_hscale);
	}

	if(teseo_fc_hscale) {
		clean_fill_colour = gtk_range_get_value( (GtkRange *) teseo_fc_hscale);
	}

    clean_greater = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(teseo_clean_greather));

    if(teseo_clean_length) {
        clean_length = gtk_spin_button_get_value (teseo_clean_length);
    }

    clean_horizontal = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(teseo_clean_horizontal));

    clean_newlayer = gtk_toggle_button_get_active((GtkToggleButton *) teseo_clean_newlayer);

    clean_transparent = gtk_toggle_button_get_active((GtkToggleButton *) teseo_clean_transparent);

    // Add a new layer if needed
    if(clean_newlayer) {
        // gint32      gimp_image_get_active_layer     (gint32 image_ID); != -1
        teseo_active_layer = gimp_image_get_active_layer     (teseo_image);
        if(teseo_active_layer != -1) {
            // gint32      gimp_layer_copy                 (gint32 layer_ID);
            teseo_copy_layer = gimp_layer_copy(teseo_active_layer);
            // gboolean    gimp_layer_add_alpha            (gint32 layer_ID);
            gimp_layer_add_alpha(teseo_copy_layer);
            // gboolean    gimp_image_add_layer            (gint32 image_ID, gint32 layer_ID, gint position); position=-1
            if(gimp_image_add_layer(teseo_image, teseo_copy_layer, -1)) {
                // gboolean    gimp_image_set_active_layer     (gint32 image_ID, gint32 active_layer_ID);
                gimp_image_set_active_layer(teseo_image, teseo_copy_layer);
            }
        } else {
            g_message("Teseo2: no layer is active !");
        }
    }

    teseo_filter_fill_continuous_segment(teseo_image, clean_colour, clean_threshold, clean_greater, clean_length, clean_fill_colour, clean_horizontal, clean_transparent);
    g_printf("%d, %d, %d, %d, %d, %d\n", clean_colour, clean_threshold, clean_greater, clean_length, clean_fill_colour, clean_horizontal);
}



void
on_split1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    gint32 *guides=NULL, n_guides;
    n_guides = teseo_gimp_image_find_guides_orientation(teseo_image, GIMP_ORIENTATION_VERTICAL, &guides);
    if(n_guides > 0) {
        teseo_path_split_at_xs(teseo_image, guides, n_guides);
    }
    g_free(guides);
}




void
on_post_analisys_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_curvature_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gtk_window_present(GTK_WINDOW(win_curvature));
}

void
on_teseo_calc_arm_shift_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	gboolean use_span=TRUE;
	gboolean use_coord=FALSE;
	gboolean rotate=TRUE;
	gboolean use_angle=FALSE;
	gdouble rotation_angle=0.;


	gchar *cur_path=NULL;

	gdouble sec=0., Bg=0., r=0., Rg=0., a=0., b=0.,Xin=0., Yin=0., Xfin=0., Yfin=0.;

	gdouble emin, emax ;
	gulong  imin,imax,i;

	gchar b_string[11]="";
		char msg[80];

	gint path_closed, num_path_point_details, num_points;
	gdouble* points_pairs=NULL;


        GtkSpinButton *teseo_spbtn_time_span = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_time_span", sec);
        GtkSpinButton *teseo_spbtn_vel = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_vel", Bg);
        GtkSpinButton *teseo_spbtn_cyl_radius = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_cyl_radius", r);
        GtkSpinButton *teseo_spbtn_arm_lenght = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_arm_lenght", Rg);
        GtkSpinButton *teseo_spbtn_displ = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_displ", a);
        GtkSpinButton *teseo_spbtn_b = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_b", b);
        GtkSpinButton *teseo_Xin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Xin", Xin);
        GtkSpinButton *teseo_Yin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Yin", Yin);
        GtkSpinButton *teseo_Xfin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Xfin", Xfin);
        GtkSpinButton *teseo_Yfin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Yfin", Yfin);
	GtkSpinButton *teseo_spbtn_angle = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_angle", rotation_angle);
	GtkCheckButton * teseo_chkbtn_extr = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_extr", use_coord);
	GtkCheckButton * teseo_chkbtn_rangle = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_rangle", use_angle);
	GtkCheckButton * teseo_chkbtn_rot = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_rot", rotate);
	GtkCheckButton * teseo_chkbtn_span = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_span",use_span );

	if(teseo_spbtn_time_span) {
		sec = gtk_spin_button_get_value (teseo_spbtn_time_span);
	}
	if(teseo_spbtn_vel) {
		Bg = gtk_spin_button_get_value (teseo_spbtn_vel);
	}
	if(teseo_spbtn_cyl_radius) {
		r = gtk_spin_button_get_value (teseo_spbtn_cyl_radius);
	}
	if(teseo_spbtn_arm_lenght) {
		Rg = gtk_spin_button_get_value (teseo_spbtn_arm_lenght);
	}
	if(teseo_spbtn_displ) {
		a = gtk_spin_button_get_value (teseo_spbtn_displ);
	}
	if(teseo_spbtn_b) {
		b = gtk_spin_button_get_value (teseo_spbtn_b);
		teseo_spbtn_arm_shift=teseo_spbtn_b;
	}
	if(teseo_Xin) {
		Xin = gtk_spin_button_get_value (teseo_Xin);
	}
	if(teseo_Yin) {
		Yin = gtk_spin_button_get_value (teseo_Yin);
	}
	if(teseo_Xfin) {
		Xfin = gtk_spin_button_get_value (teseo_Xfin);
	}
	if(teseo_Yfin) {
		Yfin = gtk_spin_button_get_value (teseo_Yfin);
	}
	if(teseo_spbtn_angle) {
		rotation_angle = gtk_spin_button_get_value (teseo_spbtn_angle);
		rotation_angle = atan(1.0)*rotation_angle/45.0;
	}

	if(teseo_chkbtn_extr) {
		use_coord = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_extr);
	}
	if(teseo_chkbtn_rangle) {
		use_angle = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_rangle);
	}
	if(teseo_chkbtn_rot) {
		rotate = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_rot);
	}
	if(teseo_chkbtn_span) {
		use_span = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_span);
	}


	b=DEFAULT_B;


	cur_path=gimp_path_get_current(teseo_image);

	if(teseo_path_semantic_type(teseo_image, cur_path) == PATH_SEMANTIC_POLYLINE) {
		teseo_wiech_estimate_b1( teseo_image, sec, Bg, r, Rg, a, b,
							rotate, TRUE,
							Xin, Yin, Xfin, Yfin,
							rotation_angle, use_angle,
							!use_coord, !use_span,
							ret_b, ret_errors, N_TRIES);

		gimp_path_get_points (teseo_image, cur_path,  &path_closed,  &num_path_point_details, &points_pairs);
		g_free(points_pairs);

		num_points=(num_path_point_details+3)/9;

		emin= emax=  ret_errors[0] ;
		imin=imax=0;

		for (i=1; i<N_TRIES;i++){
			if (emin>ret_errors[i]){
				emin = ret_errors[i];
				imin=i;
			}
		}

		//Scaling errors to 100%
		for (i=0; i<N_TRIES;i++){
			ret_errors[i]=ret_errors[i]*100./num_points;
		}
		g_sprintf(b_string,"%2.2f",ret_b[imin]);
		gtk_spin_button_set_value (teseo_spbtn_b,ret_b[imin]);

		sprintf(msg,"Minimum arm shift %2.2f",ret_b[imin] );


                // Display using gtkdatabox
                gint num_points = 5000;
                gfloat *X, *Y;  // don't use g_free if they have been passed to teseo_create_databox()
                GdkColor color;

                X = g_new0 (gfloat, num_points);
                Y = g_new0 (gfloat, num_points);

/*                for (i = 0; i < num_points; i++)
                {
                    X[i] = i;
                    Y[i] = 100. * sin (i * 2 * PI / num_points);
                }*/

                for (i = 0; i < N_TRIES; i++)
                {
                    X[i] = ret_b[i];
                    Y[i] = ret_errors[i];
                }

                color.red = 65535;
                color.green = 1035;
                color.blue = 1050;
		GtkWidget * databox_plot=
			teseo_create_databox (
			N_TRIES, X, Y, color, GTK_DATABOX_LINES, 1,
			"Teseo: arm shift", "Errors versus arm shift", "Arm shift [mm]", "Errors [counts]",
			TRUE );
		gtk_widget_show_all(databox_plot);
		gtk_main();
		//waiting events
		while (gtk_events_pending())   gtk_main_iteration();
		gtk_main_quit();
		g_free(databox_plot);

	}
	else{
		g_message("Current path is not a polyline, nothing to do");
	}
}



void
on_teseo_calc_arm_slope_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	gboolean use_span=TRUE;
	gboolean use_coord=FALSE;
	gboolean rotate=TRUE;
	gboolean use_angle=FALSE;
	gdouble rotation_angle=0.;

	gchar *cur_path=NULL;
	gdouble sec=0., Bg=0., r=0., Rg=0., a=0., b=0., Xin=0., Yin=0., Xfin=0., Yfin=0.;

        gdouble X_scale[180]={};
	gint i;
	char msg[80];


	//TODO use it ?
	//gdouble emin, emax ;
	//gulong  imin,imax,i;
	//gchar b_string[11]="";

        GtkSpinButton *teseo_spbtn_time_span = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_time_span", sec);
        GtkSpinButton *teseo_spbtn_vel = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_vel", Bg);
        GtkSpinButton *teseo_spbtn_cyl_radius = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_cyl_radius", r);
        GtkSpinButton *teseo_spbtn_arm_lenght = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_arm_lenght", Rg);
        GtkSpinButton *teseo_spbtn_displ = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_displ", a);
        GtkSpinButton *teseo_spbtn_b = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_b", b);
        GtkSpinButton *teseo_Xin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Xin", Xin);
        GtkSpinButton *teseo_Yin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Yin", Yin);
        GtkSpinButton *teseo_Xfin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Xfin", Xfin);
        GtkSpinButton *teseo_Yfin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Yfin", Yfin);
	GtkSpinButton *teseo_spbtn_angle = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_angle", rotation_angle);
	GtkCheckButton * teseo_chkbtn_extr = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_extr", use_coord);
	GtkCheckButton * teseo_chkbtn_rangle = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_rangle", use_angle);
	GtkCheckButton * teseo_chkbtn_rot = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_rot", rotate);
	GtkCheckButton * teseo_chkbtn_span = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_span",use_span );


	if(teseo_spbtn_time_span) {
		sec = gtk_spin_button_get_value (teseo_spbtn_time_span);
	}
	if(teseo_spbtn_vel) {
		Bg = gtk_spin_button_get_value (teseo_spbtn_vel);
	}
	if(teseo_spbtn_cyl_radius) {
		r = gtk_spin_button_get_value (teseo_spbtn_cyl_radius);
	}
	if(teseo_spbtn_arm_lenght) {
		Rg = gtk_spin_button_get_value (teseo_spbtn_arm_lenght);
	}
	if(teseo_spbtn_displ) {
		a = gtk_spin_button_get_value (teseo_spbtn_displ);
	}
	if(teseo_spbtn_b) {
		b = gtk_spin_button_get_value (teseo_spbtn_b);
                //TODO it is a BUG
		//teseo_spbtn_arm_shift=teseo_spbtn_b;
	}
	if(teseo_Xin) {
		Xin = gtk_spin_button_get_value (teseo_Xin);
	}
	if(teseo_Yin) {
		Yin = gtk_spin_button_get_value (teseo_Yin);
	}
	if(teseo_Xfin) {
		Xfin = gtk_spin_button_get_value (teseo_Xfin);
	}
	if(teseo_Yfin) {
		Yfin = gtk_spin_button_get_value (teseo_Yfin);
	}
	if(teseo_spbtn_angle) {
		rotation_angle = gtk_spin_button_get_value (teseo_spbtn_angle);
		rotation_angle = atan(1.0)*rotation_angle/45.0;
	}

	if(teseo_chkbtn_extr) {
		use_coord = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_extr);
	}
	if(teseo_chkbtn_rangle) {
		use_angle = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_rangle);
	}
	if(teseo_chkbtn_rot) {
		rotate = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_rot);
	}
	if(teseo_chkbtn_span) {
		use_span = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_span);
	}

	cur_path=gimp_path_get_current(teseo_image);

	if(teseo_path_semantic_type(teseo_image, cur_path) == PATH_SEMANTIC_POLYLINE) {
			teseo_wiech_slope_hist( teseo_image, sec, Bg, r, Rg, a, b,
							rotate, TRUE,
							Xin, Yin, Xfin, Yfin,
							rotation_angle, use_angle,
							!use_coord, !use_span,
							histogram);

			for (i=0; i<180;i++){
				X_scale[i]=i;
			}
			sprintf(msg,"Histogram of slope for shift=%2.2f",b );

                // Display using gtkdatabox
                gint num_points = 5000;
                gfloat *X, *Y;  // don't use g_free if they have been passed to teseo_create_databox()
                GdkColor color;

                X = g_new0 (gfloat, num_points);
                Y = g_new0 (gfloat, num_points);

                for (i = 0; i < 180; i++)
                {
                    X[i] = i;
                    Y[i] = histogram[i];
                }

                color.red = 65535;
                color.green = 1035;
                color.blue = 1050;

		GtkWidget * databox_plot=teseo_create_databox (180, X, Y, color, GTK_DATABOX_BARS, 1, "Teseo: histogram", msg, "Slope [deg]", "Counts",FALSE);
		gtk_widget_show_all(databox_plot);
		gtk_main();
		//waiting events
		while (gtk_events_pending())   gtk_main_iteration();
		gtk_main_quit();
		g_free(databox_plot);


        }
	else{
		g_message("Current path is not a polyline, nothing to do");
	}

}


void
on_btn_correct_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	gdouble sec=0., Bg=0., r=0., Rg=0., a=0., b=0., vshift=0., Xin=0., Yin=0., Xfin=0., Yfin=0.;
	gdouble * strokes_corr=NULL;
	gulong n_points=0.;
	gdouble * point_pairs=NULL;
	gulong num_details=0.;
	gdouble xfract=0.,yfract=0.;
	gboolean use_span=TRUE;
	gboolean use_coord=FALSE;
	gboolean rotate=TRUE;
	gboolean use_angle=FALSE;
	gboolean use_shift=TRUE;

	gdouble rotation_angle=0.;
	gchar current_path[FILENAMELEN]="";
	gchar corrected_path[FILENAMELEN]="";
	gchar *start_pathname=NULL;
	gulong errors=0.0;
	gchar options_str[80]="";
	gchar app[6]="";



        GtkSpinButton *teseo_spbtn_time_span = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_time_span", sec);
        GtkSpinButton *teseo_spbtn_vel = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_vel", Bg);
        GtkSpinButton *teseo_spbtn_cyl_radius = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_cyl_radius", r);
        GtkSpinButton *teseo_spbtn_arm_lenght = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_arm_lenght", Rg);
        GtkSpinButton *teseo_spbtn_displ = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_displ", a);
        GtkSpinButton *teseo_spbtn_b = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_b", b);
        GtkSpinButton *teseo_Xin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Xin", Xin);
        GtkSpinButton *teseo_Yin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Yin", Yin);
        GtkSpinButton *teseo_Xfin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Xfin", Xfin);
        GtkSpinButton *teseo_Yfin = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_Yfin", Yfin);
	GtkSpinButton *teseo_spbtn_angle = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_angle", rotation_angle);
	GtkSpinButton *teseo_spbtn_ls = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_ls", vshift);

	GtkCheckButton * teseo_chkbtn_extr = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_extr", use_coord);
	GtkCheckButton * teseo_chkbtn_rangle = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_rangle", use_angle);
	GtkCheckButton * teseo_chkbtn_rot = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_rot", rotate);
	GtkCheckButton * teseo_chkbtn_span = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_span",use_span );
	GtkCheckButton * teseo_chkbtn_transl = (GtkCheckButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_chkbtn_transl", use_shift );

	if(teseo_spbtn_time_span) {
		sec = gtk_spin_button_get_value (teseo_spbtn_time_span);
	}
	if(teseo_spbtn_vel) {
		Bg = gtk_spin_button_get_value (teseo_spbtn_vel);
	}
	if(teseo_spbtn_cyl_radius) {
		r = gtk_spin_button_get_value (teseo_spbtn_cyl_radius);
	}
	if(teseo_spbtn_arm_lenght) {
		Rg = gtk_spin_button_get_value (teseo_spbtn_arm_lenght);
	}
	if(teseo_spbtn_displ) {
		a = gtk_spin_button_get_value (teseo_spbtn_displ);
	}
	if(teseo_spbtn_b) {
		b = gtk_spin_button_get_value (teseo_spbtn_b);
	}
	if(teseo_spbtn_ls) {
		vshift = gtk_spin_button_get_value (teseo_spbtn_ls);
	}
	if(teseo_Xin) {
		Xin = gtk_spin_button_get_value (teseo_Xin);
	}
	if(teseo_Yin) {
		Yin = gtk_spin_button_get_value (teseo_Yin);
	}
	if(teseo_Xfin) {
		Xfin = gtk_spin_button_get_value (teseo_Xfin);
	}
	if(teseo_Yfin) {
		Yfin = gtk_spin_button_get_value (teseo_Yfin);
	}
	if(teseo_chkbtn_rangle) {
		use_angle = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_rangle);
	}
	if(teseo_spbtn_angle) {
		rotation_angle = gtk_spin_button_get_value (teseo_spbtn_angle);
		//rotation angle become radians
		rotation_angle = atan(1.0)*rotation_angle/45.0;
	}
	if(teseo_chkbtn_extr) {
		use_coord = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_extr);
	}
	if(teseo_chkbtn_rot) {
		rotate = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_rot);
	}
	if(teseo_chkbtn_span) {
		use_span = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_span);
	}

	if(teseo_chkbtn_transl) {
		use_shift = gtk_toggle_button_get_active((GtkToggleButton *) teseo_chkbtn_transl);
	}

	gimp_image_get_resolution (teseo_image,  &xfract,  &yfract);
	xfract= xfract/25.4;
	yfract= yfract/25.4;
	sprintf(current_path,"%s",gimp_path_get_current(teseo_image));

	if(teseo_path_semantic_type(teseo_image, current_path) == PATH_SEMANTIC_POLYLINE) {
		errors=teseo_wiech_corr( teseo_image, sec, Bg, r, Rg, a, b,
					rotate, TRUE,
					Xin, Yin, Xfin, Yfin,
					rotation_angle, vshift/3600, use_angle, !use_coord, !use_span, use_shift, &strokes_corr, &n_points);
		teseo_strokes_scale( strokes_corr, n_points, xfract*Bg/60.0, yfract, TRUE);
		teseo_strokes_point_pairs(strokes_corr, n_points, &point_pairs, &num_details);

                sprintf(options_str,"%s",(rotate)?"rotate_":"");
		if(rotate && use_angle) {
			sprintf(app,"%s%2.2f_",(use_angle)?"r_angle":"",gtk_spin_button_get_value (teseo_spbtn_angle));
			strcat(options_str,app);
		}
		if(use_coord) {
			strcat(options_str,"_Use_extrema");
		}
		if(use_span) {
			strcat(options_str,"_Use_timespan");
		}
		if(use_shift) {
			strcat(options_str,"_Shifting");
		}

		if(strlen(current_path) < (FILENAMELEN-40)) {
			gint len;
                        if( g_strstr_len (current_path, 20, "**" ) != NULL ) {
				len=strlen(g_strstr_len (current_path, 20, "**" ));
			} else{
				len=0;
			}

			len = strlen(current_path) - len;
			start_pathname = g_strndup ( current_path, len);

			if(start_pathname){
				sprintf(corrected_path, "%s**R%2.2f_a%2.2f_r%2.2f_b%+2.2f%s_Q%d", start_pathname, Rg,a,r,b,options_str, (gint) errors);
				g_free(start_pathname);
			}
			else{
				sprintf(corrected_path, "%s**R%2.2f_a%2.2f_r%2.2f_b%+2.2f%s_Q%d", current_path, Rg,a,r,b,options_str, (gint) errors);
			}
		}
		else{
			sprintf(corrected_path, "%s**R%2.2f_a%2.2f_r%2.2f_b%+2.2f%s_Q%d", "toolong",Rg,a,r,b,options_str, (gint) errors);
		}

		gimp_path_set_points(teseo_image, corrected_path, 1, num_details, point_pairs);
		//g_message("Points = %d details=%d",n_points,num_details);
	}
}










void
on_notebook2_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        guint            page_num,
                                        gpointer         user_data)
{
	GtkWidget * clean1;
	clean1 = (GtkWidget *) teseo_lookup_widget(GTK_WIDGET(win_teseo), "clean1", 0);
        gtk_widget_set_sensitive (clean1, TRUE);
}


void
on_parasites1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

#define MAX_EVENT 20

void
on_import2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
        char lps=0;
	gint i,j=0,count=0;
	gint selected_id;


	//GtkComboBox *  parasite_cbox = (GtkComboBox *) teseo_lookup_widget(GTK_WIDGET(dlg_parasites), "parasite_cbox", 0 );
	GtkHBox *  parasite_hbox = (GtkHBox *) teseo_lookup_widget(GTK_WIDGET(dlg_parasites), "parasite_hbox", 0 );
	GimpIntComboBox *  parasite_cbox;

        gchar * events[MAX_EVENT];

	events[0]=g_malloc(sizeof(gchar)*5);
	sprintf(events[0],"None");
	j=1;
	count=2;

	for(i=0;i<MAX_EVENT;i++){
		if(test_session_parasite(i)) {
			events[j]=g_malloc(sizeof(gchar)*3);
			sprintf(events[j],"%d",i);
			j++;
			count=j;
		}
	}

	parasite_cbox = (GimpIntComboBox *) gimp_int_combo_box_new_array ( count, events );//??
	gtk_box_pack_start (GTK_BOX (parasite_hbox),  GTK_WIDGET(parasite_cbox), TRUE, FALSE, 1);

	gtk_widget_show (GTK_WIDGET(parasite_cbox));
	gint result = gtk_dialog_run (GTK_DIALOG (dlg_parasites));

	switch (result)
	{
	case GTK_RESPONSE_OK:
		if (gimp_int_combo_box_get_active   ( (GimpIntComboBox*) parasite_cbox,&selected_id) ){
			if(selected_id!=0){
				lps=load_parasite_session(strtol(events[selected_id],NULL,0));
				if(lps){
					g_message("Session Parasites for event %ld imported from current image. \nRemember to save your session to apply changes",
					strtol(events[selected_id],NULL,0));
				}
				else {
					g_message("An error occurred while loading parasites for event %ld",strtol(events[selected_id],NULL,0));
				}
			}
			else {
				g_message("No parasites selected");
			}
		}
		break;
	case GTK_RESPONSE_DELETE_EVENT:
		break;
	default:
		break;
	}

	for(i=0;i<count;i++){
		g_free(events[i]);
	}

	gtk_widget_destroy (GTK_WIDGET(parasite_cbox));
//	g_object_unref (parasite_cbox);
	gtk_widget_hide (dlg_parasites);

}


void
on_export2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

	gint event=0;
	GtkSpinButton *teseo_event_number_spinbutton = (GtkSpinButton *)   teseo_lookup_widget(GTK_WIDGET(dlg_session), "teseo_event_number_spinbutton", event);
	if(teseo_event_number_spinbutton) {
		event = gtk_spin_button_get_value (teseo_event_number_spinbutton);
	}
	//saving in the image
	save_session_parasite(event);
	g_message("Session Parasites  for event %d exported in current image.\nRemember to save your image to apply changes",event);
}


void
on_remove_all1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gchar **parasites;
	gint num_parasites;
	gint i=0
	;
	gimp_image_parasite_list (teseo_image, &num_parasites, &parasites);
	for(i=0;i<num_parasites;i++){
		gimp_image_parasite_detach (teseo_image, parasites[i]);
	}
	g_message("Session Parasites removed from current image.\nRemember to save your image to apply changes");
}





gboolean
on_win_curvature_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_widget_hide(win_curvature);
  return TRUE;
}



void
on_calc_dist_btn_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

	gdouble r=0., Rg=0., a=0.;

        GtkSpinButton *teseo_spbtn_cyl_radius = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_cyl_radius", r);
        GtkSpinButton *teseo_spbtn_arm_lenght = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_arm_lenght", Rg);
        GtkSpinButton *teseo_spbtn_displ = (GtkSpinButton *) teseo_lookup_widget(GTK_WIDGET(win_curvature), "teseo_spbtn_displ", a);

	if(teseo_spbtn_cyl_radius) {
		r = gtk_spin_button_get_value (teseo_spbtn_cyl_radius);
	}
	if(teseo_spbtn_arm_lenght) {
		Rg = gtk_spin_button_get_value (teseo_spbtn_arm_lenght);
	}
	//Calculate displacement given R and r
	a = sqrt(Rg*Rg+r*r);

	if(teseo_spbtn_displ) {
		 gtk_spin_button_set_value (teseo_spbtn_displ,a);
	}

}


void
on_parasite_cbentry_changed            (GtkComboBox     *combobox,
                                        gpointer         user_data)
{

}



void                                        
on_flip1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
teseo_path_flip(teseo_image);
}


