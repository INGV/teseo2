	program driver1
c
c this driver program calls the 1_arm-shift-value-en_sub routine written
c by Antoine Schlupp
c
	character*20 fiche
	character*20 sortie

        PARAMETER (max_eval=600)
        PARAMETER (max_n_points=15000)

	real yi,yf
	real sec,alp,ret
	real xinitial,xfinal
	real Bg,r,Rg,a,b,xtest
        DIMENSION rinXY(2,max_n_points)
	real  rin
	DIMENSION ret_b_e(2,max_eval)

        sortie='ret3-b'

	write(*,*)'Name of input file'
	read(*,'(A)') fiche
c	write(*,*)'Name of output file'
c	read(*,'(A)') sortie
	write(*,*)'enter the two extremes points of the input file, xinitial xfinal yi yf, along the base line:'
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
     *value positive if the arm is inclined downward
     *in mm :'
	read(*,*)  b
	open(11,file=fiche,status='old',ERR=777,IOSTAT=iostatus)

	if ( iostatus .eq. 0) then
		write(*,*) 'Reading in file'
		read(11,*,end=1000) (rinXY(1,i),rinXY(2,i),i=1,max_n_points)
	endif
 1000 npt1=i-1

	write(*,*) 'Calling sub ',npt1
c
      call arm_shift_en_sub(rinXY, npt1,  xinitial, xfinal,
     +         yi, yf, sec, Bg, r, Rg, a, b, ret_b_e, max_eval)
c
      open(14,file=sortie,status='unknown',IOSTAT=iostatus,ERR=778)
	write(*,*) 'Writing output'
	do 77 i=1,max_eval
	if ( iostatus .eq. 0) then
		write(14,*) ret_b_e(1,i),ret_b_e(2,i)
	endif

77    continue

	if(iostatus .eq. 0) then
		stop
	endif

777   write(*,*) 'Error opening file ', fiche , iostatus
778   write(*,*) 'Error opening file ', sortie, iostatus

      stop
      end
