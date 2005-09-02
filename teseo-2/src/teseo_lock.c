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
#include <glib.h>
#include <glib/gprintf.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "teseo_lock.h"
#include "teseo_env.h"
#include "gtkaddons.h"

#define NOLOCKFILE "NOLOCKFILE"
gchar teseo_filename_lock[FILENAMELEN] = NOLOCKFILE;
FILE *FILE_lock = NULL;

gboolean teseo_lock(gchar *pattern_prefix) {
    gboolean ret = FALSE;
    gboolean is_unlocked  = TRUE;

    gchar * tgep=NULL;

    tgep=teseo_get_environment_path(LOCKPATH);
    strcpy(teseo_filename_lock, tgep);
    g_free(tgep);

    strcat(teseo_filename_lock, G_DIR_SEPARATOR_S);
    strcat(teseo_filename_lock, pattern_prefix);
    strcat(teseo_filename_lock, LOCK_EXT);

    if( g_file_test(teseo_filename_lock, G_FILE_TEST_EXISTS) ) {
	// TODO Check stat
	is_unlocked = FALSE;
    }

    if(is_unlocked) {
	// Create file lock
	FILE_lock = fopen(teseo_filename_lock, "w");
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
    // g_message("teseo_unlock() %s", teseo_filename_lock);

    if( strcmp(teseo_filename_lock, NOLOCKFILE) == 0 ) {
	g_message("Teseo-CRITICAL: you should not call teseo_unlock() ! teseo_filename_lock has not been initialized.");
    } else {
	if(FILE_lock) {
	    fclose(FILE_lock);
	    FILE_lock = NULL;
	    remove(teseo_filename_lock);
	} else {
	    g_message("Teseo-CRITICAL: you should not call teseo_unlock() ! FILE_lock has not been initialized.");
	}
    }
}


void teseo_force_unlock() {
    int fd = open(teseo_filename_lock, O_CREAT);
    struct flock lockp;
    /*
    lockp.l_type
    lockp.l_whence
    lockp.l_start
    lockp.l_len
    lockp.l_pid
    */
    fcntl (fd, F_GETLK, &lockp);
    switch(errno) {
        case EBADF:
            g_printf("fcntl() EBADF: The fd argument is invalid.\n");
            break;
        case EINVAL:
            g_printf("fcntl() EINVAL: Either the lockp argument doesn't specify valid lock information, or the file associated with filedes doesn't support locks.\n");
            break;
        default:
            break;
    } 

    lockp.l_type = F_WRLCK;

    fcntl (fd, F_SETLK, &lockp);
    // flock(fd,LOCK_EX); 
    // flock(fd,LOCK_UN);
    close(fd);
}


