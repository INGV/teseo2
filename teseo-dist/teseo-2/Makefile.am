## Process this file with automake to produce Makefile.in

SUBDIRS = po src help

EXTRA_DIST = \
	BUGS			\
	HACKING			\
	autogen.sh		\
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in	\
	pixmaps/brain_mri_small_invert_gray.xpm	\
	pixmaps/brain_mri_small_invert.xpm	\
	pixmaps/brain_mri_small.xpm


DISTCLEANFILES = \
	intltool-extract	\
	intltool-merge		\
	intltool-update

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi
