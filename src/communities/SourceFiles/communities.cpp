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
#include <vector>
#include "/src/communities/HeaderFiles/communities.h"

using namespace std;


int main() {
	vector<Communities2D> vect2d;

	int comm_id;
	double xPoint, yPoint; //They will store the casted value from the input file.
	char line[255], *ncom, *xval, *yval;

	FILE *fp;
	  fp = fopen("communities.csv", "r");
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

	  for (Communities2D const& community: vect2d) {
			cout << std::fixed;
		  cout << community.communityID << "\t" << community.xValue << "\t" << community.yValue << endl;
	  }
	  fclose(fp);
	return 0;
}
