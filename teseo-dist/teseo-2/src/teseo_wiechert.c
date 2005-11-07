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


inline double teseo_cadeck( double Rg, double  a, double r, double b, double Bg, double sec, double xfinal, double xinitial, double xi, double yi){
	double ret=0;
		ret =	60./Bg * (
			Bg*sec/(xfinal-xinitial)/60.*xi
			- r*asin( ( r*r+a*a-Rg*Rg + ((Bg*sec/(xfinal-xinitial)/60.)*yi-b)*((Bg*sec/(xfinal-xinitial)/60.)*yi-b))/2./a/r)
			+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
			);
	return ret;
}

void func(){
	int nPoints=0;
	int max_val=15000;
	double ret_b_e[2][max_val];

	int i,cpt;
	double yi,yf;
	double sec,alp,ret ;
	//real x(nPoints),y(nPoints),xx(nPoints),yy(nPoints)
	//real xa(nPoints),ya(nPoints)
	double xa[nPoints];
	double ya[nPoints];
	double rX[nPoints];
	double rY[nPoints];

	double xx[nPoints];
	double yy[nPoints];
	double xinitial,xfinal, x1;
	double Bg,r,Rg,a,b,xtest;


        int npt1;
	npt1 = nPoints;

//	write(*,*) 'Entering sub', npt1
//	Change the origin of the file to the first point of the base line
	for( i=1; i<npt1; i++){
	  xa[i]=rX[i]-xinitial;
	  ya[i]=rY[i]-yi;
	}

	/*
	Calculation of the correction angle of the base line in radian
	angle positive if the correction to apply is counter clockwise
	(this is only the correction of an angle due to the digitizing process)
	*/

	alp=atan((yf-yi)/(xfinal-xinitial));

	b=-40.;

	//do 77 cpt=1,max_val
        for (cpt=1;cpt<max_val;cpt++){
	b=b+.1;

	/*
	Correction of input file of the rotation alpha
	*/
	for(i=1; i<npt1; i++) {
	   xx[i]=+xa[i]*cos(alp)+ya[i]*sin(alp);
	   yy[i]=-xa[i]*sin(alp)+ya[i]*cos(alp);
	}


	x1 =	60./ Bg * (
		Bg * sec/ (xfinal-xinitial) /60.*xx[1]
		- r*asin( ( r*r+a*a-Rg*Rg + ((Bg*sec/(xfinal-xinitial)/60.)*yy[1]-b) * ((Bg*sec/(xfinal-xinitial)/60.)*yy[1]-b) )/2./a/r)
		+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
		);

	/*
	Count, after the whole corrections including b, the number of points
	which has a time younger that the previous point (wrong points)
	*/

	xtest=x1;
	ret=0.;
//	Counting wrong points

	for(i=1;i<npt1;i++){
		xx[i] =	60./Bg * (
			Bg*sec/(xfinal-xinitial)/60.*xx[i]
			- r*asin( ( r*r+a*a-Rg*Rg + ((Bg*sec/(xfinal-xinitial)/60.)*yy[i]-b)*((Bg*sec/(xfinal-xinitial)/60.)*yy[i]-b))/2./a/r)
			+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
			);
	//if((xx(i)-xtest).ge.0.001) then
		if ( (xx[i]-xtest) >= 0.001) {
			xtest=xx[i];
		}
		else {
			ret=ret+1;
		}
	}

	for(i=1;i<nPoints;i++){
	   xx[i]=0.;
	   yy[i]=0.;
	}

	//gulong npt2=i-1;
//	 write the value b and the number of wrong points cpt is the counter

	ret_b_e[1][cpt] = b;
	ret_b_e[2][cpt] = ret;
	}
//77    continue

}

gboolean teseo_wiech_estimate_b1(	gint32 g_image,
					gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
					gboolean rotate, gboolean translate, gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin, gboolean ignore_coord,
					gdouble ret_b[], gdouble ret_errors[], gulong n_tries){
	gboolean return_code=TRUE;
	gdouble *strokes_in=NULL;
	gdouble *strokes_copy=NULL;
	gulong n_strokes_in;
	gulong i,cpt;
	gdouble alpha,x1,xtest;
	gdouble ret;

//	gdouble b;
	gdouble * points_pairs=NULL;
	gint path_closed, num_path_point_details;

	gdouble xfract=0.,yfract=0.;

	gimp_image_get_resolution (g_image,  &xfract,  &yfract);

	//TODO take the points in the current path, the caller must insure that is a PATH_SEMANTIC_POLYLINE
        teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs);

	xfract= xfract/25.4;
	yfract= yfract/25.4;

	//allocate space for strokes 2*number_of_points
	n_strokes_in =2*( 1 + (num_path_point_details - 6) /9);
	strokes_in= (gdouble *) g_malloc( n_strokes_in * sizeof (gdouble) );

	//translate path in strokes in mm
	g_printf("copying path in strokes\n");
	for ( i=0; i<n_strokes_in/2; i++ ) {
		strokes_in[2*i]    = points_pairs[i*9]   / xfract ;
		strokes_in[2*i+1]  = points_pairs[i*9+1] / yfract ;
		g_printf("\nX=%f Y=%f",strokes_in[2*i],strokes_in[2*i+1]);
	}

	g_free(points_pairs);

        //Take XinYin, XfinYfin from the path
	if(ignore_coord){
		Xin=strokes_in[0];
		Yin=strokes_in[1];
		Xfin=strokes_in[n_strokes_in/2-2];
		Yfin=strokes_in[n_strokes_in/2-1];
	}

	//translate in Xin,Yin origin
	if(translate){
		teseo_translate(strokes_in, n_strokes_in, Xin, Yin);
	}
	//rotate considering Xin, Yin, Xfin, Yfin,
	//if rotate==false suppose eventually rotated before calling this function
	if(rotate){
		alpha=atan((Yfin-Yin)/(Xfin-Xin));
		teseo_rotate_clockwise(strokes_in, n_strokes_in, alpha);
	}

	b=-b;
	for (cpt=0;cpt<n_tries;cpt++){
		b=b+.1;
		//working copy
		strokes_copy=teseo_copy_strokes(strokes_in,n_strokes_in);
		x1 =	60./ Bg * (
			+ Bg * sec/ (Xfin-Xin) /60.*strokes_copy[0]
			- r*asin( ( r*r+a*a-Rg*Rg + ((Bg*sec/(Xfin-Xin)/60.)*strokes_copy[1]-b) * ((Bg*sec/(Xfin-Xin)/60.)*strokes_copy[1]-b) )/2./a/r)
			+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
			);
		/*
		Count, after the whole corrections including b, the number of points
		which has a time younger that the previous point (wrong points)
		*/

		xtest=x1;
		//g_printf("\nXtest=%f",xtest);
		ret=0.;
	//	Counting wrong points
		//g_printf("\nCounting  wrong points\n");
		for(i=0; i<(n_strokes_in/2); i++){
			strokes_copy[2*i] =
				60./Bg * (
				+ Bg*sec/(Xfin-Xin)/60.*strokes_copy[2*i]
				- r*asin( ( r*r+a*a-Rg*Rg + ((Bg*sec/(Xfin-Xin)/60.)*strokes_copy[2*i+1]-b)*((Bg*sec/(Xfin-Xin)/60.)*strokes_copy[2*i+1]-b))/2./a/r)
				+ r*asin( ( r*r+a*a-Rg*Rg+b*b)/2./a/r)
				);
		//if((xx(i)-xtest).ge.0.001) then
			//if ( (strokes_copy[2*i]-xtest) >= 0.00001) {
			if ( strokes_copy[2*i] > xtest ) {
				xtest=strokes_copy[2*i];
				//g_printf("\nChanged Xtest=%f",xtest);
			}
			else {
				ret=ret+1;
			}
		}
		//g_printf("Counting  wrong points done\n");
	//	 write the value b and the number of wrong points cpt is the counter

		ret_b[cpt] = b;
		ret_errors[cpt] = ret;

		printf("b=%f\t\t\tret=%f\n", b ,ret);
		g_free(strokes_copy);
	}

	g_free(strokes_in);

return return_code;
}

//TODO restituire puntatore e numero di elementi
gulong teseo_wiech_corr(	gint32 g_image, gdouble sec, gdouble Bg, gdouble r, gdouble Rg, gdouble a, gdouble b,
				gboolean rotate, gboolean translate, gdouble Xin, gdouble Yin, gdouble Xfin, gdouble Yfin, gdouble angle,
				gboolean use_angle, gboolean ignore_coord, gboolean ignore_sec,
				gdouble **corr, gulong* n_points)
{
	gdouble *strokes_in=NULL;
	gdouble *strokes_copy=NULL;
	gdouble *result=NULL;
	gulong n_strokes_in;

	gulong i,cpt,j;
	gdouble alpha,x1,y1,xtest,x_i,y_i;
	gdouble ret;

//	gdouble b;
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


	g_printf("copying path in strokes of %d points\n", n_strokes_in/2);
	//translate path in strokes in mm
	//if (!ignore_sec){
	if (TRUE){
		for ( i=0; i<n_strokes_in/2; i++ ) {
			strokes_in[2*i]    = points_pairs[i*9]   / xfract ;
			strokes_in[2*i+1]  = points_pairs[i*9+1] / yfract ;
			g_printf("\nX=%f mm Y=%f mm",strokes_in[2*i],strokes_in[2*i+1]);
		}
	}
	else {
		for ( i=0; i<n_strokes_in/2; i++ ) {
			strokes_in[2*i]    = points_pairs[i*9]   ;
			strokes_in[2*i+1]  = points_pairs[i*9+1] ;
			g_printf("\nX=%f pix Y=%f pix",strokes_in[2*i],strokes_in[2*i+1]);
		}
	}

	g_free(points_pairs);

        //Take XinYin, XfinYfin from the path if needed
	if(ignore_coord){
		Xin=strokes_in[0];
		Yin=strokes_in[1];
		Xfin=strokes_in[n_strokes_in-2];
		Yfin=strokes_in[n_strokes_in-1];
		g_printf("\nFrom path Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
	}
	else {
		g_printf("\nFrom input Xin=%f Yin=%f Xfin=%f Yfin=%f",Xin, Yin, Xfin, Yfin);
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
		//factor= Bg * sec/ sqrt((Xfin-Xin)*(Xfin-Xin) + (Yfin-Yin)*(Yfin-Yin)  ) /60.;
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
	//g_printf("\nXtest=%f",xtest);
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
			strokes_copy[j*2+1]=factor*strokes_in[2*i+1];
			//g_printf("\nChanged Xtest=%f",xtest);
			j++; //count right elements
		}
		else {
			ret=ret+1;
		}
	}
	//g_printf("\nCounting right points done: %d\n",j);

	result=teseo_copy_strokes(strokes_copy,2*j);
	//restore in starting point
	teseo_translate(result, 2*j, -Xin, -Yin);

	for(i=0; i < j ; i++) {
		g_printf( "\ni=%d X=%f Y=%f", i, result[i*2], result[i*2+1]);
	}

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
	g_printf("Rotating strokes of angle %f\n", angle);
	gulong i=0;
	//modifico strokes
	for( i=1; i < (n_strokes/2) ; i++) {
		x_rot =  (cosangle * (strokes[i*2] - offx)) + (sinangle * (strokes[i*2 +1] - offy));
		y_rot = -(sinangle * (strokes[i*2] - offx)) + (cosangle * (strokes[i*2 +1] - offy));
		strokes[i*2]    = x_rot+offx;
		strokes[i*2 +1] = y_rot+offy;
	}
	g_printf("Rotating strokes done\n");
}

/*!translate the strokes at point x,y
*/
void teseo_translate(gdouble * strokes, gulong n_strokes, gdouble x, gdouble y){
	gulong i=0;
	g_printf("Translating strokes\n");
	//modifico strokes
	for( i=0; i < (n_strokes/2) ; i++) {
		strokes[i*2]    = strokes[i*2]-x;
		strokes[i*2 +1] = strokes[i*2+1]-y;
	}
	g_printf("Translating strokes done\n");
}

/*!copy the strokes
*/
gdouble * teseo_copy_strokes(gdouble * strokes, gulong n_strokes){
	gdouble *mystrokes=NULL;
	gulong i=0;
	g_printf("Copying strokes\n");
	mystrokes=(gdouble *) g_malloc( sizeof(gdouble) *n_strokes);
	if ( mystrokes!=NULL ) {
		//modifico strokes
		for(i=0; i < (n_strokes/2) ; i++) {
			mystrokes[i*2]    = strokes[i*2];
			mystrokes[i*2+1]  = strokes[i*2+1];
		}
	}
	g_printf("Copying strokes done\n");
	return mystrokes;

}
