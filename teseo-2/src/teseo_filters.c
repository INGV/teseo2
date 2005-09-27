/* Teseo-2 Plug-in
 * Copyright (C) 2005  Stefano Pintore, Matteo Quintiliani (the "Authors").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, segment_lengthribute, sublicense,
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
#include "teseo_filters.h"


//to manage more layers, gimp add alpha channel in background layer
#define BUFOUTXY(X, Y, width, bpp) bufout[ ((Y*width) + X ) * bpp]

void teseo_filter_fill_continuous_segment(gint32 g_image, gint32 trace_colour, gint32 thresh_colour, gboolean fill_greater_segment_length, gint32 segment_length, gint32 fill_colour, gint32 angle) {
    GimpDrawable *drawable=NULL;
    GimpPixelRgn pr, pr_dest;
    int bpp = 0;
    int i, j, s_j, d_j, k;
    glong bufsize;
    guchar *bufin=NULL;
    guchar *bufout=NULL;
    gboolean fill_condition, segment_length_condition;

    gboolean non_empty;
    gint x1, y1, x2, y2, width, height;

    drawable=gimp_drawable_get(gimp_image_get_active_drawable(g_image));

    gimp_selection_bounds(g_image, &non_empty, &x1, &y1, &x2, &y2);

    if(!non_empty) {
        gimp_drawable_offsets(drawable->drawable_id, &x1, &y1);
        x2 = x1 + gimp_drawable_width(drawable->drawable_id) - 1;
        y2 = y1 + gimp_drawable_height(drawable->drawable_id) - 1;

    }

    width = x2 - x1;
    height = y2 - y1;

    bpp = drawable->bpp;
    bufsize=bpp * width * height;

    bufin = (guchar*) g_malloc( (sizeof(guchar)) * bufsize);
    bufout = (guchar*) g_malloc( (sizeof(guchar)) * bufsize);

    if ( (drawable != NULL) && (bufin != NULL) ) {

        //get bufin from the pixel region content
        gimp_pixel_rgn_init(&pr, drawable, x1, y1, width, height, FALSE, FALSE);
        gimp_pixel_rgn_init(&pr_dest, drawable, x1, y1, width, height, TRUE, TRUE);

        gimp_pixel_rgn_get_rect (&pr, bufin, x1, y1, width, height);

        for(i=0; i < bufsize; i++) {
            bufout[i] = bufin[i];
        }

        g_printf("teseo_filter_fill_continuous_segment() w %d, h %d, bpp %d\n", width, height, bpp);

        for(i=0; i < height; i++) {
            s_j = -1;
            d_j = 0;
            for(j=0; j < width; j++) {
                fill_condition = (BUFOUTXY(j, i, width, bpp) > trace_colour - thresh_colour  &&
                        BUFOUTXY(j, i, width, bpp) < trace_colour + thresh_colour);

                if(fill_condition
                        && j < width-1) {
                    // g_printf("condition s_j %d, d_j %d\n", s_j, d_j);
                    if(s_j == -1) {
                        s_j = j;
                        d_j = 0;
                    }
                    d_j++;
                } else {
                    if(s_j != -1) {
                        segment_length_condition = (d_j >= segment_length);

                        if(!fill_greater_segment_length) {
                            segment_length_condition = !segment_length_condition;
                        }

                        if(segment_length_condition) {
                            // change bufin from s_j for d_j elements
                            // g_printf("change at (%d,%d) from s_j %d, for d_j %d\n", j, i, s_j, d_j);
                            for(k=0; k < d_j; k++) {
                                BUFOUTXY(s_j + k, i, width, bpp) = fill_colour;
                            }
                        }
                        s_j = -1;
                        d_j = 0;
                    }
                }
            }
        }

        gimp_pixel_rgn_set_rect (&pr_dest, bufout, x1, y1, width, height);

        gimp_drawable_flush(drawable);
        gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
        gimp_drawable_update (drawable->drawable_id, x1, y1, width, height);

        gimp_displays_flush ();

    }

    g_free(bufin);
    g_free(bufout);
}

