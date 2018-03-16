/*
 * kmeans.cpp
 *
 *  Created on: 06/11/2017
 *      Author: lanase
 */
#include <algorithm>
#include <math.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

using namespace std;

struct Point2D_Kmeans {
    int communityID;
    double xValue;
    double yValue;
    Point2D_Kmeans(){}
    Point2D_Kmeans(int commID, double x, double y)
    	: communityID(commID), xValue(x), yValue(y)
      {}
};

struct bioFriction {
	float biomass, friction, rend;
	int x, y;
	bioFriction(){}
	bioFriction(float biomass, float friction, float rend, int x, int y)
		: biomass(biomass), friction(friction), rend(rend), x(x), y(y) {}
};


class Point_kmeans{
private:
	int id_point, id_cluster;
	vector<double> coords;
	float biomass, friction;
	int total_values;

public:
	Point_kmeans(int id_point, vector<double>& coords, float biomass, float friction){
		this->id_point = id_point;
		this->biomass = biomass;
		this->friction = friction;
    total_values = coords.size();
		for(int i = 0; i < coords.size(); i++)
			this->coords.push_back(coords[i]);
		id_cluster = -1;
	}

	int getID(){
		return id_point;
	}

	void setCluster(int id_cluster){
		this->id_cluster = id_cluster;
	}

	int getCluster(){
		return id_cluster;
	}

	double getValue(int index){
		return coords[index];
	}

	int getTotalValues(){
		return total_values;
	}

	float getBiomass() {
		return biomass;
	}

	float getFriction() {
		return friction;
	}

	void addValue(double value){
		coords.push_back(value);
	}
};


class Cluster{
private:
	int id_cluster;
	vector<int> central_values;
	vector<Point_kmeans> points;
	double avg, var, std_dev;
	float value;

public:
	Cluster(int id_cluster, Point_kmeans point){
		this->id_cluster = id_cluster;
		this->value = -1;
		int total_values = point.getTotalValues();
		for(int i = 0; i < total_values; i++)
			central_values.push_back(point.getValue(i));//mete coordenadas a central_values.
		points.push_back(point);
	}

	void addPoint(Point_kmeans point){
		points.push_back(point);
	}

	vector<Point_kmeans> getPoints() {
		return points;
	}

	bool removePoint(int id_point){
		int total_points = points.size();

		for(int i = 0; i < total_points; i++){
			if(points[i].getID() == id_point){
				points.erase(points.begin() + i);
				return true;
			}
		}
		return false;
	}

	int getCentralValue(int index){
		return central_values[index];
	}

	void setCentralValue(int index, double value){
		central_values[index] = value;
	}

	void setClusterValue(float val) { //Mayor biom, menor frict
		value = val;
	}

	Point_kmeans getPoint(int index){
		return points[index];
	}

	int getTotalPoints(){
		return points.size();
	}

	int getID(){
		return id_cluster;
	}

	void setAvg(double a){
		avg = a;
	}

	double getAvg(){
		return avg;
	}

	double getVar(){
		return var;
	}

	void setVar(double v){
		var = v;
	}

	double getStd_dev(){
		return std_dev;
	}

	void setStd_dev(double std){
		std_dev = std;
	}
};

class KMeans{
private:
	int K; // number of clusters
	int total_values, total_points, max_iterations;
	vector<Cluster> clusters;

	// return ID of nearest center (uses euclidean distance)
	int getIDNearestCenter(Point_kmeans point){
		double sum = 0.0, min_dist;
		int id_cluster_center = 0;
		for(int i = 0; i < total_values; i++){
			sum += pow(clusters[0].getCentralValue(i) -
					   point.getValue(i), 2.0);
		}
		min_dist = sqrt(sum);

		for(int i = 1; i < K; i++){
			double dist;
			sum = 0.0;

			for(int j = 0; j < total_values; j++){
				sum += pow(clusters[i].getCentralValue(j) -
						   point.getValue(j), 2.0);
			}

			dist = sqrt(sum);

			if(dist < min_dist){
				min_dist = dist;
				id_cluster_center = i;
			}
		}

		return id_cluster_center;
	}

public:
	int ROWS, COLS;
	vector<int> prohibited_indexes;

  KMeans(){}
	KMeans(int K, int total_points, int total_values, int max_iterations){
		this->K = K;
		this->total_points = total_points;
		this->total_values = total_values;
		this->max_iterations = max_iterations;
	}

	void run(vector<Point_kmeans> & points){
		if(K > total_points) {
			cout << "K: " << K << "\t Pts: " << total_points << "\t exit" << endl;
			return;
		}
		cout << "Starting run..." << endl;
		// choose K distinct values for the centers of the clusters
    //cout << "choose K distinct values for the centers of the clusters" << endl;
		for(int i = 0; i < K; i++){
			while(true){
				int index_point = rand() % total_points;
				cout << index_point << endl;
				if(find(prohibited_indexes.begin(), prohibited_indexes.end(), index_point) == prohibited_indexes.end()){//not found
					prohibited_indexes.push_back(index_point);
					points[index_point].setCluster(i);
					cout << "point " << points[index_point].getID() << " asignado a cluster " << i+1 << " Biom: " << points[index_point].getBiomass() << " Frict: " << points[index_point].getFriction() << endl;
					Cluster cluster(i, points[index_point]);
					clusters.push_back(cluster);
					break;
				}
			}
		}
		int iter = 1;

		while(true){
			bool done = true;

			// associates each point to the nearest center
      //cout << "associates each point to the nearest center" << endl;
			for(int i = 0; i < total_points; i++){
				int id_old_cluster = points[i].getCluster();
				int id_nearest_center = getIDNearestCenter(points[i]);
				//cout << "Old: " << id_old_cluster << "\t Nearest: " << id_nearest_center << endl;
				if(id_old_cluster != id_nearest_center){
					if(id_old_cluster != -1)
						clusters[id_old_cluster].removePoint(points[i].getID());

					points[i].setCluster(id_nearest_center);
					clusters[id_nearest_center].addPoint(points[i]);
					done = false;
				}
			}

			//cout << "Passed 1st for-loop of total pts." << endl;

			// recalculating the center of each cluster
          //cout << "recalculating the center of each cluster" << endl;
			for(int i = 0; i < K; i++){
				for(int j = 0; j < total_values; j++){ //de 0 a 2
					int total_points_cluster = clusters[i].getTotalPoints();
					double sum = 0.0;

					if(total_points_cluster > 0){
						for(int p = 0; p < total_points_cluster; p++) {
							sum += clusters[i].getPoint(p).getValue(j);
							//cout << "Biom: " << clusters[i].getPoint(p).getBiomass() << "\t Friction: " << clusters[i].getPoint(p).getFriction() << endl;
						}
						clusters[i].setCentralValue(j, sum / total_points_cluster);
					}
				}
			}

			if(done == true || iter >= max_iterations){
				cout << "Break in iteration " << iter << "\n";
				break;
			}
			cout << "************************************************ ITER: " << iter << " ************************************************" << endl;
			iter++;
		}

		// shows elements of clusters
		/*map<float, int> neighborsMap;
    	cout << "shows elements of clusters" << endl;
  		for(int i = 0; i < K; i++){
  			int total_points_cluster =  clusters[i].getTotalPoints(), count = 0;
				float dev = 0.0, sdev = 0.0, var, sdx, sdy;
  			cout << "Cluster " << clusters[i].getID() + 1 << endl;
  			for(int j = 0; j < total_points_cluster; j++){
  				cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
  				for(int p = 0; p < total_values; p++)
  					cout << clusters[i].getPoint(j).getValue(p) << " ";
          cout << endl;
        }
  			cout << "Cluster values: ";

  			for(int j = 0; j < total_values; j++)
  				cout << clusters[i].getCentralValue(j) << " ";

				cout << "\n\n";

  	 }
  	 exit(0);*/
  }

	void set_costs_per_cluster(){
		for(int i = 0; i < K; i++){
			float biom_tot = 0, frict_tot = 0, frict_prom = 0;
			vector<Point_kmeans> auxVect = clusters[i].getPoints();
			vector<Point_kmeans>::iterator it;
			for(it = auxVect.begin(); it != auxVect.end(); it++) {
				biom_tot += it->getBiomass();
				frict_tot += it->getFriction();
			}
			frict_prom = frict_tot / auxVect.size();
			float total_cost = biom_tot / frict_prom;
			clusters[i].setClusterValue(total_cost);
			cout << "Cluster ID: " << clusters[i].getID() << "\t Coords: " << clusters[i].getCentralValue(0) << ", " << clusters[i].getCentralValue(1) << "\t Biomass: " << biom_tot << "\t Avg Friction: " << frict_prom << "\t Value: " << total_cost << endl;
		}

	}


	/*void get_costs_per_cluster(vector<Community> communityVect){
		for(int i = 0; i < K; i++){
  			int total_points_cluster =  clusters[i].getTotalPoints();
				cout << "Cluster " << clusters[i].getID() + 1 << endl;
				double sum = 0.0, avg = 0.0;
				double dev=0, var=0, sd=0, sdev = 0;
				std::vector<double> costs;
  			for(int j = 0; j < total_points_cluster; j++){
					double cost = 0.0;
  				cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
					for(auto const& it : communityVect[clusters[i].getPoint(j).getID()].NeighborsMap) {
 						//cout << it.first << " - " << it.second+1 << endl;
						cost += it.first;
 				 }
				 cout << "Cost to all its neighbors: " << cost << endl;
				 sum += cost;
				 costs.push_back(cost);
				}
				avg = sum / costs.size();
				cout << "CLuster sum: " << sum << endl;
				cout << "Cluster avg:" << avg << endl;
				clusters[i].setAvg(avg);
				for (int i = 0; i < costs.size(); i++) {
					dev = (costs[i] - avg) * (costs[i] - avg);
					sdev += dev;
				}
				var = sdev / (costs.size());
				sd = sqrt(var);
				clusters[i].setVar(var);
				clusters[i].setStd_dev(sd);
				cout << "Std Dev: " << sd << endl << "Var: " << var << endl;
			}
			for (Community const& info: communityVect) {
				 cout << info.communityID+1 << ":\n";
				 for(auto const& it : info.NeighborsMap) {
						cout << it.first << " - " << it.second+1 << endl;
				 }
			}
	}*/

  void kmeans_k(float** biomass, float** friction, int sample){
  	//srand (time(NULL));
	 vector<Point_kmeans> points;
	 int total_points = 0, coords, K, max_iterations, i = 0, no_count = 0;
	  for (int j = 0; j < ROWS; j++) {
	      	for (int k = 0; k < COLS; k++) {
	      		if(biomass[j][k] != -9999 && friction[j][k] >= 0 && biomass[j][k] >= friction[j][k]) {
	      			vector<double> coordinates;
	  				coordinates.push_back(j);
	  				coordinates.push_back(k);
	  				Point_kmeans p(i, coordinates, biomass[j][k], friction[j][k]);
	  				points.push_back(p);
	  				total_points++;
	  				i++;
	      		} else {
	      			no_count++;
	      		}
	      	}
	      }

	K = round(1 + log2 (total_points));
    cout << "Number of clusters (K): " << K << endl;
    //cin >> K;
    cout << "Max number of iterations: 500 " << endl;
    //cin >> max_iterations;
	max_iterations = 500;
  	//vector<bioFriction> pts;
  	//vector<bioFriction>::iterator it;
    cout << "Pasé inicio con " << total_points << " No entraron: " << no_count << endl;
  	KMeans kmeans(K, points.size(), 2, max_iterations);//2 cause X,Y.
    //cout << "sdfs";
  	kmeans.run(points);
  	kmeans.set_costs_per_cluster();
		//vector<Community> communityClustersVect = kmeans.set_costs_per_cluster(points);
		//kmeans.get_costs_per_cluster(communityClustersVect);
		//mandar a csv puntos con cluster id. id,x,y,id_cluster
		//kmeans.writeCommunities();
  }

};
