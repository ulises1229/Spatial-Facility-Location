#include <iostream>
#include "communities.cpp"
using namespace std;

int main() {
    cCommunity c;
		vector<Point2D> vect2d = c.csv_to_vector();
		vector<Community> communityVect = c.neighbors_distance(vect2d);

    c.get_communities_and_neighbors(communityVect);
		c.get_min_max();
    c.get_costs_to_neighbors(communityVect);

	return 0;
}
