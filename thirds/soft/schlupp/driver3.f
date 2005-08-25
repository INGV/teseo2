      program driver3
c
c this driver program calls the geometrical_corr_en_sub routine written
c by Antoine Schlupp
c

c 	=========================
c	VARIABLES
c 	=========================

	character*20 fiche
	character*20 sortie

	integer i,k

	real yi,yf
	real sec,alp
c	real x(15000),y(15000),xx(15000),yy(15000)
c	real xa(15000),ya(15000),n(200),alpha(15000)
	real xinitial,xfinal
	real Bg,r,Rg,a,b

 	write(*,*) 'Name of input file'
	read(*,*) fiche
 	write(*,*)'Name of output file'
	read(*,*) sortie
 	write(*,*) 'enter the two extremes points of the input file, * xinitial xfinal yi yf, along the base line:'
	read(*,*) xinitial,xfinal,yi,yf
 	write(*,*) 'number of seconds between  xinitial and xfinal:'
	read(*,*) sec  
 
c	Correction of the curvature of the signal due to the geometry  
c	of the Wiechert seismograph recorder


	write(*,*) 'Enter the velocity of the paper in mm/min :'
	read(*,*)  Bg
	write(*,*) 'Enter the radius of the cylinder that carry the paper, in mm:'
	read(*,*)  r
	write(*,*) 'Enter the length of the writting arm from its rotation axe 
     *until the end of the needle in mm :'
	read(*,*)  Rg	
	write(*,*) 'Enter the distance from the rotation axe of the writing arm 
     *until the axe of the cylinder that carry the paper 
     *measured perpendicular to the cylinder axe in mm :'
	read(*,*)  a
	write(*,*) 'Enter the shift between the needle position at rest and its 
     *normal equilibrium position (arm perpendicular to the axe of the cylinder)
     * value positive if the arm is inclined downward  
     *  in mm :'
	read(*,*)  b
 
c
      call geometrical_corr_en_sub(fiche,sortie,xinitial,
     +            xfinal,yi,yf,sec, Bg,r, Rg, a, b)
c
      stop
      end

