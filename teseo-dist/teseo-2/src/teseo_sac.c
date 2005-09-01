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
#include <libgimp/gimp.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "teseo_gimp_extends.h"
#include "teseo_sac.h"
#include "teseo_path.h"
#include "teseo_env.h"
#include "sac.h"


void teseo_sac_init_header(struct SAChead *header) {
    union SAChead_un {
        struct SAChead  fields;
        struct SAChead2 arrays;
    };

    union SAChead_un *SACheader = (union SAChead_un *) header;
    int i;

    // SAC header initialization by arrays
    for(i=0; i<NUM_FLOAT; i++) {
        SACheader->arrays.SACfloat[i] = SACUNDEF;
    }

    for(i=0; i<MAXINT; i++) {
        SACheader->arrays.SACint[i] = SACUNDEF;
    }

    for(i=0; i<5; i++) {
        SACheader->arrays.SACun[i] = SACUNDEF;
    }

    for(i=0; i<MAXSTRING; i++) {
        if(i!=2) {
            g_memmove(SACheader->arrays.SACstring[i], SACSTRUNDEF, K_LEN);
        } else {
            g_memmove(SACheader->arrays.SACstring[i], "        ", K_LEN);
        }
    }

}

gboolean teseo_sac_read(gchar *filename, struct SAChead *header, float *data) {
    gboolean ret = FALSE;
    FILE *f = fopen(filename, "rb");
    if(f) {
        fread(header, sizeof(struct SAChead), 1, f);
        data = g_malloc(header->npts * sizeof(float));
        if(data) {
            fread(data, sizeof(float), header->npts, f);
        }
        fclose(f);
        ret = TRUE;
    } else {
        g_message("teseo_sac_read(): unable to read sac file \"%s\"!", filename);
    }
    return ret;
}

gboolean teseo_sac_write(gchar *filename, struct SAChead *header, float *data) {
    gboolean ret = FALSE;
    FILE *f;
    g_printf("teseo_sac_write() called\n");
    if(data) {
        f = fopen(filename, "wb");
        if(f) {
            fwrite(header, sizeof(struct SAChead), 1, f);
            fwrite(data, sizeof(float), header->npts, f);
            fclose(f);
            ret = TRUE;
        } else {
            g_message("teseo_sac_write(): unable to write sac file \"%s\"!", filename);
        }
    } else {
        g_message("teseo_sac_write(): data parameter is NULL!");
    }
    return ret;
}

gboolean teseo_sac_path_export(gint32 g_image, char* filename, float paper_velocity) {
    gboolean ret = TRUE;
    struct SAChead header;
    float *data = NULL;
    float delta_pix = 0.0;
    gdouble xresolution,yresolution;
    gdouble factor_dep;
    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;
    gint i, j;

    g_printf("Init sac header\n");

    teseo_sac_init_header(&header);

    g_printf("End Init sac header\n");

    // SAC header fill by fields
 
    // header.delta X
    // header.depmin X
    // header.depmax X
    // header.scale
    // header.b X
    // header.e X

    // GDate parameter
    // header.nzyear
    // header.nzjday
    // header.nzhour
    // header.nzmin
    // header.nzsec
    // header.nzmsec

    header.nvhdr = SACVERSION;
    // header.npts X
    header.iftype = 1; // time series
    // header.idep     // don't use yet
    // header.iztype = 1; // begin time
    header.leven  = 1; // evenly spaced data
    header.lpspol = 1; // positive polarity
    header.lovrok = 1; // overwrite on disk
    header.lcalda = 1; // DIST, AZ, BAZ, GCARC

    // Station, component, network parameters
    // header.kstnm
    // header.kcmpnm
    // header.knetwk

    // SAC floats

    if(teseo_path_semantic_type_even(g_image, gimp_path_get_current(g_image), &delta_pix) == PATH_SEMANTIC_POLYLINE) {

        g_printf("delta_pix %f\n", delta_pix);

        // if evenly spaced
        if(delta_pix > 0.0) {
            gimp_image_get_resolution (g_image,  &xresolution,  &yresolution);
            header.delta = (25.4 / xresolution) * (1.0 / paper_velocity) * delta_pix;

            teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs);

            header.npts = (((num_path_point_details-6)/9) + 1);
            
            g_printf("npts %d\n", header.npts);
            
            data = g_malloc ( header.npts * sizeof(float));
            if(data) {
                header.b = 0.0;
                header.e = (header.npts - 1) * header.delta;
                // header.depmin = 0.0;
                // header.depmax = 0.0;
                // factor to convert pixel in cm
                factor_dep = 2.54 / yresolution;
                for(j=0, i=1; i < num_path_point_details; j++, i+=9) {
                    if(j >= header.npts) {
                        g_printf("teseo_sac_path_export(): error in function near line %d.", __LINE__);
                    }
                    data[j] = - ((points_pairs[i] - points_pairs[1]) * factor_dep);
                    if(data[j] > header.depmax) {
                        // header.depmax = data[j];
                    }
                    if(data[j] < header.depmin) {
                        // header.depmin = data[j];
                    }
                }
                ret = teseo_sac_write(filename, &header, data);

                g_free(data);
            } else {
                ret = FALSE;
            }
            g_free(points_pairs);
        } else {
            // TODO unevenly spaced data
            ret = FALSE;
        }
    } else {
        ret = FALSE;
    }
    return ret;
}

