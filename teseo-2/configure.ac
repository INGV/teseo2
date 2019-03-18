dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.54)

AC_CONFIG_MACRO_DIR([m4])

dnl Name your plug-in here
m4_define([plugin_name], [teseo-2])

dnl These three define the plug-in version number
m4_define([plugin_major_version], [2])
m4_define([plugin_minor_version], [8])
m4_define([plugin_micro_version], [0])

m4_define([plugin_version],
          [plugin_major_version.plugin_minor_version.plugin_micro_version])

m4_define([bug_report_address], [matteo.quintiliani@ingv.it])

AC_INIT([plugin_name], [plugin_version], [bug_report_address])

AC_DEFINE(PLUGIN_NAME, plugin_name, [Plug-In name])
AC_DEFINE(PLUGIN_VERSION, plugin_version, [Plug-In version])

AC_DEFINE(PLUGIN_MAJOR_VERSION, plugin_major_version, [Plug-In major version])
AC_DEFINE(PLUGIN_MINOR_VERSION, plugin_minor_version, [Plug-In minor version])
AC_DEFINE(PLUGIN_MICRO_VERSION, plugin_micro_version, [Plug-In micro version])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h malloc.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor memchr memset mkdir setlocale sqrt strpbrk strstr strtol bind_textdomain_codeset])


ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

AM_MAINTAINER_MODE


dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl


GIMP_REQUIRED_VERSION=2.2.0

PKG_CHECK_MODULES(GIMP,
  gimp-2.0 >= $GIMP_REQUIRED_VERSION gimpui-2.0 >= $GIMP_REQUIRED_VERSION)

AC_SUBST(GIMP_CFLAGS)
AC_SUBST(GIMP_LIBS)

GIMP_LIBDIR=`$PKG_CONFIG --variable=gimplibdir gimp-2.0`
AC_SUBST(GIMP_LIBDIR)

#########################################

GTKADDONS_REQUIRED_VERSION=0.1
PKG_CHECK_MODULES([GTKADDONS], [gtk-addons >= $GTKADDONS_REQUIRED_VERSION]) 
AC_SUBST(GTKADDONS_CFLAGS)
AC_SUBST(GTKADDONS_LIBS)
GTKADDONS_LIBDIR=`$PKG_CONFIG --variable=libdir gtk-addons`
AC_SUBST(GTKADDONS_LIBDIR)

#########################################

NEWUOA_REQUIRED_VERSION=0.1
PKG_CHECK_MODULES([NEWUOA], [newuoa >= $NEWUOA_REQUIRED_VERSION]) 
AC_SUBST(NEWUOA_CFLAGS)
AC_SUBST(NEWUOA_LIBS)
NEWUOA_LIBDIR=`$PKG_CONFIG --variable=libdir newuoa`
AC_SUBST(NEWUOA_LIBDIR)

#########################################

GTKDATABOX_REQUIRED_VERSION=0.9.0.2
PKG_CHECK_MODULES([GTKDATABOX], [gtkdatabox >= $GTKDATABOX_REQUIRED_VERSION]) 
AC_SUBST(GTKDATABOX_CFLAGS)
AC_SUBST(GTKDATABOX_LIBS)
GTKDATABOX_LIBDIR=`$PKG_CONFIG --variable=libdir gtkdatabox`
AC_SUBST(GTKDATABOX_LIBDIR)

#########################################


dnl i18n stuff

GETTEXT_PACKAGE=gimp20-plugin-template
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
				    [The gettext translation domain.])

dnl Add the languages which your application supports here.
ALL_LINGUAS="az de fr sk sv zh_TW"

AC_PROG_INTLTOOL
AM_GLIB_GNU_GETTEXT

# TODO datadir should be required

LOCALEDIR="${datadir}/locale"
DATADIR="${datadir}/plugin_name"

AC_SUBST(LOCALEDIR)
AC_SUBST(DATADIR)


AC_MSG_CHECKING([if GTK+ is version 2.7.0 or newer])
if $PKG_CONFIG --atleast-version=2.7.0 gtk+-2.0; then
  have_gtk_2_7=yes
else
  have_gtk_2_7=no
fi
AC_MSG_RESULT($have_gtk_2_7)

if test "x$have_gtk_2_7" != "xyes"; then
  CPPFLAGS="$CPPFLAGS -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED"
fi


AC_MSG_CHECKING([if GIMP is version 2.3.0 or newer])
if $PKG_CONFIG --atleast-version=2.3.0 gimp-2.0; then
  have_gimp_2_3=yes
else
  have_gimp_2_3=no
fi
AC_MSG_RESULT($have_gimp_2_3)

if test "x$have_gimp_2_3" != "xyes"; then
  CPPFLAGS="$CPPFLAGS -DGIMP_DISABLE_DEPRECATED"
fi


AC_CONFIG_FILES([
Makefile
src/Makefile
po/Makefile.in
help/Makefile
help/en/Makefile
help/images/Makefile
])

AC_OUTPUT


# AC_MSG_NOTICE([
	       # datadir: $datadir
	       # datadir: $DATADIR
	       # ])
