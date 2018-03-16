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
	float biomass, friction, w;
	//list< iPair > *adj;
};

// This class represents a directed graph using
// adjacency list representation
class Graph{
    int V;    // No. of vertices
    vector<nodeGraph> nodes;
    // In a weighted graph, we need to store vertex
    // and weight pair for every edge
    list< iPair > *adj;

public:
    //Graph();
    //Graph(int V, vector<nodeGraph> nodes);  // Constructor
    int ROW, COL;
    float** biomass;
	float** friction;
	stack<Pair> Path;
	int** matrix_path;
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

void addEdge(int u, int v, float w){
    adj[u].push_back(make_pair(w, v));// peso en biomasa, destino
}

// A utility function used to print the solution
void printPath(vector<int> parent, int n){
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
}

void fillMatrixPath(){
	this->matrix_path = new int*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->matrix_path[i] = new int[COL];
		for(int j = 0; j < COL ; j++){
			matrix_path[i][j] = 0;
		}
	}

	int s = Path.size();
	float totCost = 0, totBiom = 0;

	 while (!Path.empty()){
		pair<int,int> p = Path.top();
		Path.pop();
		//printf("-> (%d,%d) ",p.first,p.second);
		//cout << biomass[p.first][p.second] << endl;
		totBiom += biomass[p.first][p.second];
		totCost += friction[p.first][p.second];
		matrix_path[p.first][p.second] = 1;
	}
	 cout << "Total Biomass: " << totBiom << "\t Total Cost: " << totCost << "\t Size: " << s << endl;
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
	float weight;//biomasa-friccion
	int id_dest;
	float acum;//pura biomasa
	bool operator<(const prq& rhs) const{
	        return acum < rhs.acum;
	    }
};


void dijkstra(int id_src, int src_x, int src_y, int stop){
	// Create a priority queue to store vertices that
	// are being preprocessed. This is weird syntax in C++.
	// Refer below link for details of this syntax
	// http://geeksquiz.com/implement-min-heap-using-stl/
	priority_queue< prq > pq2;
	// Create a vector for distances and initialize all
	// distances as infinite (INF)
	vector<float> dist(V, 0);
	// Parent array to store shortest path tree
	//int parent[V];
	vector<int> parent(V);
	parent[0] = -1;
	// Insert source itself in priority queue and initialize
	// its distance as 0.
	//pq.push(make_pair(nodes.at(id_src).w, id_src));//0??
	prq info = {nodes.at(id_src).biomass, id_src, nodes.at(id_src).biomass};//weight, id, acum
	pq2.push(info);
	dist[id_src] = nodes.at(id_src).biomass;
	//cout << "nodes at " << id_src << " biomasa= " << nodes.at(id_src).biomass << endl;
	/* Looping till priority queue becomes empty (or all
	  distances are not finalized) */
	//int tope = 30;
	int destino = 0;
	int acumu = 0;
	//cout << "a while" << endl;
	while (!pq2.empty()){
		// The first vertex in pair is the minimum distance
		// vertex, extract it from priority queue.
		// vertex label is stored in second of pair (it
		// has to be done this way to keep the vertices
		// sorted distance (distance must be first item
		// in pair)
		int u = pq2.top().id_dest;
		float acum = pq2.top().acum;//solo biomasa
		float weight_pq = pq2.top().weight; // biomasa-friccion
		//cout << "estoy nodo: " << u << " con peso: " << weight_pq << " con biom acumu: " << acum << endl;
		pq2.pop();

		if(acum > stop){
			acumu = acum;
			destino = u;
			break;
		}


		adj[u].sort(greater<iPair>());
		// 'i' is used to get all adjacent vertices of a vertex
		list< pair<float, int> >::iterator i;
		int costosos = 0;
		for (i = adj[u].begin(); i != adj[u].end(); ++i){
			// Get vertex label and weight of current adjacent
			// of u.
			costosos++;
			int v = (*i).second;//destino
			float weight = (*i).first - nodes.at(v).friction;//peso biomasa-friccion, se ordena en pq de acuerdo a este
			float biom = (*i).first;//biomasa
			//cout << v << ", " << biom << endl;
			//  If there is shorted path to v through u.
			if (dist[v] < dist[u] + biom)//cambiar a pura biomasa sin weight
			{
				int parent_tmp = parent[v];
				parent[v]  = u;
				if(!rutaValida(parent, parent[v])){
				// Updating distance of v
					dist[v] = dist[u] + biom;// cambiar dist[v] a pura biomasa

					info = {weight, v, dist[v]};//weight = bioma-friccion, v=id_dest, dist[v] = acum
					pq2.push(info);

					//cout << "insert a pq2 " << weight << ", " << v << ", " << dist[v] << endl;
					parent[v]  = u;
				}
				else{
					parent[v]  = parent_tmp;
				}
			}
			/*if(costosos == 2)
				i = adj[u].end();*/
		}
	}
	/*cout << "d" << endl;
	exit(0);*/
	cout << "\nDijkstra Adjacency\nTOPE\n" << stop << endl;
	cout << "ACUM\n" << acumu << endl;
	cout << "\nPATH\n";

	printPath(parent, destino);
	fillMatrixPath();
}

// Driver program to test methods of graph class
void dijkstra_inicio(float** biomass, float** friction, int src_X, int src_Y, float stop){
	this->biomass = new float*[this->ROW];
	this->friction = new float*[this->ROW];
	int** enum_grid;
	enum_grid = new int*[ROW];
	for(int i = 0; i< ROW; ++i){
		this->biomass[i] = new float[COL];
		this->friction[i] = new float[COL];
		enum_grid[i] = new int[COL];
	}

	for(int x = 0; x < ROW; x++){
		for(int y = 0; y < COL; y++){
			this->biomass[x][y] = biomass[x][y];
			this->friction[x][y] = friction[x][y];
		}
	}

	int v = 0, no_count = 0;
	vector<nodeGraph> nodes;
	nodeGraph ng;
	int id_src;

	for(int i = 0; i < ROW; i++){
		for(int j = 0; j< COL; j++){
			enum_grid[i][j] = v;
			if(src_X == i && src_Y == j){
				id_src = v;
			}
			if(this->biomass[i][j] > 0 && this->friction[i][j] > 0 && this->biomass[i][j] > this->friction[i][j]){
				ng.id = v; ng.x = i; ng.y = j; ng.biomass = this->biomass[i][j]; ng.friction = this->friction[i][j];
				ng.w = this->biomass[i][j] - this->friction[i][j];
				nodes.push_back(ng);
				//cout << "id = " << v << " bioma = "<<  ng.biomass << endl;;
				v++;
			} else {
    			no_count++;
    		}
		}
	}

	cout << "v " << v << " no count: " << no_count << endl;
	i_Graph(v, nodes);

	int dx[] = { -1, -1, 0, 1, 1,  1,  0,-1 };
	int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
	vector <nodeGraph> :: iterator it;
	int x, y;
	int id_dest;
	ofstream myfile;
    //myfile.open ("adjacency.txt");

	for (it = nodes.begin(); it != nodes.end(); ++it){
		//cout << "vector nodes " << (*it).id << " - " << (*it).x << ", " << (*it).y << endl;
		//myfile <<  "vector nodes " << (*it).id << " - " << (*it).x << ", " << (*it).y << endl;
		for(int k = 0; k < 8; k++){
			x = (*it).x + dx[k];
			y = (*it).y + dy[k];
			if(isValid(x, y) && (*it).biomass != -9999 && this->biomass[x][y] >= this->friction[x][y] && this->biomass[x][y] != -9999 && this->friction[x][y] >= 0 && (x != src_X || y != src_Y)){
				id_dest = enum_grid[x][y];
				addEdge((*it).id, id_dest, this->biomass[x][y]);
				//myfile << "pushlistadj " << (*it).id << " > " << id_dest << " - " << x <<", " << y << " - " << this->biomass[x][y] << " - "<< this->friction[x][y] << " = " << this->biomass[x][y] - this->friction[x][y] << endl;
				//cout << "pushlistadj " << (*it).id << " > " << id_dest << " - " << x <<", " << y << " - " << this->biomass[x][y] << " - "<< this->friction[x][y] << endl;
			}

		}
		//sort aqui?
	}
	//myfile.close();
	cout << "a dijkstra "<< endl;
	dijkstra(id_src, src_X, src_Y, stop);

}

};

