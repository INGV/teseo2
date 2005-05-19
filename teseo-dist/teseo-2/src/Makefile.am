## Process this file with automake to produce Makefile.in

bin_PROGRAMS = teseo-2

bindir = $(GIMP_LIBDIR)/plug-ins

teseo_2_SOURCES = \
	plugin-intl.h	\
	pixmaps/brain_mri_small_invert_gray.xpm	\
	pixmaps/brain_mri_small_invert.xpm	\
	pixmaps/brain_mri_small.xpm	\
	teseocallbacks.c	\
	teseocallbacks.h	\
	teseosupport.c	\
	teseosupport.h	\
	teseointerface.c	\
	teseointerface.h	\
	interface.c	\
	interface.h	\
	main.c		\
	main.h		\
	render.c	\
	render.h


AM_CPPFLAGS = \
	-DLOCALEDIR=\""$(LOCALEDIR)"\"		\
	-DDATADIR=\""$(DATADIR)"\"

INCLUDES =\
	-I$(top_srcdir)		\
	@GIMP_CFLAGS@		\
	-I$(includedir)

LDADD = $(GIMP_LIBS)

