#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>


#include "teseocallbacks.h"
#include "teseointerface.h"
#include "teseosupport.h"

#include "teseo_resample.h"



GtkWidget * preferencesdlg;
GtkWidget * teseowin;
GtkWidget * teseofilechooser;

GimpDrawable       *private_drawable ; //porcata
gint32  teseo_image ; //porcata

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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

}


void
on_preferences_2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_path2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  char filename[FILENAMELEN];

  //TODO resource file

  gtk_window_set_title (GTK_WINDOW (teseofilechooser), "Open Bezier Path");
  gint result = gtk_dialog_run (GTK_DIALOG (teseofilechooser));

  switch (result)
    {
      case GTK_RESPONSE_OK:
         strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (teseofilechooser)) );
         //g_message("OK pressed: open and display bezier  %s on image %d", filename, teseo_image);
	 Carica_Bzr( teseo_image, filename );
         break;
      case GTK_RESPONSE_CANCEL:
         g_message("Cancel pressed: don't do anything");
         break;
      case GTK_RESPONSE_DELETE_EVENT:
         g_message("Delete event, same as Cancel pressed");
         break;
      default:
         break;
    }
  gtk_widget_hide (teseofilechooser);

}


void
on_dxf2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

  char filename[FILENAMELEN];

  //TODO resource file
  gtk_window_set_title (GTK_WINDOW (teseofilechooser), "Open DXF file");
  gint result = gtk_dialog_run (GTK_DIALOG (teseofilechooser));


  switch (result)
    {
      case GTK_RESPONSE_OK:
         strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (teseofilechooser)) );
         //g_message("OK pressed: open and display DXF path  %s on image %d", filename, teseo_image);
	 Carica_Dxf( teseo_image, filename );
         break;
      case GTK_RESPONSE_CANCEL:
         g_message("Cancel pressed: don't do anything");
         break;
      case GTK_RESPONSE_DELETE_EVENT:
         g_message("Delete event, same as Cancel pressed");
         break;
      default:
         break;
    }
  gtk_widget_hide (teseofilechooser);
}


void
on_trace2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_sac2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_sac_xy2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_sisma2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_timemark2_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_path1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_dxf1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_track1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_sac1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_sac_xy1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_sisma1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_timemark1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_resample1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	// call_on_btnBezier_clicked(private_image, pivals);
	call_on_btnBezier_clicked(teseo_image, 1, 1);
}


void
on_align_all1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_link_all1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_move_and_rotation1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_fitting_polyline1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_fitting_bezier1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_recover_last2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_evaluate_middle_tms1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_adjustment1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_info_window_1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}




void
on_preferences_w_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

 gint result = gtk_dialog_run (GTK_DIALOG (preferencesdlg));
  switch (result)
    {
      case GTK_RESPONSE_OK:
         g_message("OK pressed: save new preferences");
         break;
      case GTK_RESPONSE_CANCEL:
         g_message("Cancel pressed: don't save anything, restore the old preferences in the preferences dialog");
         break;
      case GTK_RESPONSE_DELETE_EVENT:
         g_message("Delete event, same as Cancel pressed");
         break;
      default:
         break;
    }
  gtk_widget_hide (preferencesdlg);
  //gtk_widget_show(preferencesdlg);

}


gboolean
on_win_neuronteseo_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_main_quit();
  return TRUE;
}


gboolean
on_win_preferences_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  //gtk_widget_hide(preferencesdlg);
  //return TRUE;
}


void
on_preferences_dlg_response            (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{
  gtk_widget_hide(preferencesdlg);
  //in response_id c'è ok o cancel
  return response_id;
}


void
on_preferences_dlg_close               (GtkDialog       *dialog,
                                        gpointer         user_data)
{
  gtk_widget_hide(preferencesdlg);
  return FALSE;
}

