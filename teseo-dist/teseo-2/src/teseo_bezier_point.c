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

void teseo_bezier_point_init_points_pairs(struct teseo_bezier_point *tbp, gdouble *points_pairs) {
    teseo_bezier_point_init(tbp, 3, NULL, NULL);
    teseo_bezier_point_setPoints_points_pairs(tbp, points_pairs);
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

void teseo_bezier_point_setPoints_points_pairs(struct teseo_bezier_point *tbp, gdouble *points_pairs) {
    gdouble Px[4], Py[4];

    Px[0] = points_pairs[0];
    Py[0] = points_pairs[1];

    Px[1] = points_pairs[3];
    Py[1] = points_pairs[4];

    Px[2] = points_pairs[6];
    Py[2] = points_pairs[7];

    Px[3] = points_pairs[9];
    Py[3] = points_pairs[10];

    teseo_bezier_point_setPoints(tbp, Px, Py);
}

int teseo_bezier_point_getStrokes(struct teseo_bezier_point *tbp, gdouble points_per_pixel, double **astrokes, double X_previous, gboolean sw_abscissa_ascendent) {
    const int STROKES_MAX = 1024;
    int n_punti_strokes_max = STROKES_MAX;
    int n_punti_strokes;
    double *strokes;
    int k, p, j, i, l;
    double X, Y;
    double X_old, Y_old;
    double X_expected;
    double X_expected_back;
    double err_X_forw, err_X_back;
    double *Pxi = NULL, *Pyi = NULL;
    // N.B: width_max è la distanza in pixel, fra i punti più lontani in x dei punti di bezier
    // l'ho scritto male ma è più semplici di quanto possa sembrare
    int width_max;
    double step;
    double t;
    int imax, imin;
    double t_bef, t_aft, t_cur;
    gint iterations = 0;
    double err_margin = 10.0;
    gdouble X_previous_ideal = X_previous;

    double precision = 1000.0;
    
    /* precision = 100.0 and points_per_pixel = 0.1; */
    // double tolerable_err_points_per_pixel = points_per_pixel /  precision;
    // int fract_pixel_precision = (int) (precision / points_per_pixel);
    double tolerable_err_points_per_pixel = (1.0 / precision);
    int fract_pixel_precision = (int) (precision / 1.0);

    strokes = (double *) g_malloc(sizeof(double) * ((n_punti_strokes_max+2) * 2));
    if(!strokes) {
        *astrokes = NULL;
        g_message("teseo_bezier_point_getStrokes(): I can't allocate strokes !");
        return 0;
    }

    // non devo aggiungere il primo punto ma utilizzare X_previous e Y_previous
    // aggiungo il primo punto
    n_punti_strokes = 0;
    /*
    strokes[n_punti_strokes*2   ] = tbp->Px[0];
    strokes[n_punti_strokes*2 +1] = tbp->Py[0];
    n_punti_strokes++;
    */

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

    /* per ogni pixel ho un dettaglio massimo di 100 punti per pixel */
    width_max = fract_pixel_precision * ((int) (tbp->Px[imax] - tbp->Px[imin]) + 1);
    step =  1.0 / (double) width_max;
    // t = step;
    t = 0.0;
    // printf("\nwidth_max = %d, step = %f\n", width_max, step);

    for (k=0; k < width_max; k++) {

        for(p=0; p<tbp->n_punti; p++) {
            Pxi[p] = tbp->Px[p];
            Pyi[p] = tbp->Py[p];
        }

        // calcolo le coordinate X, Y a distanza t sulla curva
        for (j=tbp->grado; j > 0; j--) {
            for (i=0; i < j; i++) {
                Pxi[i] = (1.0-t)*Pxi[i] + t*Pxi[i+1];
                Pyi[i] = (1.0-t)*Pyi[i] + t*Pyi[i+1];
            }
        }

	X = Pxi[0];
	Y = Pyi[0];

        // posso decidere di aggiungere le coordinate con diversi criteri
        // old condition if(fabs(X - (strokes[(n_punti_strokes-1)*2])) == ((double) points_per_pixel)) 
	// X_expected = ( X_previous  + ( ((double) (n_punti_strokes + 1) ) * points_per_pixel) );

	X_expected = ( X_previous_ideal  +  points_per_pixel);
	X_expected_back = ( X_previous_ideal  -  points_per_pixel);

	if(
		fabs(X - X_expected) < (err_margin * tolerable_err_points_per_pixel)
		||
		( !sw_abscissa_ascendent && (fabs(X_expected_back - X) < (err_margin * tolerable_err_points_per_pixel) ) )
	  ) {

	    err_X_forw = X - X_expected;
	    err_X_back = X - X_expected_back;

	    // g_printf("Added (%f, %f) xpctd: %f  err:%f  %d!\n", X, Y, X_expected, err_X_forw, n_punti_strokes);

	    if(n_punti_strokes >= n_punti_strokes_max - 2) {
		n_punti_strokes_max += STROKES_MAX;
		strokes = (double *) g_realloc((void *) strokes, (sizeof(double) * ((n_punti_strokes_max+2) * 2 )));
		if(!strokes) {
		    *astrokes = NULL;
		    g_message("teseo_bezier_point_getStrokes(): I can't reallocate strokes !");
		    return 0;
		}
	    }

	    X_old = X;
	    Y_old = Y;

	    /* check if they are almost really zero ;-) */
	    if( fabs(err_X_forw) < 0.00000001  || fabs(err_X_back) < 0.00000001  ) {
		  /* Does not improve ! */
		    /* debugging */
		    g_message("Not improve following point (%11.6f, %11.6f) eXf=%+10.6f eXb=%+10.6f\n",
			     X, Y, err_X_forw, err_X_back);
		    g_printf("Not improve following point (%11.6f, %11.6f) eXf=%+10.6f eXb=%+10.6f\n",
			     X, Y, err_X_forw, err_X_back);
	    } else {

	    /* BEGIN improve X  */
	    t_cur = t;
	    t_bef = t_cur - ((err_margin * 0.75) * step);
	    t_aft = t_cur + ((err_margin * 0.75) * step);
	    iterations = 0;
	    while(
		    (
		    fabs(X - X_expected) > tolerable_err_points_per_pixel 
		    ||
		    ( !sw_abscissa_ascendent && fabs(X_expected_back - X) > tolerable_err_points_per_pixel )
		    )
		    &&  iterations < 5) {

		iterations++;

		if( sw_abscissa_ascendent  &&  ( fabs(X - X_expected) < fabs(X_expected_back - X) ) ) {
		    if(X < X_expected) {
			t_bef = t_cur;
		    } else {
			t_aft = t_cur;
		    }
		    t_cur = t_bef + ((t_aft - t_bef) / 2.0);
		} else {
		    /* sto tornando indietro sulle X allora inverto il ragionamento */
		    if(X < X_expected_back) {
			t_aft = t_cur;
		    } else {
			t_bef = t_cur;
		    }
		    t_cur = t_aft + ((t_bef - t_aft) / 2.0);
		}

		for(p=0; p<tbp->n_punti; p++) {
		    Pxi[p] = tbp->Px[p];
		    Pyi[p] = tbp->Py[p];
		}

		// calcolo le coordinate X, Y a distanza t_cur sulla curva
		for (j=tbp->grado; j > 0; j--) {
		    for (i=0; i < j; i++) {
			Pxi[i] = (1.0-t_cur)*Pxi[i] + t_cur*Pxi[i+1];
			Pyi[i] = (1.0-t_cur)*Pyi[i] + t_cur*Pyi[i+1];
		    }
		}

		/* Set new X and Y which it tried to improve */
		X = Pxi[0];
		Y = Pyi[0];

		/* debugging */
		/*
		g_printf("i%2d (%11.6f, %11.6f) eXf=%+10.6f eXff=%+10.6f eXb=%+10.6f eXfb=%+10.6f (%f) (%f, %f, %f) %4d\n",
			iterations, X, Y, err_X_forw, X-X_expected, err_X_back, X-X_expected_back, t, t_bef, t_cur, t_aft, n_punti_strokes);
			*/
	    }

	    if( sw_abscissa_ascendent  ||  ( fabs(X - X_expected) < fabs(X_expected_back - X) ) ) {
		/* check if it was able to improve X approsimation */
		if(fabs(err_X_forw) <  fabs(X-X_expected)) {
		    /* debugging */
		    g_message("RBf (%11.6f, %11.6f) (%11.6f, %11.6f) eXf=%+10.6f eXff=%+10.6f eXb=%+10.6f eXfb=%+10.6f\n",
			    X_old, Y_old, X, Y, err_X_forw, X-X_expected, err_X_back, X-X_expected_back);
		    g_printf("RBf (%11.6f, %11.6f) (%11.6f, %11.6f) eXf=%+10.6f eXff=%+10.6f eXb=%+10.6f eXfb=%+10.6f\n",
			    X_old, Y_old, X, Y, err_X_forw, X-X_expected, err_X_back, X-X_expected_back);
		    /* Rollback set X,Y to X_old,Y_old */
		    X = X_old;
		    Y = Y_old;
		}
	    } else {
		/* check if it was able to improve X approsimation */
		if(fabs(err_X_back) < fabs(X-X_expected_back)) {
		    /* debugging */
		    g_message("RBb (%11.6f, %11.6f) (%11.6f, %11.6f) eXf=%+10.6f eXff=%+10.6f eXb=%+10.6f eXfb=%+10.6f\n",
			    X_old, Y_old, X, Y, err_X_forw, X-X_expected, err_X_back, X-X_expected_back);
		    g_printf("RBb (%11.6f, %11.6f) (%11.6f, %11.6f) eXf=%+10.6f eXff=%+10.6f eXb=%+10.6f eXfb=%+10.6f\n",
			    X_old, Y_old, X, Y, err_X_forw, X-X_expected, err_X_back, X-X_expected_back);
		    /* Rollback set X,Y to X_old,Y_old */
		    X = X_old;
		    Y = Y_old;
		}
	    }

	    /* debugging */
	    /*
	    g_printf("i%2d (%11.6f, %11.6f) eXf=%+10.6f eXff=%+10.6f eXb=%+10.6f eXfb=%+10.6f (%f) (%f, %f, %f) %4d\n",
		    iterations, X, Y, err_X_forw, X-X_expected, err_X_back, X-X_expected_back, t, t_bef, t_cur, t_aft, n_punti_strokes);
		    */
	    /* END improve X  */

	    }

	    X_previous = X;
	    strokes[n_punti_strokes*2     ] = X;
	    strokes[(n_punti_strokes*2) +1] = Y;
	    n_punti_strokes++;

	    if( fabs(X_previous - X_expected) <= fabs(X_previous - X_expected_back) ) {
		X_previous_ideal = X_expected;
	    } else {
		X_previous_ideal = X_expected_back;
	    }
	}

	t += step;
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

gdouble new_coord(gdouble p1, gdouble p2, gdouble t) {
    return p1 + ( (t) * ( p2 -  p1) );
}


int teseo_bezier_point_split(struct teseo_bezier_point *tbp, struct teseo_bezier_point *tbp_split1, struct teseo_bezier_point *tbp_split2, gdouble t) {
    int ret = 1;


    if(tbp->grado == 3) {
        gdouble x12, y12;

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

void teseo_bezier_point_get_xy_from_t(struct teseo_bezier_point *tbp, gdouble t, gdouble *x, gdouble *y) {
    gint j, i, p;
    gdouble *Pxi = NULL, *Pyi = NULL;
    
    Pxi = (gdouble *) g_malloc(tbp->n_punti * sizeof(gdouble));
    Pyi = (gdouble *) g_malloc(tbp->n_punti * sizeof(gdouble));

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

    *x = Pxi[0];
    *y = Pyi[0];

    // g_printf("teseo_bezier_point_get_xy_from_t() t %f, x %f, y %f\n", t, *x, *y);

    g_free(Pxi);
    g_free(Pyi);
}

gboolean teseo_bezier_point_get_t_from_x(struct teseo_bezier_point *tbp, gdouble x, gdouble *t) {
    gboolean ret = FALSE;
    gboolean reversed = FALSE;
    gint k;
    gdouble app;
    gdouble t_cmp=0.5, t_min, t_max;
    gdouble xt, yt;
    const gint max_iter = 50;
    gint iter=0;

    g_printf("teseo_bezier_point_get_t_from_x() called, x %f\n", x);

    if(tbp->Px[0] > tbp->Px[tbp->n_punti-1]) {
        g_printf("teseo_bezier_point_get_t_from_x(): reversed array before\n");
        reversed = TRUE;
        for(k=0; k/2; k++) {
            app = tbp->Px[k];
            tbp->Px[k] = tbp->Px[tbp->n_punti-1-k];
            tbp->Px[tbp->n_punti-1-k] = app;

            app = tbp->Py[k];
            tbp->Py[k] = tbp->Py[tbp->n_punti-1-k];
            tbp->Py[tbp->n_punti-1-k] = app;
        }
    }

    // This condition limits where x must be
    if(tbp->Px[0] < x  &&  x < tbp->Px[tbp->n_punti-1]) {

        t_min = 0.0;
        t_cmp = 0.5;
        t_max = 1.0;
        iter = 0;
        while(ret == FALSE  &&  iter < max_iter) {
            teseo_bezier_point_get_xy_from_t(tbp, t_cmp, &xt, &yt);
            // g_printf("x %f, (xt %f, yt %f) t_cmp %f (t_min %.10f, t_max %.10f)\n", x, xt, yt, t_cmp, t_min, t_max);

            if((t_max - t_min) < 1.0e-36) {
            // if(fabs(xt - x) < 1.0e-6)
                ret = TRUE;
                *t = t_cmp;
            } else {
                if(xt > x) {
                    t_max = t_cmp;
                } else {
                    t_min = t_cmp;
                }
            }
            t_cmp = t_min + ((t_max - t_min) / 2.0);
            iter++;
        }
        g_printf("x %f, (xt %f, yt %f) t_cmp %f (t_min %.10f, t_max %.10f)\n", x, xt, yt, t_cmp, t_min, t_max);
        
    } else {
        if(x == tbp->Px[0]) {
            ret = TRUE;
            *t = 0.0;
        } else if(x == tbp->Px[tbp->n_punti-1]) {
            ret = TRUE;
            *t = 1.0;
        } else {
            g_printf("teseo_bezier_point_get_t_from_x(): x=%f is not belonging to anchor points [%f, %f].\n", x, tbp->Px[0], tbp->Px[tbp->n_punti-1]);
        }
    }

    if(iter >= max_iter) {
        ret = TRUE;
        *t = t_cmp;
        g_printf("teseo_bezier_point_get_t_from_x(): max iteration exceeded %d. t %f\n", max_iter, *t);
    }

    if(reversed) {
        g_printf("teseo_bezier_point_get_t_from_x(): reversed array after\n");
        for(k=0; k/2; k++) {
            app = tbp->Px[k];
            tbp->Px[k] = tbp->Px[tbp->n_punti-1-k];
            tbp->Px[tbp->n_punti-1-k] = app;

            app = tbp->Py[k];
            tbp->Py[k] = tbp->Py[tbp->n_punti-1-k];
            tbp->Py[tbp->n_punti-1-k] = app;
        }
    }

    return ret;
}

