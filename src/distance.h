/*
 * distance.h
 *
 *  Created on: 15/03/2018
 *      Author: lanase
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <set>
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace std;

class costDistance {
public:
	//structs
	struct cell2 {
		int x, y;
		cell2(int x, int y) :
				x(x), y(y) {}
	};

	struct cell {
		int x, y;
		float distance;
		cell(int x, int y, float distance) :
				x(x), y(y), distance(distance) {}
	};



	//methods
	bool isInsideGrid(int i, int j);
	inline set<cell>vecinos(int origen_x, int origen_y);
	inline void acumulados(set<cell> active_costs, int origen_x, int origen_y, float intervals, float xMin, float xMax, float yMin, float yMax);
	void inicio_cost_distance(float** grid, int srcX, int srcY, float intervals, float xMin, float xMax, float yMin, float yMax, double projection);
	void freeMem();


	//variables
	int ROW, COL;
	int** locations;
	float** output_raster;
	float** cost_raster;
	bool** active_raster;
	int rutas[8][8]{
		{0,0,0,0,0,0,0,0},
		{-1,0,0,1,0,1,-1,0},
		{0,0,0,0,0,0,0,0},
		{0,1,1,0,1,0,0,1},
		{0,0,0,0,0,0,0,0},
		{0,-1,1,0,1,0,0,-1},
		{0,0,0,0,0,0,0,0},
		{-1,0,0,-1,0,-1,-1,0},
	};

};
