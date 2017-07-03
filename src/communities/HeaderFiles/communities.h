#include <iostream>
#include <vector>

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

struct Neighbors {
	int NeighborID;
	double Distance;
};

struct CommunitiesInfo {
	struct Communities2D;
	struct Neighbors;
};
