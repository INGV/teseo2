
SHELL = /bin/sh
INCLUDEDIROPT = "-I/sw/include"
LIBDIROPT = "-L/sw/lib"

all:
	gcc ${INCLUDEDIROPT} ${LIBDIROPT} 1_arm-shift-value-en.c -o 1_arm-shift-value-en -lf2c
	gcc ${INCLUDEDIROPT} ${LIBDIROPT} 2_arm-shift-slope-en.c -o 2_arm-shift-slope-en -lf2c
	gcc ${INCLUDEDIROPT} ${LIBDIROPT} 3_geometrical-corr-en.c -o 3_geometrical-corr-en -lf2c
	
clean:
	rm -f 1_arm-shift-value-en  2_arm-shift-slope-en 3_geometrical-corr-en
	
        
