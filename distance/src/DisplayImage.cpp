#include <Python.h>
#include "/usr/lib64/python2.7/site-packages/numpy/core/include/numpy/arrayobject.h"
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>

using namespace cv;
using namespace std;

struct Point2D {
	int x, y;
	//Point2D Point2D_Centroid(int x, int y) : x(x), y(y) {}
};

struct Grid {
	vector<Point2D> elements;
	int noElements = 0;
	int invalidCells = 0;
	float sum = 0.0;
	float value;

	Grid() {}

	Grid(vector<Point2D> elements, int noElements, int invalidCells, float sum) :
		elements(elements),noElements(noElements), invalidCells(invalidCells), sum(sum) {}

};

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

		ofstream indata;
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

		indata << image << endl;
		indata.close();

		namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
		imshow( "Display window", image );                   // Show our image inside it.

		//cout << image << endl;
		ifstream infile;
		infile.open("input_data.txt");
		string semicol, com;

		vector<double> tokens;

		while(getline(infile, semicol, ';')) {
			semicol.erase(remove(semicol.begin(), semicol.end(), ' '), semicol.end());
			semicol.erase(remove(semicol.begin(), semicol.end(), '['), semicol.end());
			semicol.erase(remove(semicol.begin(), semicol.end(), ']'), semicol.end());
			semicol.erase(remove(semicol.begin(), semicol.end(), '\n'), semicol.end());
			istringstream split(semicol);
			for (string each; getline(split, each, ','); tokens.push_back(atof(each.c_str())));
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
			if (cCols == cols - 1) {
				cCols = 0;
				cRows++;
			} else {
				costos[cRows][cCols] = tokens.at(i);
				cCols++;
			}
		}
		return costos;
	}

	void define_intervals(int rows, int cols, int &yIntervals, int &xIntervals) {
			if (rows <= 1000)
				yIntervals = 10;
			else
				yIntervals = ceil(rows / 35);

			if (cols <= 1000)
				xIntervals = 10;
			else
				xIntervals = ceil(cols / 35);

		}

	Point2D define_grids(int rows, int cols, int &xIntervals, int &yIntervals, float** input_raster) {
			int xPosGrid, yPosGrid, cont = 0, c = 0, xRange = ceil(rows / (double) xIntervals), yRange = ceil(cols / (double) yIntervals);
			Grid gridPtr;
			Grid totalGrids[xIntervals][yIntervals];
			Point2D tmp;
			map<float,Grid*> gridsMap;

			for(int i = 0; i < rows; i++)
				for(int j = 0; j < cols; j++) {
					xPosGrid = floor(i / xRange);
					yPosGrid = floor(j / yRange);
					//FIXME: Change tmp
					tmp.x = i;
					tmp.y = j;

					totalGrids[xPosGrid][yPosGrid].noElements++;
					if (input_raster[i][j] != -9999) {
						totalGrids[xPosGrid][yPosGrid].elements.push_back(tmp);
						totalGrids[xPosGrid][yPosGrid].sum += input_raster[i][j];
						totalGrids[xPosGrid][yPosGrid].value = input_raster[i][j];
					}
					else {
						totalGrids[xPosGrid][yPosGrid].invalidCells++;
						totalGrids[xPosGrid][yPosGrid].value = -9999;
					}

					c++;
					if ((totalGrids[xPosGrid][yPosGrid].elements.size() + totalGrids[xPosGrid][yPosGrid].invalidCells) == (xRange * yRange)) {
						float gridSum = totalGrids[xPosGrid][yPosGrid].sum;
						gridsMap.insert(pair<float,Grid*>(gridSum, &totalGrids[xPosGrid][yPosGrid]));
						cont++;
					}
				}
			//FIXME
			for (int i = 0; i < xIntervals; i++)
				for (int j = 0; j < yIntervals; j++) {
					int suma = totalGrids[i][j].elements.size() + totalGrids[i][j].invalidCells, mult = xRange * yRange;
					if ( suma != mult ) {
						float gridSum = totalGrids[i][j].sum;
						gridsMap.insert(pair<float,Grid*>(gridSum, &gridPtr));
					}
				}

			map<float,Grid*>::iterator it;
			/*for ( it = gridsMap.begin(); it != gridsMap.end(); ++it) {
				vector<Point2D> vect = it->second->elements;
				cout << it->first << "  -  " << it->second->sum << endl;
				con++;
			}*/

			//exit(0);

			//map<float,Grid*>::iterator it;
			float xMax = FLT_MIN, xMin = FLT_MAX, yMax = FLT_MIN, yMin = FLT_MAX;

			it = (--gridsMap.end());

			//cout << it->second->sum << endl;

			for (int i = 0; i < it->second->elements.size(); i++) {
				if (it->second->value != -9999) {
					if(it->second->elements.at(i).x > xMax)
						xMax = it->second->elements.at(i).x;

					if(it->second->elements.at(i).x < xMin)
						xMin = it->second->elements.at(i).x;

					if(it->second->elements.at(i).y > yMax)
						yMax = it->second->elements.at(i).y;

					if(it->second->elements.at(i).y < yMin)
						yMin = it->second->elements.at(i).y;
				}
			}

			Point2D centroid;

			//cout << "xMin" << xMin << "   yMin" << yMin << endl;
			centroid.x = xMin + round((xMax - xMin) / 2) ;
			centroid.y = yMin + round((yMax - yMin) / 2);

			//exit(0);

			int sum = 0;
			map<float,Grid*>::iterator it2;
			it2 = (--gridsMap.end());
			for (int i = 0; i < it2->second->elements.size(); i++) {
				vector<Point2D> vect = it2->second->elements;
				if(it2->second->elements.at(i).x - 238 <= it2->second->elements.at(i).x) {
					if(it2->second->elements.at(i).x <= it2->second->elements.at(i).x + 238) {
						if (input_raster[it2->second->elements.at(i).x][it2->second->elements.at(i).y] > 0)
							sum += input_raster[it2->second->elements.at(i).x][it2->second->elements.at(i).y];
							//cout << input_raster[it2->second->elements.at(i).x][it2->second->elements.at(i).y] << endl;
					}
				}
			}
		    //cout << sum << endl;
			return centroid;
		}



	void matrix_to_tiff(int** output_raster, int rows, int cols) {

			setenv("PYTHONPATH",".",1);
			Py_Initialize();

			PyObject *pName, *pModule, *pDict, *pFunc;


			PyObject* pArgs = PyTuple_New(rows*cols + 2);
			PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", rows));
			PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", cols));

			int c = 2;

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++, c++)
					PyTuple_SetItem(pArgs, c, Py_BuildValue("i", output_raster[i][j]));

			pName = PyString_FromString((char*)"write_array");

			pModule = PyImport_Import(pName);

			pDict = PyModule_GetDict(pModule);

			pFunc = PyDict_GetItemString(pDict, (char*)"writeArray");


		   if (PyCallable_Check(pFunc)){
			   PyErr_Print();
			   PyObject_CallObject(pFunc, pArgs);
			   //cout << "Done" << endl;
			   //PyObject_CallFunctionObjArgs(pFunc, pRows, pCols, pArgs);
			   //PyErr_Print();
		   } else {
			   printf("Err\n");
			   PyErr_Print();
		   }
		   //cout << "Done" << endl;
		}
};

