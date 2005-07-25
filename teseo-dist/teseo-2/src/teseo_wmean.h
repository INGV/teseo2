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
#ifndef teseo_wmean
#define teseo_wmean 1
#include "teseo_prototype.h"
#include <glib.h>

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
int wmean( const wm_is * is, wm_os * os );


/*!
 * init_wmean initialise some const for algorithm execution
 */
void init_wmean( wmeanParams* s);

/*!
 * wmean_getinput return is extracted from previous os and drawable
 */
int wmean_getinput(
			wm_is * is,
			const wm_os * previous_os,
			gint32 drawable_ID
		  );

/*!
 * wmean_getouput
 */
int wmean_getouput(wm_os * os );

/*!
 * wmean_terminate
 */
int wmean_terminate(wm_os * os, wm_is * is, gint32 drawable_ID);

/*!
 * wmean_accumulate concatenate the os results in strokes
 */
int wmean_accumulate(gdouble ** strokes, gulong * num_strokes, wm_os * os);

/*!
 * wmean_starting_os create a starting os structure
 */
int wmean_starting_os(wm_os ** os, gint32 drawable_ID);

/*!
 * wmean_new_is create a new is structure with defaults
 */
int wmean_new_is( wm_is ** is);

/*!
 * wmean_release destructor
 */
int wmean_release(wm_is ** is, wm_os ** os);

#endif

