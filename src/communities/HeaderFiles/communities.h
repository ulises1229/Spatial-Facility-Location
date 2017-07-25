#include <iostream>
#include <vector>
#include <map>

struct Point2D {
    int communityID;
    double xValue;
    double yValue;

    Point2D(int commID, double x, double y)
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

struct Community_clusters {
    int communityID;
    int cluster_id;
    std::map<float, int> NeighborsMap;

    Community_clusters(int commID, int cluster, /*double x, double y,*/ std::map<float, int> neighsmap) : communityID(commID), cluster_id(cluster), /*xValue(x), yValue(y),*/ NeighborsMap(neighsmap) {}
};


struct Comm2D{
  int communityID;
  struct Point2D;
};
