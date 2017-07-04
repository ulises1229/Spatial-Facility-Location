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

    Neighbors(int NeighID, double dist) : NeighborID(NeighID), Distance(dist) {}
};

struct CommunitiesInfo {
  	struct Communities2D communities2D;
  	struct Neighbors neighbors;

    CommunitiesInfo(const Communities2D comms2D, const Neighbors neighs) : communities2D(comms2D), neighbors(neighs) {}
};
