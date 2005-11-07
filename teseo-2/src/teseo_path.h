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
 
#ifndef __TESEO_PATH_H__
#define __TESEO_PATH_H__ 1

#define GIMP_ENABLE_COMPAT_CRUFT 1
#include <libgimp/gimp.h>
#include <libgimp/gimpcompat.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "teseo_types.h"

#define PATHNAMELEN 160

//31/05 #include "teseo_bezier.h"

/* *********************************
   Utilità
   ********************************* */

/*!
  TODO
  */
void teseo_path_rotation(glong num_strokes, gdouble* strokes, gdouble** pstrokes_ruotato, gdouble angolo);

//31/05 perchè usa una f in bezier
//void concatena_path_path_array(gint32 g_image, glong path_n, gdouble *path);

/*!
  Semantic type for a path. When the two control points have the same coordinate the related anchor point,
  the path is PATH_SEMANTIC_POLYLINE, else is a PATH_SEMANTIC_BEZIER.
  */
typedef enum { PATH_SEMANTIC_POLYLINE, PATH_SEMANTIC_BEZIER } path_semantic_type;


/*!
  teseo_path_semantic_type returns if a path is a POLYLINE or a piecewise BEZIER curve.
  \param gint32 g_image ID of the GIMP image
  \param gchar *path_name The name of the path
  */
path_semantic_type teseo_path_semantic_type(gint32 g_image, gchar *path_name);

/*!
  teseo_path_semantic_type_even returns if a path is a POLYLINE or a piecewise BEZIER curve.
  \param gint32 g_image ID of the GIMP image
  \param gchar *path_name The name of the path
  \param float *delta_pix if path is a polyline then if path is unevenly spaced, delta is 0.0, else delta is costant distance [pix] between adjacent ancor points
  */
path_semantic_type teseo_path_semantic_type_even(gint32 g_image, gchar *path_name, float *delta_pix);


/* *********************************
   Funzioni di I/O
   ********************************* */

/*!
  TODO
  */
gdouble * teseo_path_array_to_strokes( gdouble * path_array, glong n_details, glong * n_strokes);

/*!
  TODO
  */
gdouble * teseo_open_path_to_strokes(gint32 g_image, glong* n_strokes,  char * nome_path);

/*!
  TODO
  */
gdouble * teseo_open_path_to_array(gint32 g_image, glong* n_strokes,  char * nome_path);

/*!
  TODO
  */
void teseo_strokes_to_open_path(gint32 g_image, glong num_strokes, gdouble *strokes, char * nome_path);

/* *********************************
   Funzioni di manipolazione
   ********************************* */

/*!
  TODO
  */
void teseo_link_all_path( gint32 g_image );


/*!
  TODO
  */
void teseo_link_all_path_unlocked( gint32 g_image, gboolean delete_path);

/*!
  TODO
  */
void teseo_align_all_path( gint32 g_image );

/*!
  TODO
  */
void teseo_align_all_path_unlocked(gint32 g_image, gboolean delete_path);

/*!
  TODO
  */
void teseo_path_move(gint32 g_image,gint x, gint y , gdouble rotate);

/*!
  TODO
  */
void teseo_cat_path_strokes(gint32 g_image, glong num_strokes, gdouble *strokes);

/*!
  scale the strokes for the factors scale
  */
void teseo_strokes_scale(gdouble * strokes_corr, gulong n_points, gdouble x_scale, gdouble y_scale, gboolean mantain_offset);

/*!
  Create a polyline point_pairs from a strokes vector
  */
gboolean teseo_strokes_point_pairs(gdouble *strokes, gulong n_strokes, gdouble** pp, gulong* nppd);

/*!
  teseo_path_force_polyline convert a path in a polyline without take care of control points.
  \param g_image GIMP image ID
  */
void teseo_path_force_polyline(gint32 g_image);

/*!
  teseo_path_add_points_pairs add at the current path new_points_pairs
  \param g_image GIMP image ID
  \param new_num_path_point_details number of points in new_points_pairs
  \param new_points_pairs points vector
  */
void teseo_path_add_points_pairs(gint32 g_image, gint new_num_path_point_details, gdouble *new_points_pairs);

/*!
  Split the current path at x positions of the vertical guides
  */
void teseo_path_split_at_xs(gint32 g_image, gint32 *guides, gint32 n_guides);

#endif
