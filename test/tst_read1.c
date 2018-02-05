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
   int ncid;
   int ndims, nvars, natts, unlimdimid;
   int ret;
   
   printf("\nTesting AB format dispatch layer...");
   if ((ret = nc_def_user_format(NC_UF0, &AB_dispatcher, NULL)))
      return ret;

   if ((ret = nc_open(TEST_FILE, NC_UF0, &ncid)))
      return ret;
   if ((ret = nc_inq(ncid, &ndims, &nvars, &natts, &unlimdimid)))
      return ret;
   /* printf("ndims %d nvars %d natts %d unlimdimid %d\n", ndims, nvars, natts, unlimdimid); */
   if (ndims != 4 || nvars != 6 ||natts !=10 || unlimdimid != -1)
       return 111;
   if ((ret = nc_close(ncid)))
      return ret;

   printf("SUCCESS!\n");
   return 0;
}

