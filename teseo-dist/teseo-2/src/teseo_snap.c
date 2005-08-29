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

#include "teseo_snap.h"
#include "teseo_path.h"
#include "teseo_env.h"
#include "teseo_gimp_extends.h"

inline void teseo_snap_masscenter(guchar *bufin, gint32 width, gint32 height, gint32 trace_colour, gint32 *new_x, gint32 *new_y) {
    gint32 bufsize = width * height;
    int num_x = 0;
    int num_y = 0;
    int den = 0;
    int p;
    int i = 0;

    for(i=0; i < (bufsize/2); i++) {
        p = (trace_colour==0)? 255 - bufin[i] : bufin[i];
        num_x += ( p * (i % width) );
        num_y += ( p * (i / width) );
        den += p;
    }
    *new_x = (int) ((gdouble) num_x / (gdouble) den) + 0.5;
    *new_y = (int) ((gdouble) num_y / (gdouble) den) + 0.5;
}

inline void teseo_snap_point(gint32 g_image, gint32 trace_colour, gint32 thickness_trace, gdouble x, gdouble y, gdouble *snap_x, gdouble *snap_y) {
    GimpDrawable *drawable=NULL;
    GimpPixelRgn pr;
    int bpp = 0;
    glong bufsize;
    guchar *bufin=NULL;
    int i;

    int width = thickness_trace; 
    int height = thickness_trace; 
    int x_rgn = (int) x - (width / 2);
    int y_rgn = (int) y - (width / 2);
    int new_x = (int) x;
    int new_y = (int) y;

    drawable=gimp_drawable_get(gimp_image_get_active_drawable(g_image));
    bpp = drawable->bpp;
    bufsize=bpp*width*height;

    bufin = (guchar*) g_malloc( (sizeof(guchar)) * bufsize);

    if ( (drawable != NULL) && (bufin != NULL) ) {

        //get bufin from the pixel region content
        gimp_pixel_rgn_init(&pr, drawable, x_rgn, y_rgn, width, height, FALSE, FALSE);
        gimp_pixel_rgn_get_rect (&pr, bufin, x_rgn, y_rgn, width, height);

        //to manage more layers, gimp add alpha channel in background layer
        if ( bpp==2 ) {
            //shifting out alpha bytes
            for (i=1; i<(bufsize/2); i++){
                bufin[i] = bufin[i*2];
            }
        }

        teseo_snap_masscenter(bufin, width, height, trace_colour, &new_x, &new_y);
        new_x += x_rgn;
        new_y += y_rgn;

    }
    g_free(bufin);
    *snap_x = new_x;
    *snap_y = new_y;
}

void teseo_snap_path(gint32 g_image, gint32 trace_colour, gint32 thickness_trace) {
    gint i;
    gdouble * points_pairs=NULL;
    gdouble * snap_points_pairs=NULL;
    gint path_closed, num_path_point_details;
    GString *snap_path_name=NULL;


    if(teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs)) {
            snap_points_pairs = g_malloc (num_path_point_details * sizeof(gdouble));

            gimp_progress_init("Teseo - Snap path . . .");
            gimp_progress_update(0.0);
            for(i=0; i<num_path_point_details; i+=9) {
                
                gimp_progress_update((gdouble) i / (gdouble) num_path_point_details);

                if(points_pairs[i+2]==1.0) {
                    teseo_snap_point(g_image, trace_colour, thickness_trace, points_pairs[i], points_pairs[i+1], &snap_points_pairs[i], &snap_points_pairs[i+1]);
                }

                // if I would consider path as polyline
                if(i > 0) {
                    snap_points_pairs[i-3] = snap_points_pairs[i];
                    snap_points_pairs[i-2] = snap_points_pairs[i+1];
                    snap_points_pairs[i-1] = 2.0;
                }
                snap_points_pairs[i+3] = snap_points_pairs[i];
                snap_points_pairs[i+4] = snap_points_pairs[i+1];
                snap_points_pairs[i+5] = 2.0;
            }
            gimp_progress_init("Teseo - Snap path finished.");
            gimp_progress_update(1.0);
            
            snap_path_name = g_string_new("Uname");
            g_string_printf(snap_path_name, "%s - Snap C%d T%02d", gimp_path_get_current(g_image), trace_colour, thickness_trace);
            gimp_path_set_points(g_image, snap_path_name->str, path_closed, num_path_point_details, snap_points_pairs);

    }

    g_string_free(snap_path_name, TRUE);
    g_free(points_pairs);
    g_free(snap_points_pairs);
}

