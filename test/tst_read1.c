/* Test read of AB format with netCDF. 
*
* Ed Hartnett */

#include <config.h>
#include <netcdf.h>
#include "abdispatch.h"
#include <nc4dispatch.h>
#include <stdio.h>

#define TEST_FILE "MOD29.A2000055.0005.005.2006267200024.hdf"

extern NC_Dispatch AB_dispatcher;

int
main()
{
   int ret;
   
   printf("howdy!\n");
   if ((ret = nc_def_user_format(NC_UF0, &AB_dispatcher, NULL)))
      return ret;
   
   return 0;
}

