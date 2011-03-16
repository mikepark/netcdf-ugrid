
#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

#define nc_try( fcn )						\
  {								\
    int code;							\
    code = (fcn);						\
    if ( code )							\
      {								\
        printf("%s: %d: %s: netcdf error: %s (%d)\n",		\
	       __FILE__,__LINE__,__func__,nc_strerror(code),    \
	       code);						\
	exit(code);						\
      }								\
  }

int translate_variable_dimension( int nc, char *variable_name )
{
  int var;
  int var_ndim;
  int var_dims[NC_MAX_VAR_DIMS];
  int i;

  size_t dim_length;
  char dim_name[NC_MAX_NAME+1];

  int length;

  int code;

  printf("%s:\n",variable_name);

  code = nc_inq_varid(nc, variable_name, &var);
  if ( -49 == code )
    {
      printf(" not in netcfd file\n");
      length = 0;
    }
  else
    {
      nc_try( code );
      nc_try( nc_inq_varndims(nc, var, &var_ndim) );

      nc_try( nc_inq_vardimid(nc, var, &var_dims[0] ) );
      nc_try( nc_inq_dim(nc, var_dims[0], &dim_name[0], &dim_length) );
      printf(" %s = %d\n",dim_name,(int)dim_length);
      length = (int)dim_length;
    }

  //  nc_try( nc_get_var_int(nc, var, elem->e2n ) );

  return 0;
}

int main( int argc, char *argv[] )
{
  int nc;

  if ( argc < 2 ) 
    {
      printf("usage: %s my-netcfd-grid-filename\n",argv[0]);
      return 1;
    }

  nc_try( nc_open(argv[1], NC_NOWRITE, &nc) );

  nc_try( translate_variable_dimension( nc, "points_nc" ) );
  
  nc_try( translate_variable_dimension( nc, "tris" ) );
  nc_try( translate_variable_dimension( nc, "points_of_surfacequadrilaterals" ) );

  nc_try( translate_variable_dimension( nc, "tets" ) );
  nc_try( translate_variable_dimension( nc, "pyramids" ) );
  nc_try( translate_variable_dimension( nc, "prisms" ) );
  nc_try( translate_variable_dimension( nc, "points_of_hexaeders" ) );

  nc_try( nc_close(nc) );

  return 0;
}
