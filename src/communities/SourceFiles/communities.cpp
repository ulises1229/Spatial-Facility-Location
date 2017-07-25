//============================================================================
// Name        : communities.cpp
// Author      : Andrés Vargas & René Orozco LANASE
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
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
#include "../HeaderFiles/communities.h"

using namespace std;
class cCommunity{
		public:
			int number_of_communities;
			double minX = DBL_MAX, minY = DBL_MAX, maxX = -DBL_MAX, maxY = -DBL_MAX;
			vector<Point2D> csv_to_vector();
			vector<Community> neighbors_distance(vector<Point2D> vect2d);
			void get_communities_and_neighbors(vector<Community> communityVect);
			void get_min_max();
			void get_costs_to_neighbors(vector<Community> communityVect);
			vector<Point2D> random_point();
			void get_random_points_total_cost(vector<Point2D> communitiesPts, vector<Point2D> randomPts);
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

vector<Point2D> cCommunity::csv_to_vector() {
		vector<Point2D> vect2d;
		number_of_communities = 0;
		int comm_id;
		double xPoint, yPoint; //They will store the casted value from the input file.
		char line[255], *ncom, *xval, *yval;

		FILE *fp;
	  fp = fopen("test.csv", "r");
	  fgets(line, sizeof(line), fp); //Skips the first line.

	  while(fgets(line, sizeof(line), fp)) {
		    //Get the number of community, X & Y values from each line.
		    ncom = strtok(line,",");
		    xval = strtok(NULL, ",");
		    yval = strtok(NULL, ",");

		    //Cast the values from the input file.
		    comm_id = atoi(ncom);
		    xPoint = strtod(xval, NULL);
		    yPoint = strtod(yval, NULL);

				if (xPoint > maxX)
						maxX = xPoint;
				if (yPoint > maxY)
						maxY = yPoint;

				if (xPoint < minX)
						minX = xPoint;
				if (yPoint < minY)
						minY = yPoint;

		    //Store the casted values in the Struct.
		    //vect2d.push_back({comm_id, xPoint, yPoint})
		    vect2d.emplace_back(comm_id, xPoint, yPoint);

				number_of_communities++;
		  }
		fclose(fp);
	 	return vect2d;
}

vector<Community> cCommunity::neighbors_distance(vector<Point2D> vect2d) {
		Point2D* pStruct;
		Point2D* pNeigh;
		vector<Community> communityVect;
		float dist;
		vector<Point2D>::iterator it;
		vector<Point2D>::iterator it_2;

		/*for(int i = 0; i < vect2d.size(); i++) {
				pStruct  = &vect2d[i];
				map<double, int> neighborsMap;
				for (int j = 0; j < vect2d.size(); j++) {
					pNeigh = &vect2d[j];
					if (pStruct->communityID == pNeigh->communityID) {
						 dist = 0.0;
					} else {
						 dist = sqrt( pow((pNeigh->xValue - pStruct->xValue), 2.0) + pow((pNeigh->yValue - pStruct->yValue), 2.0) );
					}
					neighborsMap[dist] = pNeigh->communityID;
				}
				//cout << fixed;
				//cout << "ID: " << pStruct->communityID << "\tX: " << pStruct->xValue << "\tY: " << pStruct->yValue << endl;
				//communityVect.emplace_back(pStruct->communityID, pStruct->xValue, pStruct->yValue, neighborsMap);
				communityVect.emplace_back(pStruct->communityID, neighborsMap);
		}*/

		for (it = vect2d.begin(); it != vect2d.end(); it++) {
			map<float, int> neighborsMap;
			for (it_2 = vect2d.begin(); it_2 != vect2d.end(); it_2++) {
				if (it->communityID == it_2->communityID) {
					dist = 0.0;
				} else {
					dist = sqrt( pow((it_2->xValue - it->xValue), 2.0)  + pow((it_2->yValue - it->yValue), 2.0) );
				}
				neighborsMap[dist] = it_2->communityID;
				/*if (dist < 15000.00)
					neighborsMap[dist] = it_2->communityID;*/
				//neighborsVect.emplace_back(neighbors.communityID, dist);
			}
			//communityVect.emplace_back(community.communityID, community.xValue, community.yValue, neighborsMap);
			communityVect.emplace_back(it->communityID, neighborsMap);
		}
		return communityVect;
}

vector<Point2D> cCommunity::random_point() {
		vector<Point2D> rpVect;
		//random_device rd;
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		default_random_engine eng(seed);
		uniform_real_distribution<double> pointX(minX, maxX);
		uniform_real_distribution<double> pointY(minY, maxY);
		for (int x = 0; x < 10; x++) {
				rpVect.emplace_back(x + 1, pointX(eng), pointY(eng));
		}
		for (Point2D const& rp: rpVect) {
			cout << "RP " << rp.communityID << ": " << rp.xValue << "   -   " << rp.yValue << endl;
		}
		return rpVect;
}

void cCommunity::get_random_points_total_cost(vector<Point2D> communitiesPts, vector<Point2D> randomPts) {
		for (Point2D const& rp: randomPts) {
			 double cost = 0.0;
			 for (Point2D const& community: communitiesPts) {
				  cost += sqrt( pow((community.xValue - rp.xValue), 2.0)  + pow((community.yValue - rp.yValue), 2.0) );
			 }
			 cout << "Total cost of Point " << rp.communityID << ": " << cost << endl;
		}
}
