/* ../1_arm-shift-value-en.f -- translated by f2c (version 20030320).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__4 = 4;
static doublereal c_b63 = 2.;

/* 	**************************************************** */
/* 	*** Search of value b(shift of the writing arm in mm) *** */
/* 	*** 									 *** */
/* 	*** include the 							*** */
/* 	*** correction of the seismogram base line ( angle *** */
/* 	*** of the data do to a not exact digitizing with    *** */
/* 	*** x coordinates along the base line of the seismogram) *** */
/* 	***  + deformation of signal due to  Wiechert recorder *** */
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
	    e_wsle(void), s_rsle(cilist *), e_rsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer f_open(olist *);
    double atan(doublereal), cos(doublereal), sin(doublereal), pow_dd(
	    doublereal *, doublereal *), asin(doublereal);
    integer f_clos(cllist *);

    /* Local variables */
    static real xinitial, a, b;
    static integer i__;
    static real r__, x[15000], y[15000], x1, bg, xa[15000], ya[15000], rg, yf,
	     yi, xx[15000], yy[15000], sec, alp;
    static integer cpt;
    static real ret;
    static integer npt1, npt2;
    static char fiche[20];
    static real xtest, xfinal;
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
    static cilist io___32 = { 0, 1, 1, 0, 0 };
    static cilist io___46 = { 0, 2, 1, 0, 0 };
    static cilist io___48 = { 0, 4, 0, 0, 0 };


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
    do_lio(&c__9, &c__1, "Enter the shift between the needle position at res"
	    "t and its \rnormal equilibrium position (arm perpendicular to th"
	    "e axe of the c value positive if the arm is inclined downward "
	    " \r                  in mm :", (ftnlen)202);
    e_wsle();
    s_rsle(&io___29);
    do_lio(&c__4, &c__1, (char *)&b, (ftnlen)sizeof(real));
    e_rsle();
    s_copy(sortie2, "ret2-b", (ftnlen)20, (ftnlen)6);
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
    i__1 = s_rsle(&io___32);
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
    b = -40.f;
    for (cpt = 1; cpt <= 600; ++cpt) {
	b += .1f;
/* 	Change the origin of the file to the first point of the base line */
/* 	(necessary if different from 0) */
	i__1 = npt1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    xa[i__ - 1] = x[i__ - 1] - xinitial;
	    ya[i__ - 1] = y[i__ - 1] - yi;
/* L5: */
	}
/* 	Calculation of the correction angle of the base line in radian */
/* 	angle positive if the correction to apply is counter clockwise */
/* 	(this is only the correction of an angle due to the digitizing process) */
	alp = atan((yf - yi) / (xfinal - xinitial));
/* 	Correction of input file of the rotation alpha */
	i__1 = npt1;
	for (i__ = 1; i__ <= i__1; ++i__) {
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
/* 	Count, after the whole corrections including b, the number of point */
/* 	which has a time younger that the previous point (wrong points) */
	xtest = x1;
	ret = 0.f;
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
	i__1 = npt1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__1 = (doublereal) (bg * sec / (xfinal - xinitial) / 60.f * yy[
		    i__ - 1] - b);
	    xx[i__ - 1] = 60.f / bg * (bg * sec / (xfinal - xinitial) / 60.f *
		     xx[i__ - 1] - r__ * asin((r__ * r__ + a * a - rg * rg + 
		    pow_dd(&d__1, &c_b63)) / 2.f / a / r__) + r__ * asin((r__ 
		    * r__ + a * a - rg * rg + b * b) / 2.f / a / r__));
	    if (xx[i__ - 1] - xtest >= .001f) {
		xtest = xx[i__ - 1];
	    } else {
		ret += 1;
	    }
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
	for (i__ = 1; i__ <= 15000; ++i__) {
	    xx[i__ - 1] = 0.f;
	    yy[i__ - 1] = 0.f;
/* L10: */
	}
	i__1 = s_rsle(&io___46);
	if (i__1 != 0) {
	    goto L200;
	}
	i__2 = npt1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__1 = do_lio(&c__4, &c__1, (char *)&xx[i__ - 1], (ftnlen)sizeof(
		    real));
	    if (i__1 != 0) {
		goto L200;
	    }
	    i__1 = do_lio(&c__4, &c__1, (char *)&yy[i__ - 1], (ftnlen)sizeof(
		    real));
	    if (i__1 != 0) {
		goto L200;
	    }
	}
	i__1 = e_rsle();
	if (i__1 != 0) {
	    goto L200;
	}
L200:
	npt2 = i__ - 1;
	cl__1.cerr = 0;
	cl__1.cunit = 2;
	cl__1.csta = 0;
	f_clos(&cl__1);
/* 	 write the value b and the number of wrong points */
	s_wsle(&io___48);
	do_lio(&c__4, &c__1, (char *)&b, (ftnlen)sizeof(real));
	do_lio(&c__4, &c__1, (char *)&ret, (ftnlen)sizeof(real));
	e_wsle();
/* L77: */
    }
    return 0;
} /* MAIN__ */

