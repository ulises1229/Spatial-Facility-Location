#include "../HeaderFiles/import_geometry.h"
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <sys/time.h>
using namespace std;

/*MortonCode::MortonCode(){
	//importCoordinates();
	cout<<"enter to constructor"<<endl;
}

//FIXME: clear memory before exiting
MortonCode::~MortonCode(){
	cout<<"enter to destructor"<<endl;
}*/

/*MortonCode::MortonCode(){

}

MortonCode::~MortonCode(){

}*/

void MortonCode::importCoordinates(string fileName, int v){
	cout<<"File name is: " << fileName <<endl;
	ifstream file (fileName);
	string line;
	std::cout<<"Reading CSV File..."<<std::endl;
	clock_t begin = clock();
	int num_lines = 0;
	//Loading files from CSV
	for(int i=0; getline(file, line) && num_lines < 20000 * v; i++, num_lines++)
	{
		stringstream  lineStream(line);
		string cell;
		//temporal community to store tmp id, x and y values
		community tmp;

		for(int j=0; getline(lineStream, cell, ','); j++)
		{
			switch(j)
			{
			case 0:
				tmp.id = stoi (cell);
				break;
			case 1:
				tmp.x = std::stod(cell);
				break;
			case 2:
				tmp.y = stod (cell);
				break;
			/*case 3:
				tmp.pop = stoi(cell);
				break;*/
			}

		}
		allCommunities.push_back(tmp);

		//Store all coordinates in a map to avoid sorting later
		communities.insert(std::pair<int, community>(tmp.id,tmp));


	}
	clock_t end = clock();
	std::cout<<"CSV file has parsed correctly."<<std::endl;
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout<<"Time to import a CSV files was:" << elapsed_secs <<endl;

	//calculate morton codes of all input communities
	calculateMortonCodes();
	//Organize communities by poopulation limits
	groupCommunities();

	//write csv file with results

	writeCommunities();

	//Calculate the Statistics of the MortonCodes, such as Average (Mean), Variance & Standard Deviation
	calculateStats();
}

//Group communities by populaion
void MortonCode::groupCommunities(){
	int k, n, elementsPerCluster, i = 1, elemsCount = 1;
	n = allCommunities.size();
	k = round(1 + log2(n));
	elementsPerCluster = n / k;
	cout << "Clusters: " << k << "   Element Per Cluster: " << elementsPerCluster << endl;
	std::map<long long, int>::iterator it;
	clusterInfo.emplace_back(i, 0, 0, 0, 0);

	for (it = sortedCommunities.begin(); it != sortedCommunities.end(); it++) {
		if (elemsCount > elementsPerCluster) {
			elemsCount = 1;
			i++;
			clusterInfo.emplace_back(i, 0, 0, 0, 0);
		}
		if (i <= k) {
			clstElements.emplace_back(i, it->first);
			elemsCount++;
		} else {
			clstElements.emplace_back(i - 1, it->first);
		}
	}

	/*for(cluster const& info: clusterInfo) {
		cout << "Cluster " << info.clusterID << " :" << endl;
		for (clusterElems const& elem: clstElements) {
			if (elem.clusterID == info.clusterID)
				cout << "\t" << elem.MortonCode << endl;
		}
	}*/
}

//Calculate morton codes of all communities
void MortonCode::calculateMortonCodes(){
	clock_t begin = clock();
	//calculates morton codes for all values
	for(int i =0; i<allCommunities.size();i++){
		//A Map automatically sort the key elements and store morton codes in a MAP
		sortedCommunities.insert(std::pair<long long, int>( testPoint.mortonIndex(allCommunities[i].x, allCommunities[i].y), allCommunities[i].id));
	}
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout<<"Time to sort and generate morton codes was:" << elapsed_secs <<endl;
}


void MortonCode::writeCommunities()
{
	clock_t begin = clock();
	std::cout<<"Writing output file..."<<std::endl;
	std::ofstream myfile;
	myfile.open ("sortedCommunities.csv");
	myfile << "Id," <<"X," <<" Y," << "Pop,"<<endl;
	std::map<long long, int>::iterator secIt;
	std::map<int, community>::iterator tmpIt;
	std::map<long long, int>::iterator it = sortedCommunities.begin();
	for(it = sortedCommunities.begin(); it!=sortedCommunities.end(); ++it){
		tmpIt = communities.find(it->second);
		myfile << std::setprecision(17) << it-> second << "," << tmpIt->second.x << "," << tmpIt->second.y<<"," /*<< tmpIt->second.pop*/ <<endl ;
		//cout << it->first << ", ";
	}
	myfile.close();
	std::cout<<"File was written correctly!"<<std::endl;
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout<<"Time to write csv with sorted communities:" << elapsed_secs <<endl;
}


void MortonCode::calculateStats()
{
	long long sum, diff;
	double dev, avg, var, sd, sdev;
	int n;
	std::vector<double> results;
	std::vector<cluster>::iterator clustInfo;
	std::vector<clusterElems>::iterator clustElem;
	for(clustInfo = clusterInfo.begin(); clustInfo != clusterInfo.end(); clustInfo++) {
		sum = 0; sdev = 0;
		for (clustElem = next(clstElements.begin()); clustElem != clstElements.end(); clustElem++) {
			if ( (clustElem->clusterID == clustInfo->clusterID) && (prev(clustElem)->clusterID == clustInfo->clusterID) ) {
				diff = clustElem->MortonCode - prev(clustElem)->MortonCode;
				sum += diff;
				results.push_back(diff);
			}
		}
		avg = sum / (double) results.size();
		for (int i = 0; i < results.size(); i++) {
			dev = (results[i] - avg) * (results[i] - avg);
			sdev += dev;
		}
		var = sdev / results.size();
		sd = sqrt(var);
		clustInfo->sum = sum; clustInfo->avg = avg; clustInfo->var = var; clustInfo->stdDev = sd;
		results.clear();
	}
}
