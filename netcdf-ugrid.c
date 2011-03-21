
#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

#if !defined(MIN)
#define MIN(a,b) ((a)<(b)?(a):(b)) 
#endif
#if !defined(MAX)
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define nc_try( fcn )							\
  {									\
    int try_code;							\
    try_code = (fcn);							\
    if ( try_code )							\
      {									\
        printf("%s: %d: %s: netcdf error: %s (%d)\n",			\
	       __FILE__,__LINE__,__func__,nc_strerror(try_code),	\
	       try_code);						\
	return(try_code);						\
      }									\
  }

int translate_dimension( int nc, char *variable_name, FILE *ugrid )
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

  fprintf(ugrid, " %d", length );

  return 0;
}

int translate_nodes( int nc, FILE *ugrid )
{
  int var_x, var_y, var_z;
  int var_ndim;
  int var_dims[NC_MAX_VAR_DIMS];
  int i;

  size_t dim_length;
  char dim_name[NC_MAX_NAME+1];

  int node, nnodes;
  size_t index[NC_MAX_VAR_DIMS];
  double x,y,z;

  nc_try( nc_inq_varid(nc, "points_xc", &var_x) );
  nc_try( nc_inq_varid(nc, "points_yc", &var_y) );
  nc_try( nc_inq_varid(nc, "points_zc", &var_z) );
  
  nc_try( nc_inq_varndims(nc, var_x, &var_ndim) );
  nc_try( nc_inq_vardimid(nc, var_x, &var_dims[0] ) );
  nc_try( nc_inq_dim(nc, var_dims[0], &dim_name[0], &dim_length) );
  printf(" %s = %d\n",dim_name,(int)dim_length);
  nnodes = (int)dim_length;

  for ( node = 0; node < nnodes ; node++ )
    {
      index[0] = node;
      nc_try( nc_get_var1_double(nc, var_x, index, &x) );
      nc_try( nc_get_var1_double(nc, var_y, index, &y) );
      nc_try( nc_get_var1_double(nc, var_z, index, &z) );
      fprintf(ugrid, " %25.15e", x );
      fprintf(ugrid, " %25.15e", y );
      fprintf(ugrid, " %25.15e", z );
      fprintf(ugrid, "\n" );
    }

  return 0;
}

int translate_ints( int nc, char *variable_name, FILE *ugrid )
{
  int var;
  int var_ndim;
  int var_dims[NC_MAX_VAR_DIMS];
  int i;

  size_t dim_length;
  char dim_name[NC_MAX_NAME+1];

  int element, elements;
  int node, nodes_per;
  int element_node;

  size_t index[NC_MAX_VAR_DIMS];

  int code;

  int smallest, largest;

  int *nodes;

  printf("%s:\n",variable_name);

  code = nc_inq_varid(nc, variable_name, &var);
  if ( -49 == code )
    {
      printf("%s:\n not in netcfd file\n",variable_name);
      return 0;
    }
  nc_try(code);

  nc_try( nc_inq_varndims(nc, var, &var_ndim) );
  nc_try( nc_inq_vardimid(nc, var, &var_dims[0] ) );

  nc_try( nc_inq_dim(nc, var_dims[0], &dim_name[0], &dim_length) );
  printf(" %s = %d\n",dim_name,(int)dim_length);
  elements = (int)dim_length;

  nodes_per = 1;
  if ( var_ndim > 1 )
    {
      nc_try( nc_inq_dim(nc, var_dims[1], &dim_name[0], &dim_length) );
      printf(" %s = %d\n",dim_name,(int)dim_length);
      nodes_per = (int)dim_length;
    }

  nodes = (int *)malloc( nodes_per * sizeof(int) );

  smallest = 2000000000;
  largest = -2000000000;
  for ( element = 0; element < elements ; element++ )
    {
      index[0] = element;
      for ( node = 0 ; node < nodes_per ; node++ )
	{
	  index[1] = node;
	  nc_try( nc_get_var1_int(nc, var, index, &nodes[node]) );
	  if ( nodes_per > 1 ) (nodes[node])++;
	  smallest = MIN( smallest, nodes[node] ) ;
	  largest = MAX( largest, nodes[node] ) ;
	}

      if ( strcmp("points_of_pyramids",variable_name) ==0 )
	{
	  int pry[5];
	  pry[0] = nodes[0];
	  pry[1] = nodes[3];
	  pry[2] = nodes[4];
	  pry[3] = nodes[1];
	  pry[4] = nodes[2];
	  nodes[0] = pry[0];
	  nodes[1] = pry[1];
	  nodes[2] = pry[2];
	  nodes[3] = pry[3];
	  nodes[4] = pry[4];
	}

      if ( strcmp("points_of_surfacetriangles",variable_name) ==0 )
	{
	  int tri[3];
	  tri[0] = nodes[2];
	  tri[1] = nodes[1];
	  tri[2] = nodes[0];
	  nodes[0] = tri[0];
	  nodes[1] = tri[1];
	  nodes[2] = tri[2];
	}

      if ( strcmp("points_of_surfacequadrilaterals",variable_name) ==0 )
	{
	  int quad[3];
	  quad[0] = nodes[3];
	  quad[1] = nodes[2];
	  quad[2] = nodes[1];
	  quad[3] = nodes[0];
	  nodes[0] = quad[0];
	  nodes[1] = quad[1];
	  nodes[2] = quad[2];
	  nodes[3] = quad[3];
	}

      for ( node = 0 ; node < nodes_per ; node++ )
	{
	  fprintf(ugrid, " %d", nodes[node] );
	}
      fprintf(ugrid, "\n" );
    }

  free( nodes );

  printf(" range [%d,%d]\n",smallest,largest);

  return 0;
}

int main( int argc, char *argv[] )
{
  int nc;
  FILE *ugrid;

  if ( argc < 2 ) 
    {
      printf("usage: %s my-netcfd-grid-filename\n",argv[0]);
      return 1;
    }

  nc_try( nc_open(argv[1], NC_NOWRITE, &nc) );

  ugrid = fopen("netcdf.ugrid","w");

  nc_try( translate_dimension( nc, "points_xc", ugrid ) );
  
  nc_try( translate_dimension( nc, "points_of_surfacetriangles", ugrid ) );
  nc_try( translate_dimension( nc, "points_of_surfacequadrilaterals", ugrid ) );

  nc_try( translate_dimension( nc, "points_of_tetraeders", ugrid ) );
  nc_try( translate_dimension( nc, "points_of_pyramids", ugrid ) );
  nc_try( translate_dimension( nc, "points_of_prisms", ugrid ) );
  nc_try( translate_dimension( nc, "points_of_hexaeders", ugrid ) );

  fprintf(ugrid, "\n");

  nc_try( translate_nodes( nc, ugrid ) );

  nc_try( translate_ints( nc, "points_of_surfacetriangles", ugrid ) );
  nc_try( translate_ints( nc, "points_of_surfacequadrilaterals", ugrid ) );
  nc_try( translate_ints( nc, "boundarymarker_of_surfaces", ugrid ) );
  nc_try( translate_ints( nc, "points_of_tetraeders", ugrid ) );
  nc_try( translate_ints( nc, "points_of_pyramids", ugrid ) );
  nc_try( translate_ints( nc, "points_of_prisms", ugrid ) );
  nc_try( translate_ints( nc, "points_of_hexaeders", ugrid ) );

  fclose( ugrid );

  nc_try( nc_close(nc) );

  return 0;
}
