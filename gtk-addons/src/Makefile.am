## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES =\
	 -I../include\
	@GIMP_CFLAGS@		

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = libgtk-addons.la

libgtk_addons_la_SOURCES = \
	gtkaddons.h\
	gtkaddons.c

libgtk_addonsincludedir = ${prefix}/include/gtk-addons
libgtk_addonsinclude_DATA = \
    gtkaddons.h


libgtk_addons_la_LDFLAGS = 

libgtk_addons_la_LIBADD = \
	 $(GIMP_LIBS)


#	 -lgtk -lgimp -lgimpui \
