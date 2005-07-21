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

#include "teseo_wmean.h"
#include "teseo_path.h"

static wmeanParams this_params = { 0, 1 , 10, 10};

void init_wmean( wmeanParams* s){
  this_params.colour = (*s).colour;
  this_params.step   = (*s).step;
  this_params.width  = (*s).width;
  this_params.height  = (*s).height;
}


int wmean( const wm_is * is, wm_os * os ){
  int ret =1;

  int x_c=(int)(this_params.width/2);
  int x_s;
  float colour;
  double sum_num, sum_den;
  double coord;
  double weight, dist;
  int Yabs, Yrel, Xabs, Xrel;
  int YY;
  int t, tmean;
  int MAX_STEP_Y = this_params.width;

  int COLOUR  = this_params.colour;
  int step_x  = this_params.step;
  int width   = this_params.width;
  int height  = this_params.height;

  //(*os).x=0.0;
  //(*os).y=0.0;

  // inizialmente mi sposto verso destra sulle x
  // se poi la Yrel ? maggiore di un certo valore MAX_STEP_Y allora faccio la media anche per le x
  x_s = x_c + 1;
  sum_num = 0.0;
  sum_den = 0.0;
  for(t=0; t < height; t++) {

	coord = t + 1;
	coord /= (double) height;
	colour = (*is).bufin[(t*width) + x_s];
	weight = (double) ( (COLOUR >= 128)? ( colour ) : (255.0 - colour) );
	weight /= 255.0;
	weight *= weight;
	weight *= weight;

	dist = fabs(t - (height/2));
	if(dist!=0.0) {
		dist = sqrt(dist);
			dist = sqrt(dist);
			weight /= ( (dist) / (double) (height/2));
		} else {
			weight = 0.0;
		}

		sum_num += ( weight * (double) coord );
		sum_den += weight;
	}

	tmean = (int) ((sum_num / sum_den) + 0.5);

	tmean = (int) (((sum_num / sum_den) * (double) height) + 0.5);

	Yabs = ( (*is).LastCentre.y - (height/2) + (tmean - 1));

	Yrel = (Yabs - (*is).LastCentre.y);

	Xrel = step_x;

	if(abs(Yrel) > MAX_STEP_Y) {
		YY = (Yabs - ((*is).LastCentre.y - (height/2)));
		if(YY < 0) {
			//g_message("YY < 0");
			ret=0;
		}
		sum_num = 0.0;
		sum_den = 0.0;
		for(t=0; t < width; t++) {
			coord = t + 1;
			coord /= (double) width;

			colour = (*is).bufin[(YY*width) + t];
			weight = (double) ( (COLOUR >= 128)? ( colour ) : (255.0 - colour) );
			weight /= 255.0;
			weight *= weight;
			weight *= weight;

			dist = fabs(t - (width/2));

			if(dist!=0.0) {
				dist = sqrt(dist);
				dist = sqrt(dist);
				weight /= ((dist) / (double) (width/2));
			} else {
				weight = 0.0;
			}

			sum_num += ( weight * (double) coord );
			sum_den += weight;
		}

		tmean = (int) ((sum_num / sum_den) + 0.5);

		tmean = (int) (((sum_num / sum_den) * (double) width) + 0.5);

		Xabs = ((*is).LastCentre.x - (width/2) + (tmean - 1));

		Xrel = (Xabs - (*is).LastCentre.x);
		// printf("\nXrel: %d, Yrel: %d\n", Xrel, Yrel);
	} else {
		//SP Xrel = 1;
		Xrel = step_x;
	}
	//(*os).x= (double) Xrel;
	//(*os).y= (double) Yrel;

	(*os).x += (double) Xrel;
	(*os).y += (double) Yrel;

    return ret;
}


/*
struct WM_I{
 float * bufin;
 point LastCentre;
 int width;
 int height;
};

*/


int wmean_getinput( wm_is * is, const wm_os * previous_os, gint32 drawable_ID){
  int ret=0;
  int width   = this_params.width;
  int height  = this_params.height;
  int x, y;
  int bpp = 0;
  glong bufsize;

  guchar *bufin=NULL;
  GimpDrawable *drawable=NULL;
  GimpPixelRgn pr;

  //g_message("wmean_getinput");

  drawable=gimp_drawable_get(drawable_ID);
  bpp = drawable->bpp;
  bufsize=bpp*width*height;

  bufin = (guchar*) g_malloc( (sizeof(guchar)) * bufsize);
  //g_message("wmean_getinput bpp %d bufsize %d", (int) bpp, (int) bufsize);

  if ( (drawable != NULL) && ( bufin != NULL) ) {
    //g_message("wmean_getinput points %d %d",(*previous_os).x ,(*previous_os).y);
    //top left corner
    x = (*previous_os).x-width/2;
    y = (*previous_os).y-height/2;
    //g_message("wmean_getinput corner %d %d ", x,y);
    //get bufin
    gimp_pixel_rgn_init(&pr, drawable, x, y, width, height, FALSE, FALSE);
    gimp_pixel_rgn_get_rect (&pr, bufin, x, y, width, height);

    //g_message("wmean_getinput corner  %d %d : first point value %d ", x, y , bufin[0]);

    (*is).bufin=bufin;
    ((*is).LastCentre).x=(*previous_os).x;
    ((*is).LastCentre).y=(*previous_os).y;

    ret=1;

  }
  else
    g_message("NULL pointers");
  return ret;
}

int wmean_getouput(wm_os * os ){
  int ret=1;
  return ret;
}

int wmean_terminate(wm_os * os, wm_is * is, gint32 drawable_ID){
  int ret=0;
  //TODO control bounds of drawable and stop before
  //(*os).x;
  return ret;
}

int wmean_accumulate( gdouble ** strokes, gulong * num_strokes, wm_os * os){
  int ret=0;
  gdouble * ptr = NULL;
  gulong dim;
  //g_message("strokes %p ptr %p",strokes, ptr);

  dim =(*num_strokes)*2 + 2 ;

  if ( (*strokes) == NULL) {
    //g_message("First time, isn't it?" );

    ptr = (gdouble *) malloc(  sizeof(gdouble) * dim  );
    if (ptr==NULL) g_message("Te credo");
    //(*strokes)=ptr;
  }
  else {
  ptr = (gdouble *) realloc( (*strokes), ( sizeof(gdouble)) * dim );
  }
  //g_message("*strokes %p ptr %p, dim %lu",(*strokes), ptr,dim);
  if ( ptr != NULL) {
    /*g_message("Prima\nos.x = %d os.y = %d\nAdded %f, %f, num_strokes= %lu now",
       (*os).x,
       (*os).y,
       *(ptr+(*num_strokes)*2) ,
       *(ptr+(*num_strokes)*2+1),
       (*num_strokes));*/
    *(ptr+dim-2) = (double) ((*os).x);
    *(ptr+dim-1) = (double) ((*os).y);
    //debug printf("x %f , y %f ... Added %f %f\n", (double) ((*os).x),(double) ((*os).y), *(ptr+dim-2), *(ptr+dim-1));
    //num_strokes is vector dimension/2
    (*num_strokes)+=1;
    ret=1;
    //g_message("Dopo\nos.x = %d os.y = %d\nAdded %f, %d, num_strokes= %lu now",
    //(*os).x,
    //(*os).y,
    //*(ptr+(*num_strokes)*2),
    //*(ptr+(*num_strokes)*2+1),
    // (*num_strokes));
  }
  (*strokes)=ptr;
  //g_message("*strokes %p ptr %p, dim %lu, num_strokes %lu" ,(*strokes), ptr, dim, (*num_strokes) );
return ret;
}

int wmean_starting_os(wm_os ** os, gint32 drawable_ID ){
  int ret=0;
  gchar pathname[PATHNAMELEN];
  gdouble * strokes=NULL;
  glong n_strokes;
  gint32 image;
  wm_os * r_os=NULL;

  r_os = (wm_os *) g_malloc(sizeof(wm_os));

  if (r_os!=NULL) {
    //g_message("wmean_starting_os");
    image=gimp_drawable_get_image( drawable_ID );
    //get the pathname
    strcpy(pathname, gimp_path_get_current ( image ) );
    //get the strokes
    strokes = open_path_to_strokes(image, &n_strokes,  pathname);
    if (strokes!=NULL) {
      (*r_os).x=strokes[(n_strokes-1)*2];
      (*r_os).y=strokes[(n_strokes-1)*2+1];
      g_free(strokes);
      ret=1;
      *os=r_os;
      //g_message("wmean_starting_os starting values %d %d", (*r_os).x,(*r_os).y);
    }
  }
  else g_message("wmean_starting_os NULL pointers");

  return ret;
}


int wmean_new_is( wm_is ** is){
  int ret=0;
  wm_is * r_is=NULL;
  r_is = (wm_is *) g_malloc(sizeof(wm_is));
  if (r_is != NULL) {
    *is=r_is;
    ret=1;
  }
  return ret;
}


int wmean_release(wm_is ** is, wm_os ** os){
  int ret=0;
  if ( *is!=NULL ) g_free(is);
  if ( *os!=NULL ) g_free(os);
  return ret;
}



/*
void neuronSismos_eseguimanuale(gint32 g_image, GDrawable *drawable, glong LastXOld, glong LastYOld, glong LastXPrec, glong LastYPrec, glong LastXCentro, glong LastYCentro, glong *pnum_strokes, gdouble **pstrokes, gint n_passi)
{
  Tpunto LastSucc;

  Tpunto OldPrec    = {LastXOld, LastYOld} ;
  Tpunto LastPrec   = {LastXPrec, LastYPrec} ;
  Tpunto LastCentro = {LastXCentro, LastYCentro} ;

  GPixelRgn srcPR;
  guchar *dest;
  unsigned long len_dest;
  gint width, height, bpp;
  gint x1, y1, x2, y2;
  int indi;
  // valori di ritorno
  glong num_strokes, cur_stroke, mod_progress;
  gdouble *strokes;

  // variabili di appoggio nell'utilizzo della rete
  float *bufin, *bufout;
	gboolean escidalciclo = 0;

  glong X_limit;
  float appx, appy;
  int xr, yr;
  float valpasso;

  gint net_width, net_height;
  char s_tmp_app[255];

  // variabile di appoggio per directory di lavoro
  char dir_gimp[80];
  gulong lbufin, lbufout, lstrokes, ldest;

  if ( n_passi==0 ) n_passi=INT_MAX;

  // memorizzo vecchia directory di lavoro di GIMP
  getcwd(dir_gimp,80);

  sprintf(s_tmp_app, "%s/tmp", getenv_teseo(TESEO_DATA));
  chdir(s_tmp_app);

  //  Get the input area. This is the bounding box of the selection in
  //  the image (or the entire image if there is no selection). Only
  //  operating on the input area is simply an optimization. It doesn't
  //  need to be done for correct operation. (It simply makes it go
  //  faster, since fewer pixels need to be operated on).
   //TODO be sure to get only a small rectangle
  gimp_drawable_mask_bounds(drawable->id, &x1, &y1, &x2, &y2);

  X_limit = x2;//il più a destra

  num_strokes = n_passi;//pu0 essere presso pari al numero di passi da fare NOOOO????
  lstrokes = num_strokes * 2;
  strokes = (gdouble *) malloc(sizeof(gdouble) * (lstrokes + 1));
  strokes[lstrokes] = (gdouble) CANARY;

  if(!strokes) {
    g_message("Not enough free memory for strokes!");
    exit(1);
  }

  mod_progress = n_passi / 50;
  if(mod_progress < 1) mod_progress = 1;


  //  Get the size of the input image. (This will/must be the same
   *  as the size of the output image.
  width = drawable->width;
  height = drawable->height;
  bpp = drawable->bpp;

	if(glob.ins_tipo==COLORE_1)
		{
			net_width 	= ins_manual.colonne;
		  net_height 	= ins_manual.righe;
		}
	else if(glob.ins_tipo==COLORE_2)
			{
				net_width 	= ins_snap.colonne;
			  net_height 	= ins_snap.righe;
				//printf("net_width %d; net_height: %d\n", net_width, net_height);
			}
	else if(glob.ins_tipo==COLORE_3)
			{
				net_width 	= ins_snap.colonne;
			  net_height 	= ins_snap.righe;
				//printf("net_width %d; net_height: %d\n", net_width, net_height);
			}


  // alloco buffer del drawable
  len_dest = (net_width * net_height) * bpp;

  ldest = len_dest;
  dest = (guchar *) malloc(ldest + 1);
  dest[ldest] = (guchar) CANARY;

  if(!dest)
  	{
    	g_message("Not enough free memory for dest!");
    	exit(1);
  	}

  // alloco buffer della rete
  lbufin = (net_width * net_height);
  bufin = (float *) malloc(sizeof(float) * (lbufin + 1));
  bufin[lbufin] = (float) CANARY;

  if(!bufin)
  	{
    	g_message("Not enough free memory for bufin!");
    	exit(1);
  	}
  lbufout =  2;
  bufout = (float *) malloc(sizeof(float) * (lbufout + 1));
  bufout[lbufout] = (float) CANARY;
  if(!bufout)
  	{
   	 	g_message("Not enough free memory for bufout!");
    	exit(1);
  	}

  // inizializzo pixel region
  gimp_pixel_rgn_init(&srcPR, drawable, 0, 0, width, height, FALSE, FALSE);

  // inizializzo la finestra con la barra progressiva
  gimp_progress_init("Neuron Teseo - Studio i punti . . .");
  gimp_progress_update(0.0);

  // utilizzando l'inseguimento per colore per il calcolo i punti seguenti
  cur_stroke = 0;
  while( LastCentro.x >0 && LastCentro.x > x1 - net_width
	 && LastCentro.x < X_limit
	 && LastCentro.y < y2 && LastCentro.y > y1
	 && cur_stroke < num_strokes
   && cur_stroke <= n_passi
	 && !escidalciclo ) {

    // calcolo coordinate del punto in alto a sinistra della rete
    // rispetto al centro
    xr = LastCentro.x - (net_width/2);
    yr = LastCentro.y - (net_height/2);

    // verifico che la rete sia compresa nell'immagine, questa verifica va tolta e va imposta alla selezione
    if(xr >=0 && xr <= width &&
       yr >=0 && yr <= height &&
       (xr+net_width) >= 0 && (xr+net_width)  < width &&
       (yr+net_height) >= 0 && (yr+net_height) < height) {

      neuron_gimp_pixel_rgn_get_rect(&srcPR,dest,xr,yr,net_width,net_height,width,height,len_dest);

      // Scrivo l'input in bufin
      for(int kx = 0; kx < net_width; kx++)
      	for(int ky = 0; ky < net_height; ky++)
					bufin[(ky * net_width) + kx] = (float) (dest[(ky * net_width) + kx]);


      // Eseguo l'inseguimento per colore
		if(glob.ins_tipo==COLORE_1)
			{
	      inseguimento_colore(bufin, bufout, LastPrec.x, LastPrec.y, LastCentro.x,LastCentro.y,net_width,net_height);
			}
			else if(glob.ins_tipo==COLORE_2)
				{
	  	    inseguimento_colore_2(bufin, bufout, LastPrec.x, LastPrec.y, LastCentro.x,LastCentro.y,net_width,net_height);
				}
			else if(glob.ins_tipo==COLORE_3)
				{
	  	    inseguimento_colore_3(bufin, bufout, LastPrec.x, LastPrec.y, LastCentro.x,LastCentro.y,net_width,net_height);
				}

//      inseguimento_colore(bufin, bufout, LastPrec.x, LastPrec.y, LastCentro.x,LastCentro.y,net_width,net_height);

      if(bufout[0]==0.0 && bufout[1]==0.0)
      	{
      	 printf("Lost trace.\n");
      	}

      appx = bufout[0];
      appy = bufout[1];

  	   LastSucc.x = (LastCentro.x + (int) appx);
	     LastSucc.y = (LastCentro.y + (int) appy);

    } else {
      // altrimenti ritorno lo stesso punto
      //       LastSucc.x = LastCentro.x;
      //       LastSucc.y = LastCentro.y;
      //       num_strokes = cur_stroke;
    }

    // se non mi muovo con la rete mi fermo
    if (LastCentro.x==LastPrec.x && LastCentro.y==LastPrec.y &&
			LastCentro.x==LastSucc.x && LastCentro.y==LastSucc.y)
      escidalciclo = 1;

    // Se cado in un cappio molto piccolo mi fermo
    if (LastSucc.x==LastPrec.x && LastSucc.y==LastPrec.y)
      escidalciclo=1;

    // aggiungo le coordinate del nuovo punto trovato
    strokes[(cur_stroke*2)] = (gdouble) LastSucc.x;
    strokes[(cur_stroke*2)+1] = (gdouble) LastSucc.y;
    cur_stroke++;

    // inizializzo le variabile per la nuova iterazione
    OldPrec.x =  LastPrec.x;
    OldPrec.y =  LastPrec.y;
    LastPrec.x = LastCentro.x;
    LastPrec.y = LastCentro.y;
    LastCentro.x = LastSucc.x;
    LastCentro.y = LastSucc.y;

    // incremento la barra progressiva
    if (((LastPrec.x - x1) % mod_progress) == 0) {
      gimp_progress_update((double) (LastPrec.x - x1) / (double) (x2 - x1));
    }
  }

  gimp_progress_update(1.0);

  num_strokes = cur_stroke - 1; // è andato avanti di uno nel ciclo

  // scrivo su file, a solo scopo di debug, gli strokes trovati ????????

  sprintf(s_tmp_app, "%s/tmp/laststrokes.txt", getenv_teseo(TESEO_DATA));
  strokes_txt(s_tmp_app, strokes, num_strokes);


  if(strokes[lstrokes] != (gdouble) CANARY)
  	g_message("Canary in strokes is dead!");
  if(dest[ldest] != (guchar) CANARY)
  	g_message("Canary in dest EseguiRete is dead!");
  if(bufin[lbufin] != CANARY)
  	g_message("Canary in bufin is dead!");
  if(bufout[lbufout] != CANARY)
  	g_message("Canary in bufout is dead!");

  realloc( strokes, ( sizeof(gdouble) * (num_strokes * 2 + 1)) );

  // assegno i valori ai parametri di ritorno
  *pstrokes = strokes;
  *pnum_strokes = num_strokes;

  // libero la memoria di tutte le variabili allocate

  if(dest)
    free(dest);
  dest = NULL;

  if(bufin)
    free(bufin);
  bufin = NULL;

  if(bufout)
    free(bufout);
  bufout = NULL;

  // ripristino la vecchia directory di lavoro
  chdir(dir_gimp);
}


*/
