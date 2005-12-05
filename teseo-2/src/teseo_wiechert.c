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

#include <glib.h>
#include <glib/gprintf.h>
#include <math.h>
#include "teseo_wiechert.h"


gdouble ret_b[N_TRIES]={};
gdouble ret_errors[N_TRIES]={};
gdouble hist_points[DEFAULT_B*2*180]={};
gdouble histogram[180]={};

inline double teseo_cadeck( double Rg, double  a, double r, double b, double Bg, double sec, double xfinal, double xinitial, double xi, double yi){
	double ret=0;
		ret =	60./Bg * (
			Bg*sec/(xfinal-xinitial)/60.*xi
			- r*asin( ( r*r+a*a-Rg*Rg + ((Bg*sec/(xfinal-xinitial)/60.)*yi-b)*((Bg*sec/(xfinal-xinitial)/60.)*yi-b))/2./a/r)
			+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
			);
	return ret;
}


gboolean teseo_wiech_estimate_b1(	gint32 g_image,
					gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
					gboolean rotate, gboolean translate, gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin,  gdouble angle,
					gboolean use_angle, gboolean ignore_coord, gboolean ignore_sec,
					gdouble ret_b[], gdouble ret_errors[], gulong n_tries){
	gboolean return_code=TRUE;
	gdouble *strokes_in=NULL;
	gdouble *strokes_copy=NULL;
	gulong n_strokes_in;
	gulong i,cpt;
	gdouble alpha,x1,xtest;
	gdouble ret;

	gdouble * points_pairs=NULL;
	gint path_closed, num_path_point_details;

	gdouble xfract=0.,yfract=0.;
	gdouble factor;

	gimp_image_get_resolution (g_image,  &xfract,  &yfract);

	//TODO take the points in the current path, the caller must insure that is a PATH_SEMANTIC_POLYLINE
        teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs);

	xfract= xfract/25.4;
	yfract= yfract/25.4;

	//allocate space for strokes 2*number_of_points
	n_strokes_in =2*( 1 + (num_path_point_details - 6) /9);
	strokes_in= (gdouble *) g_malloc( n_strokes_in * sizeof (gdouble) );

	//translate path in strokes in mm
	for ( i=0; i<n_strokes_in/2; i++ ) {
		strokes_in[2*i]    = points_pairs[i*9]   / xfract ;
		strokes_in[2*i+1]  = points_pairs[i*9+1] / yfract ;
		//g_printf("\nX=%f Y=%f",strokes_in[2*i],strokes_in[2*i+1]);
	}
	g_free(points_pairs);

        //Take XinYin, XfinYfin from the path
	if(ignore_coord){
		Xin=strokes_in[0];
		Yin=strokes_in[1];
		Xfin=strokes_in[n_strokes_in-2];
		Yfin=strokes_in[n_strokes_in-1];
		//g_printf("\nFrom path Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}//or from input
	else {
		Xin=Xin / xfract;
		Yin=Yin / yfract;
		Xfin=Xfin / xfract;
		Yfin=Yfin / yfract;
		//g_printf("\nFrom input Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}

	//translate in Xin,Yin origin
	if(translate){
		teseo_translate(strokes_in, n_strokes_in, Xin, Yin);
	}
	//rotate considering Xin, Yin, Xfin, Yfin,
	//if rotate==false suppose eventually rotated before calling this function
	if(rotate){
		if(use_angle){
			alpha=angle;
		}
		else{
			alpha=atan((Yfin-Yin)/(Xfin-Xin));
		}
		//alpha=atan((Yfin-Yin)/(Xfin-Xin));
		teseo_rotate_clockwise(strokes_in, n_strokes_in, alpha);
	}

	if(ignore_sec){
		factor=1.0;
	}
	else {
		factor= Bg * sec/ ( (Xfin-Xin) * 60.);
	}


	b=-b;
	for (cpt=0;cpt<n_tries;cpt++){
		b=b+.1;
		//working copy
		strokes_copy=teseo_copy_strokes(strokes_in,n_strokes_in);
		x1 =	60./ Bg * (
			+ factor *strokes_copy[0]
			- r*asin( ( r*r+a*a-Rg*Rg + ((factor)*strokes_copy[1]-b) * ((factor)*strokes_copy[1]-b) )/2./a/r)
			+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
			);
		/*
		Count, after the whole corrections including b, the number of points
		which has a time younger that the previous point (wrong points)
		*/

		xtest=x1;
		//g_printf("\nXtest=%f",xtest);
		ret=0.;
		//Counting wrong points
		//g_printf("\nCounting  wrong points\n");
		for(i=0; i<(n_strokes_in/2); i++){
			strokes_copy[2*i] =
				60./Bg * (
				+ factor *strokes_copy[2*i]
				- r*asin( ( r*r+a*a-Rg*Rg + (factor*strokes_copy[2*i+1]-b)*(factor*strokes_copy[2*i+1]-b) )/2./a/r)
				+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
				);
			if ( strokes_copy[2*i] > xtest ) {
				xtest=strokes_copy[2*i];
				//g_printf("\nChanged Xtest=%f",xtest);
			}
			else {
				ret=ret+1;
			}
		}
		// write the value b and the number of wrong points, cpt is the counter
		ret_b[cpt] = b;
		ret_errors[cpt] = ret;
		g_free(strokes_copy);
	}
	g_free(strokes_in);
	return return_code;
}

/*Dimension of hist_points 180*2*b, */
gboolean teseo_wiech_estimate_b2(	gint32 g_image,
					gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
					gboolean rotate, gboolean translate, gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin, gdouble angle,
					gboolean use_angle, gboolean ignore_coord, gboolean ignore_sec,
					gdouble hist_points[]){
	gboolean return_code=TRUE;
	gdouble *strokes_in=NULL;
	gdouble *strokes_copy=NULL;
        gdouble *slope_hist=NULL;

	gulong n_strokes_in;
	gulong i,cpt;
	gdouble alpha,x1,xtest;
	gdouble ret;


	gdouble * points_pairs=NULL;
	gint path_closed, num_path_point_details;
	gulong b2=b;

	gdouble xfract=0.,yfract=0.;
	gdouble factor;

	gimp_image_get_resolution (g_image,  &xfract,  &yfract);

	//TODO take the points in the current path, the caller must insure that is a PATH_SEMANTIC_POLYLINE
        teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs);

	xfract= xfract/25.4;
	yfract= yfract/25.4;

	slope_hist=(gdouble*) g_malloc(180*sizeof(gdouble));

	//allocate space for strokes 2*number_of_points
	n_strokes_in =2*( 1 + (num_path_point_details - 6) /9);
	strokes_in= (gdouble *) g_malloc( n_strokes_in * sizeof (gdouble) );

	//translate path in strokes in mm
	//g_printf("copying path in strokes\n");
	for ( i=0; i<n_strokes_in/2; i++ ) {
		strokes_in[2*i]    = points_pairs[i*9]   / xfract ;
		strokes_in[2*i+1]  = points_pairs[i*9+1] / yfract ;
		//g_printf("\nX=%f Y=%f",strokes_in[2*i],strokes_in[2*i+1]);
	}

	g_free(points_pairs);

        //Take XinYin, XfinYfin from the path
	if(ignore_coord){
		Xin=strokes_in[0];
		Yin=strokes_in[1];
		Xfin=strokes_in[n_strokes_in-2];
		Yfin=strokes_in[n_strokes_in-1];
		//g_printf("\nFrom path Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}//or from input
	else {
		Xin=Xin / xfract;
		Yin=Yin / yfract;
		Xfin=Xfin / xfract;
		Yfin=Yfin / yfract;
		//g_printf("\nFrom input Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}

	//translate in Xin,Yin origin
	if(translate){
		teseo_translate(strokes_in, n_strokes_in, Xin, Yin);
	}
	//rotate considering Xin, Yin, Xfin, Yfin,
	//if rotate==false suppose eventually rotated before calling this function
	if(rotate){
		if(use_angle){
			alpha=angle;
		}
		else{
			alpha=atan((Yfin-Yin)/(Xfin-Xin));
		}
		//alpha=atan((Yfin-Yin)/(Xfin-Xin));
		teseo_rotate_clockwise(strokes_in, n_strokes_in, alpha);
	}

	if(ignore_sec){
		factor=1.0;
	}
	else {
		factor= Bg * sec/ ( (Xfin-Xin) * 60.);
	}

	b=-b;

	for ( cpt=0; cpt<(2*b2+1); cpt++){
		b++;
		//working copy
		strokes_copy=teseo_copy_strokes(strokes_in,n_strokes_in);
		x1 =	60./ Bg * (
			+ factor *strokes_copy[0]
			- r*asin( ( r*r+a*a-Rg*Rg + ((factor)*strokes_copy[1]-b) * ((factor)*strokes_copy[1]-b) )/2./a/r)
			+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
			);

		/*
		Count, after the whole corrections including b, the number of points
		which has a time younger that the previous point (wrong points)
		*/

		xtest=x1;

		ret=0.;
		//Counting wrong points
		//g_printf("\nCounting  wrong points\n");
		for(i=0; i<(n_strokes_in/2); i++){
			strokes_copy[2*i] =
				60./Bg * (
				+ factor *strokes_copy[2*i]
				- r*asin( ( r*r+a*a-Rg*Rg + (factor*strokes_copy[2*i+1]-b)*(factor*strokes_copy[2*i+1]-b) )/2./a/r)
				+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
				);
			if ( strokes_copy[2*i] > xtest ) {
				xtest=strokes_copy[2*i];
			}
			else {
				ret=ret+1;
			}
		}

		//Zeroing
		gint j;
		gdouble pir;
		for(j=-90; j<90; j++){
			slope_hist[j+90]=0.;
		}

		pir=asin(0.5);

		for (i=0; i<(n_strokes_in/2); i++){

			alpha = (30/pir) * atan( (strokes_copy[2*(i+1)]-strokes_copy[2*i] )/( strokes_copy[2*(i+1)+1]-strokes_copy[2*i+1]));
			for(j=-90; j<90; j++){
				if ( (alpha<=(j+1)) && (alpha>(j-1)) ) {
					slope_hist[j+90]=slope_hist[j+90]+1;
				}
			}
		}
                //for a given cpt we now have the slop_hist
		for(j=-90; j<=90; j++){
				hist_points[(j+90)+cpt*180] = slope_hist[j+90];
				//g_printf("b=%f j=%d slope=%f   cpt=%d\n",b,j,hist_points[(j+90)+cpt*180], cpt);
		}

		g_free(strokes_copy);
	}

	g_free(slope_hist);
	g_free(strokes_in);

return return_code;

}





/*Dimension of hist_points 180, */
gboolean teseo_wiech_slope_hist(	gint32 g_image,
					gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
					gboolean rotate, gboolean translate, gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin, gdouble angle,
					gboolean use_angle, gboolean ignore_coord, gboolean ignore_sec,
					gdouble hist_points[]){
	gboolean return_code=TRUE;
	gdouble *strokes_in=NULL;
	gdouble *strokes_copy=NULL;
        gdouble *slope_hist=NULL;

	gulong n_strokes_in;
	gulong i;
	gdouble alpha,x1,xtest;
	gdouble ret;


	gdouble * points_pairs=NULL;
	gint path_closed, num_path_point_details;


	gdouble xfract=0.,yfract=0.;
	gdouble factor;

	gimp_image_get_resolution (g_image,  &xfract,  &yfract);

	//TODO take the points in the current path, the caller must insure that is a PATH_SEMANTIC_POLYLINE
        teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs);

	xfract= xfract/25.4;
	yfract= yfract/25.4;

	slope_hist=(gdouble*) g_malloc(180*sizeof(gdouble));

	//allocate space for strokes 2*number_of_points
	n_strokes_in =2*( 1 + (num_path_point_details - 6) /9);
	strokes_in= (gdouble *) g_malloc( n_strokes_in * sizeof (gdouble) );

	//translate path in strokes in mm
	//g_printf("copying path in strokes\n");
	for ( i=0; i<n_strokes_in/2; i++ ) {
		strokes_in[2*i]    = points_pairs[i*9]   / xfract ;
		strokes_in[2*i+1]  = points_pairs[i*9+1] / yfract ;
		//g_printf("\nX=%f Y=%f",strokes_in[2*i],strokes_in[2*i+1]);
	}

	g_free(points_pairs);

        //Take XinYin, XfinYfin from the path
	if(ignore_coord){
		Xin=strokes_in[0];
		Yin=strokes_in[1];
		Xfin=strokes_in[n_strokes_in-2];
		Yfin=strokes_in[n_strokes_in-1];
		//g_printf("\nFrom path Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}//or from input
	else {
		Xin=Xin / xfract;
		Yin=Yin / yfract;
		Xfin=Xfin / xfract;
		Yfin=Yfin / yfract;
		//g_printf("\nFrom input Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}

	//translate in Xin,Yin origin
	if(translate){
		teseo_translate(strokes_in, n_strokes_in, Xin, Yin);
	}
	//rotate considering Xin, Yin, Xfin, Yfin,
	//if rotate==false suppose eventually rotated before calling this function
	if(rotate){
		if(use_angle){
			alpha=angle;
		}
		else{
			alpha=atan((Yfin-Yin)/(Xfin-Xin));
		}
		//alpha=atan((Yfin-Yin)/(Xfin-Xin));
		teseo_rotate_clockwise(strokes_in, n_strokes_in, alpha);
	}

	if(ignore_sec){
		factor=1.0;
	}
	else {
		factor= Bg * sec/ ( (Xfin-Xin) * 60.);
	}

	//working copy
	strokes_copy=teseo_copy_strokes(strokes_in,n_strokes_in);
	x1 =	60./ Bg * (
		+ factor *strokes_copy[0]
		- r*asin( ( r*r+a*a-Rg*Rg + ((factor)*strokes_copy[1]-b) * ((factor)*strokes_copy[1]-b) )/2./a/r)
		+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
		);

	/*
	Count, after the whole corrections including b, the number of points
	which has a time younger that the previous point (wrong points)
	*/

	xtest=x1;

	ret=0.;
	//Counting wrong points
	//g_printf("\nCounting  wrong points\n");
	for(i=0; i<(n_strokes_in/2); i++){
		strokes_copy[2*i] =
			60./Bg * (
			+ factor *strokes_copy[2*i]
			- r*asin( ( r*r+a*a-Rg*Rg + (factor*strokes_copy[2*i+1]-b)*(factor*strokes_copy[2*i+1]-b) )/2./a/r)
			+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
			);
		if ( strokes_copy[2*i] > xtest ) {
			xtest=strokes_copy[2*i];
		}
		else {
			ret=ret+1;
		}
	}

	//Zeroing
	gint j;
	gdouble pir;
	for(j=-90; j<90; j++){
		slope_hist[j+90]=0.;
	}

	pir=asin(0.5);

	for (i=0; i<(n_strokes_in/2); i++){

		alpha = (30/pir) * atan( (strokes_copy[2*(i+1)]-strokes_copy[2*i] )/( strokes_copy[2*(i+1)+1]-strokes_copy[2*i+1]));
		for(j=-90; j<90; j++){
			if ( (alpha<=(j+1)) && (alpha>(j-1)) ) {
				slope_hist[j+90]=slope_hist[j+90]+1;
			}
		}
	}
	//we now have the slop_hist, copying out
	for(j=-90; j<90; j++){
			hist_points[j+90] = slope_hist[j+90];
			//g_printf("slope_hist[%d] = %f\n",j+90, slope_hist[j+90]);
	}

	g_free(strokes_copy);
	g_free(slope_hist);
	g_free(strokes_in);

return return_code;

}





gulong teseo_wiech_corr(	gint32 g_image, gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
				gboolean rotate, gboolean translate, gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin, gdouble angle, gdouble vshift,
				gboolean use_angle, gboolean ignore_coord, gboolean ignore_sec, gboolean shift,
				gdouble **corr, gulong* n_points)
{
	gdouble *strokes_in=NULL;
	gdouble *strokes_copy=NULL;
	gdouble *result=NULL;
	gulong n_strokes_in;

	gulong i,j;
	gdouble alpha,x1,y1,xtest,x_i;
	gdouble ret;

	gdouble * points_pairs=NULL;
	gint path_closed, num_path_point_details;

	gdouble xfract=0.,yfract=0.;
	gdouble factor;

	gimp_image_get_resolution (g_image,  &xfract,  &yfract);

	//take the points in the current path, the caller must insure that is a PATH_SEMANTIC_POLYLINE
        teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs);

	xfract= xfract/25.4;
	yfract= yfract/25.4;

	//allocate space for strokes 2*number_of_points
	n_strokes_in =2*( 1 + (num_path_point_details - 6) /9);
	strokes_in= (gdouble *) g_malloc( n_strokes_in * sizeof (gdouble) );

	//translate path in strokes in mm
	for ( i=0; i<n_strokes_in/2; i++ ) {
			strokes_in[2*i]    = points_pairs[i*9]   / xfract ;
			strokes_in[2*i+1]  = points_pairs[i*9+1] / yfract ;
			//g_printf("\nX=%f mm Y=%f mm",strokes_in[2*i],strokes_in[2*i+1]);
	}

	g_free(points_pairs);

        //Take XinYin, XfinYfin from the path if needed
	if(ignore_coord){
		Xin=strokes_in[0];
		Yin=strokes_in[1];
		Xfin=strokes_in[n_strokes_in-2];
		Yfin=strokes_in[n_strokes_in-1];
		//g_printf("\nFrom path Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}
	else {
		Xin=Xin / xfract;
		Yin=Yin / yfract;
		Xfin=Xfin / xfract;
		Yfin=Yfin / yfract;
		//g_printf("\nFrom input Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}

	//translate in Xin,Yin origin
	if(translate){
		teseo_translate(strokes_in, n_strokes_in, Xin, Yin);
	}
	//rotate considering Xin, Yin, Xfin, Yfin,
	//if rotate==false suppose eventually rotated before calling this function
	if(rotate){
		if(use_angle){
			alpha=angle;
		}
		else{
			alpha=atan((Yfin-Yin)/(Xfin-Xin));
		}
		teseo_rotate_clockwise(strokes_in, n_strokes_in, alpha);
	}

	if(ignore_sec){
		factor=1.0;
	}
	else {
		factor= Bg * sec/ ( (Xfin-Xin) * 60.);
	}

	//working copy
	strokes_copy=teseo_copy_strokes(strokes_in,n_strokes_in);

	x1 =	60./ Bg * (
		+ factor *strokes_in[0]
		- r*asin( ( r*r+a*a-Rg*Rg + ((factor)*strokes_in[1]-b) * ((factor)*strokes_in[1]-b) )/2./a/r)
		+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
		);
	y1=	factor*strokes_in[1];

	strokes_copy[0]=x1;
	strokes_copy[1]=y1;

	/*
	Count, after the whole corrections including b, the number of points
	which has a time younger that the previous point (wrong points)
	*/
	xtest=x1;

	//	Counting wrong points
	//g_printf("\nCounting  wrong points\n");
	for(i=1, j=1, ret=0; i<(n_strokes_in/2); i++){
		x_i =
			60./Bg * (
			+ factor *strokes_in[2*i]
			- r*asin( ( r*r+a*a-Rg*Rg + (factor*strokes_in[2*i+1]-b)*(factor*strokes_in[2*i+1]-b) )/2./a/r)
			+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
			);
		if ( x_i > xtest ) {
			xtest=x_i;
			strokes_copy[j*2]=x_i;
			if(!shift){
				strokes_copy[j*2+1]=factor*strokes_in[2*i+1];
			}
			else{
				strokes_copy[j*2+1]=factor*strokes_in[2*i+1] - vshift*(x_i-strokes_copy[0]);
			}
			j++; //count right elements
		}
		else {
			ret=ret+1;
		}
	}

	result=teseo_copy_strokes(strokes_copy,2*j);
	//restore in starting point
	teseo_translate(result, 2*j, -Xin, -Yin);

	*corr=result;
	*n_points=j;

	g_free(strokes_copy);
	g_free(strokes_in);
	return (gulong) (100.0 * (((double) (n_strokes_in/2) - (double) ret ) )/ ((double) (n_strokes_in/2)) );

}

/*!Rotate the strokes of angle radian around the first point
*/
void teseo_rotate_clockwise(gdouble * strokes, gulong n_strokes, gdouble angle){
	gdouble offx=strokes[0];
	gdouble offy=strokes[1];
	gdouble cosangle = cos(angle);
	gdouble sinangle = sin(angle);
	gdouble x_rot,y_rot;
	gulong i=0;
        //g_printf("Rotating of alpha %f radians = %f degree\n",angle,angle/atan(1.0)*45.0);
	//modifying strokes
	for( i=1; i < (n_strokes/2) ; i++) {
		x_rot =  (cosangle * (strokes[i*2] - offx)) + (sinangle * (strokes[i*2 +1] - offy));
		y_rot = -(sinangle * (strokes[i*2] - offx)) + (cosangle * (strokes[i*2 +1] - offy));
		strokes[i*2]    = x_rot+offx;
		strokes[i*2 +1] = y_rot+offy;
	}
}

/*!translate the strokes at point x,y
*/
void teseo_translate(gdouble * strokes, gulong n_strokes, gdouble x, gdouble y){
	gulong i=0;

	//modifying strokes
	for( i=0; i < (n_strokes/2) ; i++) {
		strokes[i*2]    = strokes[i*2]-x;
		strokes[i*2 +1] = strokes[i*2+1]-y;
	}
}

/*!copy the strokes
*/
gdouble * teseo_copy_strokes(gdouble * strokes, gulong n_strokes){
	gdouble *mystrokes=NULL;
	gulong i=0;

	mystrokes=(gdouble *) g_malloc( sizeof(gdouble) *n_strokes);
	if ( mystrokes!=NULL ) {
		//modifying strokes
		for(i=0; i < (n_strokes/2) ; i++) {
			mystrokes[i*2]    = strokes[i*2];
			mystrokes[i*2+1]  = strokes[i*2+1];
		}
	}
	return mystrokes;
}
