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

#include <gtk/gtk.h>
#include <libgimp/gimp.h>
#include "teseo_io.h"
#include "teseo_path.h"
#include "teseo_env.h"

extern   GtkWidget *win_teseo;
extern   GtkWidget *dlg_preferences;
extern   GtkWidget *dlg_about;
extern   GtkWidget * dlg_session;
extern   GtkWidget * dlg_move_rotation;
extern   GtkFileChooser * filechooser_import;
extern   GtkFileChooser * filechooser_export;
extern   GtkFileChooser * filechooser_session;

extern   GimpDrawable *private_drawable ;
extern   gint32  teseo_image ;

int
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

int
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
on_win_teseo_delete_event        (GtkWidget       *widget,
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
on_dlg_preferences_response            (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data);

void
on_dlg_preferences_close               (GtkDialog       *dialog,
                                        gpointer         user_data);

void
on_svg1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ascii1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_svg2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ascii2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_bezier1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_teseo_alg_go_toolbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_properties1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_properties1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_alg_wmean_radiotoolbutton_clicked   (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_teseo_wm_height_spinbutton_changed  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_teseo_wm_colour_black_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_teseo_wm_width_spinbutton_changed   (GtkEditable     *editable,
                                        gpointer         user_data);
gint
on_teseo_wm_height_spinbutton_input    (GtkSpinButton   *spinbutton,
                                        gdouble *new_value,
                                        gpointer         user_data);

void
on_teseo_wm_height_spinbutton_changed  (GtkEditable     *editable,
                                        gpointer         user_data);

gint
on_teseo_wm_width_spinbutton_input     (GtkSpinButton   *spinbutton,
                                        gdouble *new_value,
                                        gpointer         user_data);

void
on_dlg_session_show                    (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_win_teseo_show                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_dlg_about_show_label_ver            (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_dlg_session_show_teseo_eventpathname
                                        (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_align_unlocked_paths1_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_link_unlocked_paths1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_split_unlocked_paths1_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_print_for_debug1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_snap1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_salva1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_teseo_alg_down_toolbutton_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_teseo_alg_up_toolbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_add_tms_from_guides1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_force_polyline1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_teseo_alg_undo_toolbutton_clicked   (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_teseo_alg_left_toolbutton_clicked   (GtkButton       *button,
                                        gpointer         user_data);


void
on_teseo_alg2_radiotoolbutton_clicked  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_clean1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

