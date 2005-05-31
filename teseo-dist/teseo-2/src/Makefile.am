## Process this file with automake to produce Makefile.in

bin_PROGRAMS = teseo-2

bindir = $(GIMP_LIBDIR)/plug-ins

teseo_2_SOURCES = \
	teseo_io.c	\
	teseo_io.h	\
	teseo_path.c	\
	teseo_path.h	\
	teseo_env.c	\
	teseo_env.h	\
	types.h	\
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
	render.h	\
	teseo_bezier_point.c	\
	teseo_bezier_point.h	\
	teseo_resample.c	\
	teseo_resample.h


AM_CPPFLAGS = \
	-DLOCALEDIR=\""$(LOCALEDIR)"\"		\
	-DDATADIR=\""$(DATADIR)"\"

INCLUDES =\
	-I$(top_srcdir)		\
	@GIMP_CFLAGS@		\
	-I$(includedir)

LDADD = $(GIMP_LIBS)

