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

#include <libgimp/gimp.h>
#include "teseo_main.h"


static int  ( * ALGORITHM   )  ( const void *  is,  void * os );
static void ( * INIT_ALG    )  ( void * constants );
static int  ( * GETINPUT    )  ( void * is,  const void * previous_os, gint32 drawable_ID );
static int  ( * GETOUTPUT   )  ( void * os );
static int  ( * TERMINATE   )  ( void * os, void * is, gint32 drawable_ID );
static int  ( * ACCUMULATE  )  ( double ** strokes, long * num_strokes, void * os );
static int  ( * STARTING_OS )  ( void * os, gint32 drawable_ID );
static int  ( * RELEASE     )  ( void * is, void *os);
static int  ( * NEW_IS      )  ( void * is);

void teseo_main_init(
		     int  (* alg)         ( const void * is, void * os ),
		     void (* init_alg)    ( void * constants ),
		     void  * sth,
		     int  (* getinput)    ( void * is,  const void * previous_os, gint32 drawable_ID ),
		     int  (* getouput)    ( void * os ),
		     int  (* terminate)   ( void * os , void * is, gint32 drawable_ID ),
		     int  (* accumulate)  ( double ** strokes, long * num_strokes, void * os ),
		     int  (* starting_os) ( void ** os, gint32 drawable_ID ),
		     int  (* new_is)      ( void ** is ),
		     int  (* release)     ( void * is, void * os )
		     )
{
  ALGORITHM = alg;
  INIT_ALG  = init_alg;
  INIT_ALG(sth);

  GETINPUT    = getinput;
  GETOUTPUT   = getouput;
  TERMINATE   = terminate;
  ACCUMULATE  = accumulate;
  STARTING_OS = starting_os;
  NEW_IS      = new_is;
}

void teseo_main_loop(int iter, gint32 drawable_ID ){
  //after a teseo_main_init() call you can use your ALGORITHM here
  int i;
  void * os=NULL;
  void * is=NULL;
  gdouble * strokes=NULL;
  gulong num_strokes=0;
  wm_os * myos;

  //initialize os as an ipothetique last running
  STARTING_OS(&os,drawable_ID);
  //if ( os != NULL ) g_message("OK");
  //else g_message("TOO BAD");
  //myos = (wm_os*) os;
  //g_message("teseo_main_loop %d %d",(*myos).x, (*myos).y);

  NEW_IS(&is);
  //g_message("2");

  for (i=0; i<iter; i++){
   //get is from drawable and os
   //g_message("3");
   if ( ! GETINPUT(is, os, drawable_ID) ) break;
   //get os running the algorithm
   //g_message("4");
   if ( ! ALGORITHM( is, os ) ) break;
   //g_message("5");
   //accumulate results point on strokes extracted from os
   ACCUMULATE(&strokes, &num_strokes, os);
   //testing an escape condition on output os
   //g_message("6");
   //if ( TERMINATE(os, is, drawable_ID ) ) break;
   //g_message("7");
  }

  //TODO generalise?
  cat_path_strokes( gimp_drawable_get_image(drawable_ID), num_strokes, strokes);
  //RELEASE(&is,&os);
  //strokes_to_open_path(gimp_drawable_get_image(drawable_ID), num_strokes, strokes, "test");

}

