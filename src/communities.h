/*
 * communities.h
 *
 *  Created on: 20/06/2018
 *      Author: lanase
 */

#include <iostream>
#include <vector>
#include <map>

struct Point2Dcomm {
    int communityID;
    double xValue;
    double yValue;
    Point2Dcomm(){}
    Point2Dcomm(int commID, double x, double y)
    	: communityID(commID), xValue(x), yValue(y)
      {}
};

struct Point3D {
    int communityID;
    double xValue;
    double yValue;
    double zValue;
};

struct Community {
    int communityID;
    //double xValue;
    //double yValue;
    std::map<float, int> NeighborsMap;

    Community(int commID, /*double x, double y,*/ std::map<float, int> neighsmap) : communityID(commID), /*xValue(x), yValue(y),*/ NeighborsMap(neighsmap) {}
};

struct Comm2D{
  int communityID;
  struct Point2Dcomm;
};
