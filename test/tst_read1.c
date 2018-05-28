/** Test read of AB format with netCDF. 
 *
 * @author Ed Hartnett 
*/

#include <config.h>
#include <netcdf.h>
#include "abdispatch.h"
#include <nc4dispatch.h>
#include <stdio.h>
#include <math.h>

#define TEST_FILE "surtmp_100l.b"
#define NDIMS1 1
#define NDIMS3 3
#define VAR_NAME "surtmp"
#define NUM_AB_VAR_ATTS 4
#define TIME_NAME "day"
#define SPAN_NAME "span"
#define MIN_NAME "min"
#define MAX_NAME "max"
#define I_NAME "i"
#define J_NAME "j"
#define GLOBAL_ATT_NAME "att_0"
#define EXPECTED_GLOBAL_ATT "FNMOC, 6hrly, degC"
#define T_LEN 123
#define I_LEN 258
#define J_LEN 175
#define EPSILON .0000001
   
extern NC_Dispatch AB_dispatcher;

float time[T_LEN] = {36495.500000, 36495.750000, 36496.000000, 36496.250000, 36496.500000,
                     36496.750000, 36497.000000, 36497.250000, 36497.500000, 36497.750000,
                     36498.000000, 36498.250000, 36498.500000, 36498.750000, 36499.000000,
                     36499.250000, 36499.500000, 36499.750000, 36500.000000, 36500.250000,
                     36500.500000, 36500.750000, 36501.000000, 36501.250000, 36501.500000,
                     36501.750000, 36502.000000, 36502.250000, 36502.500000, 36502.750000,
                     36503.000000, 36503.250000, 36503.500000, 36503.750000, 36504.000000,
                     36504.250000, 36504.500000, 36504.750000, 36505.000000, 36505.250000,
                     36505.500000, 36505.750000, 36506.000000, 36506.250000, 36506.500000,
                     36506.750000, 36507.000000, 36507.250000, 36507.500000, 36507.750000,
                     36508.000000, 36508.250000, 36508.500000, 36508.750000, 36509.000000,
                     36509.250000, 36509.500000, 36509.750000, 36510.000000, 36510.250000,
                     36510.500000, 36510.750000, 36511.000000, 36511.250000, 36511.500000,
                     36511.750000, 36512.000000, 36512.250000, 36512.500000, 36512.750000,
                     36513.000000, 36513.250000, 36513.500000, 36513.750000, 36514.000000,
                     36514.250000, 36514.500000, 36514.750000, 36515.000000, 36515.250000,
                     36515.500000, 36515.750000, 36516.000000, 36516.250000, 36516.500000,
                     36516.750000, 36517.000000, 36517.250000, 36517.500000, 36517.750000,
                     36518.000000, 36518.250000, 36518.500000, 36518.750000, 36519.000000,
                     36519.250000, 36519.500000, 36519.750000, 36520.000000, 36520.250000,
                     36520.500000, 36520.750000, 36521.000000, 36521.250000, 36521.500000,
                     36521.750000, 36522.000000, 36522.250000, 36522.500000, 36522.750000,
                     36523.000000, 36523.250000, 36523.500000, 36523.750000, 36524.000000,
                     36524.250000, 36524.500000, 36524.750000, 36525.000000, 36525.250000,
                     36525.500000, 36525.750000, 36526.000000};
float span[T_LEN] = {0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000, 0.250000,
                     0.250000, 0.250000, 0.250000, 0.250000};
float min[T_LEN] = {9.771892, 10.784987, 9.401492, 6.628430, 4.607868, 3.897291, 4.213979,
                    4.424856, 4.226626, 4.022602, 4.136856, 4.622542, 4.554771, 4.664321,
                    4.907145, 5.749945, 6.398822, 6.757607, 7.123997, 7.393907, 7.355556,
                    7.199125, 5.509292, 4.769361, 5.141631, 5.896506, 6.674411, 7.204914,
                    7.653955, 8.095844, 8.628254, 9.426715, 10.196815, 10.805400, 11.268487,
                    11.751347, 11.904779, 12.055911, 12.182851, 12.189175, 12.196394, 12.159817,
                    9.251261, 4.093727, 0.181067, -1.352509, -0.364494, 2.234805, 4.680813,
                    5.819631, 6.325951, 6.385474, 6.446225, 7.241121, 8.016541, 9.304774,
                    10.863615, 11.906437, 12.255816, 12.667924, 10.848666, 7.711861, 4.648007,
                    3.113999, 2.058515, -0.477608, -1.811485, -1.072346, 1.302688, 2.925264,
                    4.287227, 2.943427, 2.069484, -0.971903, -2.706800, -2.914489, -3.389548,
                    -3.525549, -2.487972, -0.947699, 0.447085, 1.769978, 2.568594, 2.248194,
                    1.012455, 0.461399, 0.474416, 1.511067, 3.196703, 4.542253, 5.570292,
                    6.480618, 7.035630, 7.337607, 7.424665, 7.305072, 6.841867, 6.141395,
                    5.669068, 5.445779, 5.857729, 6.901529, 7.535413, 5.567883, 4.132844,
                    3.377980, 3.256490, 4.144887, 4.801534, 5.314370, 6.164811, 6.354491,
                    5.865624, 4.060117, 2.045688, 0.771575, -0.416543, -1.899319, -3.700320,
                    -4.506866, -3.937608, -2.206423, 0.794771};
float max[T_LEN] = {28.590569, 28.559731, 28.517277, 28.465023, 28.424067, 28.394974, 28.366539,
                    28.337183, 28.356785, 28.426109, 28.458282, 28.453896, 28.407213, 28.316628,
                    28.222075, 28.125101, 28.057232, 28.017437, 27.993252, 27.984646, 28.025936,
                    28.089796, 28.172117, 28.272289, 28.324240, 28.327641, 28.317564, 28.294710,
                    28.295286, 28.319628, 28.323137, 28.306606, 28.281540, 28.246565, 28.224665,
                    28.214849, 28.234085, 28.283339, 28.333225, 28.383362, 28.410416, 28.414783,
                    28.428968, 28.453758, 28.460472, 28.447691, 28.434694, 28.422050, 28.410313,
                    28.400667, 28.367632, 28.318863, 28.285042, 28.210398, 28.148363, 28.098141,
                    28.035227, 27.960903, 27.909443, 27.920477, 27.920479, 27.899460, 27.879280,
                    27.859049, 27.873047, 27.954206, 28.031694, 28.099384, 28.163574, 28.224279,
                    28.195379, 28.074286, 28.008223, 27.995996, 28.062635, 28.206879, 28.250772,
                    28.194433, 28.133982, 28.069729, 28.008305, 27.950317, 27.879524, 27.797016,
                    27.740938, 27.710226, 27.648287, 27.553875, 27.462049, 27.373602, 27.277552,
                    27.173742, 27.089157, 27.023396, 26.993874, 27.000980, 27.003897, 27.004131,
                    27.017021, 27.041782, 27.097788, 27.184301, 27.271276, 27.359291, 27.424963,
                    27.467936, 27.503365, 27.530914, 27.556900, 27.581215, 28.060846, 27.655060,
                    27.700809, 27.750469, 27.788662, 27.814367, 27.806931, 27.766562, 27.683887,
                    27.558491, 27.431416, 27.303328, 27.251318};

int
main()
{
   int ncid;
   int ndims, nvars, natts, unlimdimid;
   int varid, coord_varid;
   char var_name_in[NC_MAX_NAME + 1];
   int xtype_in;
   size_t len_in;
   int ndims_in, natts_in;
   int dimids_in[NDIMS3];
   char dim_name[NDIMS3][NC_MAX_NAME + 1] = {TIME_NAME, J_NAME, I_NAME};
   char att_name[NUM_AB_VAR_ATTS][NC_MAX_NAME + 1] = {TIME_NAME, SPAN_NAME,
                                                      MIN_NAME, MAX_NAME};
   size_t dim_len[NDIMS3] = {T_LEN, J_LEN, I_LEN};
   char att_value[MAX_B_LINE_LEN + 1];
   float *att_data[NUM_AB_VAR_ATTS] = {time, span, min, max};
   float fill_value = pow(2, 100);
   int ret;
   
   printf("\nTesting AB format dispatch layer...");
   if ((ret = nc_def_user_format(NC_UF0, &AB_dispatcher, NULL)))
      return ret;
   ab_set_log_level(5);
   nc_set_log_level(3);

   if ((ret = nc_open(TEST_FILE, NC_UF0, &ncid)))
      return ret;
   if ((ret = nc_inq(ncid, &ndims, &nvars, &natts, &unlimdimid)))
      return ret;
   /* printf("ndims %d nvars %d natts %d unlimdimid %d\n", ndims, nvars, natts, unlimdimid); */
   if (ndims != 3 || nvars != 2 ||natts != 2 || unlimdimid != -1)
       return 111;

   /* Check the global attribute. */
   if ((ret = nc_inq_att(ncid, NC_GLOBAL, GLOBAL_ATT_NAME, &xtype_in, &len_in)))
      return ret;
   if (xtype_in != NC_CHAR || len_in != 18)
      return 111;
   if ((ret = nc_get_att_text(ncid, NC_GLOBAL, GLOBAL_ATT_NAME, att_value)))
      return ret;
   att_value[len_in] = 0;
   if (strcmp(att_value, EXPECTED_GLOBAL_ATT))
      return 111;
   /* printf("att_value %s!\n", att_value); */

   /* Check the coord var. */
   if ((ret = nc_inq_varid(ncid, TIME_NAME, &coord_varid)))
      return ret;
   /* printf("coord_varid %d\n", coord_varid); */
   if (coord_varid != 0)
      return 111;
   if ((ret = nc_inq_var(ncid, coord_varid, var_name_in, &xtype_in, &ndims_in,
                         dimids_in, &natts_in)))
      return ret;
   if (strcmp(var_name_in, TIME_NAME) || xtype_in != NC_FLOAT || ndims_in != 1 ||
       dimids_in[0] != 0 || natts_in != 0)
       return 111;

   /* Check the data var. */
   if ((ret = nc_inq_varid(ncid, VAR_NAME, &varid)))
      return ret;
   if (varid != 1)
      return 111;
   if ((ret = nc_inq_var(ncid, varid, var_name_in, &xtype_in, &ndims_in,
                         dimids_in, &natts_in)))
      return ret;
   if (strcmp(var_name_in, VAR_NAME) || xtype_in != NC_FLOAT || ndims_in != 3 ||
       dimids_in[0] != 0 || dimids_in[1] != 1 || dimids_in[2] != 2 || natts_in != 7)
       return 111;

   /* Check the variable attributes. */
   for (int a = 0; a < NUM_AB_VAR_ATTS; a++)
   {
      float this_att_data[T_LEN];
      
      if ((ret = nc_inq_att(ncid, 1, att_name[a], &xtype_in, &len_in)))
         return ret;
      if (xtype_in != NC_FLOAT || len_in != T_LEN)
         return 111;
      if ((ret = nc_get_att_float(ncid, 1, att_name[a], this_att_data)))
         return ret;
      for (int t = 0; t < T_LEN; t++)
      {
         if (abs(this_att_data[t] - att_data[a][t]) > EPSILON)
            return 111;
      }
   }

   /* Check the dimensions. */
   for (int d = 0; d < NDIMS3; d++)
   {
      char dim_name_in[NC_MAX_NAME + 1];
      size_t len_in;
      
      if ((ret = nc_inq_dim(ncid, d, dim_name_in, &len_in)))
         return ret;
      if (strcmp(dim_name_in, dim_name[d]) || len_in != dim_len[d])
          return 111;
   }

   /* Check the coord-var data. */
   {
      size_t start[NDIMS1] = {0};
      size_t count[NDIMS1] = {T_LEN};
      float data_in[T_LEN];
      if ((ret = nc_get_vara_float(ncid, 0, start, count, data_in)))
         return ret;
      for (int t = 0; t < T_LEN; t++)
         if (data_in[t] != time[t])
            return 111;
      start[0] = 1;
      count[0] = T_LEN - 1;
      if ((ret = nc_get_vara_float(ncid, 0, start, count, data_in)))
         return ret;
      for (int t = 0; t < T_LEN - 1; t++)
         if (data_in[t] != time[t + 1])
            return 111;
   }

   /* Check the coord-var data as different types. */
   {
      size_t start[NDIMS1] = {0};
      size_t count[NDIMS1] = {T_LEN};
      int data_in[T_LEN];
      if ((ret = nc_get_vara_int(ncid, 0, start, count, data_in)))
         return ret;
      for (int t = 0; t < T_LEN; t++)
         if (data_in[t] != (int)time[t])
            return 111;
      signed char data_in_schar[T_LEN];
      if ((ret = nc_get_vara_schar(ncid, 0, start, count, data_in_schar) != NC_ERANGE))
         return ret;
      for (int t = 0; t < T_LEN; t++)
         if (data_in_schar[t] != (signed char)time[t])
            return 111;
   }

   /* Check one value. */
   {
      size_t start[NDIMS3] = {0, 0, 0};
      size_t count[NDIMS3] = {1, 1, 1};
      float expected_val = 12.554479;
      float data_in;
      if ((ret = nc_get_vara_float(ncid, 1, start, count, &data_in)))
         return ret;
      if (abs(data_in - expected_val) > EPSILON)
         return 111;
      printf("data_in %f\n", data_in);
   }
   
   /* Get all i's. */
   {
      size_t start[NDIMS3] = {0, 0, 0};
      size_t count[NDIMS3] = {1, 1, J_LEN};
      float data_in[J_LEN];
      float max = 0, min = 100;
      float expected_min = 12.554479;
      float expected_max = 28.332735;

      /* Get data. */
      if ((ret = nc_get_vara_float(ncid, 1, start, count, data_in)))
         return ret;

      /* Find min/max. */
      for (int j = 0; j < J_LEN; j++)
      {
         if (data_in[j] < min)
            min = data_in[j];
         if (data_in[j] > max)
            max = data_in[j];
      }

      /* Did we get correct results? */
      if (abs(min - expected_min) > EPSILON ||abs(max - expected_max) > EPSILON)
         return 111;
      
      printf("min %f max %f\n", min, max);
   }

   /* Get two rows of i. */
   {
      size_t start[NDIMS3] = {0, 0, 0};
      size_t count[NDIMS3] = {1, 2, I_LEN};
      float data_in[2][I_LEN];
      float max = 0, min = 100;
      float expected_min = 12.444799;
      float expected_max = 28.332735;

      /* Get data. */
      if ((ret = nc_get_vara_float(ncid, 1, start, count, (float *)data_in)))
         return ret;

      /* Find min/max. */
      for (int j = 0; j < 2; j++)
      {
         for (int i = 0; i < I_LEN; i++)
         {
            if (data_in[j][i] < min)
               min = data_in[j][i];
            if (data_in[j][i] > max)
               max = data_in[j][i];
         }
      }

      /* Did we get correct results? */
      if (abs(min - expected_min) > EPSILON ||abs(max - expected_max) > EPSILON)
         return 111;
      
      printf("min %f max %f\n", min, max);
   }

   /* Get all j's. */
   {
      size_t start[NDIMS3] = {0, 0, 0};
      size_t count[NDIMS3] = {1, 1, J_LEN};
      float data_in[J_LEN];
      float max = 0, min = 100;
      float expected_min = 12.554479;
      float expected_max = 28.332735;

      /* Get data. */
      if ((ret = nc_get_vara_float(ncid, 1, start, count, data_in)))
         return ret;

      /* Find min/max. */
      for (int j = 0; j < J_LEN; j++)
      {
         if (data_in[j] < min)
            min = data_in[j];
         if (data_in[j] > max)
            max = data_in[j];
      }

      /* Did we get correct results? */
      /* if (abs(min - expected_min) > EPSILON ||abs(max - expected_max) > EPSILON) */
      /*    return 111; */
      
      printf("min %f max %f\n", min, max);
   }

   /* /\* Get two rows of i. *\/ */
   /* { */
   /*    size_t start[NDIMS3] = {0, 0, 0}; */
   /*    size_t count[NDIMS3] = {1, 2, J_LEN}; */
   /*    float data_in[2][J_LEN]; */
   /*    float max = 0, min = 100; */
   /*    float expected_min = 12.444799; */
   /*    float expected_max = 28.332735; */

   /*    /\* Get data. *\/ */
   /*    if ((ret = nc_get_vara_float(ncid, 1, start, count, (float *)data_in))) */
   /*       return ret; */

   /*    /\* Find min/max. *\/ */
   /*    for (int j = 0; j < 2; j++) */
   /*    { */
   /*       for (int i = 0; i < I_LEN; i++) */
   /*       { */
   /*          if (data_in[j][i] < min) */
   /*             min = data_in[j][i]; */
   /*          if (data_in[j][i] > max) */
   /*             max = data_in[j][i]; */
   /*       } */
   /*    } */

   /*    /\* Did we get correct results? *\/ */
   /*    /\* if (abs(min - expected_min) > EPSILON ||abs(max - expected_max) > EPSILON) *\/ */
   /*    /\*    return 111; *\/ */
      
   /*    printf("min %f max %f\n", min, max); */
   /* } */

   /* /\* Get a whole record. *\/ */
   /* { */
   /*    size_t start[NDIMS3] = {0, 0, 0}; */
   /*    size_t count[NDIMS3] = {1, I_LEN, J_LEN}; */
   /*    float data_in[I_LEN][J_LEN]; */
   /*    float expected_min = 12.444799; */
   /*    float expected_max = 28.332735; */

   /*    /\* Get data. *\/ */
   /*    if ((ret = nc_get_vara_float(ncid, 1, start, count, (float *)data_in))) */
   /*       return ret; */

   /*    /\* Find min/max. *\/ */
   /*    for (int i = 0; i < I_LEN; i++) */
   /*    { */
   /*       float max = 0, min = 100; */
   /*       for (int j = 0; j < J_LEN; j++) */
   /*       { */
   /*          if (data_in[j][i] == fill_value) */
   /*             continue; */
   /*          if (data_in[j][i] < min) */
   /*             min = data_in[j][i]; */
   /*          if (data_in[j][i] > max) */
   /*             max = data_in[j][i]; */
   /*       } */
   /*       printf("i %d min %f max %f\n", i, min, max); */
   /*    } */

   /*    /\* Did we get correct results? *\/ */
   /*    /\* if (abs(min - expected_min) > EPSILON ||abs(max - expected_max) > EPSILON) *\/ */
   /*    /\*    return 111; *\/ */
   /* } */

   /* Get a whole record. */
   {
      size_t start[NDIMS3] = {0, 0, 0};
      size_t count[NDIMS3] = {1, J_LEN, I_LEN};
      float data_in[J_LEN][I_LEN];
      float expected_min = 12.444799;
      float expected_max = 28.332735;
      float max = 0, min = 100;

      /* Get data. */
      if ((ret = nc_get_vara_float(ncid, 1, start, count, (float *)data_in)))
         return ret;

      /* Find min/max. */
      for (int j = 0; j < J_LEN; j++)
      {
         for (int i = 0; i < I_LEN; i++)
         {
            if (data_in[j][i] == fill_value)
               continue;
            if (data_in[j][i] < min)
               min = data_in[j][i];
            if (data_in[j][i] > max)
               max = data_in[j][i];
         }
      }
      printf("min %f max %f\n", min, max);

      /* Did we get correct results? */
      /* if (abs(min - expected_min) > EPSILON ||abs(max - expected_max) > EPSILON) */
      /*    return 111; */
   }

   if ((ret = nc_close(ncid)))
      return ret;

   printf("SUCCESS!\n");
   return 0;
}

