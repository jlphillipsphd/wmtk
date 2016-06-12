#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(WMtk, 1.0.5, joshua.l.phillips@vanderbilt.edu)
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# All code is in C++
AC_LANG([C++])

# Checks for libraries
AC_CHECK_LIB([m],[pow],,[AC_MSG_ERROR([Missing Math Library...])])

# Checks for header files.
AC_CHECK_HEADERS([stddef.h math.h stdlib.h stdio.h iostream fstream string list vector sstream iomanip],,
			   [AC_MSG_ERROR([Missing Required Header File...])])

# Checks for library functions.
AC_CHECK_FUNCS([log pow sqrt exp floor nearbyint],,[AC_MSG_ERROR([Math Library Missing Required Funcion...])])
AC_CHECK_FUNCS([rand],,[AC_MSG_ERROR([Missing Required Funcion in <stdlib.h>...])])

AC_CONFIG_FILES([Makefile
                 src/Makefile
		 doc/Makefile])
AC_OUTPUT
