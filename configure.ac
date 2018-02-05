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

# Checks for libraries.
AC_CHECK_LIB(netcdf, nc_def_user_format)
if test x${ac_cv_lib_netcdf_nc_def_user_format} != xyes; then
   AC_MSG_ERROR([Must have netcdf])
fi

# Needed for HDF4, will be eliminated when AB format is used.
AC_CHECK_LIB([df], [Hclose], [], [AC_MSG_ERROR([Can't find or link to the hdf4 df library. See config.log for errors.])])
AC_CHECK_LIB([mfhdf], [SDcreate], [], [AC_MSG_ERROR([Can't find or link to the hdf4 mfhdf library. See config.log for errors.])])
AC_CHECK_LIB([jpeg], [jpeg_set_quality], [], [AC_MSG_ERROR([Can't find or link to the jpeg library (required by hdf4). See config.log for errors.])])

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
