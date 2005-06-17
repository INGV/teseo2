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
 
#ifndef TESEO_IO
#define TESEO_IO 1

#define GIMP_ENABLE_COMPAT_CRUFT 1
#include <libgimp/gimp.h>
#include <libgimp/gimpcompat.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>


#define FILENAMELEN 200

/*Funzioni di utilità sui file*/
	gint test_dir( char *nome_dir);
	gint filexist( char *nome_file);

/*Funzioni di input */
	void import_dxf ( gint32 g_image, char * NomeFileDxf  );
//31/05	void Carica_Traccia ( gint32 g_image, char * NomeFileTraccia  );
	void import_bzr ( gint32 g_image, char * NomeFileBzr  );
 	void import_timemark ( gint32 g_image, char * NomeFileTimeMarker );
/*Funzioni di output */
	gint open_path_to_save(gint32 g_image,  char * nome_path, char * filename);
	gint save_path(gpointer user_data );

	void save_path_dxf(gint32 g_image, char* filename, gint scalatura);
	void save_path_timemarker(gint32 g_image, char* filename, gint scalatura);

	void save_path_sisma(gint32 g_image, char* filename);
//31/05	void save_path_traccia(gint32 g_image, char* filename);
// 31/05 sintax C void save_path_sac(gint32 g_image, char* filename, char *dir_teseo_bin, gchar xy=0 );
	void save_path_sac(gint32 g_image, char* filename, char *dir_teseo_bin, gchar xy );
/*Funzioni di output che agiscono direttamente sugli strokes*/
// 31/05 sintax C void strokes_dxf(gint32 g_image, const char * file_dxf, gdouble* strokes , glong num_stroke, gint tracciato=1, gint scalatura=0); //Default tracciato non scalato
// 31/05 sintax C void strokes_sisma(gint32 g_image, const char * file_sisma, gdouble* strokes , glong num_stroke, gchar xy=0);
	void strokes_dxf(gint32 g_image, const char * file_dxf, gdouble* strokes , glong num_stroke, gint tracciato, gint scalatura); //Default tracciato non scalato
	void strokes_sisma(gint32 g_image, const char * file_sisma, gdouble* strokes , glong num_stroke, gchar xy);


/*NEW entries*/
	void strokes_sac(gint32 g_image, char * file_sac, gdouble* strokes , glong num_stroke);
	void save_sac_cm( char * file_sac, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height);
	void save_sisma_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar xy);
	void save_marcatempi_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start);
	void save_dxf_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start);
	void save_dxf(FILE * fp, gint num_punti, gdouble *vet_punti);



    gboolean export_svg_vectors ( gint32 g_image, char * SVGfile  );
    gboolean import_svg_vectors ( gint32 g_image, char * SVGFile  );


#endif
