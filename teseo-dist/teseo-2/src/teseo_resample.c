#include "teseo_resample.h"
#include "teseo_bezier_point.h"

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
// #include "teseo_path.h"
// #include "values.h"


// si dovrebbe includere neuronutils ma basta dichiare extern TESEO_DATA e getenv_teseo (per ora)
// #include "neuronutils.h"
extern const char TESEO_DATA[];
extern char *getenv_teseo(const char *name_var);


void sottocampiona_strokes(double *strokes, glong *pn_strokes, int passo) {
	glong n_strokes = *pn_strokes;
	glong i;
	// printf("\nBEFORE n_strokes = %d\n", n_strokes);
	
	for(i=0; (i*passo) < n_strokes; i++) {
		strokes[(i*2)   ] = strokes[(i*2*passo)];
		strokes[(i*2) +1] = strokes[(i*2*passo) +1];
	}
	*pn_strokes = n_strokes / passo;
	
	// printf("\nAFTER pn_strokes = %d,   n_strokes = %d,   i * step = %d,\n", *pn_strokes, n_strokes, i*passo);
}


void campionamento_progressivo_strokes(double *strokes, glong *pn_strokes) {
	glong n_strokes = *pn_strokes;
	glong i, j;
	// printf("\nBEFORE campionascendentex_strokes n_strokes = %d\n", n_strokes);
	
	i = 1;	
	while(i < n_strokes) {
		if(strokes[((i-1)*2)] >= strokes[(i*2)]) {
			for(j=i+1; j<n_strokes; j++) {
    		strokes[((j-1)*2)   ] = strokes[(j*2)];
    		strokes[((j-1)*2) +1] = strokes[(j*2)+1];
			}
			n_strokes--;
		} else {
			i++;
		}
	}
	*pn_strokes = n_strokes;
	
	// printf("\nAFTER campionascendentex_strokes n_strokes = %d\n", n_strokes);
}




// void call_on_btnBezier_clicked(gint32 g_image, NeuronSismosVals pivals)
void call_on_btnBezier_clicked(gint32 g_image, int sw_campionamento_progressivo, gint passo_bezier)
{

FILE *ftmp;
FILE *fbezier;
char pathname [255] ;
char newpathname [255] ;
gint32 path_type, path_closed, num_elementi_bezier, n;  
int bezier_n_strokes;
double *bezier_strokes;
int i, kk;
int max_n_strokes = 1024;
glong n_strokes;
gdouble *strokes;
gdouble *punti_bezier;
double Px[4], Py[4];

// class Bezier bezier = Bezier(3);
struct teseo_bezier_point tbp;
teseo_bezier_point_init(&tbp, 3, NULL, NULL);

int sw_cast_int = 1;
// char filetmp[] = "/tmp/last.bezier.strokes.txt";
char filetmp[255];
// char filebezier[] = "/tmp/last.bezier.txt";
char filebezier[255];

	sprintf(filetmp, "%s/tmp/last.bezier.strokes.txt", getenv_teseo(TESEO_DATA));
	sprintf(filebezier, "%s/tmp/last.bezier.txt", getenv_teseo(TESEO_DATA));

	//richiedo il path corrente
	gint num_paths=0;
	//g_message("tipo interpolazione %d",pivals.inter_type);
	/*Se esiste una traccia esegue*/
	gimp_path_list (g_image, &num_paths);
	if (num_paths>0) {
		//prendo il nome del path corrente
		strcpy(pathname, gimp_path_get_current(g_image));
		
		gimp_path_get_points (g_image, pathname, &path_closed, &num_elementi_bezier, &punti_bezier);
		fbezier = fopen(filebezier, "wt");
    if(!fbezier) {
      g_message("File \"%s\" not found!", filebezier);
      exit(1);
    }
    for(kk=0; kk<num_elementi_bezier; kk++) {
			fprintf(fbezier, "%f\n", punti_bezier[kk]);
    }
    fclose(fbezier);


		gimp_progress_init("Neuron Teseo - Resampling path . . .");
		gimp_progress_update(0.0);
		
		n_strokes = 0;
		strokes = (double *) malloc(sizeof(double) * ((max_n_strokes+2) * 2) );
    if(!strokes) {
      g_message("Not enough free memory for strokes in on_btnBezier_clicked!");
      exit(1);
    }		
		
    ftmp = fopen(filetmp, "wt");
    if(!ftmp) {
      g_message("File \"%s\" not found!", filetmp);
      exit(1);
    }
		n = 0;
		while((((n+3)*3) +2) < num_elementi_bezier) {
			// punti_bezier[n*3   ] è la x
			// punti_bezier[n*3 +1] è la y
			// punti_bezier[n*3 +2] è il tipo 1 per ANCHOR 2 per CONTROL
			// la sequenza dovrebbe essere 1221
		  printf("%d (%f, %f) n_strokes %ld\n", n, punti_bezier[(n  )*3], punti_bezier[((n  )*3) +1], n_strokes);
			if(			((punti_bezier[ (n   *3) +2]) == 1.0)
					&&	((punti_bezier[((n+1)*3) +2]) == 2.0)
					&&	((punti_bezier[((n+2)*3) +2]) == 2.0)
					&&	((punti_bezier[((n+3)*3) +2]) == 1.0) ) {
						// imposto Px e Py con i valori contenuti in punti_bezier
						Px[0] = punti_bezier[(n  )*3];	Py[0] = punti_bezier[((n  )*3) +1];
						Px[1] = punti_bezier[(n+1)*3];	Py[1] = punti_bezier[((n+1)*3) +1];
						Px[2] = punti_bezier[(n+2)*3];	Py[2] = punti_bezier[((n+2)*3) +1];
						Px[3] = punti_bezier[(n+3)*3];	Py[3] = punti_bezier[((n+3)*3) +1];
						
           	// bezier.setPoints(Px, Py);
		teseo_bezier_point_setPoints(&tbp, Px, Py);
			
           	// mi faccio tornare i punti appartenenti alla curva con passo lungo le x uguale a 1
           	// dopo dovrò ricampionare strokes con passo pivals.passo_bezier

           	// bezier_n_strokes = bezier.getStrokes(1, &bezier_strokes, sw_cast_int);
           	bezier_n_strokes = teseo_bezier_point_getStrokes(&tbp, 1, &bezier_strokes, sw_cast_int);

           	// devo aggiungere bezier_strokes a strokes
           	if((n_strokes + bezier_n_strokes) >= max_n_strokes) {
           		max_n_strokes += 2048;
           		strokes = (double *) realloc((void *) strokes, (sizeof(double) * ((max_n_strokes+2) * 2)));
              if(!strokes) {
                g_message("Not enough free memory for strokes in on_btnBezier_clicked!");
                exit(1);
              }
          	}
          	printf("bezier_n_strokes %d; \n", bezier_n_strokes);
           	for(i=0; i<bezier_n_strokes; i++) {
           		strokes[n_strokes*2   ]   = bezier_strokes[i*2];
           		strokes[(n_strokes*2) +1] = bezier_strokes[(i*2) +1];
           		n_strokes++;
         			if(sw_cast_int) {
         				fprintf(ftmp, "%d %d\n", (int) bezier_strokes[i*2], (int) bezier_strokes[(i*2) +1]);
         			} else {
         				fprintf(ftmp, "%f %f\n", bezier_strokes[i*2], bezier_strokes[(i*2) +1]);
         			}
           		if(n_strokes>=2) {
            		if(strokes[(n_strokes-1)*2] == strokes[(n_strokes-2)*2]) {
            			// printf("Resolution Bug Leo: Duplicazione coordinata in X = %d (Y = %d). Action: Scartata.\n", (int) bezier_strokes[i*2], (int) bezier_strokes[i*2 +1]);
            			n_strokes--;
            		}
           		}
           	}           	
           	if(bezier_strokes)
           		free(bezier_strokes);
           	gimp_progress_update((gdouble) (num_elementi_bezier) / (gdouble) (n*3));
			}
			
		// con questo incremento di n ci sarà qualche ciclo a vuoto,
		// ma facendo così mi sento più tranquillo
		n++;
  	}
  	fclose(ftmp);

		gimp_progress_init("Neuron Teseo - Resampling path finished.");
		gimp_progress_update(0.0);
		
  	// g_message("Fine del calcolo di Bezier.");
  	
  	// if(pivals.sw_campionamento_progressivo == CAMPIONAMENTO_PROG_SI) {
  	if(sw_campionamento_progressivo != 0) {
			campionamento_progressivo_strokes(strokes, &n_strokes);
	}		
  	  	
  	// if(pivals.passo_bezier > 1) {
  	if(passo_bezier > 1) {
  		// sottocampiona_strokes(strokes, &n_strokes, pivals.passo_bezier);
  		sottocampiona_strokes(strokes, &n_strokes, passo_bezier);
  	}

	// scrivo il path calcolato
	if(n_strokes > 0  && strokes) {
		// sprintf(newpathname, "%s - Resampling %d pixel", pathname, pivals.passo_bezier);
		sprintf(newpathname, "%s - Resampling %d pixel", pathname, passo_bezier);
  		strokes_to_open_path(g_image, n_strokes, strokes, newpathname );
  	} else {
  		// g_message("Strano: Il campionamento ha dato un risultato vuoto! sig ?!?");
  	}
  	
  	if(strokes)
  		free(strokes);
  	else {
  		// g_message("Strano! strokes doveva essere diverso da NULL! sig ?!?");
	}
  		
	} else {
		g_message("No selected path!");
	}
}

