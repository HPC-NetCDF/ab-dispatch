/* Copyright 2018, UCAR/Unidata See netcdf/COPYRIGHT file for copying
 * and redistribution conditions.*/
/**
 * @file
 * Dispatch code for HDF4. HDF4 access is read-only.
 *
 * Ed Hartnett
 */

#include "config.h"
#include <stdlib.h>
#include "abdispatch.h"
#include "nc4dispatch.h"
#include "nc.h"

static NC_Dispatch AB_dispatcher = {

NC_FORMATX_UF0,

AB_create,
AB_open,

AB_redef,
AB__enddef,
AB_sync,
AB_abort,
AB_close,
AB_set_fill,
AB_inq_base_pe,
AB_set_base_pe,
AB_inq_format,
AB_inq_format_extended,

NC4_inq,
NC4_inq_type,

AB_def_dim,
NC4_inq_dimid,
NC4_inq_dim,
NC4_inq_unlimdim,
AB_rename_dim,

NC4_inq_att,
NC4_inq_attid,
NC4_inq_attname,
AB_rename_att,
AB_del_att,
NC4_get_att,
AB_put_att,

AB_def_var,
NC4_inq_varid,
AB_rename_var,
AB_get_vara,
AB_put_vara,
NCDEFAULT_get_vars,
NCDEFAULT_put_vars,
NCDEFAULT_get_varm,
NCDEFAULT_put_varm,

NC4_inq_var_all,

AB_var_par_access,
AB_def_var_fill,

NC4_show_metadata,
NC4_inq_unlimdims,

NC4_inq_ncid,
NC4_inq_grps,
NC4_inq_grpname,
NC4_inq_grpname_full,
NC4_inq_grp_parent,
NC4_inq_grp_full_ncid,
NC4_inq_varids,
NC4_inq_dimids,
NC4_inq_typeids,
NC4_inq_type_equal,
AB_def_grp,
AB_rename_grp,
NC4_inq_user_type,
NC4_inq_typeid,

AB_def_compound,
AB_insert_compound,
AB_insert_array_compound,
AB_inq_compound_field,
AB_inq_compound_fieldindex,
AB_def_vlen,
AB_put_vlen_element,
AB_get_vlen_element,
AB_def_enum,
AB_insert_enum,
AB_inq_enum_member,
AB_inq_enum_ident,
AB_def_opaque,
AB_def_var_deflate,
AB_def_var_fletcher32,
AB_def_var_chunking,
AB_def_var_endian,
AB_def_var_filter,
AB_set_var_chunk_cache,
AB_get_var_chunk_cache,

};

NC_Dispatch* AB_dispatch_table = NULL;

/**
 * @internal Initialize AB dispatch layer.
 *
 * @return ::NC_NOERR No error.
 * @author Ed Hartnett
 */
int
AB_initialize(void)
{
    AB_dispatch_table = &AB_dispatcher;
    return NC_NOERR;
}

/**
 * @internal Finalize AB dispatch layer.
 *
 * @return ::NC_NOERR No error.
 * @author Ed Hartnett
 */
int
AB_finalize(void)
{
    return NC_NOERR;
}
