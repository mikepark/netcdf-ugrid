
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

  if ( argc < 2 ) 
    {
      printf("usage: %s my-netcfd-grid-filename\n",argv[0]);
      return 1;
    }

  nc_try( nc_open(argv[1], NC_NOWRITE, &nc_handle) );

  return 0;
}
