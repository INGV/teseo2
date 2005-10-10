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


void teseo_filter_fill_continuous_segment(gint32 g_image, gint32 trace_colour, gint32 thresh_colour, gboolean fill_greater_segment_length, gint32 segment_length, gint32 fill_colour, gboolean horizontal) {
    gint i, channels;
    gint x1, y1, x2, y2;
    GimpPixelRgn rgn_in, rgn_out;
    guchar *line = NULL;
    guchar *outline = NULL;
    gint regionwidth, regionheight;
    gint width, height;
    gint length, iterations;
    GimpDrawable *drawable;
    gint has_alpha, alpha;

    drawable=gimp_drawable_get(gimp_image_get_active_drawable(g_image));

    /* Gets upper left and lower right coordinates,
     * and layers number in the image */
    gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
    regionwidth = x2 - x1;
    regionheight = y2 - y1;
    width = drawable->width;
    height = drawable->height;

    gimp_progress_init ("Clean seismogram ...");

    channels = gimp_drawable_bpp(drawable->drawable_id);

    has_alpha = gimp_drawable_has_alpha (drawable->drawable_id);
    alpha = (has_alpha) ? drawable->bpp - 1 : drawable->bpp;

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

    if(horizontal) {
        length = regionwidth;
        iterations = regionheight;
    } else {
        length = regionheight;
        iterations = regionwidth;
    }

    /* Allocate memory for input and output tile lines */
    // init_mem(&line, &outline, length * channels);
    line = (guchar*) g_malloc( (sizeof(guchar)) * length * channels);
    outline = (guchar*) g_malloc( (sizeof(guchar)) * length * channels);

    g_printf("iterations %d, length %d, channels %d, trace_colour %d, thresh_colour %d, fill_greater_segment_length %d, segment_length %d, fill_colour %d\n", iterations, length, channels, trace_colour, thresh_colour, fill_greater_segment_length, segment_length, fill_colour);

    for (i = 0; i < iterations; i++)
    {
        if(horizontal) {
            gimp_pixel_rgn_get_row (&rgn_in, line, x1, y1 + i, length);
        } else {
            gimp_pixel_rgn_get_col (&rgn_in, line, x1 + i, y1, length);
        }

        /* To be done for each tile line */
        process_line(line, outline, length, channels, trace_colour, thresh_colour, fill_greater_segment_length, segment_length, fill_colour);

        if(horizontal) {
            gimp_pixel_rgn_set_row(&rgn_out, outline, x1, y1 + i, length);
        } else {
            gimp_pixel_rgn_set_col (&rgn_out, outline, x1 + i, y1, length);
        }

        /* shift tile lines to insert the new one at the end */
        // shuffle(&rgn_in, line, x1, y1, length, iterations, i );

        if (i % (iterations / 20) == 0)
            gimp_progress_update ((gdouble) i / 
                    (gdouble) iterations);
    }

    gimp_progress_update(1.0);
    gimp_progress_init ("Clean seismogram finished.");

    g_free (line);
    g_free (outline);

    /*  Update the modified region */
    gimp_drawable_flush (drawable);
    gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
    gimp_drawable_update (drawable->drawable_id, x1, y1, regionwidth, regionheight);

    gimp_displays_flush ();
}


void process_line(guchar *line, guchar *outline, gint length, gint channels, gint32 trace_colour, gint32 thresh_colour, gboolean fill_greater_segment_length, gint32 segment_length, gint32 fill_colour) {
    gboolean fill_condition, segment_length_condition;
    int j, s_j, d_j, k;
    s_j = -1;
    d_j = 0;

    // copy line to outline
    for(j=0; j< (length*channels); j++) {
        outline[j] = line[j];
    }
    
    // execute filling
    for(j=0; j < length; j++) {
        fill_condition = (outline[j * channels] > trace_colour - thresh_colour
                && outline[j * channels] < trace_colour + thresh_colour);

        if(fill_condition
                && j < length-1) {
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
                        outline[(s_j + k) * channels] = fill_colour;
                    }
                }
                s_j = -1;
                d_j = 0;
            }
        }
    }

    if(channels > 1) {
        for(j=0; j<length; j++) {
            outline[ (j*channels) + 1] = 255;
        }
    }
}

