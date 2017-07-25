#include <iostream>
#include "communities.cpp"
#include "kmeans.cpp"


//using namespace std;
//Test testObj;

int main() {
    cCommunity c;
		vector<Point2D> vect2d = c.csv_to_vector();
		vector<Community> communityVect = c.neighbors_distance(vect2d);

    c.get_communities_and_neighbors(communityVect);
		c.get_min_max();
    c.get_costs_to_neighbors(communityVect);
    vector<Point2D> rpVect = c.random_point();
    int sample = vect2d.size();
    cout << sample << endl;
    cout << vect2d.size() << endl;
    //c.get_random_points_total_cost(vect2d, rpVect);

    KMeans k;
    k.kmeans_k(vect2d, sample);

	  return 0;
}
