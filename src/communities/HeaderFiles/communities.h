#include <iostream>
#include <vector>
#include <map>

struct Communities2D {
    int communityID;
    double xValue;
    double yValue;

    Communities2D(int commID, double x, double y)
    	: communityID(commID), xValue(x), yValue(y)
      {}
};

struct Communities3D {
    int communityID;
    double xValue;
    double yValue;
    double zValue;
};

struct Community {
    int communityID;
    double xValue;
    double yValue;
    std::map<double, int> NeighborsMap;

    Community(int commID, double x, double y, std::map<double, int> neighsmap) : communityID(commID), xValue(x), yValue(y), NeighborsMap(neighsmap) {}
};
