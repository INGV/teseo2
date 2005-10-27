
SHELL = /bin/sh

all:
	g77 1_arm-shift-value-en.f -o 1_arm-shift-value-en
	g77 2_arm-shift-slope-en.f -o 2_arm-shift-slope-en
	g77 3_geometrical-corr-en.f -o 3_geometrical-corr-en
	g77 -c 1_arm-shift-value-en_sub.f
	g77 1_arm-shift-value-en_sub.o driver1.f -o driver1
	g77 -c 2_arm-shift-slope-en_sub.f
	g77 2_arm-shift-slope-en_sub.f driver2.f -o driver2
	g77 -c 3_geometrical-corr-en_sub.f
	g77 3_geometrical-corr-en_sub.o driver3.f -o driver3
	g77 1_arm-shift-value-en.f -o 1_arm-shift-value-en
	g77 2_arm-shift-slope-en.f -o 2_arm-shift-slope-en
	g77 -c arm-shift-en_sub.f
	g77 arm-shift-en_sub.o my_driver1.f -o my_driver1
	g77 -c arm-shift-slope_sub.f
	g77 arm-shift-slope_sub.o my_driver2.f -o my_driver2

clean:
	rm -f 1_arm-shift-value-en  2_arm-shift-slope-en 3_geometrical-corr-en arm-shift-en_sub
	
        
