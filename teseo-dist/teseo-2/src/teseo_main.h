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

#ifndef __TESEO_MAIN_H__
#define __TESEO_MAIN_H__ 1

//#include "teseo_prototype.h"
#include "teseo_wmean.h"

/*!
 * teseo_main_init to initialise the functions to call in teseo_main_loop
 */
void teseo_main_init(
		     int  (* alg)         ( const void * is, void * os ),
		     void (* init_alg)    ( void * constants ),
		     void  * sth,
		     int  (* getinput)    ( void * is,  const void * previous_os, gint32 drawable_ID ),
		     int  (* terminate)   ( void * os , void * is, gint32 drawable_ID ),
		     int  (* accumulate)  ( double ** strokes, long * num_strokes, void * os ),
		     int  (* starting_os) ( void ** os, gint32 drawable_ID ),
		     int  (* new_is)      ( void ** is, gint32 drawable_ID ),
		     int  (* get_x)       ( void * os ),
		     void (* release)    ( void ** is, void ** os )		     );


/*!
 * teseo_main_loop is the main loop of our algorithm
 */
//void teseo_main_loop(int iter, gint32 drawable_ID);
void teseo_main_loop( gulong limit, gint32 drawable_ID, char is_guide , gboolean is_bezier);
/*!
 * teseo_main_loop is the guide position oriented main loop variant of our algorithm
 */
void teseo_main_loop_guide(gulong x_guide, gint32 drawable_ID );

#endif

