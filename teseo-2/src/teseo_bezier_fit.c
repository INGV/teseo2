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

#include "teseo_bezier_fit.h"
#include "teseo_bezier_point.h"
#include "nrutil.h"
#include "nr.h"

#include <stdio.h>
#include <stdlib.h>
#include "string.h"

int N_MIS_MAX, N_MIS;
float *x_mis;
float *y_mis;
float x0_bez = 13.0;
float y0_bez = 235.0;
float x3_bez = 233.0;
float y3_bez = 229.0;

float teseo_distance(float x1, float y1, float x2, float y2) {
 return sqrt( ((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1)) );
}

float teseo_func_sum_distance_bezier(float param_bez[])
{
    // prima di richiamare questa funzione bisogna inizializzare
    // x0_bez, y0_bez, x3_bez, y3_bez,
    // i vettori x_mis e y_mis con N_MIS
    float somma_quad_dist = 0.0;
    int i;
    float t, t_step, t_min=0.0, t2, toll_su_t;
    float dist_min=0.0;
    float dist_app;
    float a0, a1, a2, a3;
    float b0, b1, b2, b3;
    float x, y;
    float x0, y0, x1, y1, x2, y2, x3, y3, x4, y4;
    // float x_min, x_max;
    // float max_len_x;
    float dx, dy, dx4, dy4, z, z_min=0.0, z2, temp;
    int finito;
    int n_iter_do;
    int max_n_iter_do = 200;


    // Calcolo i parametri delle cubiche x(u) e y(u)
    x0 = x0_bez;
    y0 = y0_bez;
    x1 = param_bez[1];
    y1 = param_bez[2];
    x2 = param_bez[3];
    y2 = param_bez[4];
    x3 = x3_bez;
    y3 = y3_bez;


    a3 = (x3 - x0 + 3.0 * (x1 - x2)) / 8.0;
    b3 = (y3 - y0 + 3.0 * (y1 - y2)) / 8.0;
    a2 = (x3 + x0 - x1 - x2) * 3.0 / 8.0;
    b2 = (y3 + y0 - y1 - y2) * 3.0 / 8.0;
    a1 = (x3 - x0) / 2.0 - a3;
    b1 = (y3 - y0) / 2.0 - b3;
    a0 = (x3 + x0) / 2.0 - a2;
    b0 = (y3 + y0) / 2.0 - b2;

    /*
    x_min = x0_bez;
    x_max = x3_bez;
    for(i=0; i<N_MIS; i++) {
    if(x_mis[i] < x_min) {
      x_min = x_mis[i];
    }
    if(x_mis[i] > x_max) {
      x_max = x_mis[i];
    }
    }
    if(x3_bez < x_min) {
    x_min = x0_bez;
    }
    if(x0_bez > x_max) {
    x_max = x0_bez;
    }

    max_len_x = x_max - x_min; // dovrò cambiarlo mettendo la massima teseo_distance in x fra i punti di bezier
    t_step = 1.0 / (max_len_x * 2.0);
    */

    toll_su_t =  1.0 / (20.0 * teseo_distance(x0, y0, x3, y3));
    if(toll_su_t < 1.0e-4) {
	printf("\ntoll_su_t = %f < 1.0e-4, set up to 1.0e-4\n", toll_su_t);
	toll_su_t = 1.0e-4;
    }
    // printf("%f\n", toll_su_t);


    t_step = 2.0 / ((2.0 * N_MIS) + 1.0);
    for(i=0; i<N_MIS; i++) {

    dist_app = 1.0;
    for(t=-1.0; t<=1.0  &&  dist_app != 0.0; t+=t_step) {
      x4 = x_mis[i];
      y4 = y_mis[i];

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
	printf("\nMax iteration exceed (%d). toll_su_t = %f # dist_min = %f\n", max_n_iter_do, toll_su_t, dist_min);
      }

    }

    somma_quad_dist += dist_min;
}

// printf("(%-06.02f, %-06.02f)   (%-06.02f, %-06.02f)   #  dist = %-06.02f                \r", param_bez[1], param_bez[2], param_bez[3], param_bez[4], somma_quad_dist);
// fflush(stdout);

return somma_quad_dist;
}

void teseo_maxima_minima_strokes(gdouble *strokes, glong num_strokes, gint **pidirezione, glong *pn_dir) {
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

glong teseo_fitting_bezier(glong num_strokes, gdouble* strokes, glong *pnum_path_inter, gdouble** ppath_inter) {
  gdouble *path_inter;
  glong max_num_path_inter;
  glong cur_path_inter;
  glong lpath;
  gint *idirezione;
  glong n_dir;
  int cur_dir, app;
  int j, i;
  int ii, jj;
  float fret,**xi;
  int iter;
  float p[1+NDIM];
  /*
  gdouble *deriv = NULL;

  derivata( strokes, num_strokes, &deriv);
  	
	teseo_maxima_minima_strokes(deriv, num_strokes, &idirezione, &n_dir);
	
	if(deriv) {
		g_free(deriv);
	}
	*/
	
	teseo_maxima_minima_strokes(strokes, num_strokes, &idirezione, &n_dir);
	// flessi_strokes(strokes, num_strokes, &idirezione, &n_dir);
	
  // filtro i massimi e minimi spuri, (quelli che hanno una teseo_distance molto piccola
  i=1;
  while(i<(n_dir-1)) {
  	if(teseo_distance((float) strokes[idirezione[i]*2], (float) strokes[idirezione[i]*2   +1],
  		(float) strokes[idirezione[i+1]*2], (float) strokes[idirezione[i+1]*2 +1]) <= 2.5) {
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

  N_MIS_MAX = 0;
  for(i=1; i<n_dir; i++) {
  	app = (int) (idirezione[i] - idirezione[i-1] - 1);
  	if(app > N_MIS_MAX)
  		N_MIS_MAX = app;  		
  	else if(app < 0) {
  		// questo caso non deve mai verificarsi !!!
  		printf("\nError: app < 0 !!\n");
  	}
  }

  if(N_MIS_MAX > 0) {
    x_mis = (float *) g_malloc(sizeof(float) * (N_MIS_MAX));
    if(!x_mis) {
      g_message("Not enough free memory for x_mis!");
      exit(1);
    }

    y_mis = (float *) g_malloc(sizeof(float) * (N_MIS_MAX));
    if(!y_mis) {
      g_message("Not enough free memory for y_mis!");
      exit(1);
    }
  } else {
  	x_mis = NULL;
  	y_mis = NULL;
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

  xi=matrix(1,NDIM,1,NDIM);

	gimp_progress_init("Neuron Teseo - Fitting Bezier . . .");
	gimp_progress_update(0.0);
  for(i=1; i<n_dir; i++) {
		gimp_progress_update((float) idirezione[i] / (float) num_strokes);
  	x0_bez = strokes[idirezione[i-1]*2   ];
  	y0_bez = strokes[idirezione[i-1]*2 +1];
  	x3_bez = strokes[idirezione[i]*2   ];
  	y3_bez = strokes[idirezione[i]*2 +1];
  	
  	N_MIS = 0;
  	
  	if(N_MIS_MAX > 0) {
    	for(j=idirezione[i-1]+1; j < idirezione[i]; j++) {
    		x_mis[N_MIS] = strokes[j*2];
    		y_mis[N_MIS] = strokes[j*2 +1];
    		N_MIS++;
    	}
    	
      p[1] = (2.0 * x_mis[0]) - x0_bez;
      p[2] = (2.0 * y_mis[0]) - y0_bez;
      p[3] = (2.0 * x_mis[N_MIS - 1]) - x3_bez;
      p[4] = (2.0 * y_mis[N_MIS - 1]) - y3_bez;
    	
      for (ii=1; ii<=NDIM; ii++) {
        for (jj=1; jj<=NDIM; jj++) {
          xi[ii][jj]=(ii == jj ? 1.0 : 0.0);
        }
      }
    }

    if(N_MIS > 0) {
     	// printf("\npowell inizio con %d misure\n", N_MIS);
    	powell(p,xi,NDIM,FTOL,&iter,&fret,teseo_func_sum_distance_bezier);
     	// printf("powell fine\n");
    	
    	path_inter[cur_path_inter * 3   ] = p[1];
    	path_inter[cur_path_inter * 3 +1] = p[2];
    	path_inter[cur_path_inter * 3 +2] = 2.0;
  	 	cur_path_inter++;
  		  	
    	path_inter[cur_path_inter * 3   ] = p[3];
    	path_inter[cur_path_inter * 3 +1] = p[4];
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
	 		printf("\ncur_path_inter = %d, max_num_path_inter = %d\n", cur_path_inter, max_num_path_inter);
	 	}
  	
  }

	path_inter[cur_path_inter * 3   ] = x3_bez;
	path_inter[cur_path_inter * 3 +1] = y3_bez;
	path_inter[cur_path_inter * 3 +2] = 2.0;
 	cur_path_inter++;
 	
 	printf("\ncur_path_inter = %d, num_strokes = %d\n", cur_path_inter, num_strokes);
 	
  if(idirezione[num_strokes] != (gint) CANARY)
		g_message("Canary in idirezione is dead!");
	if(idirezione)
		g_free(idirezione);

  if(path_inter[lpath] != (gdouble) CANARY)
  	g_message("Canary in strokes_inter is dead!");
	
  g_realloc( path_inter, ( sizeof(gdouble) * ((cur_path_inter + 1) * 3)) );

  if(x_mis) {
  	g_free(x_mis);
  }
  if(y_mis) {
  	g_free(y_mis);
  }
  	
  if(xi)
   g_free(xi);

	*ppath_inter = path_inter;
	*pnum_path_inter = cur_path_inter;
	
	gimp_progress_init("Neuron Teseo - Fitting Bezier finished.");

  return (cur_path_inter); 	
}
