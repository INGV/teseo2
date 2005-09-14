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
 
#ifndef __TESEO_GIMP_EXTENDS_H__
#define __TESEO_GIMP_EXTENDS_H__ 1

#define GIMP_ENABLE_COMPAT_CRUFT 1
#include <libgimp/gimp.h>
#include <libgimp/gimpcompat.h>
#include <gtk/gtk.h>

/*!
  TODO Wrapper to find next guide with orientation in parameter
  */
gint32 teseo_gimp_image_find_next_guide_orientation(gint32 g_image, gint32 g_guide, gint32 g_orientation);

/*! 
  Compute a sorted coordinates list of guides with orientation in parameter.
  Return number and values.
  \param g_image
  \param g_orientation
  \param guides is a vector containin coordinate and it need g_free.
*/
gint32 teseo_gimp_image_find_guides_orientation(gint32 g_image, gint32 g_orientation, gint32 **a_guides);

/*!
  teseo_gimp_image_delete_all_guides delete all guides with a specific orientation
  \param g_image
  \param g_orientation
  */
void teseo_gimp_image_delete_all_guides(gint32 g_image, gint32 g_orientation);

/*!
 TODO Wrapper to intercept error/warning and display with g_message
 */
gint teseo_gimp_path_get_points (gint32           image_ID,
			const gchar     *name,
			gint            *path_closed,
			gint            *num_path_point_details,
			gdouble        **points_pairs);

#endif
