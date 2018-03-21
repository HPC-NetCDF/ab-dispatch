# This is part of the AB Dispatch package, which allow the netCDF C
# library to read and write the HYCOM AB format.

# This automake file generates the Makefile for the AB dispach layer
# source directory.

# Ed Hartnett

AM_CPPFLAGS = -I$(top_srcdir)/include 

# This is our output. 
lib_LTLIBRARIES = libncab.la
libncab_la_LDFLAGS = -version-info 1:0:0
libncab_la_SOURCES = abdispatch.c abattr.c abdim.c abfile.c abgrp.c	\
abtype.c abvar.c abfunc.c ablogging.c





