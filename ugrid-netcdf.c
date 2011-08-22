
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

#define int_from_ugrid( ugrid, int_ptr)					\
  {									\
    if ( 1 != fscanf(ugrid.file, "%d", (int_ptr)) )			\
      {									\
        printf("%s: %d: %s: int_from_ugrid \n",				\
	       __FILE__,__LINE__,__func__);				\
	return(1);							\
      }									\
  }

#define double_from_ugrid( ugrid, double_ptr)				\
  {									\
    if ( 1 != fscanf(ugrid.file, "%lf", (double_ptr)) )			\
      {									\
        printf("%s: %d: %s: int_from_ugrid \n",				\
	       __FILE__,__LINE__,__func__);				\
	return(1);							\
      }									\
  }


typedef struct UGRID UGRID;
struct UGRID {
  FILE *file;
  int format;
};

int translate_ints( int nc, char *variable_name, UGRID ugrid )
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

  for ( element = 0; element < elements ; element++ )
    {

      for ( node = 0 ; node < nodes_per ; node++ )
	{
	  int_from_ugrid( ugrid, &nodes[node]);
	  if ( nodes_per > 1 ) (nodes[node])--;
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

      index[0] = element;
      for ( node = 0 ; node < nodes_per ; node++ )
	{
	  index[1] = node;
	  nc_try( nc_put_var1_int(nc, var, index, &nodes[node]) );
	}

    }

  free( nodes );

  return 0;
}

int main( int argc, char *argv[] )
{
  int nc;
  UGRID ugrid;

  int nnode, ntri, nquad, ntet, npyr, npri, nhex;

  int dims[NC_MAX_VAR_DIMS];
  size_t index[NC_MAX_VAR_DIMS];

  int points_xc, points_yc, points_zc;
  int var;

  int i;
  double dp;

  if ( argc < 2 ) 
    {
      printf("usage: %s my-ugrid-filename [-b8]\n",argv[0]);
      printf(" -b8 big endian steaming binary\n");
      return 1;
    }

  ugrid.file = fopen(argv[1],"r");
  if (NULL == ugrid.file)
    {
      printf("unable to open %s\n",argv[1]);
      return 1;
    }

  ugrid.format = 0;
  if( ( 2 < argc ) && ( strcmp(argv[2],"-b8") == 0 ) ) 
    {
      printf("-b8: big endian\n");
      ugrid.format = 1;
    }

  nc_try( nc_create("grid.netcdf", NC_CLOBBER, &nc) );

  int_from_ugrid( ugrid, &nnode);
  printf("nnode %d\n",nnode);

  int_from_ugrid( ugrid, &ntri);
  printf("ntri %d\n",ntri);
  int_from_ugrid( ugrid, &nquad);
  printf("nquad %d\n",nquad);

  int_from_ugrid( ugrid, &ntet);
  printf("ntet %d\n",ntet);
  int_from_ugrid( ugrid, &npyr);
  printf("npyr %d\n",npyr);
  int_from_ugrid( ugrid, &npri);
  printf("npri %d\n",npri);
  int_from_ugrid( ugrid, &nhex);
  printf("nhex %d\n",nhex);

  nc_try( nc_def_dim(nc, "no_of_elements", ntet+npyr+npri+nhex, &dims[0]) );


  if ( ntet > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_tetraeders", ntet, &dims[0]) );
      nc_try( nc_def_dim(nc, "points_per_tetraeder", 4, &dims[1]) );
      nc_try( nc_def_var(nc, "points_of_tetraeders",
			 NC_INT, 2, dims, &var) );
    }

  if ( npri > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_prisms", npri, &dims[0]) );
      nc_try( nc_def_dim(nc, "points_per_prism", 6, &dims[1]) );
      nc_try( nc_def_var(nc, "points_of_prisms", 
			 NC_INT, 2, dims, &var) );
    }

  if ( npyr > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_pyramids", npyr, &dims[0]) );
      nc_try( nc_def_dim(nc, "points_per_pyramid", 5, &dims[1]) );
      nc_try( nc_def_var(nc, "points_of_pyramids", 
			 NC_INT, 2, dims, &var) );
    }

  if ( nhex > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_hexaeders", nhex, &dims[0]) );
      nc_try( nc_def_dim(nc, "points_per_hexaeder", 8, &dims[1]) );
      nc_try( nc_def_var(nc, "points_of_hexaeders", 
			 NC_INT, 2, dims, &var) );
    }

  if ( ntri > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_surfacetriangles", ntri, &dims[0]) );
      nc_try( nc_def_dim(nc, "points_per_surfacetriangle", 3, &dims[1]) );
      nc_try( nc_def_var(nc, "points_of_surfacetriangles", 
			 NC_INT, 2, dims, &var) );
    }

  if ( nquad > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_surfacequadrilaterals", nquad, &dims[0]) );
      nc_try( nc_def_dim(nc, "points_per_surfacequadrilateral", 4, &dims[1]) );
      nc_try( nc_def_var(nc, "points_of_surfacequadrilaterals", 
			 NC_INT, 2, dims, &var) );
    }

  if ( ntri+nquad > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_surfaceelements", ntri+nquad, &dims[0]) );
      nc_try( nc_def_var(nc, "boundarymarker_of_surfaces", 
			 NC_INT, 1, dims, &var) );
    }

  nc_try( nc_def_dim(nc, "no_of_points", nnode, &dims[0]) );
  nc_try( nc_def_var(nc, "points_xc", NC_DOUBLE, 1, dims, &points_xc) );
  nc_try( nc_def_var(nc, "points_yc", NC_DOUBLE, 1, dims, &points_yc) );
  nc_try( nc_def_var(nc, "points_zc", NC_DOUBLE, 1, dims, &points_zc) );

  nc_try( nc_enddef(nc) );

  for ( i = 0; i < nnode; i++ )
    {
      index[0] = i;
      double_from_ugrid( ugrid, &dp);
      nc_try( nc_put_var1_double(nc, points_xc, index, &dp) );
      double_from_ugrid( ugrid, &dp);
      nc_try( nc_put_var1_double(nc, points_yc, index, &dp) );
      double_from_ugrid( ugrid, &dp);
      nc_try( nc_put_var1_double(nc, points_zc, index, &dp) );
    }
  
  nc_try( translate_ints( nc, "points_of_surfacetriangles", ugrid ) );
  nc_try( translate_ints( nc, "points_of_surfacequadrilaterals", ugrid ) );
  nc_try( translate_ints( nc, "boundarymarker_of_surfaces", ugrid ) );

  nc_try( translate_ints( nc, "points_of_tetraeders", ugrid ) );
  nc_try( translate_ints( nc, "points_of_pyramids", ugrid ) );
  nc_try( translate_ints( nc, "points_of_prisms", ugrid ) );
  nc_try( translate_ints( nc, "points_of_hexaeders", ugrid ) );

  nc_try( nc_close(nc) );
  fclose( ugrid.file );

  printf("%s\n", nc_inq_libvers());

  return 0;
}
