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
	teseo_resample.h	\
	teseo_bezier_fit.c	\
	teseo_bezier_fit.h	\
	teseo_session.c	\
	teseo_session.h \
	teseo_lock.c	\
	teseo_lock.h \
	teseo_wmean.h \
	teseo_wmean.c \
	teseo_main.c \
	teseo_main.h \
	teseo_gimp_extends.c \
	teseo_gimp_extends.h \
	teseo_snap.c	\
	teseo_snap.h	\
	teseo_sac.c	\
	teseo_sac.h	\
	teseo_timemark.c	\
	teseo_timemark.h	\
	nrutil.h \
	nrutil.c \
	nr.h \
	brent.c f1dim.c linmin.c mnbrak.c powell.c 

AM_CPPFLAGS = \
	-DLOCALEDIR=\""$(LOCALEDIR)"\"		\
	-DDATADIR=\""$(DATADIR)"\"

INCLUDES =\
	-I$(top_srcdir)		\
	@GIMP_CFLAGS@		\
	@GTKADDONS_CFLAGS@		\
	-I$(includedir)

LDADD = $(GIMP_LIBS) \
	$(GTKADDONS_LIBS)

