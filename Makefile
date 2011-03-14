
netcdf-ugrid: netcdf-ugrid.c
	gcc -g -o netcdf-ugrid netcdf-ugrid.c -lnetcdf

clean:
	rm -rf netcdf-ugrid