#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <gdal.h>
#include <gdal_priv.h>


#define INTERVALS 125 //50  5376

using namespace cv;
using namespace std;

/*cv::Point2d topLeft( -122.441017, 37.815664 );
cv::Point2d topRight( -122.370919, 37.815311 );
cv::Point2d bottomLeft( -122.441533, 37.747167 );
cv::Point2d bottomRight( -122.3715,   37.746814 );*/


struct Point2D {
	int x, y;
	Point2D () {};
	Point2D(int x, int y) : x(x), y(y) {}
	bool operator () (Point2D const &e) const {
		return (x == e.x && y == e.y);
	}
};

struct Grid {
	vector<Point2D> elements;
	int noElements = 0;
	int invalidCells = 0;
	int id;
	float sum = 0.0;
	float biomass = 0;
	float friction = 0;
	float value;
	float biomassAvg = 0;

	Grid() {}

	Grid(vector<Point2D> elements, int noElements, int invalidCells, float sum) :
		elements(elements),noElements(noElements), invalidCells(invalidCells), sum(sum) {}

};

struct cellVecinos {
	int x, y;
	float distance;
	cellVecinos(int x, int y, float distance) :
			x(x), y(y), distance(distance) {}
};

// Utility method for comparing two cells
static bool operator<(const cellVecinos& a, const cellVecinos& b){
	if (a.distance == b.distance){
		if (a.x != b.x)
			return (a.x < b.x);
		else
			return (a.y < b.y);
	}
	return (a.distance < b.distance);
}


class Display_image{
public:
	float avg_biomasa, pixels_necesarios = 0, xMax = FLT_MIN, xMin = FLT_MAX, yMax = FLT_MIN, yMin = FLT_MAX;
	int ROWS, COLS, intervals = 0, totValidGrids = 0, totGridsAvg = 0;
	bool flag = true;
	vector<Point2D> active_raster;
	float** costos;
	map<float,Grid> gridsMap;
	//float** friccion;
	vector<float> tokens;
    //vector<float> tokens_fricc;

	bool isInsideGrid(int i, int j){
			return (i >= 0 && i < ROWS && j >= 0 && j < COLS);
	}


	    float** tiff_to_matrix_gdal(string file, bool flag) {
		GDALDataset *dataset;
		//GDALInfoOptions* GDALInfoOptionsNew("/home/lanase/workspace/distance/Debug/Haiti_biomass.tif");
		char **MD;
		char *info;
		//char *dsInfo;
		GDALAllRegister();
		string ds = file;
		dataset = (GDALDataset *) GDALOpen(ds.c_str(), GA_ReadOnly);


		if(dataset == NULL) {
			cout << "Null dataset" << endl;
			exit(0);
		}

		double adfGeoTransform[6];
		//GDALInfo(dataset, NULL);

		/*printf( "Driver: %s/%s\n",
		        dataset->GetDriver()->GetDescription(),
		        dataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
		printf( "Size is %dx%dx%d\n",
		        dataset->GetRasterXSize(), dataset->GetRasterYSize(),
		        dataset->GetRasterCount() );

		if( dataset->GetProjectionRef()  != NULL )
		    printf( "Projection is `%s'\n", dataset->GetProjectionRef() );

		if( dataset->GetGeoTransform( adfGeoTransform ) == CE_None )
		{
		    printf( "Origin = (%.6f,%.6f)\n",
		            adfGeoTransform[0], adfGeoTransform[3] );
		    printf( "Pixel Size = (%.6f,%.6f)\n",
		            adfGeoTransform[1], adfGeoTransform[5] );
		}
		cout << "Top Left X: " << adfGeoTransform[0] << endl;
		cout << fixed << "Top Left Y: " << adfGeoTransform[3] << endl;*/
		GDALRasterBand  *poBand;
		int             nBlockXSize, nBlockYSize;
		int             bGotMin, bGotMax;
		double          adfMinMax[2];
		poBand = dataset->GetRasterBand(1);
		/*poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
		printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
		        nBlockXSize, nBlockYSize,
		        GDALGetDataTypeName(poBand->GetRasterDataType()),
		        GDALGetColorInterpretationName(
		            poBand->GetColorInterpretation()) );
		adfMinMax[0] = poBand->GetMinimum( &bGotMin );
		adfMinMax[1] = poBand->GetMaximum( &bGotMax );
		if( ! (bGotMin && bGotMax) )
		    GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
		printf( "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
		if( poBand->GetOverviewCount() > 0 )
		    printf( "Band has %d overviews.\n", poBand->GetOverviewCount() );
		if( poBand->GetColorTable() != NULL )
		   printf( "Band has a color table with %d entries.\n",
		             poBand->GetColorTable()->GetColorEntryCount() );*/

		//float *pafScanline;
		//float *pafScanlineY;

		int nXSize = poBand->GetXSize();
		int nYSize = poBand->GetYSize();

		//cout << nXSize << " -- " << nYSize << endl;

		ROWS = nYSize; COLS = nXSize;

		costos = new float*[ROWS];
		for(int i = 0; i< ROWS; ++i) {
			costos[i] = new float[COLS];
		}

		GDALDataType dataType = poBand->GetRasterDataType();
		float *pBuf = new float[nYSize * nXSize];

		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pBuf, nXSize, nYSize, dataType, 0, 0);

		float biomass = 0;
		int cCols = 0, cRows = 0, cont = 0;
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++) {
				int location = (nXSize * (i)) + j;
				if (cCols == COLS - 1) {
					cCols = 0;
					cRows++;
				}
				else {
					costos[cRows][cCols] = *(pBuf+location);
					if(*(pBuf+location) > 0 && flag){
						cont++;
						biomass += *(pBuf+location);
					}
					cCols++;
				}
			}
		}
		if(flag){
			//cout << "Biomasa = " << biomass << endl;
			//cout << "AVG = " << biomass / (cont) << endl;
			this->avg_biomasa = biomass / (cont);
		}
		//exit(0);
		return costos;
	}

	void write_image(float** output_raster, int rows, int cols, string heuristic, int stop, string map, string algName) {
		float* arr = new float[rows * cols * 3];
		int n_pixels = rows * cols, channelDiv = round(stop /255);
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if(output_raster[i][j] > 0 && output_raster[i][j] < 0.1)
					arr[i * cols + j] = output_raster[i][j] * 3;
				else if(output_raster[i][j] >= 0.1)
					arr[i * cols + j] = output_raster[i][j];
				else
					arr[i * cols + j] = 0;
			}
		}
		Mat matrix = Mat(rows, cols, CV_32FC1, arr);
		Mat outMatrix;
		Mat im_color = Mat::zeros(rows, cols, CV_32FC4);

		vector<Mat> planes;
		vector<Mat>::iterator itMat;
		for(int i = 0; i < 4; i++)
			planes.push_back(matrix);
		merge(planes, im_color);

		for(int i = 0; i < n_pixels; i++) {
			Vec4f &v = im_color.at<Vec4f>(i/cols, i%cols);
			if(v[0] > 0) {
				v.val[0] = v[0] / channelDiv;
				v.val[1] = v[1] / channelDiv;
				v.val[2] = v[2] / channelDiv;
				v.val[3] = 255;
			}
		}

		std::ostringstream ostr;
		ostr << stop;
		string sStop = ostr.str();
		string fileName = "final_route_"+map+"_"+algName+"_"+sStop+"_"+heuristic+".png";
		imwrite(fileName, im_color);
	}




	void define_intervals(int stop, int &xIntervals, int &yIntervals) {

		pixels_necesarios = ceil(stop / avg_biomasa);

		intervals = ceil(sqrt(pixels_necesarios));

		yIntervals = ceil(COLS / (double) intervals);
		xIntervals = ceil(ROWS / (double) intervals);
		}

	map<float,Grid> define_grids(int rows, int cols, const int &xIntervals, const int &yIntervals, float** biomass, float** friction) {
			int xPosGrid, yPosGrid, id = 1, c = 0, cont = 0, contValid = 0;
			Grid** totalGrids = new Grid*[xIntervals];
			for (int i = 0; i< xIntervals; i++) {
				totalGrids[i] = new Grid[yIntervals];
			}

			Point2D tmp;
			for(int i = 0; i < rows; i++)
				for(int j = 0; j < cols; j++) {
					xPosGrid = floor(i / intervals);
					yPosGrid = floor(j / intervals);
					//FIXME: Change tmp
					tmp.x = i;
					tmp.y = j;
					//totalGrids[xPosGrid][yPosGrid].elements.push_back(tmp);
					totalGrids[xPosGrid][yPosGrid].noElements++;
					if (biomass[i][j] > 0 && friction[i][j] > 0 && biomass[i][j] > friction[i][j]) {
						totalGrids[xPosGrid][yPosGrid].elements.push_back(tmp);
						totalGrids[xPosGrid][yPosGrid].sum += biomass[i][j] / friction[i][j];
						totalGrids[xPosGrid][yPosGrid].value = biomass[i][j];
						totalGrids[xPosGrid][yPosGrid].biomass += biomass[i][j];
						totalGrids[xPosGrid][yPosGrid].friction += friction[i][j];
						contValid++;

					}
					else {
						//totalGrids[xPosGrid][yPosGrid].elements.push_back(tmp);
						totalGrids[xPosGrid][yPosGrid].invalidCells = totalGrids[xPosGrid][yPosGrid].invalidCells + 1;
						totalGrids[xPosGrid][yPosGrid].value = -9999;
					}

					c++;
					if (totalGrids[xPosGrid][yPosGrid].elements.size() + totalGrids[xPosGrid][yPosGrid].invalidCells == (intervals * intervals)
					    && totalGrids[xPosGrid][yPosGrid].biomass > 0 && totalGrids[xPosGrid][yPosGrid].friction > 0
						&& totalGrids[xPosGrid][yPosGrid].biomass > totalGrids[xPosGrid][yPosGrid].friction

						&& totalGrids[xPosGrid][yPosGrid].elements.size() > totalGrids[xPosGrid][yPosGrid].invalidCells) {
							totalGrids[xPosGrid][yPosGrid].id = id;
							totalGrids[xPosGrid][yPosGrid].biomassAvg = totalGrids[xPosGrid][yPosGrid].biomass / totalGrids[xPosGrid][yPosGrid].elements.size();
							if(totalGrids[xPosGrid][yPosGrid].biomassAvg >= avg_biomasa)
								totGridsAvg++;
							id++;
							cont++;
							//cout << totalGrids[xPosGrid][yPosGrid].elements.size() << " - " << totalGrids[xPosGrid][yPosGrid].invalidCells << endl;
							float gridSum = totalGrids[xPosGrid][yPosGrid].biomass / totalGrids[xPosGrid][yPosGrid].friction;
							gridsMap.insert(pair<float,Grid>(gridSum, totalGrids[xPosGrid][yPosGrid]));
					}
				}
			totValidGrids = cont;
			return gridsMap;
		}

	Point2D find_centroid(map<float,Grid> grids, float** biomass, float** friction) {
		map<float,Grid>::iterator it;
		float xMax = FLT_MIN, xMin = FLT_MAX, yMax = FLT_MIN, yMin = FLT_MAX;
		/*map<float,Grid>::iterator it2;
		for ( it = gridsMap.begin(); it != gridsMap.end(); ++it) {
			float xMax = FLT_MIN, xMin = FLT_MAX, yMax = FLT_MIN, yMin = FLT_MAX;
			cout << it->second.elements.size() + it->second.invalidCells << "\t Relation: " << it->first  << "\t Biomass: " << it->second.biomass << "\t Friction: " << it->second.friction << endl;
		}
		cout << "Finished. " << gridsMap.size() << endl;
		exit(0);*/
		if (!grids.empty()) {
			it = (--grids.end());
		} else {
			flag = false;
		}

		Point2D centroid;

		if(flag){
			cout << "Relation: " << it->first << endl;
			for (int i = 0; i < it->second.elements.size(); i++) {
				if(it->second.elements.at(i).x > xMax)
					xMax = it->second.elements.at(i).x;

				if(it->second.elements.at(i).x < xMin)
					xMin = it->second.elements.at(i).x;

				if(it->second.elements.at(i).y > yMax)
					yMax = it->second.elements.at(i).y;

				if(it->second.elements.at(i).y < yMin)
					yMin = it->second.elements.at(i).y;
			}

			centroid.x = xMin + round((xMax - xMin) / 2) ;
			centroid.y = yMin + round((yMax - yMin) / 2);
			this->xMax = xMax; this->xMin = xMin;
			this->yMax = yMax; this->yMin = yMin;
			if(biomass[centroid.x][centroid.y] < 0 || biomass[centroid.x][centroid.y] < friction[centroid.x][centroid.y]){
				cout << "Invalid Centroid: " << centroid.x << ", " << centroid.y << endl;
				bool found = true;
				set<cellVecinos> vecinos = vecinos2(centroid.x, centroid.y);
				set<cellVecinos> celdas;
				celdas.insert(cellVecinos(centroid.x, centroid.y, 0));
				set <cellVecinos> :: iterator itr;

					while(found) {
						for (itr = vecinos.begin(); itr != vecinos.end(); ++itr){
							cout << (*itr).x << ", " << (*itr).y << endl;
							if(biomass[(*itr).x][(*itr).y] > 0 &&  biomass[(*itr).x][(*itr).y] > friction[(*itr).x][(*itr).y]){
								//cout << biomass[(*itr).x][(*itr).y] << " " << friction[(*itr).x][(*itr).y] << endl;
								centroid.x = (*itr).x;
								centroid.y = (*itr).y;
								found = false;
								break;
							}
						}
						vecinos = vecinos3(vecinos);
					}
			}
		}
		else{
			centroid.x = -1;
			centroid.y = -1;
		}
		return centroid;
	}



	set<cellVecinos>vecinos2(int origen_x, int origen_y){//busca vecinos iniciales nada mas
			set<cellVecinos>distancias;

			int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
			int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

			set<cellVecinos> st;
			Point2D tmp;

			// insert (0, 0) cell with 0 distance
			st.insert(cellVecinos(origen_x, origen_y, 0));

			cellVecinos k = *st.begin();
			st.erase(st.begin());
			//cout << "k.x = " << k.x << " k.y = " << k.y << " k.distance = " << k.distance <<endl;
			// looping through all neighbours
			for (int i = 0; i < 8; i++){
				int x = k.x + dx[i];
				int y = k.y + dy[i];
				//cout << "x = " << x << " y = " << y << endl;
				// if not inside boundry, ignore them
				if (!isInsideGrid(x, y)){
					//cout << "fuera del grid" << endl;
					continue;
				}

				distancias.insert(cellVecinos(x, y, 0));
				tmp.x = x; tmp.y=y;
				active_raster.push_back(tmp);
			}


			return distancias;
		}

		set<cellVecinos> vecinos3(set<cellVecinos> vecinos){//obtiene vecinos que aun no han sido considerados
			set<cellVecinos>distancias;
			//cout << "\t recibo estos:" << endl;
			set <cellVecinos> :: iterator itr2;
			for (itr2 = vecinos.begin(); itr2 != vecinos.end(); ++itr2){
				//cout << "\t " << (*itr2).x << ", " << (*itr2).y << endl;
			}
			int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
			int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

			Point2D tmp;
			vector<Point2D>::iterator it;
			set <cellVecinos> :: iterator itr;
			for (itr = vecinos.begin(); itr != vecinos.end(); ++itr){
				cellVecinos k = *itr;
				//cout << "\t -k" << k.x << ", "<< k.y << endl;
			// looping through all neighbours
				for (int i = 0; i < 8; i++){
					int x = k.x + dx[i];
					int y = k.y + dy[i];
					it = find_if(active_raster.begin(), active_raster.end(), Point2D(x, y));
					//cout << "\t x = " << x << " y = " << y << " active_raster= " << active_raster[x][y] << endl;
					// if not inside boundry, ignore them
					if (!isInsideGrid(x, y) || it != active_raster.end()){
						//cout << "\t fuera del grid" << endl;
						continue;
					}
					distancias.insert(cellVecinos(x, y, 0));
					tmp.x = x; tmp.y = y;
					active_raster.push_back(tmp);

				}
			}

			return distancias;
		}

/*	void matrix_to_tiff(float** output_raster, int rows, int cols) {


			setenv("PYTHONPATH",".",1);
			Py_Initialize();

			PyObject *pName, *pModule, *pDict, *pFunc;


			PyObject* pArgs = PyTuple_New(rows*cols + 2);
			PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", rows));
			PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", cols));

			int c = 2;

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++, c++)
					PyTuple_SetItem(pArgs, c, Py_BuildValue("f", output_raster[i][j]));

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
		}  */
};

