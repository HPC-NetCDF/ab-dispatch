#                                               -*- Autoconf -*-
# This is the autoconf script for the AB Dispatch library, which
# allows netCDF to read AB format files.
#
# Ed Hartnett

# Set up autoconf.
AC_PREREQ([2.69])
AC_INIT([AB-Dispatch], [1.0.0], [])
AC_CONFIG_SRCDIR([include/abdispatch.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Set up automake.
AM_INIT_AUTOMAKE([foreign dist-zip subdir-objects])

# Set up libtool.
AC_MSG_NOTICE([setting up libtool])
LT_PREREQ([2.2])
LT_INIT()

# Checks for programs.
AC_PROG_CC

# Set these to get correct results from netCDF header files.
AC_DEFINE([USE_HDF4], 1, [Set for netCDF headers.])
AC_DEFINE([USE_NETCDF4], 1, [Set for netCDF headers.])

# Does the user want to enable netcdf-4 logging?
AC_MSG_CHECKING([whether logging is enabled])
AC_ARG_ENABLE([logging],
              [AS_HELP_STRING([--enable-logging],
                              [Enable logging capability - for developers only  .])])
test "x$enable_logging" = xyes || enable_logging=no
AC_MSG_RESULT([$enable_logging])
if test "x$enable_logging" = xyes; then
   AC_DEFINE([AB_LOGGING], 1, [If true, turn on AB logging.])
   AC_DEFINE([LOGGING], 1, [If true, turn on netCDF logging.])
fi

# These are data files needed for testing.
AC_CONFIG_LINKS([test/surtmp_100l.a:test/surtmp_100l.a])
AC_CONFIG_LINKS([test/surtmp_100l.b:test/surtmp_100l.b])

# Checks for libraries.
dnl AC_CHECK_LIB(netcdf, nc_def_user_format)
dnl if test x${ac_cv_lib_netcdf_nc_def_user_format} != xyes; then
dnl    AC_MSG_ERROR([Must have netcdf])
dnl fi

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 src/Makefile])
AC_OUTPUT
