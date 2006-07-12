/* Teseo-2 Plug-in
   }
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

#include <glib.h>
#include <glib/gprintf.h>

#include "teseo_path.h"
#include "teseo_bezier_point.h"
#include "teseo_gimp_extends.h"
#include "teseo_bezier_pfit.h"
#include "teseo_io.h"


// Crea nuovi strokes ruotati di un certo angolo espresso in radianti, attenzione, modifica strokes
void teseo_path_rotation(glong num_strokes, gdouble* strokes, gdouble** pstrokes_ruotato, gdouble angolo) {
	// FILE *fstrokes;

  gdouble *strokes_ruotato;
  // gint xl, yl, wl, hl;
  gdouble offx, offy;
  glong i;
  //g_message(" teseo_path_rotation..inizio");

  strokes_ruotato = (gdouble *) g_malloc(sizeof(gdouble) * (num_strokes + 1) * 2);
  strokes_ruotato[num_strokes*2] = (gdouble) CANARY;

  if(!strokes_ruotato) {
    g_message("Not enough free memory for strokes_ruotato!");
    exit(1);
  }
  //Modificato il 03/05/02
  //  calcolagrandezzalayer(num_strokes, strokes, &xl, &yl, &wl, &hl);
  //
  //  offx = xl + (wl / 2);
  //  offy = yl + (hl / 2);
  offx=strokes[0];
  offy=strokes[1];

  //modifico strokes
  for(i=0; i < num_strokes ; i++) {
  	strokes[i*2] -= offx;
  	strokes[i*2 +1] -= offy;
  }


  gdouble cosangolo = cos(angolo);
  gdouble sinangolo = sin(angolo);

  for(i=0; i < num_strokes ; i++) {
  	strokes_ruotato[i*2]    = (cosangolo * strokes[i*2]) - (sinangolo * strokes[i*2+1]);
  	strokes_ruotato[i*2 +1] = (sinangolo * strokes[i*2]) + (cosangolo * strokes[i*2+1]);
  }

	for(i=0; i < num_strokes ; i++) {
  	strokes[i*2] += offx;
  	strokes[i*2 +1] += offy;

  	strokes_ruotato[i*2] += offx;
  	strokes_ruotato[i*2 +1] += offy;
  }

	*pstrokes_ruotato = strokes_ruotato;

  if(strokes_ruotato[num_strokes*2] != (gdouble) CANARY)
  	g_message("Canary in strokes_ruotato is dead!");
 	//g_message(" teseo_path_rotation..fine");
}

#define THREE_CMP(a, b, c)  ((a==b)? ((b==c)? TRUE : FALSE) : FALSE )

path_semantic_type teseo_path_semantic_type(gint32 g_image, gchar *path_name) {
    float delta_pix;
    return teseo_path_semantic_type_even(g_image, path_name, &delta_pix);
}

path_semantic_type teseo_path_semantic_type_even(gint32 g_image, gchar *path_name, float *delta_pix) {
    path_semantic_type ret = PATH_SEMANTIC_POLYLINE;
    gint i, j;
    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;
    gint typeofpath;

    typeofpath = teseo_gimp_path_get_points (g_image, path_name, &path_closed, &num_path_point_details, &points_pairs);

    if( (!typeofpath) && (path_closed != 0) &&  (num_path_point_details < 1) ) {
	ret = PATH_SEMANTIC_MORE_COMPONENTS;
    } else if(num_path_point_details == 0) {
	g_message("Path is empty!");
	ret = PATH_SEMANTIC_BEZIER;
    } else {

	    // Compute first delta_pix
	    if(num_path_point_details >= 18 ) {
		    *delta_pix = points_pairs[9] - points_pairs[3];
		    if(*delta_pix < 0.0) {
			    *delta_pix = 0.0;
		    }
	    } else {
		    *delta_pix = 0.0;
	    }

	    // Compare first two points
	    i=0;
	    j=0;
	    while(j<2) {
		    if( points_pairs[i+j] != points_pairs[i+j+3] ) {
			    ret = PATH_SEMANTIC_BEZIER;
		    }
		    j++;
	    }
	    j = 2;
	    if( points_pairs[i+j] != 1.0 ||
			    points_pairs[i+j+3] != 2.0 ) {
		    g_message("teseo_path_semantic_type(): error in function at line %d.", __LINE__);
	    }

	    // Compare the rest of the path
	    i=6;
	    while (i<num_path_point_details && ret==PATH_SEMANTIC_POLYLINE) {

		    // Update delta_pix
		    if( (points_pairs[i+3] - points_pairs[i-6]) != *delta_pix ) {
			    *delta_pix = 0.0;
		    }

		    // Compare coordinate
		    j=0;
		    while(j<2) {
			    if( THREE_CMP(points_pairs[i+j], points_pairs[i+j+3], points_pairs[i+j+6]) == FALSE ) {
				    ret = PATH_SEMANTIC_BEZIER;
			    }
			    j++;
		    }

		    // Check point type
		    j = 2;
		    if( points_pairs[i+j] != 2.0 ||
				    points_pairs[i+j+3] != 1.0 ||
				    points_pairs[i+j+6] != 2.0 ) {
			    g_message("teseo_path_semantic_type(): error in function at line %d.", __LINE__);
		    }

		    // Next 3 points
		    i+=9;
	    }

	    g_free(points_pairs);

	    // if path is not a polyline then it can't be evenly or unevenly spaced
	    if(ret != PATH_SEMANTIC_POLYLINE) {
		    *delta_pix = 0.0;
	    }
    }
    return ret;
}

/*Funzioni di I/O*/

//traduce un path_array in vettore nel quale non è indicato il tipo ma sono registrati anche i control
gdouble * teseo_path_array_to_strokes( gdouble * path_array, glong n_details, glong * n_strokes){
 glong lstrokes, numstrokes;
 gdouble * strokes=NULL;
 register int i=0;


 //g_message("teseo_path_array_to_strokes ..inizio");
 numstrokes =  ( n_details * 2 )/ 3;
 lstrokes = numstrokes * 2;

 strokes = ( gdouble * ) g_malloc ( sizeof(gdouble) * (lstrokes+1) );
 strokes[lstrokes] = (gdouble) CANARY;

 for(i=0; i < numstrokes; i++){
 	strokes[2*i]   = path_array[ i*3 ];
 	strokes[2*i+1] = path_array[ i*3+1 ];
 }

 if(strokes[lstrokes] != (gdouble) CANARY){
   g_message("Canary in path_array_to strokes is dead!");
 	 g_free(strokes);
 	 strokes=NULL;
 	 *n_strokes=0;
 }
 else {
  *n_strokes=numstrokes;
 }
 //g_message("teseo_path_array_to_strokes ..FINE");
 return strokes;
}

void teseo_strokes_scale(gdouble * strokes_corr, gulong n_points, gdouble x_scale, gdouble y_scale, gboolean mantain_offset)
{
	gulong i;
	gdouble Xorig;
	gdouble Yorig;
	gdouble scale;

	if (!mantain_offset){
		for(i=0; i<n_points;i++){
			//g_printf("\nscaling %f %f ",strokes_corr[2*i],strokes_corr[2*i+1]);
			strokes_corr[2*i]  = (strokes_corr[2*i])    * x_scale;
			strokes_corr[2*i+1]= (strokes_corr[2*i+1] ) * y_scale;
			//g_printf("become %f %f\n",strokes_corr[2*i],strokes_corr[2*i+1]);
		}
	} else {

		scale = (x_scale>y_scale) ? x_scale : y_scale;
		Xorig = scale*strokes_corr[0];
		Yorig = scale*strokes_corr[1];

		for(i=1; i<n_points;i++){
			//g_printf("\nscaling %f %f ",strokes_corr[2*i],strokes_corr[2*i+1]);
			strokes_corr[2*i]  = ( strokes_corr[2*i]   - strokes_corr[0] ) * x_scale + Xorig;
			strokes_corr[2*i+1]= ( strokes_corr[2*i+1] - strokes_corr[1] ) * y_scale + Yorig;
			//g_printf("become %f %f\n",strokes_corr[2*i],strokes_corr[2*i+1]);
		}
		
		strokes_corr[0] = Xorig;
		strokes_corr[1] = Yorig;
	}



	//g_printf("\nnpoints = %d\nscaling in %f %f, first point is %f %f \n",n_points, Xorig, Yorig,strokes_corr[0],strokes_corr[1]);
}

gboolean teseo_strokes_point_pairs(gdouble *strokes, gulong n_points, gdouble** pp, gulong* nppd)
{
	gboolean ret=FALSE;
	*nppd= 9*n_points - 3;
	gdouble *app = NULL;
	app = ( gdouble* ) g_malloc( sizeof(double) * (*nppd) );
	gulong i,j,k;

	if ( app!=NULL ) {
		k=0;//conta i punti
		for(j=0; j<2; j++) {
			app[k*3]=strokes[0];
			app[k*3+1]=strokes[1];
			app[k*3+2]= (j == 0) ? 1.0 : 2.0;
			k++;
		}
		for (i=1; i<n_points ; i++) {
			for(j=0; j<3 ; j++) {
				app[k*3]=strokes[i*2];
				app[k*3+1]=strokes[i*2+1];
				app[k*3+2]= ( (j % 3) == 1) ? 1.0 : 2.0;
				k++;
			}
		}
		*pp=app;
		ret=TRUE;
	}

	return ret;
}


gdouble * teseo_open_path_to_array(gint32 g_image, glong* n_strokes,  char * nome_path){
  glong lstrokes;
  gdouble * points_pairs=NULL;
  gdouble * strokes_ret=NULL;

  gint path_closed, num_path_point_details;

  //ottengo il path
	teseo_gimp_path_get_points (g_image, nome_path, &path_closed, &num_path_point_details, &points_pairs);
  //se il path è corretto ricavo lo strokes
  if (!path_closed){
     //copio il vettore


     lstrokes = num_path_point_details ;
     //oppure togli questa riga e decommenta tutto il resto
     strokes_ret=points_pairs;
//     pstrokes_ret = (gdouble *) g_malloc( sizeof(gdouble) * ( lstrokes + 1 ) );
//     pstrokes_ret[lstrokes] = (gdouble) CANARY;
//
//     if( !pstrokes_ret ) {
//       g_message("Non riesco ad allocare strokes!");
//     }
//     else {
//
//      if( pstrokes_ret[lstrokes] != (gdouble) CANARY ) {
//      	g_message("Hai ucciso il canarino in pstrokes_ret!");
//      }
//      //copio nel pstrokes_ret
//     	for (i=0; i<lstrokes; i++)    {
//     		pstrokes_ret[i]   = points_pairs[i];
//     	}
//
//      if( pstrokes_ret[lstrokes] != (gdouble) CANARY ) {
//      	g_message("Hai ucciso il canarino in pstrokes_ret!");
//        if (pstrokes_ret)
//        	g_free(pstrokes_ret);
//        lstrokes=0;
//  	  }
//  	  else
//  	  {
//  	   strokes_ret=pstrokes_ret;
//  	  }
//     }

  }
  else {
   lstrokes=0;
   strokes_ret=NULL;
  }

  *n_strokes=lstrokes;

  g_free(points_pairs);

  return strokes_ret;
}

/**/

/* Dato uno strokes scrive il path*/
void teseo_strokes_to_open_path(gint32 g_image, glong num_strokes, gdouble *strokes, char * nome_path){

  gdouble * vet_punti=NULL;
  glong lpunti=0, i=0, j=0, k=0, num_vet_punti;

  /* Se il numero di punti è multiplo di 3, gimp chiude automaticamente il PATH*/
  /*??Ridondante*/
  //  if (num_strokes%3)
  //  	{
  //  		num_strokes-=1;
  //  		printf("Tolgo un punto, num_strokes= %d",num_strokes);
  //  	}
  //Attenzione -3 aggiunto  il 14/01/2002
  lpunti = num_strokes * 3 * 3 - 3;
  /*
  g_message("num_strokes vale %d",num_strokes);
  */
  vet_punti = (gdouble *) g_malloc( ( sizeof(gdouble) ) * (lpunti + 1));
  if ( vet_punti != NULL && num_strokes > 2 ){

    vet_punti[lpunti] = (gdouble) CANARY;

    k=0;//conta i punti
   	for(j=0; j<2; j++) {
   		vet_punti[k*3]=strokes[0];
   		vet_punti[k*3+1]=strokes[1];
   		vet_punti[k*3+2]= (j == 0) ? 1.0 : 2.0;
   		k++;
   	}

    for (i=1; i<num_strokes && k<num_strokes*3; i++) {
    	for(j=0; j<3  && k<num_strokes*3; j++) {
    		vet_punti[k*3]=strokes[i*2];
    		vet_punti[k*3+1]=strokes[i*2+1];
    		vet_punti[k*3+2]= ( (j % 3) == 1) ? 1.0 : 2.0;
    		k++;
    	}
    }
    num_vet_punti = k;

    //g_printf("strokes_path: num= %d\n",num_vet_punti);
    //for(i=0;i<num_vet_punti; i++){
    //  g_printf("x=%f  y=%f: type %f\n", vet_punti[i*3], vet_punti[i*3+1], vet_punti[i*3+2]);
    //}


    /*Stampo tutto*/
    //printf("Path: %s", nome_path);

    if( num_vet_punti >  2) { //???
     gimp_path_set_points( g_image, nome_path, 1, num_vet_punti*3, vet_punti ); //gimp bug ??: vuole la dimensione del vettore
     //scrivo sull'immagine, se ho già creato un layer va a finire lì
     //gimp_path_stroke_current (g_image);
     //g_message(gimp_path_get_current(g_image));
    }
    if(vet_punti[lpunti] != (gdouble) CANARY)
    	g_message("Canary in vet_punti is dead!");
  }
  else {
     if(vet_punti!=NULL)
      g_message("Not enough free memory for vet_punti: %ld values", lpunti);
  }
  if (vet_punti)
  	g_free(vet_punti);

// gimp_displays_flush();

}


/* Dato un path ricava lo strokes corrispondente*/
gdouble * teseo_open_path_to_strokes(gint32 g_image, glong* n_strokes,  char * nome_path){
  glong i=0, num_strokes, lstrokes;
  gdouble * points_pairs=NULL;
  gdouble * pstrokes_ret=NULL;
  gint typeofpath;
  gboolean err = FALSE;

  gint path_closed, num_path_point_details;

  //ottengo il path
  typeofpath = teseo_gimp_path_get_points (g_image, nome_path, &path_closed, &num_path_point_details, &points_pairs);
  if( (!typeofpath) && (path_closed != 0) &&  (num_path_point_details < 1) ) {
	  err = TRUE;
	  g_printf("\nerr %d\n", err);
  } 
  //se il path è corretto ricavo lo strokes
  if (!path_closed && !err) {
     //alloco il vettore di strokes
     //+3 perchè per il primo punto scrive sei details, per gli altri nove
     num_strokes=(num_path_point_details+3)/9;

     lstrokes = num_strokes * 2;
     pstrokes_ret = (gdouble *) g_malloc( sizeof(gdouble) * (lstrokes + 1) );
     pstrokes_ret[lstrokes] = (gdouble) CANARY;

     if(!pstrokes_ret) {
       g_message("Not enough free memory for strokes!");
     }
     else {
      //copio nello strokes
      if( pstrokes_ret[lstrokes] != (gdouble) CANARY ) {
      	g_message("Canary in pstrokes_ret is dead!");
      }

     	for (i=0; i<num_strokes; i++)    {
     		pstrokes_ret[2*i]   = points_pairs[i*9];
     		pstrokes_ret[2*i+1] = points_pairs[i*9+1];
     	}
      if( pstrokes_ret[lstrokes] != (gdouble) CANARY ) {
      	g_message("Canary in pstrokes_ret is dead!");
        if (pstrokes_ret)
        	g_free(pstrokes_ret);
  	  }
     }
  } else {
   num_strokes=0;
   pstrokes_ret=NULL;
  }

  *n_strokes=num_strokes;

  g_free(points_pairs);
  
  return pstrokes_ret;
}



/*Funzioni di manipolazione path*/

void teseo_align_all_path(gint32 g_image)
{
    gchar **p;
    gint num_paths, num_points, num_points_tot = 0, path_closed;

    gdouble * path_strokes = NULL;
    gdouble * old_path[2];
    double a,b;

    int	num_point[2];
    int i=0;
    int j=0;
    int t=1;
    int k=0;
    int n_points_tot_eff=0;
    int n_point_eff[2];


    p = gimp_path_list (g_image, &num_paths);

    while( num_paths>1)
    {
	n_points_tot_eff=0;

	for ( i=0; i<2; i++)
	{
	    teseo_gimp_path_get_points (g_image, p[i], &path_closed, &num_points, &old_path[i]);

	    n_points_tot_eff+=num_points;
	    n_point_eff[i]= num_points;

	    num_point[i]= (num_points/3 -2)/3 +1;
	    //g_message("path %d  >>>>>>>  punti %d",t,n_point_eff[t]);
	}

	num_points_tot = num_point[0]	+	num_point[1];

	path_strokes = (gdouble *) g_malloc(sizeof(gdouble) * (n_points_tot_eff+1) );

	if(!path_strokes)
	    g_message(" Not enough free memory for strokes.");


	a=  (old_path[1][n_point_eff[1]-3] - old_path[0][0] );      //scostamento delle x
	b=  -(old_path[1][n_point_eff[1]-2] - old_path[0][1] );		  //scostamento delle y


	for (j=0; j<(n_point_eff[0]);j=j+3)
	{
	    old_path[0][j]+=a;
	    old_path[0][j+1]-=b;
	    //	 	  		old_path[0][j+2]	+=0;
	}

	for (t=1,k=0,j=0;	t>=0;	t--)
	{
	    if(t==0)
		j=3*2;
	    // numero di punti da saltare, per 3 ( x,y,tipo)...
	    //serve per eliminare il primo punto del tracciato che coincide con l'ultimo di quello percedente
	    for ( ; j<n_point_eff[t] && k<n_points_tot_eff; j++, k++)
		path_strokes[k] = old_path[t][j];
	}

	for ( i=0; i<2; i++) {
	    gimp_path_delete( g_image, p[i]);
	    g_free(old_path[i]);
	}

	gimp_path_set_points( g_image, "ALIGNALL", 1, n_point_eff[0]+n_point_eff[1]-6, path_strokes );

	if (path_strokes)
	    g_free(path_strokes);

	gimp_displays_flush();

	p = gimp_path_list (g_image, &num_paths);
    }
}

// TODO update function, don't delete old paths!!!
void teseo_align_all_path_unlocked( gint32 g_image, gboolean delete_path)
{
    gchar **p;
    gint num_paths, num_points, num_points_tot = 0, path_closed, k=0;
    gint first_x, first_y, last_x, last_y, off_x, off_y;

    gdouble * path_strokes = NULL;
    gdouble * old_path;
    int i=0;
    int j=0;

    p = gimp_path_list (g_image, & num_paths);

    // Compute num_points_tot
    for ( i=0; i<=num_paths-1; i++) {
	if(!gimp_path_get_locked(g_image, p[i])) {
	    teseo_gimp_path_get_points (g_image, p[i], &path_closed, &num_points, &old_path);
	    num_points_tot += (num_points+3-9)/3;
	    g_free(old_path);
	}
    }

    if(num_points_tot == 0) {
	g_message("There are no unlocked paths !\nYou can align all unlocked paths.");
    } else {
	// Alloc the right size for path_strokes
	path_strokes = (gdouble *) g_malloc(sizeof(gdouble) * ( num_points_tot*3*3 + 1));

	// Feed path_strokes
	for ( i=num_paths-1; i>=0; i--) {
	    if(!gimp_path_get_locked(g_image, p[i])) {
		teseo_gimp_path_get_points ( g_image, p[i], &path_closed, &num_points, &old_path);

		// Compute first_x and first_y
		first_x = old_path[0];
		first_y = old_path[1];

		// Compute last_x and last_y
		if(k>6) {
		    // Case: insert next points
		    last_x = path_strokes[k-6];
		    last_y = path_strokes[k-5];
		} else {
		    // Case: insert first point
		    for(j=0; j<6; j++, k++) {
		    path_strokes[k] = old_path[j];
		}
		last_x = old_path[0];
		last_y = old_path[1];
		}

		// Compute off_x and off_y
		off_x = -first_x + last_x;
		off_y = -first_y + last_y;

		// Jump first 6 points
		for ( j=6; j<num_points+3-9 && k<num_points_tot*3; j++, k++) {
			if( ((j-6) % 3) == 0 ) {
			    path_strokes[k] = old_path[j] + off_x;
			} else if( ((j-6) % 3) == 1 ) {
			    path_strokes[k] = old_path[j] + off_y;
			} else {
			    path_strokes[k] = old_path[j];
			}
		}

		g_free(old_path);
	    }
	}

	// If specified delete unlocked paths
	if(delete_path) {
	    for ( i=0; i<=num_paths-1; i++) {
		if(!gimp_path_get_locked(g_image, p[i])) {
		    gimp_path_delete( g_image, p[i]);
		}
	    }
	}

	gimp_path_set_points( g_image, "Aligned unlocked path", 1, (num_points_tot-1)*3, path_strokes );

	if (path_strokes)
	    g_free(path_strokes);
    }
}


void teseo_link_all_path(gint32 g_image)
{
    gchar **p;
    gint num_paths, num_points, num_points_tot = 0, path_closed, k=0;

    gdouble * path_strokes = NULL;
    gdouble * old_path;
    int i=0;
    int j=0;

    p = gimp_path_list (g_image, & num_paths);

    for ( i=0; i<=num_paths-1; i++)
    {
	teseo_gimp_path_get_points (g_image, p[i], &path_closed, &num_points, &old_path);
	num_points_tot += (num_points+3-9)/3;
	g_free(old_path);
    }

    path_strokes = (gdouble *) g_malloc(sizeof(gdouble) * ( num_points_tot*3*3 + 1));

    for ( i=num_paths-1; i>=0; i--)
    {
	teseo_gimp_path_get_points ( g_image, p[i], &path_closed, &num_points, &old_path);
	for ( j=0; j<num_points+3-9 && k<num_points_tot*3; j++, k++)
	{
	    path_strokes[k] = old_path[j];
	}
	g_free(old_path);
    }

    for ( i=0; i<=num_paths-1; i++)
	gimp_path_delete( g_image, p[i]);

    gimp_path_set_points( g_image, "LINKALL", 1, (num_points_tot-1)*3, path_strokes );

    if (path_strokes)
	g_free(path_strokes);
}


// TODO update function, don't delete old paths!!!
void teseo_link_all_path_unlocked(gint32 g_image, gboolean delete_path)
{
    gchar **p;
    gint num_paths, num_points, num_points_tot = 0, path_closed, k=0;

    gdouble * path_strokes = NULL;
    gdouble * old_path;
    int i=0;
    int j=0;

    p = gimp_path_list (g_image, & num_paths);

    // Compute num_points_tot
    for ( i=0; i<=num_paths-1; i++) {
	if(!gimp_path_get_locked(g_image, p[i])) {
	    teseo_gimp_path_get_points (g_image, p[i], &path_closed, &num_points, &old_path);
	    num_points_tot += (num_points+3-9)/3;
	    g_free(old_path);
	}
    }

    if(num_points_tot == 0) {
	g_message("There are no unlocked paths !\nYou can link all unlocked paths.");
    } else {
	// Alloc the right size for path_strokes
	path_strokes = (gdouble *) g_malloc(sizeof(gdouble) * ( num_points_tot*3*3 + 1));

	// Feed path_strokes
	for ( i=num_paths-1; i>=0; i--) {
	    if(!gimp_path_get_locked(g_image, p[i])) {
		teseo_gimp_path_get_points ( g_image, p[i], &path_closed, &num_points, &old_path);
		for ( j=0; j<num_points+3-9 && k<num_points_tot*3; j++, k++) {
		    path_strokes[k] = old_path[j];
		}
		g_free(old_path);
	    }
	}

	// If specified delete unlocked paths
	if(delete_path) {
	    for ( i=0; i<=num_paths-1; i++) {
		if(!gimp_path_get_locked(g_image, p[i])) {
		    gimp_path_delete( g_image, p[i]);
		}
	    }
	}

	gimp_path_set_points( g_image, "Linked unlocked path", 1, (num_points_tot-1)*3, path_strokes );

	if (path_strokes)
	    g_free(path_strokes);
    }
}


void teseo_path_move(gint32 g_image, gint x, gint y, gdouble rotate) {
 gdouble * strokes=NULL;
 gdouble *strokes_ruotato=NULL;
 gdouble * path_array=NULL;
 glong n_strokes, n_details;
 gint num_paths=0;
 gchar pathname [80];
 register int i;
 gdouble angolo_rad,PI_GRECO = acos(-1);

 angolo_rad=rotate*PI_GRECO/180.0;

 gimp_path_list (g_image, &num_paths);

 if (num_paths>0){
   //prendo il nome del path corrente
   strcpy(pathname, gimp_path_get_current(g_image));
   //traduco il path in array dello stessso formato di gimp
   path_array = teseo_open_path_to_array(g_image, &n_details,  pathname);

   if ( x!=0 || y!=0 )
   	{
     //modifico le coordinate contenute nell'array
     //il tipo non va toccato
     for (i=0; i< n_details / 3 ; i++) {
      path_array[i*3]+=x;
      path_array[i*3+1]+=y;
      //path_array[i*3+2]+=0;
     }
    }

   //eventuale rotazione
   if ( rotate != 0.0 )
    {
     //se devo ruotare prendo gli strokes e li modifico, poi copio le modifiche nel path_array
     strokes = teseo_path_array_to_strokes( path_array, n_details, &n_strokes);

     teseo_path_rotation( n_strokes, strokes, &strokes_ruotato, -angolo_rad);

     //in strokes_ruotato ho scritto le modifiche da apportare al path_array
     //g_message("modifico il path_array");
     for (i=0; i< n_details / 3 ; i++) {
       path_array[i*3]   = strokes_ruotato[i*2];
       path_array[i*3+1] = strokes_ruotato[i*2+1];
       //path_array[i*3+2]+=0;
     }

     if(strokes)
      g_free(strokes);//non serve più
     if(strokes_ruotato)
      g_free(strokes_ruotato);//non serve più

    }

   gimp_path_delete(g_image,pathname);

   gimp_path_set_points(g_image, pathname, 1, n_details, path_array);

 }
  if(path_array)
   g_free(path_array);
}


void teseo_path_flip(gint32 g_image) {
 gdouble * path_src=NULL;
 gdouble * path_dest=NULL;
 glong n_strokes, n_details, array_size ;
 gint num_paths=0;
 gchar pathname [PATHNAMELEN];
 register int i;
 

 gimp_path_list (g_image, &num_paths);

 if (num_paths>0){
   //prendo il nome del path corrente
   strcpy(pathname, gimp_path_get_current(g_image));
   
   if (teseo_path_semantic_type(gint32 g_image, gchar *path_name) != PATH_SEMANTIC_MORE_COMPONENTS){
    //g_message(pathname);
    //traduco il path in array dello stesso formato di gimp
    path_src = teseo_open_path_to_array(g_image, &n_details,  pathname);
    
    if (path_src != NULL) {
      array_size=n_details;
      path_dest = (gdouble *) g_malloc( sizeof(gdouble)*array_size);
  
      //change elements
      for (i=0; i<=(array_size-6)/3; i++) {
        path_dest[i*3]   = path_src[ array_size - (i+2)*3    ];
        path_dest[i*3+1] = path_src[ array_size - (i+2)*3 +1 ];
        path_dest[i*3+2] = path_src[ array_size - (i+2)*3 +2 ];
      }
      
      //copy the last anchor x, y ,t in the last control
      path_dest[i*3]   = path_dest[(i-1)*3];
      path_dest[i*3+1] = path_dest[(i-1)*3+1];
      path_dest[i*3+2] = 2.0; //BEZIER_CONTROL
  
      strcat(pathname," flip");
      gimp_path_set_points(g_image, pathname, 1, array_size, path_dest);
      //gimp_path_delete(g_image,pathname);
      //g_message("Canceled");
  
    }
  }
  
  g_free(path_dest);
 }
 else {
  g_message("teseo_path_flip(): %s can not be treated!", pathname);
 }  

}




//Modifica al 11/01/2002 Prevede che dall'esterno lo strokes passat sia consecutivo al path chiamante
//Non si controlla all'interno che il path e lo strokes non siano sovrapposti
/* Ricavo il vecchio path, vi concateno lo strokes*/
void teseo_cat_path_strokes(gint32 g_image, glong num_strokes, gdouble *strokes){

    gdouble * old_path;
    gdouble * path_strokes=NULL;

    gint32 path_type, path_closed, num_path_point_details;
    char path_name [80] ;

    glong num_points;
    glong num_points_details=0, i=0, j=0, k=0, num_vet_punti;
    /*Ricavo il vecchio path*/

    //printf("\nPrima \n");
    strcpy(path_name,gimp_path_get_current(g_image));
    path_type = teseo_gimp_path_get_points( g_image, path_name, &path_closed, &num_path_point_details,  &old_path );
    //g_message("Nome del path %s", path_name);
    //g_printf("numero di point_details: %d  path_type %d, path_closed %d\n",num_path_point_details, path_type, path_closed);

    if ( !path_closed && num_path_point_details != 1 && path_type == 1) {
	//va tolto l'ultimo punto (-9) : correggere vanno presi tutti i punti

	num_points = ( num_path_point_details + 3 ) / 9 + num_strokes; //diviso 3 ???
	num_points_details = num_points * 9 - 3; //chiuso....


	// printf("num_points_details=num_points * 3 * 3 -3=%d",num_points_details);
	path_strokes = (gdouble *) g_malloc(sizeof(gdouble) * (num_points_details + 1));

	if(!path_strokes) {
	    g_message("Not enough free memory for path_strokes!");
	}
	else {
	    // printf("Allocato path_strokes: %d elementi\n", num_points_details);
	    path_strokes[num_points_details] = (gdouble) CANARY;

	    /* Copio in path_strokes, il path e lo strokes*/

	    //qui dentro copio tutto , da fuori gli passo sempre lo strokes corretto
	    for( i=0; i < num_path_point_details; i++){
		//path_strokes[i] = round(old_path[i]); //TODO Eliminare
                path_strokes[i] = old_path[i]; //TODO Eliminare
	    }
	    k=i/3;    //poi riparto a scrivere nel vettore da k*3
	    // printf("\nk= %d; i= %d\n", k,i);


	    //Copio lo strokes
	    //parto dall'inizio; è il chiamante a garantire che strokes non ripeta i punti del vecchio path
	    for (i=0; i<num_strokes; i++) {
		for(j=0; j<3 ; j++) {
		path_strokes[k*3]=strokes[i*2];
		path_strokes[k*3+1]=strokes[i*2+1];
		path_strokes[k*3+2] =  ( (j % 3) == 1) ? 1.0 : 2.0;
		k++;
		}
	    }

	    num_vet_punti = k;

	    //g_printf("cat: num= %d\n",num_vet_punti );
	    //for(i=0;i<num_vet_punti; i++){
	    //g_printf("x=%f  y=%f: type %f\n", path_strokes[i*3], path_strokes[i*3+1],path_strokes[i*3+2]);
	    //}

	    if ( num_vet_punti*3 != num_points_details)
		g_printf("num_vet_punti*3 - 3 = %ld num_points_details = %ld",num_vet_punti*3, num_points_details);

	    if( num_vet_punti >  2) { //??
		//gimp_path_delete(g_image,path_name); //gimp bug ? if you delete before then is not visible
		gimp_path_set_points( g_image, path_name, 1, num_points_details , path_strokes ); //gimp bug ??: vuole la dimensione del vettore
		//beware i'm deleting old path with path_name
		gimp_path_delete(g_image,path_name);
	    }
	}

	if(path_strokes[num_points_details] != (gdouble) CANARY)
	    g_message("Canary in path_strokes is dead!");
    }

    g_free(old_path);

    if (path_strokes)
	g_free(path_strokes);

}


void teseo_path_force_polyline(gint32 g_image) {
    GString *path_name_new = NULL;
    gint i;
    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;
    gint typeofpath;

    typeofpath = teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs);

    if( (!typeofpath) && (path_closed != 0) &&  (num_path_point_details < 1) ) {
	    // do nothing
    } else if(num_path_point_details == 0) {
	    g_message("Path is empty!");
    } else {

	    points_pairs[3] = points_pairs[0];
	    points_pairs[4] = points_pairs[1];

	    for(i=6; i <= num_path_point_details-9; i+=9) {
		    points_pairs[i] = points_pairs[i+3];
		    points_pairs[i+1] = points_pairs[i+4];

		    points_pairs[i+6] = points_pairs[i+3];
		    points_pairs[i+7] = points_pairs[i+4];
	    }

	    path_name_new = g_string_new("Uname");
	    g_string_printf(path_name_new, "%s_P", gimp_path_get_current(g_image));
	    gimp_path_set_points(g_image, path_name_new->str, 1, num_path_point_details, points_pairs);

	    g_string_free(path_name_new, TRUE);
	    g_free(points_pairs);
    }
}

void teseo_path_add_points_pairs(gint32 g_image, gint new_num_path_point_details, gdouble *new_points_pairs) {
    GString *path_name = NULL;
    gint i;
    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;
    gint new_size;

    // get current path name
    path_name = g_string_new(gimp_path_get_current(g_image));

    // get points_pairs from current path
    teseo_gimp_path_get_points (g_image, path_name->str, &path_closed, &num_path_point_details, &points_pairs);

    // Check if path is empty
    if(num_path_point_details == 0) {
	g_message("Path is empty!");
    }

    // compute new_size
    new_size = (num_path_point_details + new_num_path_point_details + 3);
    points_pairs = g_realloc(points_pairs,  new_size * sizeof(gdouble));

    if(points_pairs) {

        // add a control point same to the first point of the new_points_pairs
        for(i=0; i < 2; i++) {
            points_pairs[num_path_point_details + i] = new_points_pairs[i];
        }
        points_pairs[num_path_point_details + 2] = 2.0;

        // copy new_points_pairs in points_pairs
        for(i=0; i < new_num_path_point_details; i++) {
            points_pairs[num_path_point_details + 3 + i] = new_points_pairs[i];
        }

        // create a new path
        gimp_path_set_points(g_image, path_name->str, 1, new_size, points_pairs);
        gimp_path_delete(g_image, path_name->str);

        g_free(points_pairs);

    } else {
        g_message("teseo_path_add_points_pairs(): g_realloc failed!\n");
    }

    g_string_free(path_name, TRUE);
}


void teseo_path_split_at_xs(gint32 g_image, gint32 *guides, gint32 n_guides) {
    gint *vi = g_malloc((n_guides+2) * sizeof(gint) );
    gdouble *vt = g_malloc((n_guides+2) * sizeof(gdouble) );
    gint i_vi = 0;
    gboolean need_split;
    gboolean started;
    gint ii;

    int i_guides=0;
    gdouble *points_pairs=NULL;
    gint path_closed, num_path_point_details;

    gint j_vi = 0;
    GString *path_name_new = NULL;
    GString *path_name_source = NULL;
    gint num_path_point_details_new;
    gdouble *points_pairs_new=NULL;

    gint i_num_path_point_details_new_alloc;
    gint num_path_point_details_new_alloc;
    gdouble *points_pairs_new_alloc=NULL;

    gdouble *new_points_pairs_split = NULL;
    gdouble t = 0.5;
    gint k;

    struct teseo_bezier_point tbp;
    gdouble app_t;

    teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs);

    if(points_pairs) {

        teseo_bezier_point_init(&tbp, 3, NULL, NULL);
        i_vi=0;
        vi[i_vi++]=0;
        vt[i_vi-1]=0.0;
        for(i_guides=0; i_guides<n_guides; i_guides++) {
            g_printf("\n* * *\ni_guides %d, guides[i_guides] %d\n", i_guides, guides[i_guides]);

            // Find first occurrence greater than n_guides[i_guides], take it in vi at index i_vi
            started = FALSE;
            need_split = FALSE;
            ii = 0;
            while( !( ii>=num_path_point_details || need_split) ) {
                if(points_pairs[ii] < guides[i_guides]) {
                    started = TRUE;
                }
                if(points_pairs[ii] >= guides[i_guides]
                        && started) {
                    if(vi[i_vi-1] != ii) {
                        need_split = TRUE;
                        vi[i_vi++]=ii;
                        teseo_bezier_point_setPoints_points_pairs(&tbp, &(points_pairs[ii - 9]));
                        if(teseo_bezier_point_get_t_from_x(&tbp, guides[i_guides], &app_t)) {
                            vt[i_vi-1] = app_t;
                        } else {
                            vt[i_vi-1] = 0.5;
                        }
                        g_printf("add index %d in vi[%d]\n", ii, i_vi-1);
                        g_printf("add index %f in vt[%d]\n", vt[i_vi-1], i_vi-1);
                    } else {
                        g_message("Teseo2-Warning: there are more than one guide in a single bezier.\nVertical guide at x=%d discarded.", guides[i_guides]);
                        ii = num_path_point_details+1;
                    }
                } else {
                    ii+=9;
                }
            }

        }
        vi[i_vi++]=num_path_point_details;
        teseo_bezier_point_free(&tbp);

        // vi should contain (0, ..., ..., num_path_point_details)
        // vi contains at least 2 items (0, num_path_point_details)

        g_printf("i_vi = %d\n", i_vi);

        path_name_source = g_string_new("Uname");
        g_string_printf(path_name_source, "%s", gimp_path_get_current(g_image));
        
        // Split array at vi[] positions
        if(i_vi > 2) {
            path_name_new = g_string_new("Uname");

            for(j_vi=1; j_vi<i_vi; j_vi++) {
                g_printf("j_vi %d vi[j_vi] %d\n", j_vi,  vi[j_vi]);

                g_string_printf(path_name_new, "%s_S%d-%d", path_name_source->str, j_vi, i_vi-1);

                points_pairs_new = points_pairs + vi[j_vi-1];
                /*
                   g_printf("%f, %f, %f\n",points_pairs_new[0], points_pairs_new[1],  points_pairs_new[2]);
                   g_printf("%f, %f, %f\n",points_pairs_new[3], points_pairs_new[4],  points_pairs_new[5]);
                   g_printf("%f, %f, %f\n",points_pairs_new[6], points_pairs_new[7],  points_pairs_new[8]);
                 */

                num_path_point_details_new = vi[j_vi] - vi[j_vi-1] - ( (j_vi==1)? 1 : 0 );
                if( (j_vi > 1)  &&  (j_vi < i_vi-1) ) {
                    num_path_point_details_new -= 3;
                }

                // g_printf("num_path_point_details_new %d\n", num_path_point_details_new);
                num_path_point_details_new_alloc = num_path_point_details_new + 9;
                if( (j_vi > 1)  &&  (j_vi < i_vi-1) ) {
                    num_path_point_details_new_alloc += 9;
                }
                points_pairs_new_alloc = (gdouble *) g_malloc(sizeof(gdouble) * num_path_point_details_new_alloc);
                i_num_path_point_details_new_alloc = 0;

                // building points_pairs_new_alloc ...
                if(j_vi > 1) {
                    // Use new_points_pairs_split built in the previous cycle
                    for(k=9; k<18; k++) {
                        points_pairs_new_alloc[i_num_path_point_details_new_alloc++] = new_points_pairs_split[k];
                    }
                }

                for(k=0; k<num_path_point_details_new; k++) {
                    points_pairs_new_alloc[i_num_path_point_details_new_alloc++] = points_pairs_new[k];
                }

                if(j_vi < i_vi-1) {
                    g_free(new_points_pairs_split);
                    t = (vt[j_vi] >= 0.0  &&  vt[j_vi] <= 1.0)? vt[j_vi] : 0.5;
                    g_printf("vt[%d] %f - t = %f\n", j_vi, vt[j_vi], t);
                    teseo_bezier_point_split_points_pairs(points_pairs_new + num_path_point_details_new - 6 - ( (j_vi==1)? 2 : 0 ), &new_points_pairs_split, t);

                    // I have to override last control point
                    if(j_vi == 1) {
                        i_num_path_point_details_new_alloc -= 5;
                    } else {
                        i_num_path_point_details_new_alloc -= 3;
                    }

                    for(k=3; k<15; k++) {
                        points_pairs_new_alloc[i_num_path_point_details_new_alloc++] = new_points_pairs_split[k];
                    }
                }

                if(i_num_path_point_details_new_alloc == num_path_point_details_new_alloc ||
                        (j_vi == 1  &&  i_num_path_point_details_new_alloc == num_path_point_details_new_alloc-2) ) {
                    gimp_path_set_points(g_image, path_name_new->str, 1, num_path_point_details_new_alloc, points_pairs_new_alloc);
                } else {
                    g_printf("i_num_path_point_details_new_alloc != num_path_point_details_new_alloc,  %d != %d\n", i_num_path_point_details_new_alloc, num_path_point_details_new_alloc);
                }

                if( (j_vi < i_vi-1) ) {
                    // set points only for debugging
                    // gimp_path_set_points(g_image, path_name_new->str, 1, 24, new_points_pairs_split);
                }

                g_free(points_pairs_new_alloc);


            }

            g_string_free(path_name_new, TRUE);

        }

        g_string_free(path_name_source, TRUE);
        g_free(points_pairs);
    }

    g_free(vi);
    g_free(vt);
}


void teseo_path_import_combining_more_components(gint32 image_ID, const gchar *filename, gboolean merge, gboolean scale) {
#define SIZELINE 1024
#define  POS_C  11
#define  POS_M  60
	FILE *f;
	FILE *fout;
	gchar line[SIZELINE];
	const gchar firstline[] = "           C";
	gboolean after_first_line = FALSE;
	gchar *filenameout = g_strconcat(filename, ".teseo2.tmp");
	

	f = fopen(filename, "rt");
	if(f) {
		fout = fopen(filenameout, "wt");
		if(fout) {
			while(fgets (line, SIZELINE,  f)) {
				if(after_first_line) {
					if( g_strncasecmp(line, firstline, POS_C + 1) ) {
						line[POS_C] = ' ';
					}
					if(line[POS_M] == 'M') {
						line[POS_M] = 0;
					}
				} else {
					if( g_strncasecmp(line, firstline, POS_C + 1) ) {
						after_first_line = TRUE;
						if(line[POS_M] == 'M') {
							line[POS_M] = 0;
						}
					}
				}
				g_fprintf(fout, "%s\n", line);
			}
			fclose(fout);
			teseo_import_svg_vectors(image_ID, filenameout );
		} else {
			g_message("teseo_path_import_combining_more_components(): %s can not be created!", filenameout);
		}
		fclose(f);
	} else {
		g_message("teseo_path_import_combining_more_components(): %s does not exist!", filename);
	}
	g_free(filenameout);
}

