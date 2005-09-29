#include <stdio.h>
#include "cfortran.h"
#include <math.h>

/*
C
C     The Chebyquad test problem (Fletcher, 1965) for N = 2,4,6 and 8,
C     with NPT = 2N+1.
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION X(10),W(10000)
      IPRINT=2
      MAXFUN=5000
      RHOEND=1.0D-6
      DO 30 N=2,8,2
      NPT=2*N+1
      DO 10 I=1,N
   10 X(I)=DFLOAT(I)/DFLOAT(N+1)
      RHOBEG=0.2D0*X(1)
      PRINT 20, N,NPT
   20 FORMAT (//4X,'Results with N =',I2,' and NPT =',I3)
      CALL NEWUOA (N,NPT,X,RHOBEG,RHOEND,IPRINT,MAXFUN,W)
   30 CONTINUE
      STOP
      END
*/

/*
CALL NEWUOA (N,NPT,X,RHOBEG,RHOEND,IPRINT,MAXFUN,W)
parametri
N intero
NPT intero
X Vettore di double
RHOBEG double
RHOEND double
IPRINT intero pari a2
MAXFUN intero 5000
W Vettore di double
*/

 PROTOCCALLSFSUB8(NEWUOA,newuoa,INT, INT, PDOUBLE, DOUBLE, DOUBLE, INT, INT, PDOUBLE)
#define NEWUOA(A,B,C,D,E,F,G,H) CCALLSFSUB8(NEWUOA,newuoa, INT, INT, PDOUBLE, DOUBLE, DOUBLE, INT, INT, PDOUBLE, A,B,C,D,E,F,G,H)

void main(int argc, char* argv[]){
	int N=2;
        double X[10];
	double W[10000]={};
	int IPRINT=2;
	int MAXFUN=5000;
	double RHOEND=1.0E-6;
	double RHOBEG=1.0E-6;
	int i,n,NPT;

	printf ("\nC driver created by teseo2 authors\n");
	for (n=2;n<=8;n++,n++){
		NPT=2*n+1;
		for (i=0;i<10;i++){
        		X[i]=((double) i)/((double) (n+1));
		}
		RHOBEG=0.2*X[1];
		printf("\n\nResults with N=%d\tNPT=%d\n",n,NPT);
		NEWUOA (n,NPT,X,RHOBEG,RHOEND,IPRINT,MAXFUN,W);
	}
}