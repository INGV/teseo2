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

#include "teseo_bezier_pfit.h"
#include "cfortran.h" 

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "string.h"

int N_P_MIS_MAX, N_P_MIS;
double *x_p_mis;
double *y_p_mis;

inline double teseo_p_distance(double x1, double y1, double x2, double y2) {
 return sqrt( ((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1)) );
}

double teseo_p_func_sum_distance_bezier__(double *param_bez)
{
    // prima di richiamare questa funzione bisogna inizializzare
    // i vettori x_p_mis e y_p_mis con N_P_MIS
    double somma_quad_dist = 0.0;
    int i;
    double t=0.0, t_step=0.0, t_min=0.0, t2=0.0, toll_su_t=0.0;
    double dist_min=0.0;
    double dist_app=0.0;
    double a0=0.0, a1=0.0, a2=0.0, a3=0.0;
    double b0=0.0, b1=0.0, b2=0.0, b3=0.0;
    double x=0.0, y=0.0;
    double x0=0.0, y0=0.0, x1=0.0, y1=0.0, x2=0.0, y2=0.0, x3=0.0, y3=0.0, x4=0.0, y4=0.0;
    // double x_min, x_max;
    // double max_len_x;
    double dx=0.0, dy=0.0, dx4=0.0, dy4=0.0, z=0.0, z_min=0.0, z2=0.0, temp=0.0;
    int finito;
    int n_iter_do;
    int max_n_iter_do = 200;

    // Calcolo i parametri delle cubiche x(u) e y(u)
    x0 = x_p_mis[0];
    y0 = y_p_mis[0];
    x1 = param_bez[0];
    y1 = param_bez[1];
    x2 = param_bez[2];
    y2 = param_bez[3];
    x3 = x_p_mis[N_P_MIS-1];
    y3 = y_p_mis[N_P_MIS-1];


    a3 = (x3 - x0 + 3.0 * (x1 - x2)) / 8.0;
    b3 = (y3 - y0 + 3.0 * (y1 - y2)) / 8.0;
    a2 = (x3 + x0 - x1 - x2) * 3.0 / 8.0;
    b2 = (y3 + y0 - y1 - y2) * 3.0 / 8.0;
    a1 = (x3 - x0) / 2.0 - a3;
    b1 = (y3 - y0) / 2.0 - b3;
    a0 = (x3 + x0) / 2.0 - a2;
    b0 = (y3 + y0) / 2.0 - b2;

    toll_su_t =  1.0 / (20.0 * teseo_p_distance(x0, y0, x3, y3));
    if(toll_su_t < 1.0e-4) {
        g_printf("\ntoll_su_t = %f < 1.0e-4, set up to 1.0e-4\n", toll_su_t);
        toll_su_t = 1.0e-4;
    }
    // g_printf("%f\n", toll_su_t);


    t_step = 2.0 / ((2.0 * N_P_MIS) + 1.0);
    for(i=0; i<N_P_MIS; i++) {

        dist_app = 1.0;
        for(t=-1.0; t<=1.0  &&  dist_app != 0.0; t+=t_step) {
            x4 = x_p_mis[i];
            y4 = y_p_mis[i];

            x = a0 + t * (a1 + t * (a2 + t * a3));
            y = b0 + t * (b1 + t * (b2 + t * b3));
            dx4 = x - x4;
            dy4 = y - y4;
            dx = a1 + t * (2 * a2 + t * 3 * a3);
            dy = b1 + t * (2 * b2 + t * 3 * b3);
            z = dx * dx4 + dy * dy4;
            dist_app = dx4 * dx4 + dy4 * dy4;

            if(t == -1.0  ||  dist_app == 0.0) {
                dist_min = dist_app;
                z_min = z;
                t_min = t;
            }
            if(dist_app < dist_min) {
                dist_min = dist_app;
                z_min = z;
                t_min = t;
            }
        }

        if(dist_min != 0.0) {
            t = t_min + t_step;
            if(t > 1.0)
                t = 1.0 - t_step;
            n_iter_do = 0;
            finito = 0;
            do {
                x = a0 + t * (a1 + t * (a2 + t * a3));
                y = b0 + t * (b1 + t * (b2 + t * b3));
                dx4 = x - x4;
                dy4 = y - y4;
                dx = a1 + t * (2 * a2 + t * 3 * a3);
                dy = b1 + t * (2 * b2 + t * 3 * b3);
                z = dx * dx4 + dy * dy4;
                dist_app = dx4 * dx4 + dy4 * dy4;
                if(dist_app != 0.0  &&  z != 0.0) {
                    t2 = t;
                    z2 = z;
                    temp = z2 - z_min;
                    if(temp != 0.0) {
                        t = ((z2 * t_min) - (z_min * t2)) / temp;
                    } else {
                        t = (t_min + t2)  / 2.0;
                    }
                    if(t > 1.0)
                        t = 1.0;
                    else if(t < -1.0)
                        t = -1.0;
                    if(fabs(t - t2) < toll_su_t) {
                        finito = 1;
                        dist_min = dist_app;
                    } else {
                        t_min = t2;
                        z_min = z2;
                    }
                } else {
                    finito = 1;
                    dist_min = dist_app;
                }

                n_iter_do++;
                // ultima possibilità
                if(n_iter_do > max_n_iter_do) {
                    finito = 1;
                }
            } while(!finito);
            if(n_iter_do > max_n_iter_do) {
                g_printf("\nMax iterations exceeded (%d). toll_su_t = %f # dist_min = %f\n", max_n_iter_do, toll_su_t, dist_min);
            }

        }

        somma_quad_dist += dist_min;
    }

    // printf("(%-06.02f, %-06.02f)   (%-06.02f, %-06.02f)   #  dist = %-06.02f                \r", param_bez[0], param_bez[1], param_bez[2], param_bez[3], somma_quad_dist);
    // fflush(stdout);

    g_printf("\nsomma_quad_dist = %f\n", somma_quad_dist);

    return somma_quad_dist;
}

void teseo_p_maxima_minima_strokes(gdouble *strokes, glong num_strokes, gint **pidirezione, glong *pn_dir) {
  gint *idirezione;
  glong n_dir;
  int i_scoll=0, in_piano;
  glong  i;

  idirezione = (gint *) g_malloc(sizeof(gint) * (num_strokes + 1));
  idirezione[num_strokes] = (gint) CANARY;
  if(!idirezione) {
    g_message("Not enough free memory for direzione!");
    exit(1);
  }

  // calcolo tutti i cambi di direzione memorizzando gli indici in idirezione
  in_piano = 0;
  n_dir = 0;
  idirezione[n_dir++] = 0;
  for(i=1; i<num_strokes-1; i++) {
  	
  	if(strokes[(i-1)*2 +1] == strokes[i*2 +1]) {
  		if(!in_piano) {
  			i_scoll = i-1;
  		}
  		in_piano = 1;
  	}
  	
  	if(in_piano) {
  		if(i_scoll > 0) {
      	// "se non è il piano infinito" I.Allende
      	if(strokes[i*2 +1] != strokes[(i+1)*2 +1]) {
      		idirezione[n_dir++] = i_scoll + ((i - i_scoll) / 2);
      		in_piano = 0;
      	}
    	}
  	} else {
    	// se strokes[i] è un massimo
    	if(strokes[(i-1)*2 +1] < strokes[i*2 +1]  &&  strokes[(i+1)*2 +1] < strokes[i*2 +1]) {
    		idirezione[n_dir++] = i;
    	} else
    	// se strokes[i] è un minimo
    	if(strokes[(i-1)*2 +1] > strokes[i*2 +1]  &&  strokes[(i+1)*2 +1] > strokes[i*2 +1]) {
    		idirezione[n_dir++] = i;
    	}
  	}
  }
  idirezione[n_dir++] = num_strokes-1;

  *pidirezione = idirezione;
  *pn_dir = n_dir;
}

PROTOCCALLSFSUB8(NEWUOA,newuoa,INT, INT, PDOUBLE, DOUBLE, DOUBLE, INT, INT, PDOUBLE)
#define NEWUOA(A,B,C,D,E,F,G,H) CCALLSFSUB8(NEWUOA,newuoa, INT, INT, PDOUBLE, DOUBLE, DOUBLE, INT, INT, PDOUBLE, A,B,C,D,E,F,G,H)

glong teseo_p_fitting_bezier(glong num_strokes, gdouble* strokes, glong *pnum_path_inter, gdouble** ppath_inter) {
    double x0_bez=0, y0_bez=0, x3_bez=0, y3_bez=0;
    gdouble *path_inter;
    glong max_num_path_inter;
    glong cur_path_inter;
    glong lpath;
    gint *idirezione;
    glong n_dir;
    int cur_dir, app;
    int j, i;
    int ii, jj;
    // double fret,**xi;
    // int iter;
    double p[NDIM];

    // NEWUOA variables
    double W[10000]={};
    int IPRINT=2;
    int MAXFUN=5000;
    double RHOEND=1.0E-3;
    double RHOBEG=1.0E-3;
    int n,NPT;


    teseo_p_maxima_minima_strokes(strokes, num_strokes, &idirezione, &n_dir);
    // flessi_strokes(strokes, num_strokes, &idirezione, &n_dir);

    // filtro i massimi e minimi spuri, (quelli che hanno una teseo_p_distance molto piccola
    i=1;
    while(i<(n_dir-1)) {
        if(teseo_p_distance(strokes[idirezione[i]*2], strokes[idirezione[i]*2   +1],
                    strokes[idirezione[i+1]*2], strokes[idirezione[i+1]*2 +1]) <= 2.5) {
            for(j=i; j<n_dir-1; j++) {
                idirezione[j] = idirezione[j+1];
            }
            n_dir--;
        } else {
            i++;
        }
    }

    // invece di utilizzare i massimi e i minimi selezione i punti di indice intermedio
    //  for(i=1; i<(n_dir-1); i++) {
    //  	idirezione[i] -= ( (idirezione[i] - idirezione[i-1]) / 2) ;
    //  }

    N_P_MIS_MAX = 0;
    for(i=1; i<n_dir; i++) {
        app = (int) (idirezione[i] - idirezione[i-1] - 1);
        if(app > N_P_MIS_MAX)
            N_P_MIS_MAX = app;  		
        else if(app < 0) {
            // questo caso non deve mai verificarsi !!!
            g_message("\nError: app < 0 !!\n");
        }
    }

    if(N_P_MIS_MAX > 0) {
        x_p_mis = (double *) g_malloc(sizeof(double) * (N_P_MIS_MAX));
        if(!x_p_mis) {
            g_message("Not enough free memory for x_p_mis!");
            exit(1);
        }

        y_p_mis = (double *) g_malloc(sizeof(double) * (N_P_MIS_MAX));
        if(!y_p_mis) {
            g_message("Not enough free memory for y_p_mis!");
            exit(1);
        }
    } else {
        x_p_mis = NULL;
        y_p_mis = NULL;
    }

    max_num_path_inter = n_dir * 3;
    lpath = (max_num_path_inter) * 3;
    path_inter = (gdouble *) g_malloc(sizeof(gdouble) * (lpath + 1));
    path_inter[lpath] = (gdouble) CANARY;
    if(!path_inter) {
        g_message("Not enough free memory for path_inter!");
        exit(1);
    }

    cur_path_inter = 0;
    path_inter[cur_path_inter * 3   ] = strokes[0];
    path_inter[cur_path_inter * 3 +1] = strokes[1];
    path_inter[cur_path_inter * 3 +2] = 1.0;
    cur_path_inter++;

    //nr xi=matrix(1,NDIM,1,NDIM);

    gimp_progress_init("Teseo - Powell fitting . . .");
    gimp_progress_update(0.0);
    for(i=1; i<n_dir; i++) {
        gimp_progress_update((float) idirezione[i] / (float) num_strokes);
        x0_bez = strokes[idirezione[i-1]*2   ];
        y0_bez = strokes[idirezione[i-1]*2 +1];
        x3_bez = strokes[idirezione[i]*2   ];
        y3_bez = strokes[idirezione[i]*2 +1];

        N_P_MIS = 0;

        if(N_P_MIS_MAX > 0) {
            for(j=idirezione[i-1]+1; j < idirezione[i]; j++) {
                x_p_mis[N_P_MIS] = strokes[j*2];
                y_p_mis[N_P_MIS] = strokes[j*2 +1];
                N_P_MIS++;
            }

            p[0] = x0_bez + ((x3_bez - x0_bez) / 4);
            p[1] = y0_bez + ((y3_bez - y0_bez) / 4);
            p[2] = x3_bez - ((x3_bez - x0_bez) / 4);
            p[3] = y3_bez - ((y3_bez - y0_bez) / 4);

            /*
            for (ii=1; ii<=NDIM; ii++) {
                for (jj=1; jj<=NDIM; jj++) {
                    xi[ii][jj]=(ii == jj ? 1.0 : 0.0);
                }
            }
            */
        }

        if(N_P_MIS > 0) {
            // printf("\npowell inizio con %d misure\n", N_P_MIS);
            //nr powell(p,xi,NDIM,FTOL,&iter,&fret,teseo_p_func_sum_distance_bezier);
            // printf("powell fine\n");

                n = NDIM;
                NPT=2*n+1;

                // RHOBEG=0.2*p[0];
                // printf("\n\nPowell start with N=%d\tNPT=%d\n",n,NPT);
                NEWUOA (n, NPT, p, RHOBEG, RHOEND, IPRINT, MAXFUN, W);
                // printf("\n\nPowell stop\n");


            path_inter[cur_path_inter * 3   ] = p[0];
            path_inter[cur_path_inter * 3 +1] = p[1];
            path_inter[cur_path_inter * 3 +2] = 2.0;
            cur_path_inter++;

            path_inter[cur_path_inter * 3   ] = p[2];
            path_inter[cur_path_inter * 3 +1] = p[3];
            path_inter[cur_path_inter * 3 +2] = 2.0;
            cur_path_inter++;
        } else {
            path_inter[cur_path_inter * 3   ] = x0_bez;
            path_inter[cur_path_inter * 3 +1] = y0_bez;
            path_inter[cur_path_inter * 3 +2] = 2.0;
            cur_path_inter++;

            path_inter[cur_path_inter * 3   ] = x3_bez;
            path_inter[cur_path_inter * 3 +1] = y3_bez;
            path_inter[cur_path_inter * 3 +2] = 2.0;
            cur_path_inter++;
        }

        path_inter[cur_path_inter * 3   ] = x3_bez;
        path_inter[cur_path_inter * 3 +1] = y3_bez;
        path_inter[cur_path_inter * 3 +2] = 1.0;
        cur_path_inter++;

        if(cur_path_inter - 3 > max_num_path_inter) {
            g_printf("\ncur_path_inter = %d, max_num_path_inter = %d\n", cur_path_inter, max_num_path_inter);
        }

    }

    path_inter[cur_path_inter * 3   ] = x3_bez;
    path_inter[cur_path_inter * 3 +1] = y3_bez;
    path_inter[cur_path_inter * 3 +2] = 2.0;
    cur_path_inter++;

    g_printf("\ncur_path_inter = %d, num_strokes = %d\n", cur_path_inter, num_strokes);

    if(idirezione[num_strokes] != (gint) CANARY)
        g_message("Canary in idirezione is dead!");

    if(idirezione)
        g_free(idirezione);

    if(path_inter[lpath] != (gdouble) CANARY)
        g_message("Canary in strokes_inter is dead!");

    g_realloc( path_inter, ( sizeof(gdouble) * ((cur_path_inter + 1) * 3)) );

    if(x_p_mis) {
        g_free(x_p_mis);
    }
    if(y_p_mis) {
        g_free(y_p_mis);
    }

    /*
    if(xi)
        g_free(xi);
        */

    *ppath_inter = path_inter;
    *pnum_path_inter = cur_path_inter * 3;

    gimp_progress_init("Teseo - Fitting Bezier finished.");

    return (cur_path_inter); 	
}
