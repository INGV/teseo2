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

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "teseo_session.h"

static char SESSION_PATH[FILENAMELEN];
static char ENVIRONMENT_PATH[FILENAMELEN];

char * get_teseo_environment_path(){

    char ret=0;
    char * home;
    char path_session[FILENAMELEN]="";
    char version[3];

    home=getenv("HOME");
    strcat(path_session,home);
    strcat(path_session,"/.gimp-");
    sprintf(version, "%d.%d",gimp_major_version,gimp_minor_version);
    strcat(path_session,version);

    strcat(path_session,"/teseo-2");
    strcpy(ENVIRONMENT_PATH,path_session);

    return ENVIRONMENT_PATH;

}

char create_teseo_environment_path(char * filename){
    char ret=0;
    char * home;

    char preferences[FILENAMELEN]="";

    if ( test_dir( get_teseo_environment_path() ) == 0 ) {
      if ( mkdir(get_teseo_environment_path(),S_IRWXU |  S_IRWXG | S_IRWXO ) == -1 ) {
          g_message("Unable to create teseo environment path");
      }
      else
          ret=1;
    }
    else {
      //g_message("teseo environment path already exist");
      ret=1;
    }

    return ret;
}


char save_session(char * filename, struct Session * s){
    char ret=0;
    char * home;

    char preferences[FILENAMELEN]="";
    sprintf(preferences,"%s/%s",get_teseo_environment_path(),filename);

    return ret;
}



char load_session(char * filename, struct Session * s){
    char ret=0;
    return ret;
}


