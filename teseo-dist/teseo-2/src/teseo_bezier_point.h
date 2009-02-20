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

#ifndef __TESEO_BEZIER_POINT_H__
#define __TESEO_BEZIER_POINT_H__ 1

struct teseo_bezier_point {
    // grado del Bezier
    int grado;
    // n_punti sarà sempre grado+1
    int n_punti;
    // vettori lunghi n_punti contenenti le coordinate dei punti di Bezier
    double *Px, *Py;
};

/*!
  i vettori devono essere lunghi agrado+1, dove il primo e l'ultimo valore sono di tipo ANCHOR_BEZIER
  */
void teseo_bezier_point_init(struct teseo_bezier_point *tbp, int agrado, double *aPx, double *aP);

/*!
  Only cubic curves
  */
void teseo_bezier_point_init_points_pairs(struct teseo_bezier_point *tbp, gdouble *points_pairs);

void teseo_bezier_point_free(struct teseo_bezier_point *tbp);

void teseo_bezier_point_setPoints(struct teseo_bezier_point *tbp, double *aPx, double *aPy);

void teseo_bezier_point_setPoints_points_pairs(struct teseo_bezier_point *tbp, gdouble *points_pairs);

void teseo_bezier_point_setPointsZero(struct teseo_bezier_point *tbp);

/*!
  ritorna il numero di elementi e il vettore vet con coordinate x,y
  campionato in x esattamente a passo freq_c
  i tipi garantiscono compatibità con i costruttori di neuronpunti
  si può scegliere se gli strokes ritornati abbiano un casting (int), per default è double
*/
int teseo_bezier_point_getStrokes(struct teseo_bezier_point *tbp, double point_per_pixel, double **astrokes, int sw_cast_int);

int teseo_bezier_point_Grado(struct teseo_bezier_point *tbp);

/*!
  teseo_bezier_point_split() split a single bezier curve into two cubic bezier curves with same degree at t position
  Only cubic curves
  */
int teseo_bezier_point_split(struct teseo_bezier_point *tbp, struct teseo_bezier_point *tbp_split1, struct teseo_bezier_point *tbp_split2, gdouble t);

/*!
  teseo_bezier_point_split_points_pairs() wrapper for teseo_bezier_point_split() using points_pairs 
  \param new_points_pairs needs to be freed
  Only cubic curves
  */
int teseo_bezier_point_split_points_pairs(const gdouble *points_pairs, gdouble **new_points_pairs, double t);

/*!
  TODO
  */
void teseo_bezier_point_get_xy_from_t(struct teseo_bezier_point *tbp, gdouble t, gdouble *x, gdouble *y);

/*!
  TODO
  */
gboolean teseo_bezier_point_get_t_from_x(struct teseo_bezier_point *tbp, gdouble x, gdouble *t);

#endif
