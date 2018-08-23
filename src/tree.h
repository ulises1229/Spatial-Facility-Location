/*
 * tree.h
 *
 *  Created on: 12/12/2017
 *      Author: lanase
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <queue>
#include <stdio.h>
#include <algorithm>
#include <list>
#include <math.h>
#include "treenode.h"

using namespace std;

class Tree {

public:

	struct Node{
		int x, y;
	};

	struct nodeInfo {
		float value, fricc, relation, acum_biomass, heuristic;
		int x, y, id;
		bool expandable;

		nodeInfo() {}

		nodeInfo(float value, float fricc, float relation, int x, int y, int id, bool expandable, float acum_biomass, float heuristic) : value(value), fricc(fricc), relation(relation), x(x), y(y), id(id)
		, expandable(expandable), acum_biomass(acum_biomass), heuristic(heuristic){}

		bool operator<(nodeInfo const &a) const{
				return relation < a.relation;
			}
	};

	struct findElement {
		int x, y;
		findElement() {};
		findElement(int x, int y) : x(x), y(y) {}
		bool operator () (findElement const &e) const {
			return (x == e.x && y == e.y);
		}
	};

	bool isValid(int row, int col);
	void printPath(vector<nodeInfo> nodes, ofstream& info);
	void printMatrix(vector<nodeInfo> nodes, Node src);
	void ruta(vector<nodeInfo> nodes, Node src, ofstream& info);
	void rutas(Node src, float stop, ofstream& info,char heuristic);
	void inicio_rutas(float** grid, float** cost_dist, int srcX, int srcY, float stop, ofstream& info, char heuristic);
	void getExplorationType(vector<nodeInfo> children, char heuristic);
	void freeMem();


	//Variables
	vector<nodeInfo>::iterator it;
	set<nodeInfo> sortedNodes;
	float** biomass;
	float** friction;
	bool** visitedNodes;
	float cost, cost_frict;
	int ROW, COL, x, y;
	float** matrix_path;



};

