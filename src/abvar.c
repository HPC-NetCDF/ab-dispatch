/**
 * @file @internal Data read functions for the AB dispatch layer.
 *
 * @author Ed Hartnett
 */

#include <nc4internal.h>
#include "nc4dispatch.h"
#include "abdispatch.h"

/**
 * @internal Get coordinate variable data. AB Format coordinate
 * variables are always NC_FLOAT32.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param startp Array of start indicies.
 * @param countp Array of counts.
 * @param mem_nc_type The type of these data after it is read into memory.
 * @param is_long Ignored for HDF4.
 * @param data pointer that gets the data.
 * @param memtype The type of these data after it is read into memory.
 *
 * @returns ::NC_NOERR for success
 * @returns ::NC_ERANGE Range error when converting data.
 * @author Ed Hartnett
 */
static int
get_ab_coord_vara(NC *nc, int ncid, int varid, const size_t *startp,
                  const size_t *countp, void *data, int memtype)
{
   NC_GRP_INFO_T *grp;
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   NC_ATT_INFO_T *att = NULL;
   int range_error = 0;
   /* int i = 0; */
   int ret;

   /* Check inputs. */
   assert(nc);

   /* Find our metadata for this file, group, and var. */
   if ((ret = nc4_find_g_var_nc(nc, ncid, varid, &grp, &var)))
      return ret;
   h5 = (NC_HDF5_FILE_INFO_T *)(nc)->dispatchdata;
   assert(grp && h5 && var && var->name && var->ndims == 1);

   /* Coordinate data is stored in variable attribute. */
   if ((ret = nc4_find_grp_att(grp, 1, TIME_NAME, 0, &att))) 
      return ret;
   assert(att);
   printf("att->len %d\n", att->len);
   float fdata[att->len];
   memcpy(fdata, att->data, att->len * sizeof(float));
   for (int c = 0; c < att->len; c++)
      printf("coord data %d %f\n", c, fdata[c]);

   /* If NC_FLOAT is requested, just copy the data. Otherwise, do type
    * conversion - note that NC_ERANGE may result.*/
   if (memtype == NC_FLOAT)
   {
      memcpy(data, &((float *)att->data)[startp[0]], countp[0] * sizeof(float));
   }
   else
   {
      if ((ret = nc4_convert_type(att->data + startp[0] * sizeof(float), data, NC_FLOAT,
                                  memtype, countp[0] - startp[0], &range_error,
                                  NULL, 0, 0, 0)))
         return ret;
   }

   /* As per netCDF rules, data are converted even if range errors
    * occur. But the function returns an error code for this, which
    * may be ignored by caller. */
   if (range_error)
      return NC_ERANGE;
   
   return NC_NOERR;
}

/**
 * Read an array of values. This is called by nc_get_vara() for
 * netCDF-4 files, as well as all the other nc_get_vara_*
 * functions. HDF4 files are handled as a special case.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param startp Array of start indicies.
 * @param countp Array of counts.
 * @param ip pointer that gets the data.
 * @param memtype The type of these data after it is read into memory.

 * @returns ::NC_NOERR for success
 * @author Ed Hartnett, Dennis Heimbigner
 */
int
AB_get_vara(int ncid, int varid, const size_t *startp,
              const size_t *countp, void *ip, int memtype)
{
   NC *nc;
   NC_GRP_INFO_T *grp;
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   int ret;

   LOG((2, "%s: ncid 0x%x varid %d memtype %d", __func__, ncid, varid,
        memtype));

   if (!(nc = nc4_find_nc_file(ncid, &h5)))
      return NC_EBADID;
   assert(nc);

   /* Find our metadata for this file, group, and var. */
   if ((ret = nc4_find_g_var_nc(nc, ncid, varid, &grp, &var)))
      return ret;
   h5 = nc->dispatchdata;
   assert(grp && h5 && var && var->name);

   /* Coordinate var is handled specially. */
   if (!strcmp(var->name, TIME_NAME))
      return get_ab_coord_vara(nc, ncid, varid, startp, countp, ip, memtype);

   return NC_NOERR;
}
