/*
 * exploracion.h
 *
 *  Created on: 19/12/2017
 *      Author: lanase
 */

#include <climits>
#include <iostream>
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <stack>
#include <cstring>
#include <map>
#include <set>
#include <utility>
#include <vector>

using namespace std;

class Explore {

public:

	// Creating a shortcut for int, int pair type
	typedef pair<int, int> Pair;
	// Creating a shortcut for pair<int, pair<int, int>> type
	typedef pair< float, pair<int, int> > pPair;

	// A structure to hold the neccesary parameters
	struct cell_exp{
	    int parent_i, parent_j;
	    double f, g, h;
	};

	struct Prop{
		int srcX, srcY, size;
		float biomass, total_cost;
	};

	struct cell_info{
		float biomass_acum, friction, relation, heuristic;
		int i, j;
		bool operator<(cell_info const& a) const{
			return relation < a.relation;
		}
	};

	struct info_ruta{
		int x, y;
		float biomass, friction, biomass_acum, heuristic;
		info_ruta() {}

		info_ruta(int x,int y, float b, float f, float ba, float h) : x(x), y(y), biomass(b), friction(f), biomass_acum(ba), heuristic(h){}
	};

	//Methods
	bool isValid(int row, int col);
	void printPath(stack<Pair> Path, ofstream& info);
	bool checkFutureInsert(int x, int y, float acum, float parada);
	void printPath2(vector<info_ruta> p, ofstream& info, Pair src);
	void aStarSearch(Pair src, float stop, ofstream& info, char heuristic);
	void explore(float** friction, int src_X, int src_Y, float stop, ofstream& info, char heuristic);
	void freeMem();
	void inicio(float** biomass);

	//Variables
	bool** closedList;
	bool** closedopenList;
	float** biomass;
	float** friction;
	int ROW, COL;
	cell_exp** cellDetails;
	//ell_exp** cellParents;
	float** matrix_path;
	float cost, cost_frict;
	int X, Y;


};
