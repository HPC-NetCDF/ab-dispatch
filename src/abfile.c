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
#include <math.h>
#include <libgen.h>

#define AB_DIMSIZE_STRING "i/jdm ="
#define AB_MAX_DIM_DIGITS 10
#define AB_NDIMS3 3
#define AB_NDIMS1 1
#define NUM_AB_VAR_ATTS 4
#define TIME_NAME "day"
#define SPAN_NAME "span"
#define MIN_NAME "min"
#define MAX_NAME "max"
#define I_NAME "i"
#define J_NAME "j"

extern int nc4_vararray_add(NC_GRP_INFO_T *grp, NC_VAR_INFO_T *var);

/** @internal These flags may not be set for open mode. */
static const int ILLEGAL_OPEN_FLAGS = (NC_MMAP|NC_64BIT_OFFSET|NC_MPIIO|NC_MPIPOSIX|NC_DISKLESS);

static void
trim(char *s)
{
    char *p = s;
    int l = strlen(p);

    while(isspace(p[l - 1]))
       p[--l] = 0;
    while(*p && isspace(*p))
       ++p, --l;

    memmove(s, p, l + 1);
}   

/**
 * @internal Parse the B file for metadata info.
 *
 * @param h5 Pointer to file info.
 * @param num_header_atts Pointer that gets the number of header
 * attributes.
 * @param header_att Pointer to an array of fixed size which gets the
 * header atts.
 * @param var_name Pointer that gets variable name.
 * @param t_len Pointer that gets length of time dimension.
 * @param i_len Pointer that gets length of the i dimension.
 * @param j_len Pointer that gets length of the j dimension.
 * @param time Pointer to a pointer that gets array of time values,
 * of length t_len. Must be freed by caller.
 * @param span Pointer to a pointer that gets array of span values,
 * of length t_len. Must be freed by caller.
 * @param min Pointer to a pointer that gets array of minimum values,
 * of length t_len. Must be freed by caller.
 * @param max Pointer to a pointer that gets array of maximum values,
 * of length t_len. Must be freed by caller.
 *
 * @author Ed Hartnett
 */
static int
parse_b_file(NC_HDF5_FILE_INFO_T *h5, int *num_header_atts,
             char header_att[MAX_HEADER_ATTS][MAX_B_LINE_LEN],
             char *var_name, int *t_len, int *i_len, int *j_len, float **time,
             float **span, float **min, float **max)
{
   AB_FILE_INFO_T *ab_file;
   char line[MAX_B_LINE_LEN + 1];
   int header = 1;
   int time_start_pos = 0;
   int time_count = 0;

   /* Check inputs. */
   assert(h5 && h5->format_file_info && t_len && i_len && j_len && num_header_atts
          && header_att && var_name && time && span && min && max);

   /* Get the AB-specific file metadata. */
   ab_file = h5->format_file_info;
   assert(ab_file->b_file);

   /* Start record and header atts count at zero. */
   *t_len = 0;
   *num_header_atts = 0;

   /* Read the B file line by line. */
   while(fgets(line, sizeof(line), ab_file->b_file))
   {
      /* Is this line blank? */
      int blank = 1;
      for (int p = 0; p < strlen(line); p++)
         if (!isspace(line[p]))
         {
            blank = 0;
            break;
         }

      /* Skip blank lines. */
      if (blank)
         continue;

      /* Have we reached last line of header? */
      if (!(strncmp(line, AB_DIMSIZE_STRING, sizeof(AB_DIMSIZE_STRING) - 1)))
      {
         char *tok = line;
         char i_val[AB_MAX_DIM_DIGITS + 1];
         char j_val[AB_MAX_DIM_DIGITS + 1];
         int tok_count = 0;

         /* Get the i/j values. */
         while ((tok = strtok(tok, " ")) != NULL)
         {
            if (tok_count == 2)
               strncpy(i_val, tok, AB_MAX_DIM_DIGITS);
            if (tok_count == 3)
               strncpy(j_val, tok, AB_MAX_DIM_DIGITS);
            tok_count++;
            tok = NULL;
         }
         LOG((3, "i_val %s j_val %s", i_val, j_val));
         sscanf(i_val, "%d", i_len);
         sscanf(j_val, "%d", j_len);

         /* Remember we are done with header. */
         header = 0;

      }
      
      if (header)
      {
         LOG((3, "header = %d %s", header, line));
         if (*num_header_atts < MAX_HEADER_ATTS)
         {
            char hdr[MAX_B_LINE_LEN + 1];
            /* Lose last char - a line feed. */
            strncpy(hdr, line, strlen(line) - 1);
            trim(hdr);
            LOG((3, "hdr %s!", hdr));
            strncpy(header_att[*num_header_atts], hdr, MAX_B_LINE_LEN);
            (*num_header_atts)++;
         }
      }
      else
      {
         if (!time_start_pos)
            time_start_pos = ftell(ab_file->b_file);
         (*t_len)++;
      }
   }

   /* Allocate storage for the time, span, min, and max values. */
   if (!(*time = malloc(*t_len * sizeof(float))))
      return NC_ENOMEM;
   if (!(*span = malloc(*t_len * sizeof(float))))
      return NC_ENOMEM;
   if (!(*min = malloc(*t_len * sizeof(float))))
      return NC_ENOMEM;
   if (!(*max = malloc(*t_len * sizeof(float))))
      return NC_ENOMEM;

   /* Now go back and get the time info. */
   fseek(ab_file->b_file, time_start_pos, SEEK_SET);
   while(fgets(line, sizeof(line), ab_file->b_file))
   {
      char *tok = line;
      int tok_count = 0;
      int var_named = 0;

      /* Get the time values. */
      while ((tok = strtok(tok, " ")) != NULL)
      {
         LOG((3, "tok_count %d tok %s", tok_count, tok));
         if (tok_count == 0 && !var_named)
         {
            strncpy(var_name, tok, strlen(tok) - strlen(index(tok, ':')));
            var_named++;
         }
         else if (tok_count == 3)
         {
            sscanf(tok, "%f", &(*time)[time_count]);
         }
         else if (tok_count == 4)
         {
            sscanf(tok, "%f", &(*span)[time_count]);
         }
         else if (tok_count == 5)
         {
            sscanf(tok, "%f", &(*min)[time_count]);
         }
         else if (tok_count == 6)
         {
            sscanf(tok, "%f", &(*max)[time_count]);
         }
         
         tok_count++;
         tok = NULL;
      }
      time_count++;
      LOG((3, "%s", line));
   }
   
   return NC_NOERR;
}

/**
 * @internal Add dimensions for the AB file.
 *
 * @param h5 Pointer to file info.
 *
 * @author Ed Hartnett
 */
static int
add_ab_global_atts(NC_HDF5_FILE_INFO_T *h5, int num_header_atts,
                   char header_att[MAX_HEADER_ATTS][MAX_B_LINE_LEN])
{
   NC_ATT_INFO_T **att_list;
   int retval;
   
   att_list = &h5->root_grp->att;      

   for (int a = 0; a < num_header_atts; a++)
   {
      NC_ATT_INFO_T *att;
      char att_name[NC_MAX_NAME + 1];
      
      /* Come up with a name. */
      sprintf(att_name, "att_%d", a);
      
      /* Add to the end of the list of atts. */
      if ((retval = nc4_att_list_add(att_list, &att)))
         return retval;
      att->attnum = h5->root_grp->natts++;
      att->created = NC_TRUE;
      
      /* Learn about this attribute. */
      if (!(att->name = strndup(att_name, NC_MAX_NAME)))
         return NC_ENOMEM;
      att->nc_typeid = NC_CHAR;
      att->len = sizeof(header_att[a]);;
      LOG((4, "att->name %s att->nc_typeid %d att->len %d", att->name,
           att->nc_typeid, att->len));
      
      /* Allocate memory to hold the data. */
      if (!(att->data = malloc(att->len * sizeof(char))))
         return NC_ENOMEM;

      /* Copy the data. */
      memcpy(att->data, header_att[a], att->len);
   }
   
   return NC_NOERR;
}

/**
 * @internal Add dimensions for the AB file.
 *
 * @param h5 Pointer to file info.
 * @param dim Pointer to array of NC_DIM_INFO_T pointers.
 *
 * @author Ed Hartnett
 */
static int
add_ab_dims(NC_HDF5_FILE_INFO_T *h5, NC_DIM_INFO_T **dim, int t_len,
            int i_len, int j_len)
{
   int retval;
   char dim_name[AB_NDIMS3][NC_MAX_NAME + 1] = {TIME_NAME, I_NAME, J_NAME};
   
   for (int d = 0; d < AB_NDIMS3; d++)
   {
      if ((retval = nc4_dim_list_add(&h5->root_grp->dim, &dim[d])))
         return retval;
      if (!(dim[d]->name = strndup(dim_name[d], NC_MAX_NAME)))
         return NC_ENOMEM;
      dim[d]->dimid = h5->root_grp->nc4_info->next_dimid++;
      dim[d]->hash = hash_fast(dim_name[d], strlen(dim_name[d]));
   }
   dim[0]->len = t_len;
   dim[1]->len = i_len;
   dim[2]->len = j_len;

   return NC_NOERR;
}

/**
 * @internal Add a variable to the metadata structures.
 *
 * @param h5 Pointer to file info.
 * @param var Pointer to the variable.
 * @param var_name Pointer that gets variable name.
 * 
 * @return NC_NOERR No error.
 * @return NC_ENOMEM Out of memory.
 * @return NC_EINVAL Invalid input.
 * @author Ed Hartnett
 */
static int
add_ab_var(NC_HDF5_FILE_INFO_T *h5, NC_VAR_INFO_T **varp, char *var_name,
           nc_type xtype, int ndims, const int *dimids, int use_fill_value)
{
   NC_VAR_INFO_T *var;   
   int retval;

   /* Check inputs. */
   assert(h5 && varp && var_name);
   if (ndims < 0)
      return NC_EINVAL;
   if (ndims && !dimids)
      return NC_EINVAL;
   
   /* Create and init a variable metadata struct for the data variable. */
   if ((retval = nc4_var_add(varp)))
      return retval;
   var = *varp;
   var->varid = h5->root_grp->nvars++;
   var->created = NC_TRUE;
   var->written_to = NC_TRUE;

   /* Add the var to the variable array, growing it as needed. */
   if ((retval = nc4_vararray_add(h5->root_grp, var)))
      return retval;

   /* Remember var name. */
   if (!(var->name = strndup(var_name, NC_MAX_NAME)))
      return NC_ENOMEM;

   /* Create hash for names for quick lookups. */
   var->hash = hash_fast(var->name, strlen(var->name));

   /* Fill special type_info struct for variable type information. */
   if (!(var->type_info = calloc(1, sizeof(NC_TYPE_INFO_T)))) 
      return NC_ENOMEM;
   var->type_info->nc_typeid = xtype;

   /* Indicate that the variable has a pointer to the type */
   var->type_info->rc++;

   /* Get the size of the type. */
   if ((retval = nc4_get_typelen_mem(h5, var->type_info->nc_typeid, 0,
                                     &var->type_info->size))) 
      return retval;

   /* Allocate storage for the fill value. */
   if (use_fill_value)
   {
      if (!(var->fill_value = malloc(var->type_info->size))) 
         return NC_ENOMEM;
      *((float *)var->fill_value) = powf(2, 100);
   }
   
   /* AB files are always contiguous. */
   var->contiguous = NC_TRUE;

   /* Store dimension info in this variable. */
   var->ndims = ndims;
   if (!(var->dim = malloc(sizeof(NC_DIM_INFO_T *) * var->ndims))) 
      return NC_ENOMEM;
   if (!(var->dimids = malloc(sizeof(int) * var->ndims))) 
      return NC_ENOMEM;
   memcpy(var->dimids, dimids, var->ndims * sizeof(float));
   
   
   return NC_NOERR;
}

/**
 * @internal Add attributes to an AB variable.
 *
 * @param h5 Pointer to file info.
 * @param var Pointer to the variable.
 * @param t_len Length of time dimension, and all variable attribute
 * arrays.
 * @param 
 * @param var_name Pointer that gets variable name.
 * 
 * @return NC_NOERR No error.
 * @return NC_ENOMEM Out of memory.
 * @author Ed Hartnett
 */
static int
add_ab_var_atts(NC_HDF5_FILE_INFO_T *h5, NC_VAR_INFO_T *var, int t_len,
                float *time, float *span, float *min, float *max)
{
   char att_name[NUM_AB_VAR_ATTS][NC_MAX_NAME + 1] = {TIME_NAME, SPAN_NAME,
                                                      MIN_NAME, MAX_NAME};
   float *att_data[NUM_AB_VAR_ATTS] = {time, span, min, max};
   int retval;

   /* Check inputs. */
   assert(h5 && var && t_len > 0 && time && span && min && max);
   
   for (int a = 0; a < NUM_AB_VAR_ATTS; a++)
   {
      NC_ATT_INFO_T *att;   
      
      /* Add to the end of the list of atts. */
      if ((retval = nc4_att_list_add(&var->att, &att)))
         return retval;
      att->attnum = var->natts++;
      att->created = NC_TRUE;
      
      /* Add attribute metadata. */
      if (!(att->name = strndup(att_name[a], NC_MAX_NAME)))
         return NC_ENOMEM;
      att->nc_typeid = NC_FLOAT;
      att->len = t_len;
      LOG((4, "att->name %s att->nc_typeid %d att->len %d", att->name,
           att->nc_typeid, att->len));
      
      /* Allocate memory to hold the data. */
      if (!(att->data = malloc(sizeof(float) * att->len)))
         return NC_ENOMEM;
      
      /* Copy the attribute data. */
      memcpy(att->data, att_data[a], att->len * sizeof(float));
   }
   
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
   NC_VAR_INFO_T *var;
   NC_VAR_INFO_T *time_var;
   NC_DIM_INFO_T *dim[AB_NDIMS3];
   AB_FILE_INFO_T *ab_file;
   char *a_path;
   char *dot_loc;
   int num_header_atts;
   char header_att[MAX_HEADER_ATTS][MAX_B_LINE_LEN];
   int t_len, i_len, j_len;
   float *time;
   float *span;
   float *min;
   float *max;
   char var_name[NC_MAX_NAME + 1];
   int dimids[AB_NDIMS3] = {0, 1, 2};
   int time_dimid = 0;
   int retval;

   /* Check inputs. */
   assert(nc && path);
   LOG((1, "%s: path %s mode %d", __func__, path, mode));

   /* B file name must end in .b. */
   if (!(dot_loc = rindex(path, '.')))
      return NC_EINVAL;
   if (strcmp(dot_loc, ".b"))
      return NC_EINVAL;

   /* Get the A file name. */
   if (!(a_path = strdup(path)))
      return NC_ENOMEM;
   a_path[strlen(path) - 1] = 'a';
   
   /* Add necessary structs to hold file metadata. */
   if ((retval = nc4_nc4f_list_add(nc, path, mode)))
      return retval;
   h5 = (NC_HDF5_FILE_INFO_T *)(nc)->dispatchdata;
   assert(h5 && h5->root_grp);
   h5->no_write = NC_TRUE;
   h5->root_grp->nc4_info->controller = nc;

   /* Allocate data to hold AB specific file data. */
   if (!(ab_file = malloc(sizeof(AB_FILE_INFO_T))))
      return NC_ENOMEM;
   h5->format_file_info = ab_file;

   /* Open the A file. */
   if (!(ab_file->a_file = fopen(a_path, "r")))
      return NC_EIO;

   /* Open the B file. */
   if (!(ab_file->b_file = fopen(path, "r")))
      return NC_EIO;

   /* Parse the B file. */
   if ((retval = parse_b_file(h5, &num_header_atts, header_att, var_name, &t_len,
                              &i_len, &j_len, &time, &span, &min, &max)))
      return retval;
   LOG((3, "num_header_atts %d var_name %s t_len %d i_len %d j_len %d",
        num_header_atts, var_name, t_len, i_len, j_len));

   for (int h = 0; h < num_header_atts; h++)
   {
      LOG((3, "h %d header_att %s!", h, header_att[h]));
   }
   for (int t = 0; t < t_len; t++)
   {
      LOG((3, "t %d time %f span %f min %f max %f", t, time[t], span[t],
           min[t], max[t]));
   }

   /* Add the global attributes. */
   if ((retval = add_ab_global_atts(h5, num_header_atts, header_att)))
      return retval;

   /* Add the dimensions. */
   if ((retval = add_ab_dims(h5, dim, t_len, i_len, j_len)))
      return retval;

   /* Add the data variable. */
   if ((retval = add_ab_var(h5, &var, var_name, NC_FLOAT, AB_NDIMS3, dimids, 1)))
      return retval;

   /* Add the coordinate variable. */
   if ((retval = add_ab_var(h5, &time_var, TIME_NAME, NC_FLOAT, AB_NDIMS1, &time_dimid, 0)))
      return retval;

   /* Variable attributes. */
   if ((retval = add_ab_var_atts(h5, var, t_len, time, span, min, max)))
      return retval;
   
   /* Free resources. */
   free(a_path);
   free(time);
   free(span);
   free(min);
   free(max);

#ifdef LOGGING
   /* This will print out the names, types, lens, etc of the vars and
      atts in the file, if the logging level is 2 or greater. */
   log_metadata_nc(h5->root_grp->nc4_info->controller);
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
   AB_FILE_INFO_T *ab_file;
   int retval;

   LOG((1, "%s: ncid 0x%x", __func__, ncid));

   /* Find our metadata for this file. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;
   assert(nc && h5 && h5->format_file_info);

   /* Get the AB specific info. */
   ab_file = h5->format_file_info;

   /* Close the A/B files. */
   fclose(ab_file->a_file);
   fclose(ab_file->b_file);

   /* Free AB file info struct. */
   free(h5->format_file_info);

   /* Delete all the list contents for vars, dims, and atts, in each
    * group. */
   if ((retval = nc4_rec_grp_del(&h5->root_grp, h5->root_grp)))
      return retval;

   /* Free the nc4_info struct; above code should have reclaimed
      everything else */
   free(h5);

   return NC_NOERR;
}
