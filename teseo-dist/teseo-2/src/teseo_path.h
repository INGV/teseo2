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
 
#ifndef TESEO_PATH
#define TESEO_PATH 1

#define GIMP_ENABLE_COMPAT_CRUFT 1
#include <libgimp/gimp.h>
#include <libgimp/gimpcompat.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "teseo_types.h"

//31/05 #include "teseo_bezier.h"

/*Utilità*/
void neuronSismos_ruotastrokes(glong num_strokes, gdouble* strokes, gdouble** pstrokes_ruotato, gdouble angolo);

//31/05 perchè usa una f in bezier
//void concatena_path_path_array(gint32 g_image, glong path_n, gdouble *path);

/*Funzioni di I/O*/
gdouble * path_array_to_strokes( gdouble * path_array, glong n_details, glong * n_strokes);
gdouble * open_path_to_strokes(gint32 g_image, glong* n_strokes,  char * nome_path);
gdouble * open_path_to_array(gint32 g_image, glong* n_strokes,  char * nome_path);

void strokes_to_open_path(gint32 g_image, glong num_strokes, gdouble *strokes, char * nome_path);

/*Funzioni di manipolazione*/
void unisci_path( gint32 g_image );
void allinea_path( gint32 g_image );
void muovi_tracciato(gint32 g_image,gint x, gint y , gdouble rotate);
void concatena_path_strokes(gint32 g_image, glong num_strokes, gdouble *strokes);

	
#endif
