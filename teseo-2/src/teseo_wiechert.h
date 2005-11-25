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

 /*!Take the current path, verify that is a polyline, create a new path correcting the curvature of the signal due to the geometry of the Wiechert seismograph recorder
	 \param g_image
	 \param sec: number of seconds between  xinitial and xfinal
	 \param Bg: The velocity of the paper in mm/min
	 \param r: Radius of the cylinder that carry the paper, in mm.
	 \param Rg: Length of the writing arm from its rotation axe until the end of the needle in mm.
	 \param a: Distance from the rotation axe of the writing arm until the axe of the cylinder that carry the paper measured perpendicular to the cylinder axe in mm :
	 \param b: Shift between the needle position at rest and its normal equilibrium position (arm perpendicular to the axe of the cylinder)
	           value positive if the arm is inclined downward  in mm
	Return true if successfully created a corrected path.
 */
#ifndef TESEO_WIECHERT
#define TESEO_WIECHERT
#include <glib.h>
#include <glib/gprintf.h>
#include <libgimp/gimp.h>
#include "teseo_gimp_extends.h"

#define  N_TRIES 600
#define DEFAULT_B 30


/*!Parametric Cadeck correction
*/
gulong teseo_wiech_corr(	gint32 g_image, gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
				gboolean rotate, gboolean translate, gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin, gdouble angle, gdouble vshift,
				gboolean use_angle, gboolean ignore_coord, gboolean ignore_sec, gboolean shift,
				gdouble **corr, gulong* n_points);
/*!Estimate of arm shift based on errors in corrected trace
*/
gboolean teseo_wiech_estimate_b1(	gint32 g_image,
					gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
					gboolean rotate, gboolean translate, gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin,  gdouble angle,
					gboolean use_angle, gboolean ignore_coord, gboolean ignore_sec,
					gdouble ret_b[], gdouble ret_errors[], gulong n_tries);
/*!Estimate of arm shift based on histogram of slope
*/
gboolean teseo_wiech_estimate_b2(	gint32 g_image,
					gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
					gboolean rotate, gboolean translate,
					gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin,
					gdouble angle, gboolean use_angle, gboolean ignore_coord, gboolean ignore_sec,
					gdouble hist_points[]);
/*!Rotate the strokes of angle radian (clockwise if angle positive) around the first point
*/
void teseo_rotate_clockwise(gdouble * strokes, gulong n_strokes, gdouble angle);

/*!translate the strokes at point x,y
*/
void teseo_translate(gdouble * strokes, gulong n_strokes, gdouble x, gdouble y);

/*!Copy in a new strokes
*/
gdouble * teseo_copy_strokes(gdouble * strokes, gulong n_strokes);

#endif
