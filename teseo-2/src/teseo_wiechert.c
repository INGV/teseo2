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

#include <glib.h>
#include <glib/gprintf.h>

gboolean teseo_wiech_corr(gint32 g_image, gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b){
	gboolean ret=FALSE;
	return ret;
}

gboolean teseo_wiech_estimate_b1(gint32 g_image, gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b, gdouble** ret_b, gdouble ** errors, gdouble *n){
	gboolean ret=FALSE;
	gdouble *strokes_in;

	//take the current path

	//check if path is polyline
	if(TRUE)
	{
		//allocate space for strokes 2*number_of_points

		//translate path in strokes

		//call fortran function
		//assign return values
	}
	else
	{
	}
return ret;
}

gboolean teseo_wiech_estimate_b2(gint32 g_image, gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b){
	gboolean ret=FALSE;
	return ret;

}
