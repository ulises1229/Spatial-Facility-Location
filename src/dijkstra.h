//
// Created by Ulises Olivares on 18/06/20.
//

#ifndef SRC_DIJKSTRA_H
#define SRC_DIJKSTRA_H

#endif //SRC_DIJKSTRA_H


#include <stdio.h>
#include <stdlib.h>
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
    float biomass, friction, relation;
    //list< iPair > *adj;
};

// This class represents a directed graph using
// adjacency list representation
class Dijkstra{

    // Struct definition
    struct prq{
        float relation;//biomasa/friccion
        int id_dest;
        float acum;//pura biomasa
        bool operator<(const prq& rhs) const{
            return relation < rhs.relation;
        }
    };



public:

    // FIXME: USE AS A PRIVATE ELEMENT
    int V;    // No. of vertices
    vector<nodeGraph> nodes;
    list< iPair > *adj;

    // separated code
    int ROW, COL;
    float** biomass;
    float** friction;
    bool** closedList;
    stack<Pair> Path;
    int** matrix_path;

    // Method definition
    void i_Graph(int V, vector<nodeGraph> nodes);
    void addEdge(int u, int v, float w);
    void printPath(vector<int> parent, int n);
    void fillMatrixPath();
    bool validRoute(vector<int> parent, int n);
    bool validRoute(int c, vector<int> parent, int n);
    bool isValid(int row, int col);
    void dijkstra(int id_src, int src_x, int src_y, int stop);
    void initDijkstra(float** biomass, float** friction, int src_X, int src_Y, float stop, char heuristic);


private:



};
