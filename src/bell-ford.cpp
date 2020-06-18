/*
 * bell-ford.cpp
 * Author: Ulises Olivares
 * uolivares@unam.mx
 * June 12, 2020
 */

// A C / C++ program for Bellman-Ford's single source
// shortest path algorithm.
#include "bell-ford.h"

using namespace std;


	// A utility function used to print the solution
	void BellmanFord::printArr(int dist[], int n)
	{
		printf("Vertex   Distance from Source\n");
		for (int i = 0; i < n; ++i)
			printf("%d \t\t %d\n", i, dist[i]);
	}

	bool BellmanFord::isValid(int row, int col){
	    // Returns true if row number and column number
	    // is in range
	    return (row >= 0) && (row < ROW) &&
	           (col >= 0) && (col < COL);
	}

	bool BellmanFord::rutaValida(vector<int> parent, int n){
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


	void BellmanFord::addEdge(int u, int v, float w){
	    adj[u].push_back(make_pair(w, v));
	}

	void BellmanFord::i_Graph(int V, vector<nGraph> nodes){
	    this->V = V;
	    this->nodes = nodes;
	    adj = new list<pairBio> [V];
	}

	void BellmanFord::fillMatrixPath(int srcX, int srcY) {
		this->matrix_path = new int*[this->ROW];
		for(int i = 0; i< ROW; ++i){
			this->matrix_path[i] = new int[COL];
			/*for(int j = 0; j < COLS ; j++){
				matrix_path[i][j] = 0;
			}*/
		}
		int s = Path.size();
		float totCost = 0, totBiom = 0;

		vector<nGraph>::iterator it;
		for(it = Path.begin(); it != Path.end(); it++) {
			totBiom += this->biomass[it->x][it->y];
			totCost += this->friction[it->x][it->y];
			if(it->x == srcX && it->y == srcY)
				matrix_path[it->x][it->y] = 2;
			else
				matrix_path[it->x][it->y] = 1;
		}
		 cout << "Total Biomass: " << totBiom << "\t Total Cost: " << totCost << "\t Size: " << s << endl;
	}
	// The main function that finds shortest distances from src to
	// all other vertices using Bellman-Ford algorithm.  The function
	// also detects negative weight cycle

	void BellmanFord::runBellmanFord(struct Graf* graph, int src)
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
				printf("Dijkstra contains negative weight cycle\n");
		}

		printArr(dist, V);

		return;
	}

	void BellmanFord::printPath(vector<int> parent, int n){
	    //printf("Vertex   Distance from Source\n");
		// Base Case : If j is source
		    if (parent[n]==-1)
		        return;

		    printPath(parent, parent[n]);
		    //list< pair<int, int> >::iterator i;
		    //i = this->adj[n].begin();
		    printf("-> %d (%d, %d)  %f\n ", n, this->nodes.at(n).x, this->nodes.at(n).y, this->nodes.at(n).biomass);
		    //Path.push(make_pair (this->nodes.at(n).x, this->nodes.at(n).y));
		    //printf("%d ", n);
	}

	void BellmanFord::bellford(int src_id, int srcX, int srcY, int stop) {
		priority_queue<nInfo> nodeSet;
		vector<findNode> visitedNodes;
		vector<findNode>::iterator itNodes;
		vector<findNode>::iterator itNodesAux;
		nInfo aux;
		aux.relation = nodes.at(src_id).biomass; aux.id_dest = src_id; aux.acum = nodes.at(src_id).biomass; aux.x = srcX; aux.y = srcY;
		nodeSet.push(aux);
		float totAcum = 0;

		while(!nodeSet.empty()) {
			int u = nodeSet.top().id_dest;
			int acumBio = nodeSet.top().acum;
			//cout << x << ", " << y << endl;
			//cout << this->nodes.at(u).x << ", " << this->nodes.at(u).y << endl;
			itNodes = find_if(visitedNodes.begin(), visitedNodes.end(), findNode(this->nodes.at(u).x, this->nodes.at(u).y));
			if(itNodes == visitedNodes.end()) {
				//cout << "estoy id nodo: " << u << " con peso: " << nodeW << " con acumulado: " << acum <<  endl;
				//int u = nodeSet.top().id_dest;
				//float acum = nodeSet.top().acum;
				//float nodeW = nodeSet.top().weight;
				totAcum += biomass[nodes.at(u).x][nodes.at(u).y];
				//cout << totAcum << endl;
				Path.push_back(nGraph(this->nodes.at(u).x, this->nodes.at(u).y));
				visitedNodes.push_back(findNode(this->nodes.at(u).x, this->nodes.at(u).y));
				nodeSet.pop();
				//nodeSet = priority_queue<nodeInfo>();
				//nodeSet.erase(--(nodeSet.end()));
				if (totAcum >= stop) {
					//totAcum = acum;
					//destino = u;
					//cout << "Terminé los " << stop << " con " << acumF << endl;
					goto finish;
				}
				//adj[u].sort(greater<pairBio>());
				list< pair<float, int> >::iterator it;
				list< pair<float, int> >::iterator it2;
				for(it = adj[u].begin(); it != adj[u].end(); ++it) {
					int i = nodes.at((*it).second).x, j = nodes.at((*it).second).y, uAux = (*it).second;
					for(it2 = adj[uAux].begin(); it2 != adj[uAux].end(); ++it2){
						int a = nodes.at((*it2).second).x, b = nodes.at((*it2).second).y;
						itNodesAux = find_if(visitedNodes.begin(), visitedNodes.end(), findNode(a, b));
						if(itNodesAux == visitedNodes.end()) {
							int v = (*it2).second;
							float w = (*it2).first;
							float biom = nodes.at(v).biomass;
							aux.relation = w; aux.id_dest = v; aux.acum = acumBio + biom, aux.x = i; aux.y = j;
							nodeSet.push(aux);
						}
					}
					itNodesAux = find_if(visitedNodes.begin(), visitedNodes.end(), findNode(i, j));
					if(itNodesAux == visitedNodes.end()) {
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
		//cout << "Terminé los " << stop << " con " << acumF << endl;
		//printPath(parent, destino);
		if(!nodeSet.empty()) {
			finish:
				fillMatrixPath(srcX, srcY);
		}
	}


	void BellmanFord::bellford_start(float** biomass, float** friction, int srcX, int srcY, float stop, char heuristic) {
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

			int v = 0;
			vector<nGraph> nodes;
			nGraph ng;
			int id_src;
			float heuristicResult = 0, relation = 0;
			for(int i = 0; i < ROW; i++){
				for(int j = 0; j< COL; j++){
					if(srcX == i && srcY == j){
						id_src = v;
					}
					if(this->biomass[i][j] != -9999 && this->friction[i][j] > 0){
						if (heuristic == 'e') {
							heuristicResult = sqrt(pow((srcX - i), 2) + pow((srcY - j), 2));
						}
						else if (heuristic == 'm') {
							heuristicResult = abs(srcX - i) + abs(srcY - j);
						}
						else if(heuristic == 'd') {
							heuristicResult = max(abs(srcX - i), abs(srcY - j));
						}
						if (heuristicResult > 0)
							relation = (biomass[i][j] + heuristicResult) / friction[i][j];
						else
							relation = biomass[i][j] / friction[i][j];
						ng.id = v; ng.x = i; ng.y = j; ng.biomass = this->biomass[i][j]; ng.friction = this->friction[i][j];
						ng.relation = relation;
						nodes.push_back(ng);
						enum_grid[i][j] = v;
						v++;
					}
				}
			}
			i_Graph(v, nodes);

			int dx[] = { -1, -1, 0, 1, 1,  1,  0,-1 };
			int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
			vector <nGraph> :: iterator it;
			int x, y;
			int id_dest;

			for (it = nodes.begin(); it != nodes.end(); ++it){
				for(int k = 0; k < 8; k++){
					x = (*it).x + dx[k];
					y = (*it).y + dy[k];
					if(isValid(x, y) && this->biomass[x][y] != -9999 && this->friction[x][y] > 0 && (x != srcX || y != srcY)){
						id_dest = enum_grid[x][y];
						addEdge((*it).id, id_dest, nodes.at(id_dest).relation);
					}

				}
			}
		bellford(id_src, srcX, srcY, stop);
	}


	// Driver program to test above functions
	/*int main()
	{
		 //Let us create the graph given in above example
		int V = 5;  // Number of vertices in graph
		int E = 8;  // Number of edges in graph
		struct Dijkstra* graph = createGraph(V, E);

		// add edge 0-1 (or A-B in above figure)
		graph->edge[0].src = 0;
		graph->edge[0].dest = 1;
		graph->edge[0].weight = -1;

		// add edge 0-2 (or A-C in above figure)
		graph->edge[1].src = 0;
		graph->edge[1].dest = 2;
		graph->edge[1].weight = 4;

		// add edge 1-2 (or B-C in above figure)
		graph->edge[2].src = 1;
		graph->edge[2].dest = 2;
		graph->edge[2].weight = 3;

		// add edge 1-3 (or B-D in above figure)
		graph->edge[3].src = 1;
		graph->edge[3].dest = 3;
		graph->edge[3].weight = 2;

		// add edge 1-4 (or A-E in above figure)
		graph->edge[4].src = 1;
		graph->edge[4].dest = 4;
		graph->edge[4].weight = 2;

		// add edge 3-2 (or D-C in above figure)
		graph->edge[5].src = 3;
		graph->edge[5].dest = 2;
		graph->edge[5].weight = 5;

		// add edge 3-1 (or D-B in above figure)
		graph->edge[6].src = 3;
		graph->edge[6].dest = 1;
		graph->edge[6].weight = 1;

		// add edge 4-3 (or E-D in above figure)
		graph->edge[7].src = 4;
		graph->edge[7].dest = 3;
		graph->edge[7].weight = -3;

		BellmanFord(graph, 0);

		return 0;
	}*/
