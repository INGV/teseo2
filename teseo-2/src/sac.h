
/*		SAC Header Include File

*	This include file should be used when ever the SAC header
*	is accessed.
*	Definitions are for those items that the UofW `tosac'
*	program includes from the UW-PING type data and associated files.
*	Values of `-12345' are used for any varriable which is UNKNOWN.

*/

/*
64 bits architecture patch 1: 
long->gint32
unsigned long -> guint32
*/

#ifndef __TESEO_SACHEADER_H__
#define __TESEO_SACHEADER_H__ 1
#include <glib.h>



#define SACHEADERSIZE	632	/* # of bytes in SAC header */

#define SACVERSION	6	/* version number of SAC */
#define SAC_I_D		2.0	/* SAC I.D. number */

#define SACWORD		float	/* SAC data is an array of floats */
#define SACUNDEF	-12345	/* undefined value */
#define SACSTRUNDEF	"-12345  "	/* undefined value for strings */

#define SACTEMP4	11

#define SACTEMP6	3
#define SACTEMP7	10

#define	KEVNMLEN	16	/* length of event name */
#define K_LEN		8	/* length of all other string fields */

/*	the following are the offsets of various fields in the header */
#define	DEPMIN	1
#define	DEPMAX	2
#define	SAC_O	7
#define	SAC_A	8
#define	EVLA	35
#define	EVLO	36
#define	EVDP	38
#define	USER0	40
#define	DEPMEN	56

/*	This is the actual data structure which is used to enter the header
	variables.
*/
struct SAChead {
	/* floating point fields */
	float delta;		/* increment between data points (sec) */
	float depmin;
	float depmax;
	float scale;
/* 5*/	float odelta;
	float b;		/* beginning value of indep var (0. sec) */
	float e;		/* ending value of indep var (sec) */
	float o;		/* event origin time, rel to zero (sec) */
	float a;		/* first arrival time, rel to zero (sec) */
/*10*/	float internal1;	/* SAC header ID ?? */
	float t0;		/* S-wave arrival time, rel to zero (sec) */
	float t1;
	float t2;
	float t3;
/*15*/	float t4;
	float t5;
	float t6;
	float t7;
	float t8;
/*20*/	float t9;
	float f;		/* End of event time, rel to zero (sec) */
	float resp0;
	float resp1;
	float resp2;
/*25*/	float resp3;
	float resp4;
	float resp5;
	float resp6;
	float resp7;
/*30*/	float resp8;
	float resp9;
	float stla;		/* station latitude (deg north) */
	float stlo;		/* station longitude (deg east */
	float stel;
/*35*/	float stdp;
	float evla;		/* event location, latitude (deg north) */
	float evlo;		/* event location, longitude (deg east) */
	float evel;
	float evdp;		/* event depth (km) */
/*40*/	float blank1;
	float user0;
	float user1;
	float user2;
	float user3;
/*45*/	float user4;
	float user5;
	float user6;
	float user7;
	float user8;
/*50*/	float user9;
	float dist;		/* event-station epicentral distance (km) */
	float az;		/* event to station azimuth (deg) */
	float baz;		/* event to station back-azimuth (deg) */
	float gcarc;		/* event to station arc distance (deg) */
/*55*/	float internal2;
	float internal3;
	float depmen;
	float cmpaz;		/* component azimuth (deg) */
	float cmpinc;		/* component inclination (deg) */
        float blank4[SACTEMP4];

	/* integer fields */
	gint32 nzyear;		/* Trace beginning time (year) */
	gint32 nzjday;		/*  (julian day) */
	gint32 nzhour;		/*  (hour) */
	gint32 nzmin;		/*  (minute) */
	gint32 nzsec;		/*  (second) */
	gint32 nzmsec;		/*  (millisecond) */
	gint32 nvhdr;		/* header version number */
	gint32 internal5;
	gint32 internal6;
	gint32 npts;		/* number of points in trace */
	gint32 internal7;
	gint32 internal8;
        gint32 blank6[SACTEMP6];
	gint32 iftype;		/* Type of data  =1 for time series */
	gint32 idep;		/* Type of dependent data =UNKNOWN */
	gint32 iztype;		/* zero time equivalence  =1 for beginning */
	gint32 iblank6a;
	gint32 iinst;
	gint32 istreg;
	gint32 ievreg;
	gint32 ievtyp;
	gint32 iqual;
	gint32 isynth;
        gint32 blank7[SACTEMP7];
	guint32 leven;	/* =1 for evenly spaced data */
	guint32 lpspol;	/* =1 for correct polarity, 0 for reversed */
	guint32 lovrok;
	guint32 lcalda;
	guint32 lblank1;

	/* character string fields */
	char kstnm[K_LEN];	/* station name (blank padded) */
	char kevnm[KEVNMLEN];	/* event name (UofW pickfile name) */
	char khole[K_LEN];
	char ko[K_LEN];		/* Origin time lable */
	char ka[K_LEN];		/* First arrival time lable */
	char kt0[K_LEN];	/* S-wave time lable */
	char kt1[K_LEN];
	char kt2[K_LEN];
	char kt3[K_LEN];
	char kt4[K_LEN];
	char kt5[K_LEN];
	char kt6[K_LEN];
	char kt7[K_LEN];
	char kt8[K_LEN];
	char kt9[K_LEN];
	char kf[K_LEN];
	char kuser0[K_LEN];
	char kuser1[K_LEN];
	char kuser2[K_LEN];
	char kcmpnm[K_LEN];
	char knetwk[K_LEN];
	char kdatrd[K_LEN];
	char kinst[K_LEN];
};

/*	This is a structure of the same size as 'SAChead', but in a form
	that is easier to initialize
*/

#define NUM_FLOAT	70	/* number of floats in header */
#define MAXINT		35	/* number of ints in header */
#define MAXSTRING	24	/* number of strings in header */

struct SAChead2 {
	float SACfloat[NUM_FLOAT];
	gint32 SACint[MAXINT];
	guint32 SACun[5];
	char SACstring[MAXSTRING][K_LEN];
};

/* eof sachead.h */
#endif
