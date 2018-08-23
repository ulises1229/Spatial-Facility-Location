/*
 * communities.cpp
 *
 *  Created on: 20/06/2018
 *      Author: lanase
 */




//============================================================================
// Name        : communities.cpp
// Author      : Andrés Vargas & René Orozco LANASE
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <iterator>
#include "communities.h"

using namespace std;
class cCommunity{
		public:
			int number_of_communities;
			double minX = DBL_MAX, minY = DBL_MAX, maxX = -DBL_MAX, maxY = -DBL_MAX;
			vector<Point2Dcomm> csv_to_vector(string fileName, int v);
			vector<Community> neighbors_distance(vector<Point2Dcomm> vect2d);
			void get_communities_and_neighbors(vector<Community> communityVect);
			void get_min_max();
			void get_costs_to_neighbors(vector<Community> communityVect);
			vector<Point2Dcomm> random_point();
			void get_random_points_total_cost(vector<Point2Dcomm> communitiesPts, vector<Point2Dcomm> randomPts);
};

void cCommunity::get_min_max(){
	cout << fixed << "Min X: " << minX << "\tMin Y: " << minY << "\nMax X: " << maxX << "\tMax Y: " << maxY << endl;
}

void cCommunity::get_costs_to_neighbors(vector<Community> communityVect){
	for (Community const& info: communityVect) {
		 double cost = 0.0;
		 cout << "Community " << info.communityID << " - ";
		 for(auto const& it : info.NeighborsMap) {
			 cost += it.first;
		 }
		 cout << "Cost to all its neighbors: " << cost << endl;
	}
}

void cCommunity::get_communities_and_neighbors(vector<Community> communityVect){
	for (Community const& info: communityVect) {
		 cout << info.communityID << ":\n";
		 for(auto const& it : info.NeighborsMap) {
				cout << it.first << " - " << it.second << endl;
		 }
	}
}



vector<Community> cCommunity::neighbors_distance(vector<Point2Dcomm> vect2d) {
	cout << "Calculate distances " << endl;
		Point2Dcomm* pStruct;
		Point2Dcomm* pNeigh;
		vector<Community> communityVect;
		float dist;
		vector<Point2Dcomm>::iterator it;
		vector<Point2Dcomm>::iterator it_2;

		for (it = vect2d.begin(); it != vect2d.end(); it++) {
			map<float, int> neighborsMap;
			for (it_2 = vect2d.begin(); it_2 != vect2d.end(); it_2++) {
				if (it->communityID == it_2->communityID) {
					dist = 0.0;
				} else {
					dist = sqrt( pow((it_2->xValue - it->xValue), 2.0)  + pow((it_2->yValue - it->yValue), 2.0) );
				}
				//neighborsMap[dist] = it_2->communityID;
				neighborsMap.insert(pair<float, int>(dist,it_2->communityID));
				/*if (dist < 15000.00)
					neighborsMap[dist] = it_2->communityID;*/
				//neighborsVect.emplace_back(neighbors.communityID, dist);
			}
			//communityVect.emplace_back(community.communityID, community.xValue, community.yValue, neighborsMap);
			communityVect.emplace_back(it->communityID, neighborsMap);
		}
		cout << "Finish calculating distances between neighbors" << '\n';
		return communityVect;
}

vector<Point2Dcomm> cCommunity::random_point() {
		vector<Point2Dcomm> rpVect;
		//random_device rd;
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		default_random_engine eng(seed);
		uniform_real_distribution<double> pointX(minX, maxX);
		uniform_real_distribution<double> pointY(minY, maxY);
		for (int x = 0; x < 10; x++) {
				rpVect.emplace_back(x + 1, pointX(eng), pointY(eng));
		}
		for (Point2Dcomm const& rp: rpVect) {
			cout << "RP " << rp.communityID << ": " << rp.xValue << "   -   " << rp.yValue << endl;
		}
		return rpVect;
}

void cCommunity::get_random_points_total_cost(vector<Point2Dcomm> communitiesPts, vector<Point2Dcomm> randomPts) {
		for (Point2Dcomm const& rp: randomPts) {
			 double cost = 0.0;
			 for (Point2Dcomm const& community: communitiesPts) {
				  cost += sqrt( pow((community.xValue - rp.xValue), 2.0)  + pow((community.yValue - rp.yValue), 2.0) );
			 }
			 cout << "Total cost of Point " << rp.communityID << ": " << cost << endl;
		}
}
