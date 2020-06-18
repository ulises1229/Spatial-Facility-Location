//
// Created by Ulises Olivares on 18/06/20.
//

#ifndef SRC_BELL_FORD_H
#define SRC_BELL_FORD_H

#endif //SRC_BELL_FORD_H

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

// Evaluate whether to include these variables inside class



class BellmanFord {




public:
    void printArr(int dist[], int n);
    bool isValid(int row, int col);
    bool rutaValida(vector<int> parent, int n);
    void addEdge(int u, int v, float w);
    void i_Graph(int V, vector<nGraph> nodes);
    void fillMatrixPath(int srcX, int srcY);
    void runBellmanFord(struct Graf* graph, int src);
    void printPath(vector<int> parent, int n);
    void bellford(int src_id, int srcX, int srcY, int stop);
    void bellford_start(float** biomass, float** friction, int srcX, int srcY, float stop, char heuristic);

    //FIXME:declare this code as private
    int V;    // No. of vertices
    vector<nGraph> nodes;
    // In a weighted graph, we need to store vertex
    // and weight pair for every edge
    list< pairBio > *adj;

    int ROW, COL;
    float** biomass;
    float** friction;
    vector<nGraph> Path;
    int** matrix_path;

private:
    /*int ROW, COL;
    float** biomass;
    float** friction;
    vector<nGraph> Path;
    int** matrix_path;*/


};