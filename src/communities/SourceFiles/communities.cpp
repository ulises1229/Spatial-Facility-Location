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
#include <iterator>
#include "../HeaderFiles/communities.h"

using namespace std;

vector<Communities2D> csv_to_vector() {
		vector<Communities2D> vect2d;
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

		    //Store the casted values in the Struct.
		    //vect2d.push_back({comm_id, xPoint, yPoint})
		    vect2d.emplace_back(comm_id, xPoint, yPoint);
		  }
		fclose(fp);
	 	return vect2d;
}

vector<Community> neighbors_distance(vector<Communities2D> vect2d) {
		vector<Community> communityVect;
		double dist;
		for (Communities2D const& community: vect2d) {
			map<double, int> neighborsMap;
			for (Communities2D const& neighbors: vect2d) {
				if (community.communityID == neighbors.communityID) {
					dist = 0.0;
				} else {
					dist = sqrt( pow((neighbors.xValue - community.xValue), 2.0)  + pow((neighbors.yValue - community.yValue), 2.0) );
				}
				neighborsMap[dist] = neighbors.communityID;
				//neighborsVect.emplace_back(neighbors.communityID, dist);
			}
			communityVect.emplace_back(community.communityID, community.xValue, community.yValue, neighborsMap);
		}
		return communityVect;
}

int main() {
		vector<Communities2D> vect2d = csv_to_vector();
		vector<Community> communityVect = neighbors_distance(vect2d);

		/*for (Community const& info: communityVect) {
			 cout << info.communityID << ":\n";
			 for(auto const& it : info.NeighborsMap) {
				 	cout << it.first << " - " << it.second << endl;
			 }
		}*/
	return 0;
}
