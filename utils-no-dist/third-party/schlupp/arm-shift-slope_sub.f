      subroutine arm_shift_slope_sub(rXY, nPoints, xinitial, xfinal,
     +           yi, yf, sec, Bg, r, Rg, a, b, ret_b_sl_e, max_val)
c	****************************************************
c	*** Calculation of the histogram of the slope at ***
c	*** each point of the seismograme function of b value	***
c	*** (arm shift) 				***
c	*** input format = x, y                     	***
c	****************************************************
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
c	Rg: 	Length of the writing arm from its rotation axe until the end of the needle in mm.
c	a: 	Distance from the rotation axe of the writing arm until the axe of the cylinder that carry the paper
c               measured perpendicular to the cylinder axe in mm :'
c	b:      Maximum shift (positive value in mm) to be tested,
c               between the needle position at rest and its normal equilibrium position
c              (arm perpendicular to the axe of the cylinder) :'
c


c 	=========================
c	VARIABLES
c 	=========================

        PARAMETER (max_slope=180)

	integer nPoints
	integer max_val
	REAL rXY(2,nPoints)
	REAL ret_b_e(2,max_val)


	integer i,cpt,b2

	real yi,yf,pir
	real sec,alp
	real x(nPoints),y(nPoints),xx(nPoints),yy(nPoints)
	real xa(nPoints),ya(nPoints),alpha(nPoints)
c WARNING only to insure to access in vector bounds always
	real n(0:200)
	real xinitial,xfinal
	real Bg,r,Rg,a,b


c	Correction of the curvature of the signal due to the geometry
c	of the Wiechert seismograph recorder


	open(1,file=fiche,status='old')
	read(1,*,end=1000) (x(i),y(i),i=1,nPoints)
1000    npt1=i-1


        sortie2='slope'

      b2=b
      b=-b-1
      do 77 cpt=1,b2*2
         b=b+1


      open(4,file=sortie2)

c	Change the origin of the file to the first point of the base line
c	(necessary if different from 0)

	do 5 i=1,npt1
 	   xa(i)=x(i)-xinitial
 	   ya(i)=y(i)-yi
 5	continue

c	Calculation of the correction angle of the base line in radian
c	angle positive if the correction to apply is counter clockwise
c	(this is only the correction of an angle due to the digitizing process)

 	alp=atan((yf-yi)/(xfinal-xinitial))

c	Correction of input file of the rotation alpha

	do 6 i=1,npt1
	   xx(i)=+xa(i)*cos(alp)+ya(i)*sin(alp)
	   yy(i)=-xa(i)*sin(alp)+ya(i)*cos(alp)
 6	continue

      x1=60./Bg*(Bg*sec/(xfinal-xinitial)/60.*xx(1)-
     &     r*asin((r*r+a*a-Rg*Rg+
     &      ((Bg*sec/(xfinal-xinitial)/60.)*yy(1)-b)**2.)/2./a/r)+
     &     r*asin((r*r+a*a-Rg*Rg+b*b)/2./a/r))

c	*** next step: Correction of the curvature of the data including
c	the ��b�� shift of  the writing arm ***


c *** next step: time correction ***

	open(2,file=sortie)
	do 20 i=1,npt1

 	xx(i)=60./Bg*(Bg*sec/(xfinal-xinitial)/60.*xx(i)-
     &     r*asin((r*r+a*a-Rg*Rg+
     &	((Bg*sec/(xfinal-xinitial)/60.)*yy(i)-b)**2.)/2./a/r)+
     &     r*asin((r*r+a*a-Rg*Rg+b*b)/2./a/r))


c  	*** warning�: the polarity of data must be checked.  If inversion of
c	polarity necessary, use the formula here under.***
c	   y(i)=-((Bg*sec/(xfinal-xinitial)/60)*y(i))



	   xx(i)=xx(i)-x1
	   yy(i)=(Bg*sec/(xfinal-xinitial)/60.)*yy(i)

c	 output file in time (sec) and amplitude (mm)

	   write(2,*) xx(i),yy(i)
20	continue

      close(1)
      close(2)

c	Count the histogram of each slope

      do 54 j=-90,90
      n(j+90)=0.
54    continue

	pir=asin(0.5)
	do 11 i=1,npt1-1
	alpha(i)=(30/pir)*atan((xx(i+1)-xx(i))/(yy(i+1)-yy(i)))

	do 44 j=-90,90
	if (alpha(i).le.(j+1).and.alpha(i).gt.(j-1)) then
	n(j+90)=n(j+90)+1
	endif
44 	continue

11    continue

C	*** Write in output file, b, slope, number of points

	do 58 j=1,max_slope
            write(4,*) b,j,n(j)
58	continue
		close(4)

77    continue

	end



