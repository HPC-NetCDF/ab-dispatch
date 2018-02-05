#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([AB-Dispatch], [1.0.0], [])
AM_INIT_AUTOMAKE([foreign dist-zip subdir-objects])
AC_CONFIG_SRCDIR([include/abdispatch.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Set up libtool.
AC_MSG_NOTICE([setting up libtool])
LT_PREREQ([2.2])
LT_INIT()

# Checks for programs.
AC_PROG_CC

AC_DEFINE([USE_HDF4], 1, [Set for netCDF headers.])
AC_DEFINE([USE_NETCDF4], 1, [Set for netCDF headers.])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 test/Makefile
                 src/Makefile])
AC_OUTPUT
