
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

typedef struct ElemStruct ElemStruct;
typedef ElemStruct * Elem;

struct ElemStruct {
  int dims;
  int *dim;
  int total;
  int *e2n;
};

int elem_create_from( Elem elem, int nc_handle, char *variable_name )
{
  int var_handle;
  int var_ndim;
  int var_dimids[NC_MAX_VAR_DIMS];
  int i;

  int dim_handle;
  size_t dim_length;
  char dim_name[NC_MAX_NAME+1];

  nc_try( nc_inq_varid(nc_handle, variable_name, &var_handle) );
  nc_try( nc_inq_varndims(nc_handle, var_handle, &var_ndim) );
  printf("dims %d\n",var_ndim);
  elem->dims = var_ndim;

  nc_try( nc_inq_vardimid(nc_handle, var_handle, &var_dimids[0] ) );
  elem->dim = (int *) malloc( elem->dims * sizeof(int) );
  elem->total = 1;
  for ( i = 0 ; i < elem->dims ; i++ )
    {
      nc_try( nc_inq_dim(nc_handle, var_dimids[i], &dim_name[0], &dim_length) );
      printf("%s = %d\n",dim_name,(int)dim_length);
      elem->dim[i] = (int)dim_length;
      elem->total = elem->total * elem->dim[i];
    }
  printf("total = %d\n",elem->total);
  elem->e2n = (int *) malloc( (elem->total) * sizeof(int) );

  nc_try( nc_get_var_int(nc_handle, var_handle, elem->e2n ) );

  return 0;
}

int main( int argc, char *argv[] )
{
  int nc_handle;
  Elem hexes;

  if ( argc < 2 ) 
    {
      printf("usage: %s my-netcfd-grid-filename\n",argv[0]);
      return 1;
    }

  nc_try( nc_open(argv[1], NC_NOWRITE, &nc_handle) );

  hexes = (Elem)malloc(sizeof(ElemStruct));
  nc_try( elem_create_from( hexes, nc_handle, "points_of_hexaeders" ) );

  nc_try( nc_close(nc_handle) );

  return 0;
}
