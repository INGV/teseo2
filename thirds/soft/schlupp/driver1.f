	program driver1
c
c this driver program calls the 1_arm-shift-value-en_sub routine written
c by Antoine Schlupp
c
	character*20 fiche
	character*20 sortie
c	character*20 sortie2

c	integer i,cpt

	real yi,yf
	real sec,alp,ret
c	real x(15000),y(15000),xx(15000),yy(15000)
c	real xa(15000),ya(15000)
	real xinitial,xfinal
	real Bg,r,Rg,a,b,xtest

	write(*,*)'Name of input file'
	read(*,'(A)') fiche
	write(*,*)'Name of output file'
	read(*,'(A)') sortie
	write(*,*)'enter the two extremes points of the input file, * xinitial xfinal yi yf, along the base line:'
	read(*,*) xinitial,xfinal,yi,yf
	write(*,*)'number of seconds between  xinitial and xfinal:'
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
      call arm_shift_value_en_sub(fiche, sortie, xinitial,
     +         xfinal,yi,yf,sec, Bg,r, Rg, a, b, ret)
c
      stop
      end
