         subroutine arm_shift_value_en_sub(fiche, sortie, xinitial,
     +         xfinal,yi,yf,sec, Bg,r, Rg, a, b, ret)
c
c    Variables passed in the routine:
c
c 	fiche: 	Name of input file
c 	sortie: Name of output file
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

	character*20 fiche
	character*20 sortie
	character*20 sortie2

	integer i,cpt

	real yi,yf
	real sec,alp,ret
	real x(15000),y(15000),xx(15000),yy(15000)
	real xa(15000),ya(15000)
	real xinitial,xfinal
	real Bg,r,Rg,a,b,xtest 

	sortie2='ret2-b'

	open(4,file=sortie2)

 
	open(1,file=fiche,status='old')
	read(1,*,end=1000) (x(i),y(i),i=1,15000)
 1000 npt1=i-1

      b=-40.
      do 77 cpt=1,600
         b=b+.1

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

c	Count, after the whole corrections including b, the number of point 
c	which has a time younger that the previous point (wrong points) 


	xtest=x1
	ret=0.

	open(2,file=sortie)


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

      close(1)
      close(2)


 
	open(2,file=sortie)
	do 10 i=1,15000
	   xx(i)=0.
	   yy(i)=0.
 10	continue
	read(2,*,end=200) (xx(i),yy(i),i=1,npt1)
 200  npt2=i-1
      close(2)

c	 write the value b and the number of wrong points
 
c            write(4,*) b,ret
 
77    continue
      return 
      end


