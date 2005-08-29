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
 
#ifndef __TESEO_SNAP_H__
#define __TESEO_SNAP_H__ 1

#define GIMP_ENABLE_COMPAT_CRUFT 1
#include <libgimp/gimp.h>
#include <libgimp/gimpcompat.h>
#include "teseo_types.h"


/*!
  TODO
  */
void teseo_snap_masscenter(guchar *bufin, gint32 width, gint32 height, gint32 trace_colour, gint32 *snap_x, gint32 *snap_y);

/*!
  TODO
  */
void teseo_snap_point(gint32 g_image, gint32 trace_colour, gint32 thickness_trace, gdouble x, gdouble y, gdouble *snap_x, gdouble *snap_y);

/*!
  TODO
  */
void teseo_snap_path(gint32 g_image, gint32 trace_colour, gint32 thickness_trace);

#endif
