#include <iostream>
#include "communities.cpp"
#include "kmeans.cpp"
#include "morton.cpp"


using namespace std;
//Test testObj;

int main(int argc, char* argv[]) {
    vector<double> tiempos;
    string fileName = argv[1];
    cCommunity c;
		//vector<Point2D> vect2d = c.csv_to_vector(argv[1]);
    int i = ceil(573740/(double)20000);
    cout<< i << endl;
    /*for(int v = 1; v <= i; v++){
		  vector<Point2D> vect2d = c.csv_to_vector(fileName, v);
		//vector<Community> communityVect = c.neighbors_distance(vect2d);

    //c.get_communities_and_neighbors(communityVect);
		//c.get_min_max();
    //c.get_costs_to_neighbors(communityVect);
    //vector<Point2D> rpVect = c.random_point();
    //int sample = vect2d.size();
    //cout << sample << endl;
    int sample = vect2d.size();
    cout << sample << endl;
    //cout << vect2d.size() << endl;
    //c.get_random_points_total_cost(vect2d, rpVect);

    KMeans k;
    clock_t begin = clock();
    k.kmeans_k(vect2d, sample);
    clock_t end = clock();
  	cout << "KMeans " << v << " done\n" << endl;
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  	//cout<<"Time:" << elapsed_secs <<endl;
    tiempos.push_back(elapsed_secs);
  }*/

    for (int v = 1; v <= i; v++) {
      Morton m;
      clock_t begin = clock();
      m.read_csv(fileName);
      clock_t end = clock();
      cout << "Morton Codes: " << v << " done\n" << endl;
      double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
      tiempos.push_back(elapsed_secs);
    }

    vector<double>::iterator it;
    int a=1;
    for (it = tiempos.begin(); it != tiempos.end(); it++) {
      cout << a++ << " - " << *it << endl;
    }
	  return 0;
}
