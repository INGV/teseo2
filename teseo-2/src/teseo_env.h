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

#ifndef TESEO_ENVH
#define TESEO_ENVH 1

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "teseo_io.h"

#ifndef PROCEDURE_NAME
#define PROCEDURE_NAME   "teseo-2"
#endif




/*OLDIES*/
extern const char TESEO_BIN[];
extern const char TESEO_DATA[];

char *valore_parse(const char *nomefile, const char *nomevariabile);
char *getenv_teseo(const char *name_var) ;
/*END OLDIES*/



/*!
Environment default subpaths
*/
/* When you modify PATHNAMES, modify SUBPATHS in the teseo_env.c */
typedef enum { SVGPATH, SESSIONPATH, BEZIERPATH, DXFPATH, SACPATH, ASCIIPATH, TMARKPATH, PREFPATH, LOCKPATH } PATHNAMES;
extern const char *SUBPATHS[];

extern const gchar SLASH[];


/*!
get_teseo_environment_path returns a static string containing the teseo environment path, $HOME/.gimp-majorversio.minorversion/PROCEDURE_NAME
*/
char * get_teseo_environment_path( );

/*!
get_environment_path returns a newly allocated string containing the absolute path of the corresponding path resource.
Free it after use!
	\param char * pathname, in PATHNAMES
*/
char * get_environment_path( int pathname );


/*!
create_teseo_environment_path returns 1 if teseo environment path creation succeed
	\param char * filename
*/
char create_teseo_environment_path( char* filename );


/*!
create_environment returns 1 if environment paths creation succeed,
	\param char * name, the directory name to create under get_teseo_environment_path resulting path
*/
char create_environment();


#endif
