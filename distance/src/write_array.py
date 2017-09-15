#!/usr/bin/python
import numpy, sys
from osgeo import gdal
from osgeo.gdalconst import *

def writeArray(*array):
	print 'Creating Tiff image. Please, wait...'
	rows = array[0]
	cols = array[1]
	
	# register all of the GDAL drivers
	gdal.AllRegister()

	# create the output image
	driver = gdal.GetDriverByName('GTiff')

	#print driver
	outDs = driver.Create("misiones_routes.tif", cols, rows, 1, gdal.GDT_Int32)
	if outDs is None:
	    print 'Could not create reclass_40.tif'
	    sys.exit(1)

	outBand = outDs.GetRasterBand(1)
	outData = numpy.zeros((rows,cols), numpy.int32)

	cont = 2
	
	#nanCount = 0
	#valCount = 0

	#print("Array is: ", len(array))

	for i in range(0, rows):
	    for j in range(0, cols):
		if array[cont] > 0:
			outData[i,j] = array[cont] 
			print outData[i,j]
		cont = cont  + 1
		#if array[cont] < 0:
		#	outData[i,j] = 0
		#	nanCount = nanCount + 1
			#print("NaN Value")
		#else:
		#	outData[i,j] = 100
		#	valCount = valCount + 1
		
		#if array[cont] >= 0:
			#print("Pos: ", cont, "  Val: ", array[cont])
		
		#print outData[i,j]
	# write the data
	outBand.WriteArray(outData)

	# flush data to disk, set the NoData value and calculate stats
	outBand.FlushCache()
	outBand.SetNoDataValue(-9999)

	# georeference the image and set the projection
	#outDs.SetGeoTransform(inDs.GetGeoTransform())
	#outDs.SetProjection(inDs.GetProjection())

	#print("NaN: ", nanCount, "   Values: ", valCount)

	del outData

	print 'Tiff succesfuly created.'
