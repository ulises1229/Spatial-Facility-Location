// A C / C++ program for Bellman-Ford's single source
// shortest path algorithm.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <stack>
#include <set>
#include <algorithm>
#include <queue>
#include <fstream>
#include <math.h>

using namespace std;
// a structure to represent a weighted edge in graph
typedef pair<float, int> pairBio;
typedef pair<int, int> Pair;
struct Edge
{
    int src, dest, weight;
};

struct nGraph {
	int id, x, y;
	float biomass, friction, relation;

	nGraph() {}

	nGraph(int x, int y) : x(x), y(y) {}
};

// a structure to represent a connected, directed and
// weighted graph
struct Graf
{
    // V-> Number of vertices, E-> Number of edges
    int V, E;

    // graph is represented as an array of edges.
    struct Edge* edge;
};

// Creates a graph with V vertices and E edges
/*struct Graf* createGraf(int V, int E)
{
    struct Graf* graph =
         (struct Graf*) malloc( sizeof(struct Graf) );
    graph->V = V;
    graph->E = E;

    graph->edge =
       (struct Edge*) malloc( graph->E * sizeof( struct Edge ) );

    return graph;
}*/

struct nInfo {
	float relation;
	int id_dest, x, y;
	float acum;
	bool operator<(const nInfo& rhs) const{
		return relation < rhs.relation;
	}
};

struct nodeParent {
	int id, parent, x, y;
	float weight, acum;
};

struct findNode {
	int x, y;
	findNode(){};
	findNode(int x, int y) : x(x), y(y) {}
	bool operator () (findNode const &e) const {
		return (x == e.x && y == e.y);
	}
};

class Bellmanford {
	int V;    // No. of vertices
	vector<nGraph> nodes;
	// In a weighted graph, we need to store vertex
	// and weight pair for every edge
	list< pairBio > *adj;

public:
	int ROW, COL, X, Y;
	float cost, cost_frict;
	float** biomass;
	float** friction;
    bool** visitedNodes;
	stack<Pair> Path;
	float** matrix_path;
	// A utility function used to print the solution
	void printArr(int dist[], int n)
	{
		printf("Vertex   Distance from Source\n");
		for (int i = 0; i < n; ++i)
			printf("%d \t\t %d\n", i, dist[i]);
	}

	bool isValid(int row, int col){
	    // Returns true if row number and column number
	    // is in range
	    return (row >= 0) && (row < ROW) &&
	           (col >= 0) && (col < COL);
	}

	bool rutaValida(vector<int> parent, int n){
		//cout << "valdiar" << n << " "  << parent[n] << endl;
		bool ciclo = true;
		int destino = n;
		vector<int> vis(V);
		vis.push_back(destino);
		std::vector<int>::iterator it;
		while(1){
			if(parent[n] == 0)
				break;
			it = find (vis.begin(), vis.end(), parent[n]);
			if(it != vis.end()){//found = ciclo
				ciclo = false; break;
			}else{
				vis.push_back(parent[n]);
				n = parent[n];
			}
		}
		return ciclo;
	}


	void addEdge(int u, int v, float w){
	    adj[u].push_back(make_pair(w, v));
	}

	void i_Graph(int V, vector<nGraph> nodes){
	    this->V = V;
	    this->nodes = nodes;
	    adj = new list<pairBio> [V];
	}

	void fillMatrixPath(ofstream& info) {
        this->matrix_path = new float*[this->ROW];
        for(int i = 0; i< ROW; ++i){
            this->matrix_path[i] = new float[COL];
            for(int j = 0; j < COL ; j++){
                matrix_path[i][j] = 0;
            }
        }
		int s = Path.size();
		float totCost = 0, totBiom = 0, frict = 0;

		while(!Path.empty()) {
            pair<int,int> p = Path.top();
            Path.pop();
			totBiom += this->biomass[p.first][p.second];
			if(friction[p.first][p.second] > 0) {
				totCost += (this->biomass[p.first][p.second] / this->friction[p.first][p.second]);
				frict += friction[p.first][p.second];
				matrix_path[p.first][p.second] = friction[p.first][p.second];
			} else {
				matrix_path[p.first][p.second] = 255;
			}
		}
        info << s << ",";
        info << totBiom << ",";
        info << totCost << endl;
        cout << "Total Biomass: " << totBiom << "\nTotal Cost: " << totCost << "\nTotal Cost ($): " << frict << "\nSize: " << s << endl;
        this->cost = totCost;
        this->cost_frict = frict;
	}
	// The main function that finds shortest distances from src to
	// all other vertices using Bellman-Ford algorithm.  The function
	// also detects negative weight cycle
	void BellmanFord(struct Graf* graph, int src)
	{
		int V = graph->V;
		int E = graph->E;
		int dist[V];

		// Step 1: Initialize distances from src to all other vertices
		// as INFINITE
		for (int i = 0; i < V; i++)
			dist[i]   = INT_MIN;
		dist[src] = 0;

		// Step 2: Relax all edges |V| - 1 times. A simple shortest
		// path from src to any other vertex can have at-most |V| - 1
		// edges
		for (int i = 1; i <= V-1; i++)
		{
			for (int j = 0; j < E; j++)
			{
				int u = graph->edge[j].src;
				int v = graph->edge[j].dest;
				int weight = graph->edge[j].weight;
				cout << dist[u] << " - " << dist[u] + weight << "  -  " << dist[v] << endl;
				if (dist[u] != INT_MIN && dist[u] + weight > dist[v]) {
					dist[v] = dist[u] + weight;
					cout << "Yes: " << dist[u] << "\t" << dist[u] + weight << "\t" << dist[v] << endl;
				}
			}
		}

		// Step 3: check for negative-weight cycles.  The above step
		// guarantees shortest distances if graph doesn't contain
		// negative weight cycle.  If we get a shorter path, then there
		// is a cycle.
		for (int i = 0; i < E; i++)
		{
			int u = graph->edge[i].src;
			int v = graph->edge[i].dest;
			int weight = graph->edge[i].weight;
			if (dist[u] != INT_MIN && dist[u] + weight > dist[v])
				printf("Graph contains negative weight cycle\n");
		}

		printArr(dist, V);

		return;
	}

	void bellford(int src_id, int srcX, int srcY, int stop, ofstream& info, int xMin, int xMax, int yMin, int yMax, int** enum_grid, char heuristic) {
		priority_queue<nInfo> nodeSet;
		nInfo aux;
		aux.relation = nodes.at(src_id).biomass; aux.id_dest = src_id; aux.acum = nodes.at(src_id).biomass; aux.x = srcX; aux.y = srcY;
		nodeSet.push(aux);
		float totAcum = 0;

		while(!nodeSet.empty()) {
			int u = nodeSet.top().id_dest;
			int acumBio = nodeSet.top().acum;
			if(visitedNodes[nodes.at(u).x][nodes.at(u).y] == false) {
				totAcum += biomass[nodes.at(u).x][nodes.at(u).y];
				Path.push(make_pair(this->nodes.at(u).x, this->nodes.at(u).y));
                visitedNodes[nodes.at(u).x][nodes.at(u).y] = true;
				nodeSet.pop();
				if (totAcum >= stop) {
					nodeSet =  priority_queue <nInfo>();
                    for(int m=0; m < ROW; m++){
                        delete[] enum_grid[m];
                    }
					goto finish;
				}

                int dx[] = { -1, -1, 0, 1, 1,  1,  0,-1 };
                int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
                int id_dest;

                for(int k = 0; k < 8; k++){
                    bool flag = true;
                    int x = this->nodes.at(u).x  + dx[k];
                    int y = this->nodes.at(u).y + dy[k];
                    if(isValid(x, y) && this->biomass[x][y] > 0 && this->friction[x][y] >= 0 && (x != srcX || y != srcY) && x >= xMin && x <= xMax && y >= yMin && y <= yMax){
                        id_dest = enum_grid[x][y];
                        list< pair<float, int> >::iterator iter;
                        if(adj[id_dest].size() == 0) {
                            float heuristicResult = 0, relation = 0;
                            if (heuristic == 'e') {
                                heuristicResult = sqrt(pow((srcX - x), 2) + pow((srcY - y), 2));
                            }
                            else if (heuristic == 'm') {
                                heuristicResult = abs(srcX - x) + abs(srcY - y);
                            }
                            else if(heuristic == 'd') {
                                heuristicResult = max(abs(srcX - x), abs(srcY - y));
                            }
                            if (heuristicResult > 0)
                                relation = (biomass[x][y] / friction[x][y] + heuristicResult);
                            else
                                relation = biomass[x][y] / friction[x][y];
                            addEdge(nodes.at(u).id, id_dest, relation);
                        } else {
                            for (iter = adj[id_dest].begin(); iter != adj[id_dest].end(); ++iter){
                                if((*iter).second == nodes.at(u).id) {
                                    flag = false;
                                    break;
                                }
                            }
                            if(flag) {
                                float heuristicResult = 0, relation = 0;
                                if (heuristic == 'e') {
                                    heuristicResult = sqrt(pow((srcX - x), 2) + pow((srcY - y), 2));
                                }
                                else if (heuristic == 'm') {
                                    heuristicResult = abs(srcX - x) + abs(srcY - y);
                                }
                                else if(heuristic == 'd') {
                                    heuristicResult = max(abs(srcX - x), abs(srcY - y));
                                }
                                if (heuristicResult > 0)
                                    relation = biomass[x][y] / (friction[x][y] + heuristicResult);
                                else
                                    relation = biomass[x][y] / friction[x][y];
                                addEdge(nodes.at(u).id, id_dest, relation);
                            }
                        }
                    }
                }

				list< pair<float, int> >::iterator it;
				for(it = adj[u].begin(); it != adj[u].end(); ++it) {
					int i = this->nodes.at((*it).second).x, j = this->nodes.at((*it).second).y, uAux = (*it).second;
                    if(visitedNodes[i][j] == false) {
                        int v = (*it).second;
                        float w = (*it).first;
                        float biom = nodes.at(v).biomass;
                        aux.relation = w; aux.id_dest = v; aux.acum = acumBio + biom, aux.x = i; aux.y = j;
                        nodeSet.push(aux);
                    }
				}
			} else {
				nodeSet.pop();
			}
		}
		cout << "No More Elements" << endl;
			finish:
				fillMatrixPath(info);
	}


	void bellford_start(float** biomass, float** friction, int srcX, int srcY, float stop, char heuristic, int xMin, int xMax, int yMin, int yMax, int intervals, ofstream& info) {		   this->X = srcX;
	        this->Y = srcY;
			this->biomass = new float*[this->ROW];
			this->friction = new float*[this->ROW];
            this->visitedNodes = new bool*[this->ROW];
			int** enum_grid;
			enum_grid = new int*[ROW];
			for(int i = 0; i< ROW; ++i){
				this->biomass[i] = new float[COL];
				this->friction[i] = new float[COL];
                this->visitedNodes[i] = new bool[COL];
				enum_grid[i] = new int[COL];
			}

			for(int x = 0; x < ROW; x++){
				for(int y = 0; y < COL; y++){
					this->biomass[x][y] = biomass[x][y];
					this->friction[x][y] = friction[x][y];
                    this->visitedNodes[x][y] = false;
				}
			}

			int v = 0;
			vector<nGraph> nodes;
			nGraph ng;
			int id_src;
			float heuristicResult = 0, relation = 0;
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

			for(int i = xMin; i <= xMax; i++){
				for(int j = yMin; j <= yMax; j++){
					if(srcX == i && srcY == j){
						id_src = v;
					}
					if(this->biomass[i][j] > 0 && this->friction[i][j] >= 0){
						ng.id = v; ng.x = i; ng.y = j; ng.biomass = this->biomass[i][j];// ng.friction = this->friction[i][j];
						//ng.relation = relation;
						nodes.push_back(ng);
						enum_grid[i][j] = v;
						v++;
					}
				}
			}

			i_Graph(v, nodes);
			nodes.clear();

			/*for (it = this->nodes.begin(); it != this->nodes.end(); ++it){
				for(int k = 0; k < 8; k++){
					x = (*it).x + dx[k];
					y = (*it).y + dy[k];
					if(isValid(x, y) && this->biomass[x][y] > 0 && this->friction[x][y] >= 0 && x >= xMin && x <= xMax && y >= yMin && y <= yMax){
						id_dest = enum_grid[x][y];
						addEdge((*it).id, id_dest, this->nodes.at(id_dest).relation);
					}

				}
			}
		for(int m=0; m < ROW; m++){
			delete[] enum_grid[m];
		}*/
		bellford(id_src, srcX, srcY, stop, info, xMin, xMax, yMin, yMax, enum_grid, heuristic);
	}

	void freeMem() {
		for(int m=0; m < ROW; m++){
			delete[] this->biomass[m];
			delete[] this->friction[m];
			delete[] this->matrix_path[m];
            delete[] this->visitedNodes[m];
		}
			adj->clear();
			nodes.clear();
	}


};
