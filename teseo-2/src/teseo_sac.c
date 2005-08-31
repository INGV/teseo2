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

#include <unistd.h>
// windows compile patch #include <sys/types.h>
// windows compile patch #include <sys/wait.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "teseo_sac.h"
#include "teseo_path.h"
#include "teseo_env.h"
#include "sac.h"


void teseo_sac_path_export(gint32 g_image, char* filename, float delta){
    union SAChead_un {
        struct SAChead  fields;
        struct SAChead2 arrays;
    };

    union SAChead_un SACheader;
    int i;

    // SAC header initialization by arrays
    for(i=0; i<NUM_FLOAT; i++) {
        SACheader.arrays.SACfloat[i] = SACUNDEF;
    }

    for(i=0; i<MAXINT; i++) {
        SACheader.arrays.SACint[i] = SACUNDEF;
    }

    for(i=0; i<5; i++) {
        SACheader.arrays.SACun[i] = SACUNDEF;
    }

    for(i=0; i<MAXSTRING; i++) {
        g_memmove(SACSTRUNDEF, SACheader.arrays.SACstring[i], K_LEN);
    }

    // SAC header fill by fields
 
    // SACheader.fields.delta
    // SACheader.fields.depmin
    // SACheader.fields.depmax
    // SACheader.fields.scale
    // SACheader.fields.b
    // SACheader.fields.nzyear
    // SACheader.fields.nzjday
    // SACheader.fields.nzhour
    // SACheader.fields.nzmin
    // SACheader.fields.nzsec
    // SACheader.fields.nzmsec
    // SACheader.fields.npts
    // SACheader.fields.leven

    // SAC floats

}

