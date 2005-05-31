#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "teseo_bezier_point.h"

// Bezier::Bezier(int agrado, double *aPx, double *aPy) {
void teseo_bezier_point_init(struct teseo_bezier_point *tbp, int agrado, double *aPx, double *aPy) {
  tbp->grado = agrado;
  tbp->n_punti = agrado+1;
  tbp->Px = (double *) malloc(tbp->n_punti * sizeof(double));
  tbp->Py = (double *) malloc(tbp->n_punti * sizeof(double));
  if(aPx  &&  aPy)
    teseo_bezier_point_setPoints(tbp, aPx, aPy);
  else
    teseo_bezier_point_setPointsZero(tbp);
}

// Bezier::~Bezier() {
void teseo_bezier_point_free(struct teseo_bezier_point *tbp) {
  if(tbp->Px)
    free(tbp->Px);
  if(tbp->Py)
    free(tbp->Py);
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
  // int X, Y;
  int k, p, j, i, l;
  double X, Y;
  double *Pxi, *Pyi;
  // N.B: width_max è la distanza in pixel, fra i punti più lontani in x dei punti di bezier
  // l'ho scritto male ma è più semplici di quanto possa sembrare
  int width_max;
  double step;
  double t;
  int imax, imin;

  strokes = (double *) malloc(sizeof(double) * ((n_punti_strokes_max+2) * 2));
  if(!strokes) {
  	*astrokes = NULL;
  	printf("Non riesco ad allocare strokes in Bezier::getStrokes!");
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

  Pxi = (double *) malloc(tbp->n_punti * sizeof(double));
  Pyi = (double *) malloc(tbp->n_punti * sizeof(double));

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
    // System.arraycopy(Px,0,Pxi,0,n);
    // System.arraycopy(Py,0,Pyi,0,n);
    for(p=0; p<tbp->n_punti; p++) {
      Pxi[p] = tbp->Px[p];
      Pyi[p] = tbp->Py[p];
    }

    // calcolo le coordinate X, Y a distanza t sulla curva
    // for (j=n-1; j > 0; j--) {
    for (j=tbp->grado; j > 0; j--) {
      for (i=0; i < j; i++) {
        Pxi[i] = (1-t)*Pxi[i] + t*Pxi[i+1];
        Pyi[i] = (1-t)*Pyi[i] + t*Pyi[i+1];
      }
    }
    if(n_punti_strokes >= n_punti_strokes_max) {
      n_punti_strokes_max += STROKES_MAX;
      strokes = (double *) realloc((void *) strokes, (sizeof(double) * ((n_punti_strokes_max+2) * 2 )));
        if(!strokes) {
          *astrokes = NULL;
          printf("Non riesco ad Reallocare strokes in Bezier::getStrokes!");
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
    // buffGraphics.drawLine(Xold,Yold, X,Y );
    // Xold = X; Yold = Y;
    t += step;

    // posso decidere di aggiungere le coordinate con diversi criteri
    if(fabs(X - (strokes[(n_punti_strokes-1)*2])) == ((double) freq_c)) {
      strokes[n_punti_strokes*2     ] = X;
      strokes[(n_punti_strokes*2) +1] = Y;
      n_punti_strokes++;
    }
//     if(abs((int) X - (int) (strokes[(n_punti_strokes-1)*2])) >= freq_c) {
//       strokes[n_punti_strokes*2] = X;
//       strokes[(n_punti_strokes*2) +1] = Y;
//       n_punti_strokes++;
//     }

    /* // mtheo inizio
    if(Math.abs(X - Xold_mtheo) >= step_mtheo) {
      buffGraphics.setColor(Color.black);
      buffGraphics.drawLine(Xold_mtheo,Yold_mtheo, X, Y );
      buffGraphics.drawRect(X-1,Y-1, 2,2);
      Xold_mtheo = X;
      Yold_mtheo = Y;
      buffGraphics.setColor(Color.red);
    }
    // mtheo fine */
  }

  // aggiungo l'ultimo
  // strokes[n_punti_strokes*2] = Px[n_punti-1];
  // strokes[n_punti_strokes*2 +1] = Py[n_punti-1];
  // n_punti_strokes++;

  if(Pxi)
    free(Pxi);
  if(Pyi)
    free(Pyi);

  strokes = (double *) realloc((void *) strokes, (sizeof(double) * ((n_punti_strokes+2) * 2)));
  if(!(strokes)) {
    *astrokes = NULL;
    printf("Non riesco ad Reallocare *astrokes in Bezier::getStrokes!");
    return 0;
  }
  *astrokes = strokes;
  return n_punti_strokes;
}


// int Bezier::Grado() {
int teseo_bezier_point_Grado(struct teseo_bezier_point *tbp) {
  return tbp->grado;
}
