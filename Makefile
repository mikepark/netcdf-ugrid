
default: netcdf-ugrid ugrid-netcdf

netcdf-ugrid: netcdf-ugrid.c
	gcc -g -o netcdf-ugrid netcdf-ugrid.c -lnetcdf

ugrid-netcdf: ugrid-netcdf.c
	gcc -g -o ugrid-netcdf ugrid-netcdf.c -lnetcdf

clean:
	rm -rf netcdf-ugrid
	rm -rf ugrid-netcfd
	rm -rf *.dSYM

