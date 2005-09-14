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

void teseo_wmean_init( wmeanParams* s){
  this_params.colour = (*s).colour;
  this_params.step   = (*s).step;
  this_params.width  = (*s).width;
  this_params.height  = (*s).height;
  this_params.dir  = RIGHT;

}

void teseo_wmean_set_dir(int dir){
  this_params.dir = dir;
}

 

int teseo_wmean( const wm_is * is, wm_os * os ){
  int ret =1;
  //int ab,ord;
  int ab_c,ab_s, step_ab;
  int width, height;
  long num=0;

  float colour;
  double sum_num, sum_den;
  double coord;
  double weight, dist;
  int Ord_abs, Ord_rel, Ab_abs, Ab_rel;
//  int Yabs, Yrel, Xabs, Xrel;
  int ORD;
  int t, tmean;
  int MAX_STEP_ORD;

  int dir=this_params.dir;

  int COLOUR  = this_params.colour;

   
  switch (dir) {
    case LEFT:
    case RIGHT:
     //starting LEFT-RIGHT parameters
     MAX_STEP_ORD = this_params.width;
     width        = this_params.width;
     height       = this_params.height;
     num= width*height;
     ab_c         = (int)(width/2);
     // 1 pixel move
     ab_s = ab_c + 1;  
     break;
   case UP:
   case DOWN:
     //starting UP-DOWN parameters
     MAX_STEP_ORD = this_params.height;
     width        = this_params.height;
     height       = this_params.width;
     ab_c         = (int)(width/2);
     // n pixel move
     ab_s = ab_c + 3;  
     break;
  }

  step_ab      = this_params.step;


  sum_num = 0.0;
  sum_den = 0.0;

  // inizialmente mi sposto verso destra sulle x
  // se poi la Ord_rel ? maggiore di un certo valore MAX_STEP_ORD allora faccio la media anche per le x

  
  //column mean
  for(t=0; t < height; t++) {

	coord = t + 1;
	coord /= (double) height;
 
      switch (dir) {
        case RIGHT:
         colour = (*is).bufin[(t*width) + ab_s];
         break;
        case LEFT:
         colour = (*is).bufin[ num - 1 - (width*t+ab_s) ];
         break;
        case UP:
         colour = (*is).bufin[ height*(width - ab_s -1) + t ];
         break;
        case DOWN:
         colour = (*is).bufin[ height*(ab_s +1) - t - 1 ];
         break;
      }

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

	tmean = (int) (((sum_num / sum_den) * (double) height) + 0.5);

	Ord_abs = ( (*is).LastCentre.y - (height/2) + (tmean - 1));

	Ord_rel = (Ord_abs - (*is).LastCentre.y);

	Ab_rel = step_ab;

	if(abs(Ord_rel) > MAX_STEP_ORD) {
		ORD = (Ord_abs - ((*is).LastCentre.y - (height/2)));
		if(ORD < 0) {
			ret=0;
		}
		sum_num = 0.0;
		sum_den = 0.0;
		for(t=0; t < width; t++) {
			coord = t + 1;
			coord /= (double) width;

                  switch (dir) {
                    case RIGHT:
                      colour = (*is).bufin[(ORD*width) + t];
                      break;
                    case LEFT:
                      colour = (*is).bufin[ num - 1 - (width*ORD+t) ];
                      break;
                    case UP:
                      colour = (*is).bufin[ height*(width - t -1) + ORD ];
                      break;
                    case DOWN:
                      colour = (*is).bufin[ height*(t +1) - ORD - 1 ];
                      break;
                  }


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

		tmean = (int) (((sum_num / sum_den) * (double) width) + 0.5);

		Ab_abs = ((*is).LastCentre.x - (width/2) + (tmean - 1));

		Ab_rel = (Ab_abs - (*is).LastCentre.x);

	} else {
		Ab_rel = step_ab;
	}

       switch (dir) {
         case RIGHT:
          (*os).x += (double) Ab_rel;
          (*os).y += (double) Ord_rel;
           break;
         case LEFT:
          (*os).x += (double) -Ab_rel;
          (*os).y += (double) -Ord_rel;
          break;
         case UP:
          (*os).x += (double) Ord_rel;
          (*os).y += (double) -Ab_rel;
           break;
         case DOWN:
          (*os).x += (double) -Ord_rel;
          (*os).y += (double) Ab_rel;
           break;
       }

    return ret;
}


/*
int teseo_wmean( const wm_is * is, wm_os * os ){
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

	tmean = (int) (((sum_num / sum_den) * (double) height) + 0.5);

	Yabs = ( (*is).LastCentre.y - (height/2) + (tmean - 1));

	Yrel = (Yabs - (*is).LastCentre.y);

	Xrel = step_x;

	if(abs(Yrel) > MAX_STEP_Y) {
		YY = (Yabs - ((*is).LastCentre.y - (height/2)));
		if(YY < 0) {
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

		tmean = (int) (((sum_num / sum_den) * (double) width) + 0.5);

		Xabs = ((*is).LastCentre.x - (width/2) + (tmean - 1));

		Xrel = (Xabs - (*is).LastCentre.x);

	} else {
		Xrel = step_x;
	}

	(*os).x += (double) Xrel;
	(*os).y += (double) Yrel;

    return ret;
}
*/

int teseo_wmean_getinput( wm_is * is, const wm_os * previous_os, gint32 drawable_ID){
  int ret=0;

  int width   = this_params.width;
  int height  = this_params.height;

  int x, y;
  int bpp = 0;

  int i;

  glong bufsize;
  guchar *bufin=(*is).bufin;
  GimpDrawable *drawable=NULL;

  GimpPixelRgn pr;

  drawable=gimp_drawable_get(drawable_ID);
  bpp = drawable->bpp;
  bufsize=bpp*width*height;

  if ( (drawable != NULL) && (bufin != NULL) ) {

    //top left corner
    x = (*previous_os).x-width/2;
    y = (*previous_os).y-height/2;

    //get bufin from the pixel region content
    gimp_pixel_rgn_init(&pr, drawable, x, y, width, height, FALSE, FALSE);
    gimp_pixel_rgn_get_rect (&pr, bufin, x, y, width, height);

    //to manage more layers, gimp add alpha channel in background layer
    if ( bpp==2 ) {
      //shifting out alpha bytes
      for (i=1; i<(bufsize/2); i++){
	bufin[i] = bufin[i*2];
      }
    }

    ((*is).LastCentre).x=(*previous_os).x;
    ((*is).LastCentre).y=(*previous_os).y;

    ret=1;
  }
  else
    g_message("NULL pointers");
  return ret;
}

gulong teseo_wmean_get_x(wm_os * os){
 return (*os).x;
}


int teseo_wmean_terminate(wm_os * os, wm_is * is, gint32 drawable_ID){
  int ret=1;
  //control bounds of drawable and stop before
  GimpDrawable *drawable=NULL;
  gulong xmin, xmax, ymin, ymax;

  drawable = gimp_drawable_get(drawable_ID);

  if (drawable) {

    xmin = ((this_params).width )/2;
    ymin = ((this_params).height)/2;

    xmax = drawable->width  - xmin;
    ymax = drawable->height - ymin;

    if ( (((*os).x) > xmin ) && (((*os).x) < xmax ) && ( ((*os).y) > ymin ) && (((*os).y) < ymax ) )
      ret = 0;
  }

  return ret;
}

int teseo_wmean_accumulate( gdouble ** strokes, gulong * num_strokes, wm_os * os){
  int ret=0;
  gdouble * ptr = NULL;
  gulong dim;

  dim =(*num_strokes)*2 + 2 ;

  if ( (*strokes) == NULL) {
    ptr = (gdouble *) g_malloc(  sizeof(gdouble) * dim  );
  }
  else {
    ptr = (gdouble *) g_realloc( (*strokes), ( sizeof(gdouble)) * dim );
  }

  if ( ptr != NULL) {
    *(ptr+dim-2) = (double) ((*os).x);
    *(ptr+dim-1) = (double) ((*os).y);
    //num_strokes is vector dimension/2
    (*num_strokes)+=1;
    ret=1;
  }
  (*strokes)=ptr;

return ret;
}

int teseo_wmean_starting_os(wm_os ** os, gint32 drawable_ID ){
  int ret=0;
  gchar pathname[PATHNAMELEN];
  gdouble * strokes=NULL;
  glong n_strokes;
  gint32 image;
  gint num_paths=0;
  wm_os * r_os=NULL;

  r_os = (wm_os *) g_malloc(sizeof(wm_os));

  if (r_os!=NULL) {
    image=gimp_drawable_get_image( drawable_ID );
    gimp_path_list  (image, &num_paths);

    if (num_paths!=0){
      //get the pathname
      strcpy(pathname, gimp_path_get_current ( image ) );
      //get the strokes
      strokes = teseo_open_path_to_strokes(image, &n_strokes,  pathname);
      if (strokes!=NULL) {
        (*r_os).x=strokes[(n_strokes-1)*2];
        (*r_os).y=strokes[(n_strokes-1)*2+1];
        g_free(strokes);
        ret=1;
        *os=r_os;
      }
    }
    else{
      *os=NULL;
      g_message("Please create a starting path");
    }
  }
  else g_message("teseo_wmean_starting_os NULL pointers");
  return ret;
}


int teseo_wmean_new_is( wm_is ** is, gint32 drawable_ID){
  int ret=0;
  wm_is * r_is=NULL;

  int width   = this_params.width;
  int height  = this_params.height;

  int bpp = 0;

  glong bufsize;
  guchar *bufin=NULL;
  GimpDrawable *drawable=NULL;

  drawable=gimp_drawable_get(drawable_ID);
  bpp = drawable->bpp;
  bufsize=bpp*width*height;

  bufin = (guchar*) g_malloc( (sizeof(guchar)) * bufsize);
  r_is  = (wm_is *) g_malloc(sizeof(wm_is));

  if ( (drawable != NULL) && ( bufin != NULL) && (r_is != NULL)) {
      (*r_is).bufin=bufin;
      *is=r_is;
      ret=1;
  }
  else
      g_message("NULL pointers");
  return ret;
}


void teseo_wmean_release(wm_is ** is, wm_os ** os){
  //g_free bufin memory, is, os
  g_free((*(*is)).bufin);
  g_free(*is);
  g_free(*os);
}


