/* Teseo-2 Plug-in
 * Copyright (C) 2000-2004 Stefano Pintore, Matteo Quintiliani (the "Authors").
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

#include "config.h"

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"
#include "interface.h"

#include "teseointerface.h"
#include "teseocallbacks.h"
#include "teseosupport.h"
#include "teseo_session.h"

#include "plugin-intl.h"


/*  Constants  */

//#define SCALE_WIDTH        180
#define SPIN_BUTTON_WIDTH   75
#define RANDOM_SEED_WIDTH  100



/*  Local function prototypes  */

static gboolean   dialog_image_constraint_func (gint32    image_id,
                                                gpointer  data);



/*  Local variables  */

static PlugInUIVals *ui_state = NULL;

/*  Public functions  */

gboolean
dialog (gint32              image_ID,
	GimpDrawable       *drawable,
	//PlugInVals         *vals,
	PlugInImageVals    *image_vals,
	PlugInDrawableVals *drawable_vals,
	PlugInUIVals       *ui_vals)
{

  gboolean   run = FALSE;

  char * name_noext=NULL;
  char * name=NULL;
  char * token;

  GtkFileFilter *filter =NULL;
  const char delimiters[] = ".";
  char pattern[FILENAMELEN]="";
  char noname_str[] = "/tmp/NONAME.xcf";



/*GimpUnit   unit;
  gdouble    xres, yres;*/

  ui_state = ui_vals;

  gimp_ui_init (PLUGIN_NAME, TRUE);
/*
  dlg = gimp_dialog_new (_("teseo-2 Plug-In"), PLUGIN_NAME,
                         NULL, 0,
			 gimp_standard_help_func, "teseo-2",

			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 GTK_STOCK_OK,     GTK_RESPONSE_OK,

			 NULL);*/

  teseo_image=image_ID;

  teseowin = create_win_neuronteseo();
  preferencesdlg = create_preferences_dlg ();
  aboutdlg = create_about_dlg ();
  teseofilechooser = create_filechooserimport();
  sessiondlg = create_session_dlg();

  teseosessionfilechooser = create_teseo_session_filechooser ();
  gtk_window_set_title (GTK_WINDOW (teseosessionfilechooser), "Open session file");

  //setting filter for session files
  //filter based on basename of the image file name
  if(gimp_image_get_filename(teseo_image)) {
  	name=g_path_get_basename (gimp_image_get_filename(teseo_image)); //to be freed
  } else {
  	name=(char *) malloc (sizeof(noname_str) + 1);
  	strcpy(name, noname_str);
  }
  token = strpbrk(name, delimiters);
  name_noext = g_strndup(name , strlen(name) - strlen(token) ); //without extensions
  strcpy(pattern,name_noext);
  strcat(pattern,"*");
  strcat(pattern,SESSION_EXT);
  filter = gtk_file_filter_new ();
  gtk_file_filter_add_pattern (filter, pattern);
  gtk_file_filter_set_name    (filter, "Session");
  gtk_file_chooser_add_filter (teseosessionfilechooser,filter);
  if (name != NULL) g_free(name);
  if (name_noext != NULL) g_free(name_noext);


  gtk_widget_show (teseowin);
  gtk_main ();


  /*TODO*/
  //gtk_widget_show (dlg);

  //run = (gimp_dialog_run (GIMP_DIALOG (dlg)) == GTK_RESPONSE_OK);
  /*EXP*/
//     if (run)
//     {
//       /*  Save ui values  */
// /*      ui_state->chain_active =
//         gimp_chain_button_get_active (GIMP_COORDINATES_CHAINBUTTON (coordinates));*/
//     }
  /*EXP*/
  //gtk_widget_destroy (dlg);

  return run;
}


/*  Private functions  */

static gboolean
dialog_image_constraint_func (gint32    image_id,
                              gpointer  data)
{
  return (gimp_image_base_type (image_id) == GIMP_RGB);
}




