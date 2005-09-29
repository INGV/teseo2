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
 
#ifndef __TESEO_BEZIER_PFIT_H__
#define __TESEO_BEZIER_PFIT_H__ 1

#include <libgimp/gimp.h>
#include "teseo_types.h"

#define NDIM 4
//nr #define FTOL 0.1

/*!
  Compute teseo_p_distance between 2 points
  */
double teseo_p_distance(double x1, double y1, double x2, double y2);

/*!
  minimization function
  */
double teseo_p_func_sum_distance_bezier__(double *param_bez);

/*!
  Find maximum and minimum point of a path
  */
void teseo_p_maxima_minima_strokes(gdouble *strokes, glong num_strokes, gint **pidirezione, glong *pn_dir);

/*!
  Return a piecewise cubic bezier curves from a polyline
  */
glong teseo_p_fitting_bezier(glong num_strokes, gdouble* strokes, glong *pnum_path_inter, gdouble** ppath_inter);
    
#endif
