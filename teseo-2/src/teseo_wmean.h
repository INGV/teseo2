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

#ifndef __TESEO_WMEAN_H__
#define __TESEO_WMEAN_H__ 1

#include <glib.h>

struct SPoint{
	gulong x;
	gulong y;
};

typedef struct SPoint point;


struct WM_I{
 guchar * bufin;
 point LastCentre;
};


typedef struct SPoint wm_os;
typedef struct WM_I   wm_is;


struct wmeanParam{
	int colour;
	int step;
	int width;
	int height;
};

typedef struct wmeanParam wmeanParams;

/*!
 * wmean: wmean algorithm executions
 */
int teseo_wmean( const wm_is * is, wm_os * os );


/*!
 * teseo_wmean_init initialise some const for algorithm execution
 */
void teseo_wmean_init( wmeanParams* s);

/*!
 * teseo_wmean_getinput return is extracted from previous os and drawable
 */
int teseo_wmean_getinput(
			wm_is * is,
			const wm_os * previous_os,
			gint32 drawable_ID
		  );
/*!
 * teseo_wmean_terminate test terminate condition
 */
int teseo_wmean_terminate(wm_os * os, wm_is * is, gint32 drawable_ID);

/*!
 * teseo_wmean_accumulate concatenate the os results in strokes
 */
int teseo_wmean_accumulate(gdouble ** strokes, gulong * num_strokes, wm_os * os);

/*!
 * teseo_wmean_starting_os create a starting os structure
 */
int teseo_wmean_starting_os(wm_os ** os, gint32 drawable_ID);


/*!
 * teseo_wmean_get_x get os currespondent abscissa 
 */
gulong teseo_wmean_get_x(wm_os * os);

/*!
 * teseo_wmean_new_is create a new is structure with defaults
 */
int teseo_wmean_new_is( wm_is ** is, gint32 drawable_ID);

/*!
 * teseo_wmean_release structures destructor
 */
void teseo_wmean_release(wm_is ** is, wm_os ** os);

#endif

