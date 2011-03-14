
netcdf-ugrid: netcdf-ugrid.c
	gcc -o netcdf-ugrid netcdf-ugrid.c

clean:
	rm -rf netcdf-ugrid