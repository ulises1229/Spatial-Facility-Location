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


	 struct community
	 {
		int id;
		double x;
		double y;
		//int pop;
	};

private:
	unsigned int numCommunities;
	int tmpMorton;
	std::vector<community> allCommunities;					//temporally stores all communities
	std::map<long long, int> sortedCommunities;
	std::map<int, community> communities;
};
