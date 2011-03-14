
#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

#define nc_try( fcn )						\
  {								\
    int code;							\
    code = (fcn);						\
    if ( code )							\
      {								\
	printf("netcdf error: %s\n", nc_strerror(code));	\
	exit(code);						\
      }								\
  }

int main( int argc, char *argv[] )
{
  int nc_handle;
  int dim_handle;
  size_t dim_length;

  int nhex;
  int nodes_per_hex;

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

  return 0;
}
