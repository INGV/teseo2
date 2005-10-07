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


void teseo_filter_fill_continuous_segment(gint32 g_image, gint32 trace_colour, gint32 thresh_colour, gboolean fill_greater_segment_length, gint32 segment_length, gint32 fill_colour, gint32 angle) {
    gint i, channels;
    gint x1, y1, x2, y2;
    GimpPixelRgn rgn_in, rgn_out;
    guchar *row = NULL;
    guchar *outrow = NULL;
    gint width, height;
    GimpDrawable *drawable;

    drawable=gimp_drawable_get(gimp_image_get_active_drawable(g_image));

    /* Gets upper left and lower right coordinates,
     * and layers number in the image */
    gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
    width = x2 - x1;
    height = y2 - y1;

    gimp_progress_init ("Clean seismogram ...");

    channels = gimp_drawable_bpp(drawable->drawable_id);

    /* Allocate a big enough tile cache */
    gimp_tile_cache_ntiles (2 * (drawable->width / 
                gimp_tile_width () + 1));

    /* Initialises two PixelRgns, one to read original data,
     * and the other to write output data. That second one will
     * be merged at the end by the call to
     * gimp_drawable_merge_shadow() */
    gimp_pixel_rgn_init (&rgn_in, drawable, x1, y1, width, height, 
            FALSE, FALSE);
    gimp_pixel_rgn_init (&rgn_out, drawable, x1, y1, width, height, 
            TRUE, TRUE);

    /* Allocate memory for input and output tile rows */
    // init_mem(&row, &outrow, width * channels);
    row = (guchar*) g_malloc( (sizeof(guchar)) * width * channels);
    outrow = (guchar*) g_malloc( (sizeof(guchar)) * width * channels);

    for (i = 0; i < height; i++)
    {
        gimp_pixel_rgn_get_row (&rgn_in, row, x1, y1 + i, width);
        /* To be done for each tile row */
        process_row(row, outrow, width, channels, trace_colour, thresh_colour, fill_greater_segment_length, segment_length, fill_colour);
        gimp_pixel_rgn_set_row(&rgn_out, outrow, x1, y1 + i, width);
        /* shift tile rows to insert the new one at the end */
        // shuffle(&rgn_in, row, x1, y1, width, height, i );
        if (i % (height / 20) == 0)
            gimp_progress_update ((gdouble) i / 
                    (gdouble) height);
    }

    gimp_progress_update(1.0);
    gimp_progress_init ("Clean seismogram finished.");

    g_free (row);
    g_free (outrow);

    /*  Update the modified region */
    gimp_drawable_flush (drawable);
    gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
    gimp_drawable_update (drawable->drawable_id, x1, y1, width, height);

    gimp_displays_flush ();
}


#define ROWOUTXY(X, width, channels) outrow[X  * channels]
void process_row(guchar *row, guchar *outrow, gint width, gint channels, gint32 trace_colour, gint32 thresh_colour, gboolean fill_greater_segment_length, gint32 segment_length, gint32 fill_colour) {
    gboolean fill_condition, segment_length_condition;
    int j, s_j, d_j, k;
    s_j = -1;
    d_j = 0;

    for(j=0; j<width*channels; j++) {
        outrow[j] = row[j];
    }
    
    for(j=0; j < width; j++) {
        fill_condition = (ROWOUTXY(j, width, channels) > trace_colour - thresh_colour  &&
                ROWOUTXY(j, width, channels) < trace_colour + thresh_colour);

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
                        ROWOUTXY(s_j + k, width, channels) = fill_colour;
                    }
                }
                s_j = -1;
                d_j = 0;
            }
        }
    }
}

