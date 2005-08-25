       subroutine geometrical_corr_en_sub(fiche,sortie,xinitial,
     +            xfinal,yi,yf,sec, Bg,r, Rg, a, b)
c	********************************************************
c	*** correction of the seismogram base line ( angle *** 
c	*** of the data do to a not exact digitizing with    ***
c	*** x coordinates along the base line of the seismogram) ***
c	***  + deformation of signal due to  Wiechert recorder ***
c	*** The output has amplitude in mm and time in seconds ***
c	*** input file format is x(digitizing unit) y(digitizing unit)  ***
c	*** the format of output file is x(sec) y(mm)       ***
c	***                                                    ***
c	****************************************************
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
c	Rg: 	Length of the writing arm from its rotation axe until the end of the needle in mm.
c	a: 	Distance from the rotation axe of the writing arm until the axe of the cylinder that carry the paper 
c               measured perpendicular to the cylinder axe in mm :'
c	b:      Shift between the needle position at rest and its normal equilibrium position 
c               (arm perpendicular to the axe of the cylinder) value positive if the arm 
c               is inclined downward  in mm :'
c    



c 	=========================
c	VARIABLES
c 	=========================

	character*20 fiche
	character*20 sortie
c	character*20 final

	integer i,k
	real yi,yf
	real sec,alp
	real x(15000),y(15000),xx(15000),yy(15000)
	real xinitial,xfinal
	real Bg,r,Rg,a,b,xtest

	open(1,file=fiche,status='old')
	open(2,file=sortie)
c	open(3,file=final)

c	Change the origin of the file to the first point of the base line 
c	(necessary if different from 0) 

	k=0
	do 5 i=1,100000
	   k=k+1
	   read(1,*,end=1000) x(i),y(i)
 	   x(i)=x(i)-xinitial
 	   y(i)=y(i)-yi
 5	continue

c	Calculation of the correction angle of the base line in radian 
c	angle positive if the correction to apply is counter clockwise  
c	(this is only the correction of an angle due to the digitizing process) 

 1000	alp=atan((yf-yi)/(xfinal-xinitial))

c	Correction of input file of the rotation alpha 

	do 6 i=1,k
	   xx(i)=+x(i)*cos(alp)+y(i)*sin(alp)
	   yy(i)=-x(i)*sin(alp)+y(i)*cos(alp)
	   x(i)=xx(i)
	   y(i)=yy(i)
 6	continue

c	Correction of the curvature of the signal due to the geometry  
c	of the Wiechert seismograph recorder

	x(1)=60/Bg*(Bg*sec/(xfinal-xinitial)/60*x(1)-
     &     r*asin((r*r+a*a-Rg*Rg+
     &      ((Bg*sec/(xfinal-xinitial)/60)*y(1)-b)**2)/2./a/r)+
     &     r*asin((r*r+a*a-Rg*Rg+b*b)/2./a/r))

	   y(1)=(Bg*sec/(xfinal-xinitial)/60)*y(1)


	   write(2,*) x(1),y(1)


c	*** next step: Correction of the curvature of the data including 
c	the « b » shift of  the writing arm ***


c *** next step: time correction ***

	xtest=x(1)

	do 20 i=2,k-1

	x(i)=60/Bg*(Bg*sec/(xfinal-xinitial)/60*x(i)-
     &     r*asin((r*r+a*a-Rg*Rg+
     &	((Bg*sec/(xfinal-xinitial)/60)*y(i)-b)**2)/2./a/r)+
     &     r*asin((r*r+a*a-Rg*Rg+b*b)/2./a/r))

C *** next step: to avoid local point corrected backward (in time) from 
C   previous point ***

	   if((x(i)-xtest).le.0.001) then
	      goto 20
         else
	      xtest=x(i)
	   endif


c  	*** warning : the ploarity of data must be checked.  If inversion of 
c	polarity necessary, use the formula here under.***
c	   y(i)=-((Bg*sec/(xfinal-xinitial)/60)*y(i))


c	*** next step: amplitude correction (output in millimetres) ***

	   y(i)=(Bg*sec/(xfinal-xinitial)/60)*y(i)


	   write(2,*) x(i),y(i)
 20	continue


c	*** next step: final change of the origin of the output file ***
 
      k=0
      do 30 i=1,100000
         k=k+1
         read(2,*,end=5000) x(i),y(i)
         x(i)=x(i)-x(1)
	   y(i)=y(i)

	   write(3,*) x(i),y(i)

 30   continue
 

5000	end


