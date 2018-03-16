/*
 * morton.cpp
 *
 *  Created on: 10/11/2017
 *      Author: lanase
 */
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <stdlib.h>
#include <iomanip>

using namespace std;

struct cluster{
	int clusterID;
	long long sum;
	double totBiomass;
	double avgFriction;
	double avg;
	double var;
	double stdDev;

	cluster(int clusterID, long long sum, double totBiomass, double avgFriction, double avg, double var, double stdDev): clusterID(clusterID), sum(sum), totBiomass(totBiomass), avgFriction(avgFriction), avg(avg), var(var), stdDev(stdDev) {}
};

class Morton {

private:
    float m_x , m_y;

public:
    float** biomass;
	float** friction;
	int ROWS, COLS;
	map<long long, int> clusters;
	vector<cluster> clusterInfo;

	inline long long mortonIndex(float x, float y ) {

			m_x = x;
			m_y = y;
	        // Pun the x and y coordinates as integers: Just re-interpret the bits.
	        //
	        auto ix = reinterpret_cast<const unsigned &>(this->m_x);
	        auto iy = reinterpret_cast<const unsigned &>(this->m_y);

	        // Since we're assuming 2s complement arithmetic (99.99% of hardware today),
	        // we'll need to convert these raw integer-punned floats into
	        // their corresponding integer "indices".

	        // Smear their sign bits into these for twiddling below.
	        //
	        const auto ixs = static_cast<int>(ix) >> 31;
	        const auto iys = static_cast<int>(iy) >> 31;

	        // This is a combination of a fast absolute value and a bias.
	        //
	        // We need to adjust the values so -FLT_MAX is close to 0.
	        //
	        ix = (((ix & 0x7FFFFFFFL) ^ ixs) - ixs) + 0x7FFFFFFFL;
	        iy = (((iy & 0x7FFFFFFFL) ^ iys) - iys) + 0x7FFFFFFFL;

	        // Now we have -FLT_MAX close to 0, and FLT_MAX close to UINT_MAX,
	        // with everything else in-between.
	        //
	        // To make this easy, we'll work with x and y as 64-bit integers.
	        //
	        long long xx = ix;
	        long long yy = iy;

	        // Dilate and combine as usual...

	        xx = (xx | (xx << 16)) & 0x0000ffff0000ffffLL;
	        yy = (yy | (yy << 16)) & 0x0000ffff0000ffffLL;

	        xx = (xx | (xx <<  8)) & 0x00ff00ff00ff00ffLL;
	        yy = (yy | (yy <<  8)) & 0x00ff00ff00ff00ffLL;

	        xx = (xx | (xx <<  4)) & 0x0f0f0f0f0f0f0f0fLL;
	        yy = (yy | (yy <<  4)) & 0x0f0f0f0f0f0f0f0fLL;

	        xx = (xx | (xx <<  2)) & 0x3333333333333333LL;
	        yy = (yy | (yy <<  2)) & 0x3333333333333333LL;

	        xx = (xx | (xx <<  1)) & 0x5555555555555555LL;
	        yy = (yy | (yy <<  1)) & 0x5555555555555555LL;

	        return xx | (yy << 1);
	  }

	void fillClusters() {
		int k, n, elementsPerCluster, i = 1, elemsCount = 1;
		n = clusters.size();
		k = round(1 + log2(n));
		elementsPerCluster = n / k;
		map<long long, int>::iterator it;
	}

	void calculateMortonCodes(float** biomass, float** friction) {
		this->biomass = new float*[this->ROWS];
		this->friction = new float*[this->ROWS];
		for(int i = 0; i< ROWS; ++i){
			this->biomass[i] = new float[COLS];
			this->friction[i] = new float[COLS];
		}

		for(int x = 0; x < ROWS; x++){
			for(int y = 0; y < COLS; y++){
				this->biomass[x][y] = biomass[x][y];
				this->friction[x][y] = friction[x][y];
			}
		}

		int id = 1;
		for(int i = 0; i < ROWS; i++) {
			for(int j = 0; j < COLS; j++) {
				if(biomass[i][j] != -9999 && friction[i][j] >= 0 && biomass[i][j] >= friction[i][j]) {
					clusters.insert(pair<long long, int>(mortonIndex(i, j), id));
					id++;
				}
			}
		}
	}

};
