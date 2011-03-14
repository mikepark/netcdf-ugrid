
#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

#define nc_try( fcn )						\
  {								\
    int code;							\
    code = (fcn);						\
    if ( code )							\
      {								\
        printf("%s: %d: %s: netcdf error: %s\n",		\
	       __FILE__,__LINE__,__func__,nc_strerror(code));	\
	exit(code);						\
      }								\
  }

int main( int argc, char *argv[] )
{
  int nc_handle;
  int dim_handle;
  size_t dim_length;

  int var_handle;


  int nhex;
  int nodes_per_hex;
  int *hexes;
  int i;
  if ( argc < 2 ) 
    {
      printf("usage: %s my-netcfd-grid-filename\n",argv[0]);
      return 1;
    }

  nc_try( nc_open(argv[1], NC_NOWRITE, &nc_handle) );

  nc_try( nc_inq_dimid(nc_handle, "no_of_hexaeders", &dim_handle) );
  nc_try( nc_inq_dimlen(nc_handle, dim_handle, &dim_length) );
  nhex = (int)dim_length;

  printf("nhex %d\n",nhex);

  nc_try( nc_inq_dimid(nc_handle, "points_per_hexaeder", &dim_handle) );
  nc_try( nc_inq_dimlen(nc_handle, dim_handle, &dim_length) );
  nodes_per_hex = (int)dim_length;

  printf("nodes_per_hex %d\n",nodes_per_hex);


  hexes = (int *) malloc( nhex*nodes_per_hex*sizeof(int) );
  nc_try( nc_inq_varid(nc_handle, "points_of_hexaeders", &var_handle) );
  nc_try( nc_get_var_int(nc_handle, var_handle, hexes) );

  for (i = 0; i < 8; i++)
    {
      printf(" %d",hexes[i]);
    }
      printf("\n");

  nc_try( nc_close(nc_handle) );

  return 0;
}
