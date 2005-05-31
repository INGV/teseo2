#include <gtk/gtk.h>
#include <libgimp/gimp.h>
#include "teseo_io.h"
#include "teseo_path.h"


extern   GtkWidget *teseowin;
extern   GtkWidget *preferencesdlg;
extern   GtkWidget *teseofilechooser;
extern   GimpDrawable *private_drawable ; //porcata
extern   gint32  private_image ; //porcata

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences_2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_path2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dxf2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_trace2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sac2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sac_xy2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sisma2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_timemark2_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_path1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dxf1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_track1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sac1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sac_xy1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sisma1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_timemark1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_resample1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_align_all1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_link_all1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_move_and_rotation1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_fitting_polyline1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_fitting_bezier1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_recover_last2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_evaluate_middle_tms1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_adjustment1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_info_window_1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences_2_activate_item         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences_2_activate_item         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences_w_activate_item         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences_w_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_win_neuronteseo_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_win_preferences_destroy             (GtkObject       *object,
                                        gpointer         user_data);

gboolean
on_win_preferences_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_preferences_dlg_response            (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data);

void
on_preferences_dlg_close               (GtkDialog       *dialog,
                                        gpointer         user_data);
