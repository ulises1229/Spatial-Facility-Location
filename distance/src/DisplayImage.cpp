/*
 * DisplayImage.cpp
 *
 *  Created on: 17/08/2017
 *      Author: lanase
 */
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

class Display_image{
public:

	int get_imageWidth(string file){
		Mat image;
		image = imread("/home/lanase/workspace/distance/Debug/"+file, CV_LOAD_IMAGE_ANYDEPTH);
		return image.size().width;
	}

	int get_imageHeight(string file){
		Mat image;
		image = imread("/home/lanase/workspace/distance/Debug/"+file, CV_LOAD_IMAGE_ANYDEPTH);
		return image.size().height;
	}

	float** tiff_to_matrix(string file){


		int imageWidth, imageHeight, rows, cols;

		ofstream indata, outfl;
		indata.open("input_data.txt");

		Mat image;
		image = imread("/home/lanase/workspace/distance/Debug/"+file, CV_LOAD_IMAGE_ANYDEPTH);   // Read the file

		if(! image.data ){
			cout <<  "Could not open or find the image" << std::endl ;
			exit(0);
		}

		imageWidth = image.size().width;
		imageHeight = image.size().height;

		rows = imageHeight; cols = imageWidth;

		cout << rows << "   " << cols << endl;



		//cout << image << endl;
		//cout << image.at<float>(11,15) << endl;
		indata << image << endl;
		indata.close();

		namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
		imshow( "Display window", image );                   // Show our image inside it.

		ifstream infile;
		infile.open("input_data.txt");
		string semicol, com;

		vector<double> tokens;
		outfl.open("cost_raster.txt");

		while(getline(infile, semicol, ';')) {
			//tokens.clear();
			//string::iterator end_pos = remove(semicol.begin(), semicol.end(), ' ');
			//cout << semicol << endl;
			semicol.erase(remove(semicol.begin(), semicol.end(), ' '), semicol.end());
			semicol.erase(remove(semicol.begin(), semicol.end(), '['), semicol.end());
			semicol.erase(remove(semicol.begin(), semicol.end(), ']'), semicol.end());
			semicol.erase(remove(semicol.begin(), semicol.end(), '\n'), semicol.end());
			//cout << semicol << endl;
			istringstream split(semicol);
			for (string each; getline(split, each, ','); tokens.push_back(atof(each.c_str())));
			//outfl << semicol << endl;
		}

		int cCols = 0, cRows = 0;
		float** costos = new float*[rows];
		for(int i = 0; i< rows; ++i)
			costos[i] = new float[cols];

		for (int j = 0; j < rows; j++) {
			for (int k = 0; k < cols; k++) {
				costos[j][k] = -9999;
			}
		}

		for (int i = 0; i < tokens.size(); i++) {
			if (cCols == cols) {
				outfl << endl;
				cCols = 0;
				cRows++;
			} else {
				costos[cRows][cCols] = tokens.at(i);
				outfl << tokens.at(i) << " ";
				cCols++;
			}
		}

		outfl.close();
		//infile.close();
		//waitKey(0);// Wait for a keystroke in the window
		return costos;
	}
};

