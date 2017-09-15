/*
 * main.cpp
 *
 *  Created on: 17/08/2017
 *      Author: lanase
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <float.h>
#include <vector>

#include "DisplayImage.cpp"
#include "distance.cpp"
//#include "gdal.cpp"
#include "exploracion.cpp" //A*
#include "tree.cpp" //Nodos

int main(){
	//vector<double> tiempos;

	Display_image di;
	clock_t begin = clock();
	float** costos = di.tiff_to_matrix("misiones.tiff");
	clock_t end = clock();

	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "TIFF to matrix = " << elapsed_secs << endl;
	//tiempos.push_back(elapsed_secs);

	int rows = di.get_imageHeight("misiones.tiff");
	int cols = di.get_imageWidth("misiones.tiff");
	int xIntervals = 0, yIntervals = 0;
	di.define_intervals(rows, cols, xIntervals, yIntervals);
	cout << "Passed Define Intervals" << endl;

	clock_t begin1 = clock();
	Point2D centroid = di.define_grids(rows, cols, xIntervals, yIntervals, costos);
	clock_t end1 = clock();
	double elapsed_secs1 = double(end1 - begin1) / CLOCKS_PER_SEC;
	cout << "Define grids = " << elapsed_secs1 << endl;
	cout << "Source = " << centroid.x << ", " << centroid.y << endl;

	/*cost_distance d;
	d.COLS = cols;
	d.ROWS = rows;
	d.inicio_cost_distance(costos);*/

	rutas_nodos rn;
	rn.COL = cols;
	rn.ROW = rows;
	cout << "Centroid Value: " << costos[centroid.x][centroid.y] << endl;
	clock_t begin2 = clock();
	rn.inicio_rutas(costos, centroid.x, centroid.y, 3000);
	clock_t end2 = clock();
	double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
	cout << "Busqueda rutas = " << elapsed_secs2 << endl;

	clock_t begin3 = clock();
	di.matrix_to_tiff(rn.matrix_path, rows, cols);
	exit(0);
	clock_t end3 = clock();
	double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
	cout << "Matrix to TIFF = " << elapsed_secs3 << endl;


	/*exploracion e;
	e.COL = cols;
	e.ROW = rows;
	e.explore(costos, 2.5);
	*/

}




