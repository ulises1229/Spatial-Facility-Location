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
		    //vect2d.push_back({comm_id, xPoint, yPoint});
		    vect2d.emplace_back(comm_id, xPoint, yPoint);
		  }
		fclose(fp);
	 	return vect2d;
}

vector<Neighbors> neighbors_distance(vector<Communities2D> vect2d) {
		vector<Neighbors> neighborsVect;
		double dist;
		for (Communities2D const& community: vect2d) {
			for (Communities2D const& neighbors: vect2d) {
				if (community.communityID == neighbors.communityID) {
					dist = 0.0;
				} else {
					dist = sqrt( pow((neighbors.xValue - community.xValue), 2.0)  + pow((neighbors.yValue - community.yValue), 2.0) );
				}
				neighborsVect.emplace_back(neighbors.communityID, dist);
			}
		}
		return neighborsVect;
}

vector<CommunitiesInfo> population(vector<Communities2D> vect2d, vector<Neighbors> neighborsVect) {
		vector<CommunitiesInfo> populationVect;
		populationVect.emplace_back(vect2d, neighborsVect);

		return populationVect;
}

int main() {
		vector<Communities2D> vect2d = csv_to_vector();
		vector<Neighbors> neighborsVect = neighbors_distance(vect2d);
		vector<CommunitiesInfo> populationVect = population(vect2d, neighborsVect);


		for (CommunitiesInfo const& pop: populationVect) {
			 cout << pop.neighbors.Distance << endl;
		}
		/*for (Neighbors const& neigh: neighborsVect) {
				cout << neigh.NeighborID << "\t" << neigh.Distance << endl;
		}*/

	  /*for (Communities2D const& community: vect2d) {
			cout << std::fixed;
		  cout << community.communityID << "\t" << community.xValue << "\t" << community.yValue << endl;
	  }*/

	return 0;
}
