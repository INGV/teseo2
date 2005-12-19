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

#ifndef __TESEO_DATABOX_H__
#define __TESEO_DATABOX_H__ 1


void create_signals (void);

/*!
  teseo_create_databox() create a new window displaying the signal X,Y,num_points with a specified color and type.\
  This function is based on the example signals.c of the package gtkdatabox.
  \param gint num_points Number of points of the signal
  \param gfloat *X Horizontal coordinates. Don't free X after calling teseo_create_databox()!
  \param gfloat *Y Horizontal coordinates. Don't free Y after calling teseo_create_databox()!
  \param GdkColor Color color of the signal
  \param guint type Type of the signal, it's based on enum GtkDataboxDisplayType in gtkdatabox.h
  \param guint size Size of the signal.
  \param gchar *title Title of the windows.
  \param gchar *description Description.
  */
//void
GtkWidget*
teseo_create_databox (
		gint num_points, gfloat *X, gfloat *Y,
		GdkColor color, guint type, guint size,
		const gchar *title, const gchar *description,
		const gchar * labelX , const gchar *labelY);

#endif
