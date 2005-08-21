/* ../2_arm-shift-slope-en.f -- translated by f2c (version 20030320).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__4 = 4;
static doublereal c_b63 = 2.;
static integer c__3 = 3;

/* 	**************************************************** */
/* 	*** Calculation of the histogram of the slope at *** */
/* 	*** each point of the seismograme function of b value *** */
/* 	*** (arm shift) 						*** */
/* 	*** input format = x, y                     *** */
/* 	**************************************************** */
/* 	========================= */
/* 	VARIABLES */
/* 	========================= */
/* Main program */ int MAIN__(void)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(void), s_rsle(cilist *), e_rsle(void), f_open(olist *);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double atan(doublereal), cos(doublereal), sin(doublereal), pow_dd(
	    doublereal *, doublereal *), asin(doublereal);
    integer f_clos(cllist *);

    /* Local variables */
    static real xinitial, a, b;
    static integer i__, j;
    static real n[200], r__, x[15000], y[15000];
    static integer b2;
    static real x1, bg, xa[15000], ya[15000], rg, yf, yi, xx[15000], yy[15000]
	    , sec, alp;
    static integer cpt;
    static real pir;
    static integer npt1;
    static char fiche[20];
    static real alpha[15000], xfinal;
    static char sortie[20], sortie2[20];

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 6, 0, 0, 0 };
    static cilist io___2 = { 0, 5, 0, 0, 0 };
    static cilist io___4 = { 0, 6, 0, 0, 0 };
    static cilist io___5 = { 0, 5, 0, 0, 0 };
    static cilist io___7 = { 0, 6, 0, 0, 0 };
    static cilist io___8 = { 0, 5, 0, 0, 0 };
    static cilist io___13 = { 0, 6, 0, 0, 0 };
    static cilist io___14 = { 0, 5, 0, 0, 0 };
    static cilist io___16 = { 0, 6, 0, 0, 0 };
    static cilist io___17 = { 0, 5, 0, 0, 0 };
    static cilist io___19 = { 0, 6, 0, 0, 0 };
    static cilist io___20 = { 0, 5, 0, 0, 0 };
    static cilist io___22 = { 0, 6, 0, 0, 0 };
    static cilist io___23 = { 0, 5, 0, 0, 0 };
    static cilist io___25 = { 0, 6, 0, 0, 0 };
    static cilist io___26 = { 0, 5, 0, 0, 0 };
    static cilist io___28 = { 0, 6, 0, 0, 0 };
    static cilist io___29 = { 0, 5, 0, 0, 0 };
    static cilist io___31 = { 0, 1, 1, 0, 0 };
    static cilist io___45 = { 0, 2, 0, 0, 0 };
    static cilist io___50 = { 0, 4, 0, 0, 0 };


    s_wsle(&io___1);
    do_lio(&c__9, &c__1, "Name of input file", (ftnlen)18);
    e_wsle();
    s_rsle(&io___2);
    do_lio(&c__9, &c__1, fiche, (ftnlen)20);
    e_rsle();
    s_wsle(&io___4);
    do_lio(&c__9, &c__1, "Name of output file", (ftnlen)19);
    e_wsle();
    s_rsle(&io___5);
    do_lio(&c__9, &c__1, sortie, (ftnlen)20);
    e_rsle();
    s_wsle(&io___7);
    do_lio(&c__9, &c__1, "enter the two extremes points of the input file, *"
	    " xinitial xfinal yi yf, along the base line:", (ftnlen)94);
    e_wsle();
    s_rsle(&io___8);
    do_lio(&c__4, &c__1, (char *)&xinitial, (ftnlen)sizeof(real));
    do_lio(&c__4, &c__1, (char *)&xfinal, (ftnlen)sizeof(real));
    do_lio(&c__4, &c__1, (char *)&yi, (ftnlen)sizeof(real));
    do_lio(&c__4, &c__1, (char *)&yf, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___13);
    do_lio(&c__9, &c__1, "number of seconds between  xinitial and xfinal:", (
	    ftnlen)47);
    e_wsle();
    s_rsle(&io___14);
    do_lio(&c__4, &c__1, (char *)&sec, (ftnlen)sizeof(real));
    e_rsle();
/* 	Correction of the curvature of the signal due to the geometry */
/* 	of the Wiechert seismograph recorder */
    s_wsle(&io___16);
    do_lio(&c__9, &c__1, "Enter the velocity of the paper in mm/min :", (
	    ftnlen)43);
    e_wsle();
    s_rsle(&io___17);
    do_lio(&c__4, &c__1, (char *)&bg, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___19);
    do_lio(&c__9, &c__1, "Enter the radius of the cylinder that carry the pa"
	    "per, in mm:", (ftnlen)61);
    e_wsle();
    s_rsle(&io___20);
    do_lio(&c__4, &c__1, (char *)&r__, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___22);
    do_lio(&c__9, &c__1, "Enter the length of the writting arm from its rota"
	    "tion axe \runtil the end of the needle in mm :", (ftnlen)95);
    e_wsle();
    s_rsle(&io___23);
    do_lio(&c__4, &c__1, (char *)&rg, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___25);
    do_lio(&c__9, &c__1, "Enter the distance from the rotation axe of the wr"
	    "iting arm \runtil the axe of the cylinder that carry the paper"
	    " \r              measured perpendicular to the cylinder axe in m"
	    "m :", (ftnlen)177);
    e_wsle();
    s_rsle(&io___26);
    do_lio(&c__4, &c__1, (char *)&a, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___28);
    do_lio(&c__9, &c__1, "Enter the maximum shift (positive value in mm) to "
	    "be tested, \r between the needle position at rest and its normal"
	    " equilibrium po (arm perpendicular to the axe of the cylinder) :",
	     (ftnlen)177);
    e_wsle();
    s_rsle(&io___29);
    do_lio(&c__4, &c__1, (char *)&b, (ftnlen)sizeof(real));
    e_rsle();
    o__1.oerr = 0;
    o__1.ounit = 1;
    o__1.ofnmlen = 20;
    o__1.ofnm = fiche;
    o__1.orl = 0;
    o__1.osta = "old";
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
    i__1 = s_rsle(&io___31);
    if (i__1 != 0) {
	goto L1000;
    }
    for (i__ = 1; i__ <= 15000; ++i__) {
	i__1 = do_lio(&c__4, &c__1, (char *)&x[i__ - 1], (ftnlen)sizeof(real))
		;
	if (i__1 != 0) {
	    goto L1000;
	}
	i__1 = do_lio(&c__4, &c__1, (char *)&y[i__ - 1], (ftnlen)sizeof(real))
		;
	if (i__1 != 0) {
	    goto L1000;
	}
    }
    i__1 = e_rsle();
    if (i__1 != 0) {
	goto L1000;
    }
L1000:
    npt1 = i__ - 1;
    s_copy(sortie2, "slope", (ftnlen)20, (ftnlen)5);
    b2 = b;
    b = -b - 1;
    i__1 = b2 << 1;
    for (cpt = 1; cpt <= i__1; ++cpt) {
	b += 1;
	o__1.oerr = 0;
	o__1.ounit = 4;
	o__1.ofnmlen = 20;
	o__1.ofnm = sortie2;
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
/* 	Change the origin of the file to the first point of the base line */
/* 	(necessary if different from 0) */
	i__2 = npt1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    xa[i__ - 1] = x[i__ - 1] - xinitial;
	    ya[i__ - 1] = y[i__ - 1] - yi;
/* L5: */
	}
/* 	Calculation of the correction angle of the base line in radian */
/* 	angle positive if the correction to apply is counter clockwise */
/* 	(this is only the correction of an angle due to the digitizing process) */
	alp = atan((yf - yi) / (xfinal - xinitial));
/* 	Correction of input file of the rotation alpha */
	i__2 = npt1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    xx[i__ - 1] = xa[i__ - 1] * cos(alp) + ya[i__ - 1] * sin(alp);
	    yy[i__ - 1] = -xa[i__ - 1] * sin(alp) + ya[i__ - 1] * cos(alp);
/* L6: */
	}
	d__1 = (doublereal) (bg * sec / (xfinal - xinitial) / 60.f * yy[0] - 
		b);
	x1 = 60.f / bg * (bg * sec / (xfinal - xinitial) / 60.f * xx[0] - r__ 
		* asin((r__ * r__ + a * a - rg * rg + pow_dd(&d__1, &c_b63)) /
		 2.f / a / r__) + r__ * asin((r__ * r__ + a * a - rg * rg + b 
		* b) / 2.f / a / r__));
/* 	*** next step: Correction of the curvature of the data including */
/* 	the « b » shift of  the writing arm *** */
/* *** next step: time correction *** */
	o__1.oerr = 0;
	o__1.ounit = 2;
	o__1.ofnmlen = 20;
	o__1.ofnm = sortie;
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
	i__2 = npt1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    d__1 = (doublereal) (bg * sec / (xfinal - xinitial) / 60.f * yy[
		    i__ - 1] - b);
	    xx[i__ - 1] = 60.f / bg * (bg * sec / (xfinal - xinitial) / 60.f *
		     xx[i__ - 1] - r__ * asin((r__ * r__ + a * a - rg * rg + 
		    pow_dd(&d__1, &c_b63)) / 2.f / a / r__) + r__ * asin((r__ 
		    * r__ + a * a - rg * rg + b * b) / 2.f / a / r__));
/*  	*** warning : the ploarity of data must be checked.  If inversion of */
/* 	polarity necessary, use the formula here under.*** */
/* 	   y(i)=-((Bg*sec/(xfinal-xinitial)/60)*y(i)) */
	    xx[i__ - 1] -= x1;
	    yy[i__ - 1] = bg * sec / (xfinal - xinitial) / 60.f * yy[i__ - 1];
/* 	 output file in time (sec) and amplitude (mm) */
	    s_wsle(&io___45);
	    do_lio(&c__4, &c__1, (char *)&xx[i__ - 1], (ftnlen)sizeof(real));
	    do_lio(&c__4, &c__1, (char *)&yy[i__ - 1], (ftnlen)sizeof(real));
	    e_wsle();
/* L20: */
	}
	cl__1.cerr = 0;
	cl__1.cunit = 1;
	cl__1.csta = 0;
	f_clos(&cl__1);
	cl__1.cerr = 0;
	cl__1.cunit = 2;
	cl__1.csta = 0;
	f_clos(&cl__1);
/* 	Count the histogram of each slope */
	for (j = -90; j <= 90; ++j) {
	    n[j + 89] = 0.f;
/* L54: */
	}
	pir = asin(.5f);
	i__2 = npt1 - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    alpha[i__ - 1] = 30 / pir * atan((xx[i__] - xx[i__ - 1]) / (yy[
		    i__] - yy[i__ - 1]));
	    for (j = -90; j <= 90; ++j) {
		if (alpha[i__ - 1] <= (real) (j + 1) && alpha[i__ - 1] > (
			real) (j - 1)) {
		    n[j + 89] += 1;
		}
/* L44: */
	    }
/* L11: */
	}
/* 	*** Write in output file, b, slope, number of point */
	for (j = 1; j <= 180; ++j) {
	    s_wsle(&io___50);
	    do_lio(&c__4, &c__1, (char *)&b, (ftnlen)sizeof(real));
	    do_lio(&c__3, &c__1, (char *)&j, (ftnlen)sizeof(integer));
	    do_lio(&c__4, &c__1, (char *)&n[j - 1], (ftnlen)sizeof(real));
	    e_wsle();
/* L58: */
	}
	cl__1.cerr = 0;
	cl__1.cunit = 4;
	cl__1.csta = 0;
	f_clos(&cl__1);
/* L77: */
    }
    return 0;
} /* MAIN__ */

