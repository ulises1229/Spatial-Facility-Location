#include "../HeaderFiles/import_geometry.h"
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
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

void MortonCode::importCoordinates(string fileName){
	cout<<"File name is: " << fileName <<endl;
	ifstream file (fileName);
	string line;
	std::cout<<"Reading CSV File..."<<std::endl;
	clock_t begin = clock();
	//Loading files from CSV
	for(int i=0; getline(file, line); i++)
	{
		stringstream  lineStream(line);
		string cell;
		clock_t begin = clock();
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
}

//Group communities by populaion
void MortonCode::groupCommunities(){
	for(unsigned int i =0; i< sortedCommunities.size(); i++){
	}
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
	std::cout<<"Writing output file..."<<std::endl;
	std::ofstream myfile;
	myfile.open ("sortedCommunities.csv");
	myfile << "Id," <<"X," <<" Y," << "Pop,"<<endl;
	long long sum = 0;
	double dev, avg, var, sd, sdev = 0;
	int n;
	std::vector<double> results;
	std::map<long long, int>::iterator secIt;
	std::map<int, community>::iterator tmpIt;
	std::map<long long, int>::iterator it = sortedCommunities.begin();
	for(it = sortedCommunities.begin(); it!=sortedCommunities.end(); ++it){
		tmpIt = communities.find(it->second);
		myfile << std::setprecision(17) << it-> second << "," << tmpIt->second.x << "," << tmpIt->second.y<<"," /*<< tmpIt->second.pop*/ <<endl ;
		cout << it->first << ", ";
	}
	myfile.close();
	std::cout<<"File was written correctly!"<<std::endl;

	//std::map<long long, int>::iterator secIt = sortedCommunities.begin();

	for (it = next(sortedCommunities.begin()); it!=sortedCommunities.end(); it++) {

		sum += it->first - prev(it)->first;
		results.push_back(it->first - prev(it)->first);
		//printf("%lld - %lld = %lld\n", it->first, prev(it)->first, it->first - prev(it)->first);
		cout << it->first - prev(it)->first << ", " << endl;
	}
	avg = sum / (results.size());
	//cout << "Avg: " << avg << endl;
	for (int i = 0; i < results.size(); i++) {
		dev = (results[i] - avg) * (results[i] - avg);
		sdev += dev;
	}
	var = sdev / (results.size());
	sd = sqrt(var);
	//printf(" Avg: %f\nStd Dev: %f\nVar: %f ", avg, sd, var);
	cout << setprecision(17) << "Avg: " << avg << endl << "Std Dev: " << sd << endl << "Var: " << var << endl;
	//cout << "\n" << endl;
}
