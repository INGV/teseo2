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

#include "teseo_lock.h"
#include "teseo_env.h"
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
  gchar * name_noext=NULL;
  gchar * name_ext=NULL;
  gchar * name_ext_xcf=NULL;
  gchar * name=NULL;
  gchar * token;
  GtkFileFilter *filter =NULL;
  const gchar delimiters[] = ".";
  gchar pattern_prefix[FILENAMELEN]="";
  gchar pattern_session[FILENAMELEN]="";
  gboolean is_xcf = FALSE;

  /*
  GimpUnit   unit;
  gdouble    xres, yres;
  */

  ui_state = ui_vals;

  gimp_ui_init (PLUGIN_NAME, TRUE);

  /*
  dlg = gimp_dialog_new (_("teseo-2 Plug-In"), PLUGIN_NAME,
                         NULL, 0,
			 gimp_standard_help_func, "teseo-2",

			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 GTK_STOCK_OK,     GTK_RESPONSE_OK,

			 NULL);
  */

  teseo_image=image_ID;

  // Setting filter for session files
  // Filter based on basename of the image file name
  if(gimp_image_get_filename(teseo_image)) {

	// name need free
  	name=g_path_get_basename (gimp_image_get_filename(teseo_image)); 
	token = strpbrk(name, delimiters);

	// Without extensions, name_noext need free
	name_noext = g_strndup(name , strlen(name) - strlen(token) );

	// Pointer to extensions, name_ext not need free 
	name_ext = g_strrstr(name, ".xcf");
	if(name_ext) {
		name_ext_xcf = g_strdup(name_ext);
	} else {
		name_ext_xcf = g_strdup("NOXCF");
	}

	// Set "pattern_prefix" variable
	strcpy(pattern_prefix, name_noext);

	// Set "pattern_session" variable
	strcpy(pattern_session, pattern_prefix);
	strcat(pattern_session, "???");
	strcat(pattern_session, SESSION_EXT);

	// Check file extension
	if( strcmp(name_ext_xcf, ".xcf") == 0 ||
	    strcmp(name_ext_xcf, ".xcf.bz2") == 0 ||
	    strcmp(name_ext_xcf, ".xcf.gz") == 0 ||
	    strcmp(name_ext_xcf, ".xcfbz2") == 0 ||
	    strcmp(name_ext_xcf, ".xcfgz") == 0
			) {
		is_xcf = TRUE;
	}

	if (name != NULL)
	    g_free(name);
	if (name_noext != NULL)
	    g_free(name_noext);
	if (name_ext_xcf != NULL)
	    g_free(name_ext_xcf);

  } else {
	// is_xcf = FALSE;
  }

  if(is_xcf) {
      if(teseo_lock(pattern_prefix)) {
	      teseowin = create_win_teseo();
	      gtk_window_set_title (GTK_WINDOW (teseowin), TESEO_CAPTION);
	      preferencesdlg = create_dlg_preferences ();
	      aboutdlg = create_dlg_about ();
	      sessiondlg = create_dlg_session();

	      teseofilechooser = (GtkFileChooser *) create_filechooser_import();
	      teseosessionfilechooser = (GtkFileChooser *) create_filechooser_session ();
	      gtk_window_set_title (GTK_WINDOW (teseosessionfilechooser), "Open session file");

	      filter = gtk_file_filter_new ();
	      gtk_file_filter_add_pattern (filter, pattern_session);
	      gtk_file_filter_set_name    (filter, "Session");
	      gtk_file_chooser_add_filter (teseosessionfilechooser, filter);

	      gtk_widget_show (teseowin);
	      gtk_main ();
      } else {
	      g_message("Teseo-2 seems to be just running on file %s.\nRemember: it is possible to load only one instance for each file !", pattern_session);
	      gtk_main_quit();
      }
  } else {
	  g_message("Please, save file in .xcf (.xcf.bz2, xcf.gz) format before to run Teseo-2 !");
  	  gtk_main_quit();
  }

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




