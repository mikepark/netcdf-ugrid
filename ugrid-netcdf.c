
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

#define EI( aa, bb )							\
  {									\
    int a, b;								\
    a = (aa);								\
    b = (bb);								\
    if ( a != b )							\
      {									\
        printf("%s: %d: %s: was %d, expected %d\n",			\
	       __FILE__,__LINE__,__func__,b,a);				\
	return(1);							\
      }									\
  }


int main( int argc, char *argv[] )
{
  int nc;
  FILE *ugrid;

  int nnode, ntri, nquad, ntet, npyr, npri, nhex;

  int dims[NC_MAX_VAR_DIMS];
  size_t index[NC_MAX_VAR_DIMS];

  int points_xc, points_yc, points_zc;
  int points_of_surfacetriangles;
  int points_of_surfacequadrilaterals;

  int i;
  double dp;

  if ( argc < 2 ) 
    {
      printf("usage: %s my-ugrid-filename\n",argv[0]);
      return 1;
    }

  ugrid = fopen(argv[1],"r");
  if (NULL == ugrid)
    {
      printf("unable to open %s\n",argv[1]);
      return 1;
    }

  nc_try( nc_create("grid.netcdf", NC_CLOBBER, &nc) );

  EI( 7, fscanf(ugrid, "%d %d %d %d %d %d %d",
		&nnode, &ntri, &nquad, &ntet, &npyr, &npri, &nhex) );

  printf("nnode %d\n",nnode);
  printf("ntri %d\n",ntri);
  printf("nquad %d\n",nquad);
  printf("ntet %d\n",ntet);
  printf("npyr %d\n",npyr);
  printf("npri %d\n",npri);
  printf("nhex %d\n",nhex);

  nc_try( nc_def_dim(nc, "no_of_points", nnode, &dims[0]) );
  nc_try( nc_def_var(nc, "points_xc", NC_DOUBLE, 1, dims, &points_xc) );
  nc_try( nc_def_var(nc, "points_yc", NC_DOUBLE, 1, dims, &points_yc) );
  nc_try( nc_def_var(nc, "points_zc", NC_DOUBLE, 1, dims, &points_zc) );

  points_of_surfacetriangles = 0;
  if ( ntri > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_surfacetriangles", ntri, &dims[0]) );
      nc_try( nc_def_dim(nc, "points_per_surfacetriangle", 3, &dims[1]) );
      nc_try( nc_def_var(nc, "points_of_surfacetriangles", NC_INT, 2, dims, 
			 &points_of_surfacetriangles) );
    }

  points_of_surfacequadrilaterals = 0;
  if ( nquad > 0 )
    {
      nc_try( nc_def_dim(nc, "no_of_surfacequadrilaterals", nquad, &dims[0]) );
      nc_try( nc_def_dim(nc, "points_per_surfacequadrilateral", 4, &dims[1]) );
      nc_try( nc_def_var(nc, "points_of_surfacequadrilaterals", NC_INT, 2, dims, 
			 &points_of_surfacequadrilaterals) );
    }

  nc_try( nc_enddef(nc) );

  for ( i = 0; i < nnode; i++ )
    {
      index[0] = i;
      EI( 1, fscanf(ugrid, "%lf", &dp ) );
      nc_try( nc_put_var1_double(nc, points_xc, index, &dp) );
      EI( 1, fscanf(ugrid, "%lf", &dp ) );
      nc_try( nc_put_var1_double(nc, points_yc, index, &dp) );
      EI( 1, fscanf(ugrid, "%lf", &dp ) );
      nc_try( nc_put_var1_double(nc, points_zc, index, &dp) );
    }

  nc_try( nc_close(nc) );
  fclose( ugrid );

  return 0;
}
