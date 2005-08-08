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

#ifndef __TESEO_ENV_H__
#define __TESEO_ENV_H__ 1

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "teseo_io.h"

#ifndef PROCEDURE_NAME
#define PROCEDURE_NAME   "teseo-2"
#endif

#define T_MAJOR_VER 2
#define T_MINOR_VER 0
#define T_DEVEL_VER 3
#define TESEO_CAPTION_DEV teseo_caption(TRUE)
#define TESEO_CAPTION     teseo_caption(FALSE)
#define TESEO_YEAR "2005"


/* ***********
   OLDIES
 ************* */

/*!
  Deprecated !
  */
extern const char TESEO_BIN_deprecated[];

/*!
  Deprecated !
  */
extern const char TESEO_DATA_deprecated[];

/*!
  Deprecated !
  */
char *valore_parse_deprecated(const char *nomefile, const char *nomevariabile);

/*!
  Deprecated !
  */
char *getenv_teseo_deprecated(const char *name_var) ;

/* ***********
   END OLDIES
 ************* */


/*!
  teseo_caption returns a string with name and current version
  */
const gchar * teseo_caption(gboolean ver_devel);


/*!
Environment default subpaths
*/
/* When you modify PATHNAMES, modify SUBPATHS in the teseo_env.c */
typedef enum { SVGPATH, SESSIONPATH, BEZIERPATH, DXFPATH, SACPATH, ASCIIPATH, TMARKPATH, PREFPATH, LOCKPATH, TMPPATH } PATHNAMES;
extern const char *SUBPATHS[];

extern const gchar SLASH[];


/*!
teseo_get_teseo_environment_path returns a static string containing the teseo environment path, $HOME/.gimp-majorversio.minorversion/PROCEDURE_NAME
*/
char * teseo_get_teseo_environment_path( );

/*!
teseo_get_environment_path returns a newly allocated string containing the absolute path of the corresponding path resource.
Free it after use!
	\param char * pathname, in PATHNAMES
*/
char * teseo_get_environment_path( int pathname );


/*!
teseo_create_environment_path returns 1 if teseo environment path creation succeed
	\param char * filename
*/
char teseo_create_environment_path( char* filename );


/*!
create_environment returns 1 if environment paths creation succeed,
	\param char * name, the directory name to create under teseo_get_teseo_environment_path resulting path
*/
char teseo_create_environment();


#endif
