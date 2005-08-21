/* ../3_geometrical-corr-en.f -- translated by f2c (version 20030320).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__4 = 4;

/* 	******************************************************** */
/* 	*** correction of the seismogram base line ( angle *** */
/* 	*** of the data do to a not exact digitizing with    *** */
/* 	*** x coordinates along the base line of the seismogram) *** */
/* 	***  + deformation of signal due to  Wiechert recorder *** */
/* 	*** The output has amplitude in mm and time in seconds *** */
/* 	*** input file format is x(digitizing unit) y(digitizing unit)  *** */
/* 	*** the format of output file is x(sec) y(mm)       *** */
/* 	***                                                    *** */
/* 	**************************************************** */
/* 	========================= */
/* 	VARIABLES */
/* 	========================= */
/* Main program */ int MAIN__(void)
{
    /* System generated locals */
    integer i__1;
    real r__1;
    olist o__1;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(void), s_rsle(cilist *), e_rsle(void), f_open(olist *);
    double atan(doublereal), cos(doublereal), sin(doublereal), asin(
	    doublereal);

    /* Local variables */
    static real xinitial, a, b;
    static integer i__, k;
    static real r__, x[15000], y[15000], bg, rg, yf, yi, xx[15000], yy[15000],
	     sec, alp;
    static char fiche[20];
    static real xtest, xfinal;
    static char sortie[20];

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 6, 0, 0, 0 };
    static cilist io___2 = { 0, 5, 0, 0, 0 };
    static cilist io___4 = { 0, 6, 0, 0, 0 };
    static cilist io___5 = { 0, 5, 0, 0, 0 };
    static cilist io___7 = { 0, 6, 0, 0, 0 };
    static cilist io___8 = { 0, 5, 0, 0, 0 };
    static cilist io___13 = { 0, 6, 0, 0, 0 };
    static cilist io___14 = { 0, 5, 0, 0, 0 };
    static cilist io___18 = { 0, 1, 1, 0, 0 };
    static cilist io___24 = { 0, 6, 0, 0, 0 };
    static cilist io___25 = { 0, 5, 0, 0, 0 };
    static cilist io___27 = { 0, 6, 0, 0, 0 };
    static cilist io___28 = { 0, 5, 0, 0, 0 };
    static cilist io___30 = { 0, 6, 0, 0, 0 };
    static cilist io___31 = { 0, 5, 0, 0, 0 };
    static cilist io___33 = { 0, 6, 0, 0, 0 };
    static cilist io___34 = { 0, 5, 0, 0, 0 };
    static cilist io___36 = { 0, 6, 0, 0, 0 };
    static cilist io___37 = { 0, 5, 0, 0, 0 };
    static cilist io___39 = { 0, 2, 0, 0, 0 };
    static cilist io___41 = { 0, 2, 0, 0, 0 };
    static cilist io___42 = { 0, 2, 1, 0, 0 };
    static cilist io___43 = { 0, 3, 0, 0, 0 };


/* 	character*20 final */
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
/* 	open(3,file=final) */
/* 	Change the origin of the file to the first point of the base line */
/* 	(necessary if different from 0) */
    k = 0;
    for (i__ = 1; i__ <= 100000; ++i__) {
	++k;
	i__1 = s_rsle(&io___18);
	if (i__1 != 0) {
	    goto L1000;
	}
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
	i__1 = e_rsle();
	if (i__1 != 0) {
	    goto L1000;
	}
	x[i__ - 1] -= xinitial;
	y[i__ - 1] -= yi;
/* L5: */
    }
/* 	Calculation of the correction angle of the base line in radian */
/* 	angle positive if the correction to apply is counter clockwise */
/* 	(this is only the correction of an angle due to the digitizing process) */
L1000:
    alp = atan((yf - yi) / (xfinal - xinitial));
/* 	Correction of input file of the rotation alpha */
    i__1 = k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	xx[i__ - 1] = x[i__ - 1] * cos(alp) + y[i__ - 1] * sin(alp);
	yy[i__ - 1] = -x[i__ - 1] * sin(alp) + y[i__ - 1] * cos(alp);
	x[i__ - 1] = xx[i__ - 1];
	y[i__ - 1] = yy[i__ - 1];
/* L6: */
    }
/* 	Correction of the curvature of the signal due to the geometry */
/* 	of the Wiechert seismograph recorder */
    s_wsle(&io___24);
    do_lio(&c__9, &c__1, "Enter the velocity of the paper in mm/min :", (
	    ftnlen)43);
    e_wsle();
    s_rsle(&io___25);
    do_lio(&c__4, &c__1, (char *)&bg, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___27);
    do_lio(&c__9, &c__1, "Enter the radius of the cylinder that carry the pa"
	    "per, in mm:", (ftnlen)61);
    e_wsle();
    s_rsle(&io___28);
    do_lio(&c__4, &c__1, (char *)&r__, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___30);
    do_lio(&c__9, &c__1, "Enter the length of the writting arm from its rota"
	    "tion axe \runtil the end of the needle in mm :", (ftnlen)95);
    e_wsle();
    s_rsle(&io___31);
    do_lio(&c__4, &c__1, (char *)&rg, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___33);
    do_lio(&c__9, &c__1, "Enter the distance from the rotation axe of the wr"
	    "iting arm \runtil the axe of the cylinder that carry the paper"
	    " \r              measured perpendicular to the cylinder axe in m"
	    "m :", (ftnlen)177);
    e_wsle();
    s_rsle(&io___34);
    do_lio(&c__4, &c__1, (char *)&a, (ftnlen)sizeof(real));
    e_rsle();
    s_wsle(&io___36);
    do_lio(&c__9, &c__1, "Enter the shift between the needle position at res"
	    "t and its \rnormal equilibrium position (arm perpendicular to th"
	    "e axe of the c value positive if the arm is inclined downward "
	    " \r                  in mm :", (ftnlen)202);
    e_wsle();
    s_rsle(&io___37);
    do_lio(&c__4, &c__1, (char *)&b, (ftnlen)sizeof(real));
    e_rsle();
/* Computing 2nd power */
    r__1 = bg * sec / (xfinal - xinitial) / 60 * y[0] - b;
    x[0] = 60 / bg * (bg * sec / (xfinal - xinitial) / 60 * x[0] - r__ * asin(
	    (r__ * r__ + a * a - rg * rg + r__1 * r__1) / 2.f / a / r__) + 
	    r__ * asin((r__ * r__ + a * a - rg * rg + b * b) / 2.f / a / r__))
	    ;
    y[0] = bg * sec / (xfinal - xinitial) / 60 * y[0];
    s_wsle(&io___39);
    do_lio(&c__4, &c__1, (char *)&x[0], (ftnlen)sizeof(real));
    do_lio(&c__4, &c__1, (char *)&y[0], (ftnlen)sizeof(real));
    e_wsle();
/* 	*** next step: Correction of the curvature of the data including */
/* 	the « b » shift of  the writing arm *** */
/* *** next step: time correction *** */
    xtest = x[0];
    i__1 = k - 1;
    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	r__1 = bg * sec / (xfinal - xinitial) / 60 * y[i__ - 1] - b;
	x[i__ - 1] = 60 / bg * (bg * sec / (xfinal - xinitial) / 60 * x[i__ - 
		1] - r__ * asin((r__ * r__ + a * a - rg * rg + r__1 * r__1) / 
		2.f / a / r__) + r__ * asin((r__ * r__ + a * a - rg * rg + b *
		 b) / 2.f / a / r__));
/* *** next step: to avoid local point corrected backward (in time) from */
/*   previous point *** */
	if (x[i__ - 1] - xtest <= .001f) {
	    goto L20;
	} else {
	    xtest = x[i__ - 1];
	}
/*  	*** warning : the ploarity of data must be checked.  If inversion of */
/* 	polarity necessary, use the formula here under.*** */
/* 	   y(i)=-((Bg*sec/(xfinal-xinitial)/60)*y(i)) */
/* 	*** next step: amplitude correction (output in millimetres) *** */
	y[i__ - 1] = bg * sec / (xfinal - xinitial) / 60 * y[i__ - 1];
	s_wsle(&io___41);
	do_lio(&c__4, &c__1, (char *)&x[i__ - 1], (ftnlen)sizeof(real));
	do_lio(&c__4, &c__1, (char *)&y[i__ - 1], (ftnlen)sizeof(real));
	e_wsle();
L20:
	;
    }
/* 	*** next step: final change of the origin of the output file *** */
    k = 0;
    for (i__ = 1; i__ <= 100000; ++i__) {
	++k;
	i__1 = s_rsle(&io___42);
	if (i__1 != 0) {
	    goto L5000;
	}
	i__1 = do_lio(&c__4, &c__1, (char *)&x[i__ - 1], (ftnlen)sizeof(real))
		;
	if (i__1 != 0) {
	    goto L5000;
	}
	i__1 = do_lio(&c__4, &c__1, (char *)&y[i__ - 1], (ftnlen)sizeof(real))
		;
	if (i__1 != 0) {
	    goto L5000;
	}
	i__1 = e_rsle();
	if (i__1 != 0) {
	    goto L5000;
	}
	x[i__ - 1] -= x[0];
	y[i__ - 1] = y[i__ - 1];
	s_wsle(&io___43);
	do_lio(&c__4, &c__1, (char *)&x[i__ - 1], (ftnlen)sizeof(real));
	do_lio(&c__4, &c__1, (char *)&y[i__ - 1], (ftnlen)sizeof(real));
	e_wsle();
/* L30: */
    }
L5000:
    return 0;
} /* MAIN__ */

