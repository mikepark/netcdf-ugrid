
NETCDF=/usr/local
CFLAGS=-g

default: netcdf-ugrid ugrid-netcdf

netcdf-ugrid: netcdf-ugrid.c
	gcc ${CFLAGS} -I${NETCDF}/include -o netcdf-ugrid netcdf-ugrid.c -L${NETCDF}/lib -lnetcdf

ugrid-netcdf: ugrid-netcdf.c
	gcc ${CFLAGS} -I${NETCDF}/include -o ugrid-netcdf ugrid-netcdf.c -L${NETCDF}/lib -lnetcdf

clean:
	rm -rf netcdf-ugrid
	rm -rf ugrid-netcdf
	rm -rf *.dSYM

