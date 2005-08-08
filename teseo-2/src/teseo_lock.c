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
#include <string.h>

#include "teseo_lock.h"
#include "teseo_env.h"
#include "gtkaddons.h"

#define NOFILELOCK "NOFILELOCK"
gchar filename_lock[FILENAMELEN] = NOFILELOCK;
FILE *FILE_lock = NULL;

gboolean teseo_lock(gchar *pattern_prefix) {
    gboolean ret = FALSE; 
    gboolean is_unlocked  = TRUE; 
    
    strcpy(filename_lock, teseo_get_environment_path(LOCKPATH));
    strcat(filename_lock, SLASH);
    strcat(filename_lock, pattern_prefix);
    strcat(filename_lock, LOCK_EXT);

    // Debug only
    // g_message("teseo_lock() %s", filename_lock);

    if( g_file_test(filename_lock, G_FILE_TEST_EXISTS) ) {
	// TODO Check stat
	is_unlocked = FALSE;
    }

    if(is_unlocked) {
	// Create file lock
	FILE_lock = fopen(filename_lock, "w");
	if(FILE_lock) {
	    ret = TRUE;
	}
    } else {
	// g_message("File is locked and can't be removed !");
    }
    return ret;
}


void teseo_unlock() {
    // Debug only
    // g_message("teseo_unlock() %s", filename_lock);

    if( strcmp(filename_lock, NOFILELOCK) == 0) {
	g_message("Teseo-CRITICAL: you should not call teseo_unlock() ! filename_lock has not been initialized.");
    } else {
	if(FILE_lock) {
	    fclose(FILE_lock);
	    FILE_lock = NULL;
	    remove(filename_lock);
	} else {
	    g_message("Teseo-CRITICAL: you should not call teseo_unlock() ! FILE_lock has not been initialized.");
	}
    }
}




