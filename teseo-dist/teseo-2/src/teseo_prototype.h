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
#ifndef prototype
#define prototype 1
#include <glib.h>


struct SI{
 char * input_code;
};

struct SO{
 char * output_code;
};

void BB_prototype( const  struct SI si, int (* F) (const struct SI, struct SO) , struct SO so);


struct SPoint{
	int x;
	int y;
};

typedef struct SPoint point;


/*!
 *BB_buffer_point is the BlackBox prototype taking a buffer in input, and giving a  point in output
 */
int BB_buffer_point(
                     const char * bufin,
		     int (* F)(
		                  const char * in,
		                  point * out
			      ),
		     point * out  );


/*Functions prototypes for WM-like algorithms*/

struct WM_I{
 guchar * bufin;
 point LastCentre;
};

typedef struct SPoint wm_os;
typedef struct WM_I   wm_is;

int WM_prototype( const void * is,
                  int (* F) (
		  	 	const void * is,
		  		void * os
		  ),
		  void * os );


#endif
