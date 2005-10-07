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
 
#ifndef __TESEO_FILTERS_H__
#define __TESEO_FILTERS_H__ 1

#include <libgimp/gimp.h>

/*!
  teseo_filter_fill_continuous_segment() fills continuous line or non continuous lines that contain pixel with colour trace_colour and threshol trace_colour
  \param g_image GIMP image ID
  \param trace_colour base colour of the trace
  \param thresh_colour threshold colour of the trace
  \param fill_greater_segment_length if TRUE, function fills segment greater than length, else it fills segment less than length
  \param segment_length
  \param fill_colour value of the colour used to fill segment
  \param angle at the moment it's possible use 0, horizontal line, or not equal to 0, vertical line
  */
void teseo_filter_fill_continuous_segment(gint32 g_image, gint32 trace_colour, gint32 thresh_colour, gboolean fill_greater_segment_length, gint32 segment_length, gint32 fill_colour, gint32 angle);

/*!
  TODO
  */
void process_row(guchar *row, guchar *outrow, gint width, gint channels, gint32 trace_colour, gint32 thresh_colour, gboolean fill_greater_segment_length, gint32 segment_length, gint32 fill_colour);

#endif
