/*
 * dijkstra.cpp
 *
 *  Created on: 19/10/2017
 *      Author: lanase
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <utility>
#include <list>
#include <iterator>
#include <queue>
#include <vector>
#include <algorithm>
#include <stack>
#include <fstream>
#include <math.h>
using namespace std;

// iPair ==>  Integer Pair
typedef pair<float, int> iPair;
typedef pair<int, int> Pair;
//typedef pair< int, pair<int, int> > pPair;

struct nodes_info{
	int id_dest, weight, x, y;
};

struct nodeGraph{
	int id, x, y;
	float biomass;// friction, relation;
	//list< iPair > *adj;
};

// This class represents a directed graph using
// adjacency list representation


class Graph{
    int V;    // No. of vertices
    vector<nodeGraph> nodes;
    list< iPair > *adj;

public:
    //Graph();
    //Graph(int V, vector<nodeGraph> nodes);  // Constructor
    int ROW, COL, X, Y;
    float cost, cost_frict;
    float** biomass;
	float** friction;
	bool** closedList;
	stack<Pair> Path;
	float** matrix_path;
    // function to add an edge to graph
    /*void addEdge(int u, int v, float w);

    void dijkstra(int id_src, int src_x, int src_y);
    bool rutaValida(vector<int> parent, int n);
    void printPath(vector<int> parent, int n, int acum);
    void dijkstra_inicio(float** biomass, float** friction, int src_X, int src_Y, float stop);*/


//Graph::Graph(){}
// Allocates memory for adjacency list
void i_Graph(int V, vector<nodeGraph> nodes){
    this->V = V;
    this->nodes = nodes;
    adj = new list<iPair> [V];
}

void addEdge(int u, int v, float w){//lista de adyacencia de id_nodo u, id_destino v , relation w
    adj[u].push_back(make_pair(w, v));// relacion biomasa/friccion, destino
}

// A utility function used to print the solution
/*void printPath(vector<int> parent, int n){
    //printf("Vertex   Distance from Source\n");
	// Base Case : If j is source
	    if (parent[n]==-1)
	        return;

	    printPath(parent, parent[n]);
	    //list< pair<int, int> >::iterator i;
	    //i = this->adj[n].begin();
	    printf("-> %d (%d, %d)  %f %f\n ", n, this->nodes.at(n).x, this->nodes.at(n).y, this->nodes.at(n).biomass, this->nodes.at(n).friction);
	    Path.push(make_pair (this->nodes.at(n).x, this->nodes.at(n).y));
	    //printf("%d ", n);
}*/

void fillMatrixPath(ofstream& info){
	this->matrix_path = new float*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->matrix_path[i] = new float[COL];
		for(int j = 0; j < COL ; j++){
			matrix_path[i][j] = 0;
		}
	}

	int s = Path.size();
	float cost = 0, biomass= 0, frict = 0;
	 while (!Path.empty()){
		pair<int,int> p = Path.top();
		Path.pop();
		//printf("-> (%d,%d) - %f - %f\n ",p.first,p.second, this->biomass[p.first][p.second], this->friction[p.first][p.second]);
		//cout << friction[p.first][p.second] << endl;
		if(friction[p.first][p.second]>0) {
            cost += (this->biomass[p.first][p.second] / friction[p.first][p.second]);
            frict += friction[p.first][p.second];
            matrix_path[p.first][p.second] = friction[p.first][p.second];
        } else {
            matrix_path[p.first][p.second] = 255;
        }

		biomass += this->biomass[p.first][p.second];
	}
    info << s << ",";
    info << biomass << ",";
    info << cost << endl;
	cout << "Size: " << s << endl;
    cout << "Cost: " << cost << endl;
    cout << fixed << "Cost ($): " << frict << endl;
    cout << "Biomass: " << biomass << endl;
    this->cost = cost;
    this->cost_frict = frict;
}


bool rutaValida(vector<int> parent, int n){
	//cout << "valdiar" << n << " "  << parent[n] << endl;
	bool ciclo = false;
	int destino = n;
	vector<int> vis(V);
	vis.push_back(destino);
	std::vector<int>::iterator it;
	while(1){
		if(parent[n] == 0)
			break;
		it = find (vis.begin(), vis.end(), parent[n]);
		if(it != vis.end()){//found = ciclo
			ciclo = true; break;
		}else{
			vis.push_back(parent[n]);
			n = parent[n];
		}
	}
	return ciclo;
}





// A Utility Function to check whether given cell (row, col)
// is a valid cell or not.
bool isValid(int row, int col){
    // Returns true if row number and column number
    // is in range
    return (row >= 0) && (row < ROW) &&
           (col >= 0) && (col < COL);
}


bool rutaValida(int c, vector<int> parent, int n){
	//cout << "valdiar" << n << " "  << parent[n] << endl;
	//visited[n] = true;
	bool ciclo = false;
	int destino = n;
	vector<int> vis(c);
	vis.push_back(destino);
	std::vector<int>::iterator it;
	while(1){
		if(parent[n] == 0)
			break;
		it = find (vis.begin(), vis.end(), parent[n]);
		if(it != vis.end()){//found = ciclo
			//cout << "Cicloooo " << parent[n] << endl;
			ciclo = true; break;
		}else{
			vis.push_back(parent[n]);
			n = parent[n];
		}
	}
	return ciclo;
}

struct prq{
	float relation;//biomasa/friccion
	int id_dest;
	float acum;//pura biomasa
	bool operator<(const prq& rhs) const{
	        return relation < rhs.relation;
	    }
};


void dijkstra(int id_src, int src_x, int src_y, int stop, ofstream &points, int xMin, int xMax, int yMin, int yMax, int** enum_grid, char heuristic){
	// Create a priority queue to store vertices that
	// are being preprocessed. This is weird syntax in C++.
	// Refer below link for details of this syntax
	// http://geeksquiz.com/implement-min-heap-using-stl/
	priority_queue< prq > pq2;

	// Insert source itself in priority queue and initialize
	// its distance as 0.
	//pq.push(make_pair(nodes.at(id_src).w, id_src));//0??
	prq info = {nodes.at(id_src).biomass, id_src, nodes.at(id_src).biomass};//weight, id, acum
	pq2.push(info);

	//dist[id_src] = nodes.at(id_src).biomass;
	//cout << "nodes at " << id_src << " " << this->nodes.at(id_src).x << ", " << this->nodes.at(id_src).y <<  " biomasa= " << nodes.at(id_src).biomass << " biomass[][]" << this->biomass[this->nodes.at(id_src).x][this->nodes.at(id_src).y] << endl;

	/* Looping till priority queue becomes empty (or all
	  distances are not finalized) */
	//int tope = 30;
	int destino = 0;
	int acumu = 0;
	float acum = 0;
	float biomasa_total = 0;
	//cout << "a while" << endl;
  	while (!pq2.empty()){
		// The first vertex in pair is the minimum distance
		// vertex, extract it from priority queue.
		// vertex label is stored in second of pair (it
		// has to be done this way to keep the vertices
		// sorted distance (distance must be first item
		// in pair)
		int u = pq2.top().id_dest;//ori
		acum = pq2.top().acum;//solo biomasa
		float weight_pq = pq2.top().relation; // biomasa/friccion

		if(closedList[this->nodes.at(u).x][this->nodes.at(u).y] == false){
			biomasa_total += this->biomass[this->nodes.at(u).x][this->nodes.at(u).y];

			Path.push(make_pair (this->nodes.at(u).x, this->nodes.at(u).y));
			//cout << "estoy nodo: " << u << " " << this->nodes.at(u).x << ", " << this->nodes.at(u).y << " biomass[][] = " << this->biomass[this->nodes.at(u).x][this->nodes.at(u).y] << " friction[][] = " << this->friction[this->nodes.at(u).x][this->nodes.at(u).y] << " con relacion: " << weight_pq << " con biom acumu: " << acum << endl;
			pq2.pop();
			closedList[this->nodes.at(u).x][this->nodes.at(u).y] = true;
			//cout << "CLOSED LISTT " << this->nodes.at(u).x << ", " << this->nodes.at(u).y << endl;

			if(biomasa_total > stop){
				pq2 = priority_queue <prq>();
				acumu = biomasa_total;
				destino = u;
                for(int m=0; m < ROW; m++){
                    delete[] enum_grid[m];
                }
				goto finish;
			}


			//adj[u].sort(greater<iPair>());
			//list< pair<float, int> >::iterator x;
			/*for (x = adj[u].begin(); x != adj[u].end(); ++x){
				cout << (*x).first << " " << (*x).second << endl;
			}*/
            int dx[] = { -1, -1, 0, 1, 1,  1,  0,-1 };
            int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
            int id_dest;
            for(int k = 0; k < 8; k++){
                bool flag = true;
                //cout << "k = " << k << endl;
                int x = this->nodes.at(u).x  + dx[k];
                int y = this->nodes.at(u).y + dy[k];
                //cout << this->biomass[x][y] << " " << this->friction[x][y] << endl;
                //if((*it).x == 1902 && (*it).y == 1498)
                //cout << x << ", " << y << endl;
                if(isValid(x, y) && this->biomass[x][y] > 0 && this->friction[x][y] >= 0 && (x != src_x || y != src_y) && x >= xMin && x <= xMax && y >= yMin && y <= yMax){
                    id_dest = enum_grid[x][y];
                    list< pair<float, int> >::iterator iter;
                    //cout << "size de lista de adyacencia de "<< id_dest << " : " << adj[id_dest].size() << endl;
                    if(adj[id_dest].size() == 0) {
                        float heuristicResult = 0, relation = 0;
                        if (heuristic == 'e') {
                            //cout << "I'm inside euclidean" << endl;
                            heuristicResult = sqrt(pow((src_x - x), 2) + pow((src_y - y), 2));
                        }
                        else if (heuristic == 'm') {
                            heuristicResult = abs(src_x - x) + abs(src_y - y);
                        }
                        else if(heuristic == 'd') {
                            heuristicResult = max(abs(src_x - x), abs(src_y - y));
                        }
                        if (heuristicResult > 0)
                            relation = biomass[x][y] / (friction[x][y] + heuristicResult);
                        else
                            relation = biomass[x][y] / friction[x][y];
                        addEdge(nodes.at(u).id, id_dest, relation);
                        /*if(nodes.at(u).x == 1902 && nodes.at(u).y == 1498)*/
                        //cout << "added edge in " << nodes.at(u).x << "," << nodes.at(u).y << ": " << x << ", " << y << endl;
                    } else {
                        for (iter = adj[id_dest].begin(); iter != adj[id_dest].end(); ++iter){
                            //		cout << "Lista de adyacencia de " << id_dest << ": " << (*iter).second << endl;
                            if((*iter).second == nodes.at(u).id) {
                                flag = false;
                                break;
                                //if((*it).id == 1729)
                                //cout << "Added edge 1729.ñ.ñ." << endl;
                                //addEdge((*it).id, id_dest, this->nodes.at(id_dest).relation);
                                //cout << "added edge in " << (*it).id << ": " << id_dest << endl;
                            }
                        }
                        if(flag) {
                            float heuristicResult = 0, relation = 0;
                            if (heuristic == 'e') {
                                //cout << "I'm inside euclidean" << endl;
                                heuristicResult = sqrt(pow((src_x - x), 2) + pow((src_y - y), 2));
                            }
                            else if (heuristic == 'm') {
                                heuristicResult = abs(src_x - x) + abs(src_y - y);
                            }
                            else if(heuristic == 'd') {
                                heuristicResult = max(abs(src_x - x), abs(src_y - y));
                            }
                            if (heuristicResult > 0)
                                relation = (biomass[x][y] / friction[x][y] + heuristicResult);
                            else
                                relation = biomass[x][y] / friction[x][y];
                            addEdge(nodes.at(u).id, id_dest, relation);
                            /*if(nodes.at(u).x == 1902 && nodes.at(u).y == 1498)*/
                            //cout << "added edge in " << nodes.at(u).x << "," << nodes.at(u).y << ": " << x << ", " << y << endl;
                        }
                    }
                }
            }

			// 'i' is used to get all adjacent vertices of a vertex
			list< pair<float, int> >::iterator i;

			for (i = adj[u].begin(); i != adj[u].end(); ++i){
				if(closedList[this->nodes.at((*i).second).x][this->nodes.at((*i).second).y] == false){
                    //cout << "Hijo de " << this->nodes.at(u).x << ", " << this->nodes.at(u).y << ": " << this->nodes.at((*i).second).x << ", " << this->nodes.at((*i).second).y << endl;
					//cout << "closedList is FALSE" << this->nodes.at((*i).second).x << ", " << this->nodes.at((*i).second).y << endl;
					int v = (*i).second;//destino
					float weight = (*i).first; /// nodes.at(v).friction;//peso biomasa-friccion, se ordena en pq de acuerdo a este
					float biom = nodes.at(v).biomass;//(*i).first;//biomasa
					//cout << v << ", " << biom << endl;
					info = {weight, v, acum + biom}; //dist[v]};//weight = bioma/friccion, v=id_dest, dist[v] = acum
					pq2.push(info);
					//cout << "insert a pq2 " << weight << ", " << v << " - " << this->nodes.at(v).x << ", " << this->nodes.at(v).y << " " << acum + biom << endl;

				}
			}
		}
		else{
			//cout << "nodo ya visitado: " << u << " - " << this->nodes.at(u).x << ", " << this->nodes.at(u).y << endl;
			pq2.pop();
		}
	}
	/*cout << "d" << endl;
	exit(0);*/
	/*cout << "\nDijkstra Adjacency\nTOPE\n" << stop << endl;
	cout << "ACUM\n" << acumu << endl;
	cout << "\nPATH\n";*/

	//printPath(parent, destino);
	finish:
  		fillMatrixPath(points);
	//cout << "file matrixpath " << endl;
	//ofstream file;
	/*file.open("matrixpath.txt");
	for(int a = 0; a < ROW; a++){
		for(int b = 0; b < COL; b++){
			file << matrix_path[a][b] << " ";
		}
		file << endl;
	}*/
}

// Driver program to test methods of graph class
void dijkstra_inicio(float** biomass, float** friction, int src_X, int src_Y, float stop, char heuristic, int xMin, int xMax, int yMin, int yMax, float intervals, ofstream &info){

	this->biomass = new float*[this->ROW];
	this->friction = new float*[this->ROW];
	this->closedList = new bool*[this->ROW];
	int** enum_grid;
	enum_grid = new int*[ROW];
	for(int i = 0; i< ROW; ++i){
		this->biomass[i] = new float[COL];
		this->friction[i] = new float[COL];
		this->closedList[i] = new bool[COL];
		enum_grid[i] = new int[COL];
	}

	for(int x = 0; x < ROW; x++){
		for(int y = 0; y < COL; y++){
			this->biomass[x][y] = biomass[x][y];
			this->friction[x][y] = friction[x][y];
			this->closedList[x][y] = false;
		}
	}

	int v = 0;
	vector<nodeGraph> nodes;
	nodeGraph ng;
	int id_src;
	cout << src_X << " " << src_Y << endl;
	//ofstream nodos;
	//nodos.open("nodos.txt");
	int exp;

	exp = intervals * 4;

	xMin = xMin - exp;
	yMin = yMin - exp;
	xMax = xMax + exp;
	yMax = yMax + exp;

	if(xMin < 0)
		xMin = 0;

	if(yMin < 0)
		yMin = 0;

	if(xMax >= ROW)
		xMax = ROW - 1;

	if(yMax >= COL)
		yMax = COL - 1;

	//cout << "ROWS: " << ROW << " - COLS: " << COL << " - xMin: " << xMin << " - xMax: " << xMax << " - yMin: " << yMin << " - yMax: " << yMax << endl;

	for(int i = xMin; i <= xMax; i++){
		for(int j = yMin; j <= yMax; j++){

			if(src_X == i && src_Y == j){
				id_src = v;
				//cout << "id_Src " << id_src << endl;
			}
			if(this->biomass[i][j] > 0 && this->friction[i][j] >= 0){
                ng.id = v; ng.x = i; ng.y = j;ng.biomass = this->biomass[i][j];// ng.friction = this->friction[i][j];
                nodes.push_back(ng);
				//nodos << ng.id << " " << ng.x << ", " << ng.y << " " << ng.biomass << " / " << ng.friction << " = " << ng.relation << endl;
				//cout << "id = " << v << " bioma = "<<  ng.biomass << endl;;
				enum_grid[i][j] = v;
				v++;
			}
			//cout << i << ", " << j << endl;
		}
	}

	//nodos.close();
	cout << "nodos " << v << endl;
	//cout << "antes I nodes at " << id_src << " " << nodes.at(id_src).x << ", " << nodes.at(id_src).y << endl;
	i_Graph(v, nodes);

	nodes.clear();
	//cout << "tTAMAÑO VECTOR NODES TEMPORAL = "<< nodes.size() << endl;




	vector <nodeGraph> :: iterator it;
	int x, y;
	int id_dest;
	//ofstream myfile;
    //myfile.open ("adjacency.txt");
	it = this->nodes.begin();
    //cout << "id inicio : " << (*it).id << endl;
	/*for(int k = 0; k < 8; k++){
		//cout << "k = " << k << endl;
		x = (*it).x + dx[k];
		y = (*it).y + dy[k];
		//cout << x << " " << y << endl;
		//cout << this->biomass[x][y] << " " << this->friction[x][y] << endl;
		if(isValid(x, y) && this->biomass[x][y] > 0 && this->friction[x][y] >= 0 && (x != src_X || y != src_Y) && x >= xMin && x <= xMax && y >= yMin && y <= yMax){
			id_dest = enum_grid[x][y];
		//	cout << id_dest << endl;
			addEdge((*it).id, id_dest, this->nodes.at(id_dest).relation);
		}
	}*/
	cout << this->nodes.size() << endl; //exit(0);

	/*for (it = this->nodes.begin(); it != this->nodes.end(); ++it){
		//cout << "vector nodes " << (*it).id << " - " << (*it).x << ", " << (*it).y << endl;

			//myfile <<  "vector nodes " << (*it).id << " - " << (*it).x << ", " << (*it).y << endl;
        if((*it).x == 1902 && (*it).y == 1498)
            cout << "here" << endl;
		for(int k = 0; k < 8; k++){
		    bool flag = true;
			//cout << "k = " << k << endl;
			x = (*it).x + dx[k];
			y = (*it).y + dy[k];
 			//cout << this->biomass[x][y] << " " << this->friction[x][y] << endl;
            if((*it).x == 1902 && (*it).y == 1498)
                cout << x << ", " << y << endl;
			if(isValid(x, y) && this->biomass[x][y] > 0 && this->friction[x][y] >= 0 && (x != src_X || y != src_Y) && x >= xMin && x <= xMax && y >= yMin && y <= yMax){
				id_dest = enum_grid[x][y];
				list< pair<float, int> >::iterator iter;
				//cout << "size de lista de adyacencia de "<< id_dest << " : " << adj[id_dest].size() << endl;
				if(adj[id_dest].size() == 0) {
				    float heuristicResult = 0, relation = 0;
                    if (heuristic == 'e') {
                        //cout << "I'm inside euclidean" << endl;
                        heuristicResult = sqrt(pow((src_X - x), 2) + pow((src_Y - y), 2));
                    }
                    else if (heuristic == 'm') {
                        heuristicResult = abs(src_X - x) + abs(src_Y - y);
                    }
                    else if(heuristic == 'd') {
                        heuristicResult = max(abs(src_X - x), abs(src_Y - y));
                    }
                    if (heuristicResult > 0)
                        relation = (biomass[x][y] / friction[x][y] + heuristicResult);
                    else
                        relation = biomass[x][y] / friction[x][y];
					addEdge((*it).id, id_dest, relation);
                    if((*it).x == 1902 && (*it).y == 1498)
				        cout << "added edge in " << (*it).x << "," << (*it).y << ": " << x << ", " << y << endl;
				} else {
					for (iter = adj[id_dest].begin(); iter != adj[id_dest].end(); ++iter){
				//		cout << "Lista de adyacencia de " << id_dest << ": " << (*iter).second << endl;
						if((*iter).second == (*it).id) {
						    flag = false;
                            break;
							//if((*it).id == 1729)
								//cout << "Added edge 1729.ñ.ñ." << endl;
							//addEdge((*it).id, id_dest, this->nodes.at(id_dest).relation);
                            //cout << "added edge in " << (*it).id << ": " << id_dest << endl;
						}
					}
					if(flag) {
                        float heuristicResult = 0, relation = 0;
                        if (heuristic == 'e') {
                            //cout << "I'm inside euclidean" << endl;
                            heuristicResult = sqrt(pow((src_X - x), 2) + pow((src_Y - y), 2));
                        }
                        else if (heuristic == 'm') {
                            heuristicResult = abs(src_X - x) + abs(src_Y - y);
                        }
                        else if(heuristic == 'd') {
                            heuristicResult = max(abs(src_X - x), abs(src_Y - y));
                        }
                        if (heuristicResult > 0)
                            relation = (biomass[x][y] / friction[x][y] + heuristicResult);
                        else
                            relation = biomass[x][y] / friction[x][y];
                        addEdge((*it).id, id_dest, relation);
                        if((*it).x == 1902 && (*it).y == 1498)
                            cout << "added edge in " << (*it).x << "," << (*it).y << ": " << x << ", " << y << endl;
					}
				}
			}
		}*/
		/*if(adj[(*it).id].size() >= 8){
		    cout << "Lidsta mayor a 8 = " << (*it).id << endl;
		}
	}*/
	this->X = src_X; this->Y = src_Y;
	//exit(0);
	dijkstra(id_src, src_X, src_Y, stop, info, xMin, xMax, yMin, yMax, enum_grid, heuristic);
	        //fillMatrixPath();


}

void freeMem() {
	for(int m=0; m < ROW; m++){
		delete[] this->biomass[m];
		delete[] this->friction[m];
		delete[] this->closedList[m];
		delete[] this->matrix_path[m];

	}
	adj->clear();
	nodes.clear();
    /*cout << "TAMAÑO ADJ = " << adj->size() << endl;
    cout << "TAMAÑO NODES = " << nodes.size() << endl;*/
}

};
