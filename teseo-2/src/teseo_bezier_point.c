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
#include <math.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "teseo_bezier_point.h"

// Bezier::Bezier(int agrado, double *aPx, double *aPy) {
void teseo_bezier_point_init(struct teseo_bezier_point *tbp, int agrado, double *aPx, double *aPy) {
  tbp->grado = agrado;
  tbp->n_punti = agrado+1;
  tbp->Px = (double *) g_malloc(tbp->n_punti * sizeof(double));
  tbp->Py = (double *) g_malloc(tbp->n_punti * sizeof(double));
  if(aPx  &&  aPy)
    teseo_bezier_point_setPoints(tbp, aPx, aPy);
  else
    teseo_bezier_point_setPointsZero(tbp);
}

// Bezier::~Bezier() {
void teseo_bezier_point_free(struct teseo_bezier_point *tbp) {
    g_free(tbp->Px);
    g_free(tbp->Py);
}

// void Bezier::setPoints(double *aPx, double *aPy) {
void teseo_bezier_point_setPoints(struct teseo_bezier_point *tbp, double *aPx, double *aPy) {
  int i;
  for(i=0; i<tbp->n_punti; i++) {
    tbp->Px[i] = aPx[i];
    tbp->Py[i] = aPy[i];
  }
}

// void Bezier::setPointsZero() {
void teseo_bezier_point_setPointsZero(struct teseo_bezier_point *tbp) {
  int i;
  for(i=0; i<tbp->n_punti; i++) {
    tbp->Px[i] = 0;
    tbp->Py[i] = 0;
  }
}

// int Bezier::getStrokes(int freq_c, double **astrokes, int sw_cast_int) {
int teseo_bezier_point_getStrokes(struct teseo_bezier_point *tbp, int freq_c, double **astrokes, int sw_cast_int) {
    const int STROKES_MAX = 1024;
    int n_punti_strokes_max = STROKES_MAX;
    int n_punti_strokes;
    double *strokes;
    int k, p, j, i, l;
    double X, Y;
    double *Pxi = NULL, *Pyi = NULL;
    // N.B: width_max è la distanza in pixel, fra i punti più lontani in x dei punti di bezier
    // l'ho scritto male ma è più semplici di quanto possa sembrare
    int width_max;
    double step;
    double t;
    int imax, imin;

    strokes = (double *) g_malloc(sizeof(double) * ((n_punti_strokes_max+2) * 2));
    if(!strokes) {
        *astrokes = NULL;
        g_message("teseo_bezier_point_getStrokes(): I can't allocate strokes !");
        return 0;
    }

    // aggiungo il primo punto
    n_punti_strokes = 0;
    if(sw_cast_int) {
        strokes[n_punti_strokes*2   ] = (int) tbp->Px[0];
        strokes[n_punti_strokes*2 +1] = (int) tbp->Py[0];
    } else {
        strokes[n_punti_strokes*2   ] = tbp->Px[0];
        strokes[n_punti_strokes*2 +1] = tbp->Py[0];
    }
    n_punti_strokes++;

    Pxi = (double *) g_malloc(tbp->n_punti * sizeof(double));
    Pyi = (double *) g_malloc(tbp->n_punti * sizeof(double));

    imin = 0;
    imax = 0;
    for(l=0; l<tbp->n_punti; l++) {
        if(tbp->Px[l] < tbp->Px[imin])
            imin = l;
        if(tbp->Px[l] > tbp->Px[imax])
            imax = l;
    }

    width_max = 4 * ((int) (tbp->Px[imax] - tbp->Px[imin]) + 1);
    step =  1./ (double) width_max;
    t = step;
    // printf("\nwidth_max = %d, step = %f\n", width_max, step);

    for (k=1; k < width_max; k++){

        for(p=0; p<tbp->n_punti; p++) {
            Pxi[p] = tbp->Px[p];
            Pyi[p] = tbp->Py[p];
        }

        // calcolo le coordinate X, Y a distanza t sulla curva
        for (j=tbp->grado; j > 0; j--) {
            for (i=0; i < j; i++) {
                Pxi[i] = (1-t)*Pxi[i] + t*Pxi[i+1];
                Pyi[i] = (1-t)*Pyi[i] + t*Pyi[i+1];
            }
        }

        if(n_punti_strokes >= n_punti_strokes_max) {
            n_punti_strokes_max += STROKES_MAX;
            strokes = (double *) g_realloc((void *) strokes, (sizeof(double) * ((n_punti_strokes_max+2) * 2 )));
            if(!strokes) {
                *astrokes = NULL;
                g_message("teseo_bezier_point_getStrokes(): I can't reallocate strokes !");
                return 0;
            }
        }

        if(sw_cast_int) {
            X = (int) (Pxi[0] + 0.5);
            Y = (int) (Pyi[0] + 0.5);
        } else {
            X = Pxi[0];
            Y = Pyi[0];
        }
        t += step;

        // posso decidere di aggiungere le coordinate con diversi criteri
        if(fabs(X - (strokes[(n_punti_strokes-1)*2])) == ((double) freq_c)) {
            strokes[n_punti_strokes*2     ] = X;
            strokes[(n_punti_strokes*2) +1] = Y;
            n_punti_strokes++;
        }

    }

    g_free(Pxi);
    g_free(Pyi);

    strokes = (double *) g_realloc((void *) strokes, (sizeof(double) * ((n_punti_strokes+2) * 2)));
    if(!(strokes)) {
        *astrokes = NULL;
        g_message("teseo_bezier_point_getStrokes(): I can't reallocate *astrokes !");
        return 0;
    }
    *astrokes = strokes;
    return n_punti_strokes;
}


// int Bezier::Grado() {
int teseo_bezier_point_Grado(struct teseo_bezier_point *tbp) {
  return tbp->grado;
}

int teseo_bezier_point_split(struct teseo_bezier_point *tbp, struct teseo_bezier_point *tbp_split1, struct teseo_bezier_point *tbp_split2, double t) {
    int ret = 1;
    // int j, i;
    // double *Pxi = NULL, *Pyi = NULL;
    // double step =  1.0 / 100.0;
    
    // calcolo le coordinate X, Y a distanza t sulla curva
    /*
    for (j=tbp->grado; j > 0; j--) {
        for (i=0; i < j; i++) {
            Pxi[i] = (1-t)*Pxi[i] + t*Pxi[i+1];
            Pyi[i] = (1-t)*Pyi[i] + t*Pyi[i+1];
        }
    }
    */

#define new_coord(p1, p2, t)  (p1 + t * (p2 - p1))

    if(tbp->grado == 3) {
        double x12, y12;

        tbp_split1->Px[0] = tbp->Px[0]; 
        tbp_split1->Py[0] = tbp->Py[0]; 

        tbp_split2->Px[3] = tbp->Px[3]; 
        tbp_split2->Py[3] = tbp->Py[3]; 

        tbp_split1->Px[1] = new_coord(tbp->Px[0], tbp->Px[1], t); 
        tbp_split1->Py[1] = new_coord(tbp->Py[0], tbp->Py[1], t); 

        tbp_split2->Px[2] = new_coord(tbp->Px[2], tbp->Px[3], t);; 
        tbp_split2->Py[2] = new_coord(tbp->Py[2], tbp->Py[3], t);; 

        x12 = new_coord(tbp->Px[1], tbp->Px[2], t); 
        y12 = new_coord(tbp->Py[1], tbp->Py[2], t); 

        tbp_split1->Px[2] = new_coord(tbp_split1->Px[1], x12, t); 
        tbp_split1->Py[2] = new_coord(tbp_split1->Py[1], y12, t); 

        tbp_split2->Px[1] = new_coord(x12, tbp_split2->Px[2], t); 
        tbp_split2->Py[1] = new_coord(y12, tbp_split2->Py[2], t); 

        tbp_split1->Px[3] = new_coord(tbp_split1->Px[2], tbp_split2->Px[1], t); 
        tbp_split1->Py[3] = new_coord(tbp_split1->Py[2], tbp_split2->Py[1], t); 

        tbp_split2->Px[0] = tbp_split1->Px[3]; 
        tbp_split2->Py[0] = tbp_split1->Py[3]; 

    }

    return ret;
}

int teseo_bezier_point_split_points_pairs(const gdouble *points_pairs, gdouble **new_points_pairs, double t) {
    int ret = 0;
    gdouble *pnew_points_pairs;
    struct teseo_bezier_point tbp, tbp_split1, tbp_split2;

    /*
    int i;
    for(i=0; i<12; i+=3) {
        g_printf("%f %f %f\n", points_pairs[i],points_pairs[i+1],points_pairs[i+2]);
    }
    */

    teseo_bezier_point_init(&tbp, 3, NULL, NULL);
    teseo_bezier_point_init(&tbp_split1, 3, NULL, NULL);
    teseo_bezier_point_init(&tbp_split2, 3, NULL, NULL);
    
    tbp.Px[0] = points_pairs[0];
    tbp.Py[0] = points_pairs[1];

    tbp.Px[1] = points_pairs[3];
    tbp.Py[1] = points_pairs[4];

    tbp.Px[2] = points_pairs[6];
    tbp.Py[2] = points_pairs[7];

    tbp.Px[3] = points_pairs[9];
    tbp.Py[3] = points_pairs[10];

    ret = teseo_bezier_point_split(&tbp, &tbp_split1, &tbp_split2, t);

    pnew_points_pairs = (gdouble *) g_malloc(sizeof(gdouble) * (6 + (9 * 2)));

    pnew_points_pairs[0] = tbp_split1.Px[0];
    pnew_points_pairs[1] = tbp_split1.Py[0];
    pnew_points_pairs[2] = 1.0;
    pnew_points_pairs[3] = tbp_split1.Px[1];
    pnew_points_pairs[4] = tbp_split1.Py[1];
    pnew_points_pairs[5] = 2.0;
    pnew_points_pairs[6] = tbp_split1.Px[2];
    pnew_points_pairs[7] = tbp_split1.Py[2];
    pnew_points_pairs[8] = 2.0;
    pnew_points_pairs[9] = tbp_split1.Px[3];
    pnew_points_pairs[10] = tbp_split1.Py[3];
    pnew_points_pairs[11] = 1.0;
    pnew_points_pairs[12] = tbp_split2.Px[1];
    pnew_points_pairs[13] = tbp_split2.Py[1];
    pnew_points_pairs[14] = 2.0;
    pnew_points_pairs[15] = tbp_split2.Px[2];
    pnew_points_pairs[16] = tbp_split2.Py[2];
    pnew_points_pairs[17] = 2.0;
    pnew_points_pairs[18] = tbp_split2.Px[3];
    pnew_points_pairs[19] = tbp_split2.Py[3];
    pnew_points_pairs[20] = 1.0;
    pnew_points_pairs[21] = tbp_split2.Px[3];
    pnew_points_pairs[22] = tbp_split2.Py[3];
    pnew_points_pairs[23] = 2.0;
        
    teseo_bezier_point_free(&tbp);
    teseo_bezier_point_free(&tbp_split1);
    teseo_bezier_point_free(&tbp_split2);
    
    *new_points_pairs = pnew_points_pairs;
    
    return ret;
}

