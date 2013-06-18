#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.54)

AC_CONFIG_MACRO_DIR([m4])

dnl Name your plug-in here
m4_define([full_package_name], [newuoa])

dnl These three define the plug-in version number
m4_define([major_version], [0])
m4_define([minor_version], [1])
m4_define([micro_version], [0])

m4_define([version],
          [major_version.minor_version.micro_version])

m4_define([bug_report_address], [matteo.quintiliani@ingv.it])

AC_INIT([full_package_name], [version], [bug_report_address])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

AM_PROG_LIBTOOL

AC_PROG_F77
AC_F77_LIBRARY_LDFLAGS


# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB Using `AC_PROG_RANLIB' is rendered obsolete by `AC_PROG_LIBTOOL'
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([malloc.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memchr memset])


AC_CONFIG_FILES([Makefile
                 src/Makefile
		 newuoa.pc
		 ])
AC_OUTPUT

AC_MSG_NOTICE([
	       Fortran compiler: $F77
	       Fortran library: $FLIBS
	       ])
