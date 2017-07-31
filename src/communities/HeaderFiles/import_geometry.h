#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "morton.h"
#include <map>
#include <stdlib.h>
#include <iomanip>


using std::vector;
using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::stringstream;
using std::stoi;
using std::stod;


class MortonCode{

public:
	//MortonCode();
	//~MortonCode();

	Point testPoint;

	void importCoordinates(string fileName);	//this function imports coordinates from an .csv file.
	void calculateMortonCodes();				//this function calculate morton codes for a given communities
	void writeCommunities();
	void groupCommunities();
	void calculateStats();


	 struct community
	 {
		int id;
		double x;
		double y;
		//int pop;
	};

	struct clusterElems
	{
		int clusterID;
		long long MortonCode;

		clusterElems(int clusterID, long long MortonCode) : clusterID(clusterID), MortonCode(MortonCode) {}
	};

	struct cluster
	{
		int clusterID;
		long long sum;
		double avg;
		double var;
		double stdDev;

		cluster(int clusterID, long long sum, double avg, double var, double stdDev): clusterID(clusterID), sum(sum), avg(avg), var(var), stdDev(stdDev) {}
	};

private:
	unsigned int numCommunities;
	int tmpMorton;
	std::vector<clusterElems> clstElements;
	std::vector<cluster> clusterInfo;
 	std::vector<community> allCommunities;					//temporally stores all communities
	std::map<long long, int> sortedCommunities;
	std::map<int, community> communities;
};
