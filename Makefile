
netcdf-ugrid: netcdf-ugrid.c
	gcc -o netcdf-ugrid netcdf-ugrid.c -lnetcdf

clean:
	rm -rf netcdf-ugrid