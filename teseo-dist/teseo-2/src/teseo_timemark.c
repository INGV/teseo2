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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "teseo_timemark.h"
#include "teseo_gimp_extends.h"

GList* teseo_timemark_check_path(gint32 teseo_image) {
    GList *ret = NULL;
    gint i, j;
    gint x_missing = 0;
    gint n_missings_cur = 0;
    gdouble delta_mean = 0, delta_cur = 0, delta_middle = 0;
    gdouble x_cur, x_prev;
    const gdouble delta_factor = 1.5;
    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;

    teseo_gimp_path_get_points (teseo_image, gimp_path_get_current(teseo_image), &path_closed, &num_path_point_details, &points_pairs);

    // Set first delta_mean
    if(num_path_point_details >= 18 ) {
        delta_mean = points_pairs[9] - points_pairs[0];
        if(delta_mean < 0.0) {
            delta_mean = 0.0;
        }
    } else {
        delta_mean = 0.0;
    }

    // Update delta_min and check missing points
    i=18;
    while (i < num_path_point_details  &&  delta_mean != 0.0) {

        // Update delta_mean
        x_cur  = points_pairs[i];
        x_prev =  points_pairs[i-9];
        delta_cur = x_cur - x_prev;

        if(delta_cur > delta_mean * delta_factor) {
            
            n_missings_cur = ((gint) delta_cur / (gint) delta_mean) - 1;
            delta_middle = delta_cur / (gdouble) (n_missings_cur + 1);
            
            for(j=1; j<=n_missings_cur; j++) {
                x_missing = x_prev + delta_middle;
                // g_printf("xmissing %d\n", x_missing);
                ret = g_list_append(ret, GINT_TO_POINTER(x_missing));  
                delta_mean = (delta_mean + delta_middle) / 2.0;
                x_prev=x_missing;
            }

        } else {
            delta_mean = (delta_mean + delta_cur) / 2.0;
        }

	// Next 3 points
	i+=9;
    }


    if(delta_mean == 0.0) {
        g_message("teseo_timemark_check_path(): delta_mean is zero !");
        g_list_free(ret);
        ret = NULL;
    }

    g_free(points_pairs);

    return ret;
}


