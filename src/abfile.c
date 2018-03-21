/**
 * @file
 * @internal The AB file functions.
 *
 * @author Ed Hartnett
 */

#include "config.h"
#include <errno.h>  /* netcdf functions sometimes return system errors */
#include "nc.h"
#include "nc4internal.h"
#include "abdispatch.h"
#include <strings.h>
#include <libgen.h>

extern int nc4_vararray_add(NC_GRP_INFO_T *grp, NC_VAR_INFO_T *var);

/** @internal These flags may not be set for open mode. */
static const int ILLEGAL_OPEN_FLAGS = (NC_MMAP|NC_64BIT_OFFSET|NC_MPIIO|NC_MPIPOSIX|NC_DISKLESS);

/**
 * @internal This function will free all allocated metadata memory,
 * and close the AB file.
 *
 * @param h5 Pointer to HDF5 file info struct.
 * @param abort True if this is an abort.
 *
 * @return ::NC_NOERR No error.
 * @author Ed Hartnett
 */
static int
ab_close_file(NC_HDF5_FILE_INFO_T *h5, int abort)
{
   int retval;

   assert(h5 && h5->root_grp);
   LOG((3, "%s: abort %d", __func__, abort));

   /* Delete all the list contents for vars, dims, and atts, in each
    * group. */
   if ((retval = nc4_rec_grp_del(&h5->root_grp, h5->root_grp)))
      return retval;

   /* Free the nc4_info struct; above code should have reclaimed
      everything else */
   free(h5);

   return NC_NOERR;
}

/**
 * @internal Parse the file name.
 *
 * @param path The path passed to nc_open().
 * @param dirname A pointer that will point to the directory. Must be
 * freed by caller.
 * @param a_basename A pointer that will point to the A filename. Must
 * be freed by caller.
 * @param b_basename A pointer that will point to the B filename. Must
 * be freed by caller.
 * 
 * @return NC_NOERR No error.
 * @return NC_ENOMEM Out of memory.
 * @return NC_EINVAL Name of file must end in .b.
 * @author Ed Hartnett
 */
static int
ab_parse_path(const char *path, char *dirname, char *a_basename,
              char *b_basename)
{
   char *rindex_char;
   char *dot_loc;

   /* Find the B file name. */
   rindex_char = rindex(path, '/');
   if (!rindex_char)
      rindex_char = (char *)path;
   else
      rindex_char++; /* Skip backslash. */
   if (!(b_basename = strdup(rindex_char)))
      return NC_ENOMEM;
   LOG((3, "b_basename %s", b_basename));

   /* B file name must end in .b. */
   if (!(dot_loc = rindex(b_basename, '.')))
      return NC_EINVAL;
   if (strcmp(dot_loc, ".b"))
      return NC_EINVAL;

   /* Get the A filename - same as the B filename, but with a .a at
    * the end. */
   if (!(a_basename = strdup(b_basename)))
      return NC_ENOMEM;
   a_basename[strlen(b_basename) - 1] = 'a';

   /* Find the directory name, if any. */
   if (!(dirname = strndup(path, strlen(path) - strlen(b_basename))))
      return NC_ENOMEM;
   
   LOG((2, "%s: dirname %s a_basename %s b_basename %s", __func__, dirname,
        a_basename, b_basename));
   
   return NC_NOERR;
}

/**
 * @internal Open an AB format file. The .b file should be given as
 * the path. A matching .a file will be expected in the same
 * directory.
 *
 * @param path The file name of the new file.
 * @param mode The open mode flag.
 * @param nc Pointer that gets the NC file info struct.
 *
 * @return ::NC_NOERR No error.
 * @return NC_ENOMEM Out of memory.
 * @author Ed Hartnett
 */
static int
ab_open_file(const char *path, int mode, NC *nc)
{
   NC_HDF5_FILE_INFO_T *h5;
   char *dirname;
   char *b_basename;
   char *a_basename;
   
   int retval;

   /* Check inputs. */
   assert(nc && path);
   LOG((1, "%s: path %s mode %d", __func__, path, mode));

   /* Parse the path. */
   if ((retval = ab_parse_path(path, dirname, a_basename, b_basename)))
      return retval;

   /* Free the filename and path. */
   free(dirname);
   free(a_basename);
   free(b_basename);

   /* Add necessary structs to hold netcdf-4 file data. */
   if ((retval = nc4_nc4f_list_add(nc, path, mode)))
      return retval;
   h5 = (NC_HDF5_FILE_INFO_T *)(nc)->dispatchdata;
   assert(h5 && h5->root_grp);

#ifdef LOGGING
   /* This will print out the names, types, lens, etc of the vars and
      atts in the file, if the logging level is 2 or greater. */
   /*log_metadata_nc(h5->root_grp->nc4_info->controller);*/
#endif
   return NC_NOERR;
}

/**
 * @internal Open a AB file.
 *
 * @param path The file name of the file.
 * @param mode The open mode flag.
 * @param basepe Ignored by this function.
 * @param chunksizehintp Ignored by this function.
 * @param use_parallel Must be 0 for sequential, access. Parallel
 * access not supported for AB.
 * @param parameters pointer to struct holding extra data (e.g. for
 * parallel I/O) layer. Ignored if NULL.
 * @param dispatch Pointer to the dispatch table for this file.
 * @param nc_file Pointer to an instance of NC.
 *
 * @return ::NC_NOERR No error.
 * @return ::NC_EINVAL Invalid input.
 * @author Ed Hartnett
 */
int
AB_open(const char *path, int mode, int basepe, size_t *chunksizehintp,
        int use_parallel, void *parameters, NC_Dispatch *dispatch,
        NC *nc_file)
{
   assert(nc_file && path);

   LOG((1, "%s: path %s mode %d params %x", __func__, path, mode,
        parameters));

   /* Check inputs. */
   assert(path && !use_parallel);

   /* Check the mode for validity */
   if (mode & ILLEGAL_OPEN_FLAGS)
      return NC_EINVAL;

   /* We don't maintain a separate internal ncid for AB format. */
   nc_file->int_ncid = nc_file->ext_ncid;

   /* Open the file. */
   return ab_open_file(path, mode, nc_file);
}

/**
 * @internal Close the AB file.
 *
 * @param ncid File ID.
 *
 * @return ::NC_NOERR No error.
 * @return ::NC_EBADID Bad ncid.
 * @author Ed Hartnett
 */
int
AB_close(int ncid)
{
   NC_GRP_INFO_T *grp;
   NC *nc;
   NC_HDF5_FILE_INFO_T *h5;
   int retval;

   LOG((1, "%s: ncid 0x%x", __func__, ncid));

   /* Find our metadata for this file. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;
/*   assert(nc && h5 && grp && !grp->parent);*/

   /* Call the nc4 close. */
   if ((retval = ab_close_file(h5, 0)))
      return retval;

   return NC_NOERR;
}
