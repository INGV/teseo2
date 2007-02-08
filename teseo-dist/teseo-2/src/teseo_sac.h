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
 
#ifndef __TESEO_SAC_H__
#define __TESEO_SAC_H__ 1

#define GIMP_ENABLE_COMPAT_CRUFT 1
#include <libgimp/gimp.h>
#include "sac.h"

/*
64 bits architecture patch 1: 
long->gint32
unsigned long -> guint32
*/
#define SAC_EXT ".sac"

    /*!
      TODO
      */
    void teseo_sac_init_header(struct SAChead *header);

    /*!
      TODO
      */
    void teseo_sac_station_header(struct SAChead *header, gchar *KSTNM, float CMPAZ, float CMPINC, float STLA, float STLO, float STEL);
    /*!
      TODO
      */
    void teseo_sac_gdate_header(struct SAChead *header,gint32 NZYEAR, gint32 NZJDAY, gint32 NZHOUR, gint32 NZMIN, gint32 NZSEC, gint32 NZMSEC);
    /*!
      TODO
      */
    gboolean teseo_sac_read(gchar *filename, struct SAChead *header, float *data);

    /*!
      TODO
      */
    gboolean teseo_sac_write(gchar *filename, struct SAChead *header, float *data);

    /*!
      TODO
      */
gboolean teseo_sac_path_export(gint32 g_image, char* filename, float paper_velocity, gchar *KSTNM, float CMPAZ, float CMPINC, float STLA, float STLO, float STEL, gint32 NZYEAR, gint32 NZJDAY, gint32 NZHOUR, gint32 NZMIN, gint32 NZSEC, gint32 NZMSEC);

#endif
