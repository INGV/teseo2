         subroutine arm_shift_en_sub( rXY, nPoints,  xinitial, xfinal,
     +         yi, yf, sec, Bg, r, Rg, a, b, ret_b_e, max_val)
c
c    Variables passed in the routine:
c
c	inXY array of coordinates of input points
c	nPoints dimension of inXY
c
c 	xinitial,xfinal,yi,yf: 	the two extremes points of the input file ( xinitial xfinal yi yf) along the base line:
c 	sec: 	number of seconds between  xinitial and xfinal
c
c	Correction of the curvature of the signal due to the geometry
c	of the Wiechert seismograph recorder

c       Bg: 	The velocity of the paper in mm/min
c	r:	Radius of the cylinder that carry the paper, in mm.
c	Rg: 	Length of the writting arm from its rotation axe until the end of the needle in mm.
c	a: 	Distance from the rotation axe of the writing arm until the axe of the cylinder that carry the paper
c               measured perpendicular to the cylinder axe in mm :'
c	b:      Enter the shift between the needle position at rest and its
c               normal equilibrium position (arm perpendicular to the axe of the cylinder)
c               value positive if the arm is inclined downward in mm.
c
c	ret_b_e output 2-dim array: calculated b, errors
c
c	max_val dimension of ret_b_e
c
c	****************************************************
c	*** Search of value b(shift of the writing arm in mm) ***
c	*** 									 ***
c	*** include the 							***
c	*** correction of the seismogram base line ( angle ***
c	*** of the data do to a not exact digitizing with    ***
c	*** x coordinates along the base line of the seismogram) ***
c	***  + deformation of signal due to  Wiechert recorder ***
c	****************************************************

c 	=========================
c	VARIABLES
c 	=========================

	integer nPoints
	integer max_val
	REAL rXY(2,nPoints)
	REAL ret_b_e(2,max_val)

	integer i,cpt,iostatus

	real yi,yf
	real sec,alp,ret
	real x(nPoints),y(nPoints),xx(nPoints),yy(nPoints)
	real xa(nPoints),ya(nPoints)
	real xinitial,xfinal
	real Bg,r,Rg,a,b,xtest

        npt1 = nPoints

c	write(*,*) 'Entering sub', npt1

	b=-40.

	do 77 cpt=1,max_val
	b=b+.1

c	Change the origin of the file to the first point of the base line
c	(necessary if different from 0)
c	write(*,*) 'Before shifting'

	do 5 i=1,npt1
	  xa(i)=rXY(1,i)-xinitial
	  ya(i)=rXY(2,i)-yi
c	write(*,*) 'shifting', xa(i), ya(i), i
5	continue

c	Calculation of the correction angle of the base line in radian
c	angle positive if the correction to apply is counter clockwise
c	(this is only the correction of an angle due to the digitizing process)

	alp=atan((yf-yi)/(xfinal-xinitial))

c	Correction of input file of the rotation alpha
c	write(*,*) 'Correction of input file of the rotation alpha'
	do 6 i=1,npt1
	   xx(i)=+xa(i)*cos(alp)+ya(i)*sin(alp)
	   yy(i)=-xa(i)*sin(alp)+ya(i)*cos(alp)
6	continue

      x1=60./Bg*(Bg*sec/(xfinal-xinitial)/60.*xx(1)-
     &     r*asin((r*r+a*a-Rg*Rg+
     &      ((Bg*sec/(xfinal-xinitial)/60.)*yy(1)-b)**2.)/2./a/r)+
     &     r*asin((r*r+a*a-Rg*Rg+b*b)/2./a/r))

c	Count, after the whole corrections including b, the number of point
c	which has a time younger that the previous point (wrong points)

	xtest=x1
	ret=0.
c	write(*,*) 'Counting wrong points'
	do 20 i=1,npt1
 	xx(i)=60./Bg*(Bg*sec/(xfinal-xinitial)/60.*xx(i)-
     &     r*asin((r*r+a*a-Rg*Rg+
     &	((Bg*sec/(xfinal-xinitial)/60.)*yy(i)-b)**2.)/2./a/r)+
     &     r*asin((r*r+a*a-Rg*Rg+b*b)/2./a/r))
	if((xx(i)-xtest).ge.0.001) then
	      xtest=xx(i)
		 else
		ret=ret+1
	endif

20	continue

	do 10 i=1,nPoints
	   xx(i)=0.
	   yy(i)=0.
10	continue

	npt2=i-1


c	 write the value b and the number of wrong points cpt is the counter

	ret_b_e(1,cpt) = b
	ret_b_e(2,cpt) = ret

77    continue

      return
      end


