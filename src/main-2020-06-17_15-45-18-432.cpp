/*
 * main.cpp
 * Author: Ulises Olivares
 * uolivares@unam.mx
 * June 12, 2020
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "exploracion.h" //A*
#include "tree.h" //Nodos

// FIXME: CORRECT THESE TWO FILES, INCLUDE THE H FILE
#include "dijkstra.cpp" //Dijkstra
#include "bell-ford.cpp" //Bellman-Ford

#include <tclap/CmdLine.h>
#include "raster.h"
#include "distance.h"

// Variable declaration
struct points_export{
	int x, y, xMin, xMax, yMin, yMax;
};

char is_usable;
string map_biomass, map_friction, hName, region, map_npa, map_waterbodies;
char algorithm, heuristic;
int demanda, optValidation, grids_to_validate, export_points;

// Methods declarations
void parseParameters(int argc, const char** argv);

int main(int argc, const char** argv){

    // Object definition
	Raster objRaster;

	//parse command-line parameters
    parseParameters(argc, argv);

	// Import biomass and friction
    cout << "-------------------------------------------------------"<< endl;
    cout << "Importing data ... " << endl;

    //FIXME:  add a directive to detect OS and use relative path
    //string path = "/Users/ulisesolivares2/Documents/GitHub/PowerPlantOptimization/src/input/"; // IMAC
    string path = "/Users/ulisesolivares/Documents/GitHub/PowerPlantOptimization/src/input/"; // MACBOOK
    string outPath = "/Users/ulisesolivares/Documents/GitHub/PowerPlantOptimization/src/output/";
	map_biomass =  path + map_biomass;
	map_friction = path + map_friction;

	clock_t begin = clock();    // start timer

	// Import input maps
	float** biomass = objRaster.tiff_to_matrix_gdal(map_biomass, true);
	float** friction = objRaster.tiff_to_matrix_gdal(map_friction, false);

	// In case of a non-supported projection
	//cout << objRaster.epsg << endl;
	//objRaster.reproject_coords(map_biomass);

	clock_t end = clock();
	int rows = objRaster.getRows();
	int cols = objRaster.getCols();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Total importation time: " << elapsed_secs << " secs." << endl;
    cout << "-------------------------------------------------------"<< endl;

    // FIXME: Use a common class for all strucutures
    Point2D centroid;

    // Create a file to export time
	ofstream info, bestInfo, coords;
	ostringstream ss, bestSs;
	ss << "centroids_" << demanda << "_" << hName << ".csv";
	bestSs << "Exported_points.csv";


	//string fileName = "puntos_" + stop + ".csv"

	info.open(ss.str().c_str());
	info << "X, Y, Size, Biomass, Cost" << endl;
	bestInfo.open(bestSs.str().c_str());

	// Verify if there is a map of Natural Protected Areas
	if(map_npa != "") {
		float** npa_matrix = objRaster.tiff_to_matrix_gdal(map_npa, false);
		objRaster.check_npa(npa_matrix, biomass);
	}
    // Verify if there is a map of Water bodies
	if(map_waterbodies != "") {
		float** water_matrix = objRaster.tiff_to_matrix_gdal(map_waterbodies, false);
		objRaster.check_waterbodies(water_matrix, biomass);
	}

	// Define the number of grids for each axis
	int xIntervals = 0, yIntervals = 0;
	objRaster.define_intervals(demanda, xIntervals, yIntervals);


	switch (optValidation){
	    case 1:
	        break;
	    case 2:
	        break;
	    case 3:
	        break;
	    case 4:
	        break;
	}

	if(optValidation == 4) {
		costDistance d;
		d.COL = cols;
		d.ROW = rows;

		float bestCost = 0, bestxMin, bestxMax, bestyMin, bestyMax;
		int bestX, bestY, cont = 1;

		for (int i = 0; i < rows; i++) {
		    for (int j = 0; j < cols; j++) {
		        centroid.x = i; centroid.y = j;
		        if (biomass[i][j] > 0) {
                        cout << biomass[i][j] << endl;
                        coords.open("coords.txt");
                        cout << centroid.x << ", " << centroid.y << endl;
                        cout << "No. " << cont << " / " << objRaster.getValidPoints() << endl;
                        coords << centroid.y << " " << centroid.x;
                        coords.close();
                        objRaster.reproject_coords(map_biomass);
                        clock_t begin_cd = clock();

                        d.inicio_cost_distance(friction, centroid.x, centroid.y, biomass, objRaster.getIntervals(), i - 80, i + 80, j - 80, j + 80, objRaster.getProjection());


                        clock_t end_cd = clock();
                        double elapsed_secs_cd = double(end_cd - begin_cd) / CLOCKS_PER_SEC;

                        cout << "Cost Distance time = " << elapsed_secs_cd << " secs." << endl;
                        switch(algorithm) {
                            case 'B': //Binary search
                            case 'b': {
                                string algName = "Breadth_First_Search";
                                Tree rn;
                                rn.COL = cols;
                                rn.ROW = rows;
                                clock_t begin2 = clock();
                                rn.inicio_rutas(biomass, d.output_raster, centroid.x, centroid.y, demanda, info, heuristic);
                                clock_t end2 = clock();
                                double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
                                cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

                                if(rn.cost > bestCost) {
                                    bestCost = rn.cost;
                                    bestX = rn.x;
                                    bestY = rn.y;
                                    bestxMin = i - 50;
                                    bestxMax = i + 50;
                                    bestyMin = j - 50;
                                    bestyMax = j + 50;
                                }

                                d.freeMem();
                                rn.freeMem();
                                break;
                            }

                            case 'A': //A* search
                            case 'a': {
                                Explore e;
                                string algName = "AStar";
                                e.COL = cols;
                                e.ROW = rows;
                                e.inicio(biomass);
                                clock_t begin2 = clock();
                                e.explore(d.output_raster, centroid.x, centroid.y, demanda, info, heuristic);
                                clock_t end2 = clock();
                                double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
                                cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;

                                if(e.cost > bestCost) {
                                    bestCost = e.cost;
                                    bestX = e.X;
                                    bestY = e.Y;
                                    bestxMin = i - 50;
                                    bestxMax = i + 50;
                                    bestyMin = j - 50;
                                    bestyMax = j + 50;
                                }

                                e.freeMem();
                                d.freeMem();
                                break;

                            }
                        }
                        cont++;
                    }
                }
		}
		cout << "*** Best point ***" << endl;
		coords.open("coords.txt");
		coords << bestY << " " << bestX;
		coords.close();
		objRaster.reproject_coords(map_biomass);
		//cout << "Source = " << bestX << ", " << bestY << endl;
		bestInfo << bestX << ", " << bestY << ",";
		if(algorithm == 'a' || algorithm == 'A') {
			d.inicio_cost_distance(friction, bestX, bestY, biomass, objRaster.getIntervals(), bestxMin, bestxMax, bestyMin, bestyMax, objRaster.getProjection());
			Explore e;
			string algName = "AStar";
			e.COL = cols;
			e.ROW = rows;
			e.inicio(biomass);
			clock_t begin2 = clock();
			e.explore(d.output_raster, bestX, bestY, demanda, bestInfo, heuristic);
			clock_t end2 = clock();
			double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
			cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;
			clock_t begin3 = clock();
			objRaster.write_image(e.matrix_path, rows, cols, hName, demanda, region, algName);
			objRaster.matrix_to_tiff(outPath, e.matrix_path, rows, cols, hName, demanda, region, algName);
			clock_t end3 = clock();
			double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
			cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
			e.freeMem();
		}
		else if(algorithm == 'b' || algorithm == 'B') {
			d.inicio_cost_distance(friction, bestX, bestY, biomass, objRaster.getIntervals(), bestxMin, bestxMax, bestyMin, bestyMax, objRaster.getProjection());
			string algName = "Breadth_First_Search";
			Tree rn;
			rn.COL = cols;
			rn.ROW = rows;
			clock_t begin2 = clock();
			rn.inicio_rutas(biomass, d.output_raster, bestX, bestY, demanda, bestInfo, heuristic);
			clock_t end2 = clock();
			double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
			cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

			clock_t begin3 = clock();
			cout << "Image creation..." << endl;
			objRaster.write_image(rn.matrix_path, rows, cols, hName, demanda, region, algName);
			objRaster.matrix_to_tiff(outPath, rn.matrix_path, rows, cols, hName, demanda, region, algName);
			clock_t end3 = clock();
			double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
			cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
			rn.freeMem();
		}
	}
	else {
		map<float,Grid> grids;
		grids = objRaster.define_grids(rows, cols, xIntervals, yIntervals, biomass, friction);

		if(optValidation == 2)
			grids_to_validate = objRaster.getTotalGrids();
		else if(optValidation == 1)
			grids_to_validate = 1;

		//bestInfo << "X, Y, Size, Biomass, Cost" << endl;
		string validac;
		if(optValidation == 1)
			validac = "Best-candidate search";
		else if(optValidation == 2)
			validac = "Candidates where relation >= average validation";
		else if(optValidation == 3)
			validac = "Custom validation";
		if(algorithm == 'a')
			bestInfo << "A*,";
		else if(algorithm == 't')
			bestInfo << "Depth Search";
		bestInfo << demanda << "," << hName << "," << region << "," << validac << "," << grids_to_validate << "," << export_points << endl;
		costDistance d;
		d.COL = cols;
		d.ROW = rows;

		int cont = 1;
		float bestCost = 0, bestxMin, bestxMax, bestyMin, bestyMax;
		int bestX, bestY;

		map <float, points_export> mapa_puntos;

		while(cont <= grids_to_validate) {
			coords.open("coords.txt");
			cout << "\n\n" << endl;
			centroid = objRaster.find_centroid(grids, biomass, friction);
			if(!grids.empty())
				grids.erase(--grids.end());

			cout << "No. " << cont << " / " << grids_to_validate << endl;
			//centroid.x = 49; centroid.y = 93;
			//cout << "Source = " << centroid.x << ", " << centroid.y << endl;
			info << centroid.x << ", " << centroid.y << ",";
			coords << centroid.y << " " << centroid.x;
			coords.close();
			objRaster.reproject_coords(map_biomass);

			clock_t begin_cd = clock();
			d.inicio_cost_distance(friction, centroid.x, centroid.y, biomass, objRaster.getIntervals(), objRaster.getXMin(), objRaster.getXMax(), objRaster.getYMin(), objRaster.getYMax(), objRaster.getProjection());
			//cout << centroid.x << ", " << centroid.y << " - " << objRaster.getXMin() << " - " << objRaster.getXMax() << " - " << objRaster.getYMin() << " - " << objRaster.getYMax() << endl;
			//d.inicio_cost_distance(friction, centroid.x, centroid.y, biomass, objRaster.getIntervals(), 0, 498, 0, 256, objRaster.getProjection());
			clock_t end_cd = clock();
			double elapsed_secs_cd = double(end_cd - begin_cd) / CLOCKS_PER_SEC;

			cout << "Cost Distance time = " << elapsed_secs_cd << " secs." << endl;

			//objRaster.matrix_to_tiff(d.output_raster, rows, cols, "", 0, "", "cost_distance");

			//exit(0);

			switch(algorithm) {
				case 'B': //Binary search
				case 'b': {
					string algName = "Breadth_First_Search";
					Tree rn;
					rn.COL = cols;
					rn.ROW = rows;
					clock_t begin2 = clock();
					if(optValidation == 1) {
						bestInfo << centroid.x << ", " << centroid.y << ",";
						rn.inicio_rutas(biomass, d.output_raster, centroid.x, centroid.y, demanda, bestInfo, heuristic);
					}
					else
						rn.inicio_rutas(biomass, d.output_raster, centroid.x, centroid.y, demanda, info, heuristic);
					clock_t end2 = clock();
					double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
					cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

					if (optValidation == 1) {
						clock_t begin3 = clock();
						cout << "Image creation..." << endl;
						objRaster.write_image(rn.matrix_path, rows, cols, hName, demanda, region, algName);
						cout << "Tiff path: " << rn.matrix_path << endl;
						objRaster.matrix_to_tiff(outPath, rn.matrix_path, rows, cols, hName, demanda, region, algName);
						clock_t end3 = clock();
						double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
						cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
						exit(0);
					}

					if(export_points == 1){
						if(rn.cost > bestCost) {
							bestCost = rn.cost;
							bestX = rn.x;
							bestY = rn.y;
							bestxMin = objRaster.getXMin();
							bestxMax = objRaster.getXMax();
							bestyMin = objRaster.getYMin();
							bestyMax = objRaster.getYMax();
						}
					}else{
						points_export pp; pp.x = centroid.x; pp.y = centroid.y; pp.xMin = objRaster.getXMin(); pp.xMax = objRaster.getXMax(); pp.yMin = objRaster.getYMin(); pp.yMax = objRaster.getYMax();
						mapa_puntos.insert(make_pair(rn.cost, pp));
					}

					d.freeMem();
					rn.freeMem();
					break;
				}

				case 'A': //A* search
				case 'a': {
					Explore e;
					string algName = "AStar";
					e.COL = cols;
					e.ROW = rows;
					e.inicio(biomass);
					clock_t begin2 = clock();
					if(optValidation == 1) {
						bestInfo << centroid.x << ", " << centroid.y << ",";
						e.explore(d.output_raster, centroid.x, centroid.y, demanda, bestInfo, heuristic);
					}
					else
						e.explore(d.output_raster, centroid.x, centroid.y, demanda, info, heuristic);
					clock_t end2 = clock();
					double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
					cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;

					if (optValidation == 1) {
						clock_t begin3 = clock();
                        cout << "Tiff path: " << e.matrix_path << endl;
						objRaster.write_image(e.matrix_path, rows, cols, hName, demanda, region, algName);
						objRaster.matrix_to_tiff(outPath, e.matrix_path, rows, cols, hName, demanda, region, algName);
						clock_t end3 = clock();
						double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
						cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
						exit(0);
					}

					if(export_points == 1){
						if(e.cost > bestCost) {
							bestCost = e.cost;
							bestX = e.X;
							bestY = e.Y;
							bestxMin = objRaster.getXMin();
							bestxMax = objRaster.getXMax();
							bestyMin = objRaster.getYMin();
							bestyMax = objRaster.getYMax();
						}
					}else{
						points_export pp; pp.x = centroid.x; pp.y = centroid.y; pp.xMin = objRaster.getXMin(); pp.xMax = objRaster.getXMax(); pp.yMin = objRaster.getYMin(); pp.yMax = objRaster.getYMax();
						mapa_puntos.insert(make_pair(e.cost, pp));
					}

					e.freeMem();
					d.freeMem();
					break;
				}

				case 'D': //Dijkstra search
				case 'd': {
					Graph gr;
					gr.COL = cols;
					gr.ROW = rows;
					clock_t begin2 = clock();
					gr.dijkstra_inicio(biomass, d.output_raster, centroid.x, centroid.y, demanda, heuristic);
					clock_t end2 = clock();
					double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
					cout << "Dijkstra time = " << elapsed_secs2 << " secs." << endl;

					/*clock_t begin3 = clock();
					objRaster.matrix_to_tiff(gr.matrix_path, rows, cols);
					clock_t end3 = clock();
					double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
					cout << "Matrix to TIFF = " << elapsed_secs3 << endl;*/
					break;
				}

				case 'F': //Bellman-Ford search
				case 'f': {
					Bellmanford bf;
					bf.COL = cols;
					bf.ROW = rows;
					clock_t begin2 = clock();
					bf.bellford_start(biomass, d.output_raster, centroid.x, centroid.y, demanda, heuristic);
					clock_t end2 = clock();
					double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
					cout << "Bellman-Ford time = " << elapsed_secs2 << " secs." << endl;

					/*clock_t begin3 = clock();
					objRaster.matrix_to_tiff(bf.matrix_path, rows, cols);
					clock_t end3 = clock();
					double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
					cout << "Matrix to TIFF = " << elapsed_secs3 << endl;*/
					break;
				}
				default:
					exit(0);
					break;
			}
			cont++;
		}

		cout << endl;

		map <float, points_export> :: reverse_iterator itr;

		int cont_puntos = 1;
		if(export_points == 1){
			cout << "*** Best point ***" << endl;
			coords.open("coords.txt");
			coords << bestY << " " << bestX;
			coords.close();
			objRaster.reproject_coords(map_biomass);
			//cout << "Source = " << bestX << ", " << bestY << endl;
			bestInfo << bestX << ", " << bestY << ",";
			if(algorithm == 'a' || algorithm == 'A') {
				d.inicio_cost_distance(friction, bestX, bestY, biomass, objRaster.getIntervals(), bestxMin, bestxMax, bestyMin, bestyMax, objRaster.getProjection());
				Explore e;
				string algName = "AStar";
				e.COL = cols;
				e.ROW = rows;
				e.inicio(biomass);
				clock_t begin2 = clock();
				e.explore(d.output_raster, bestX, bestY, demanda, bestInfo, heuristic);
				clock_t end2 = clock();
				double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
				cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;
				clock_t begin3 = clock();
				objRaster.write_image(e.matrix_path, rows, cols, hName, demanda, region, algName);
				objRaster.matrix_to_tiff(outPath, e.matrix_path, rows, cols, hName, demanda, region, algName);
				clock_t end3 = clock();
				double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
				cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
				e.freeMem();
			}
			else if(algorithm == 'b' || algorithm == 'B') {
				d.inicio_cost_distance(friction, bestX, bestY, biomass, objRaster.getIntervals(), bestxMin, bestxMax, bestyMin, bestyMax, objRaster.getProjection());
				string algName = "Breadth_First_Search";
				Tree rn;
				rn.COL = cols;
				rn.ROW = rows;
				clock_t begin2 = clock();
				rn.inicio_rutas(biomass, d.output_raster, bestX, bestY, demanda, bestInfo, heuristic);
				clock_t end2 = clock();
				double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
				cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

				clock_t begin3 = clock();
				cout << "Image creation..." << endl;
				objRaster.write_image(rn.matrix_path, rows, cols, hName, demanda, region, algName);
				objRaster.matrix_to_tiff(outPath, rn.matrix_path, rows, cols, hName, demanda, region, algName);
				clock_t end3 = clock();
				double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
				cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
				rn.freeMem();
			}

		}else{
			for(itr = mapa_puntos.rbegin(); itr != mapa_puntos.rend(); itr++){
				if(cont_puntos <= export_points){
					cout << "Best points *** " << cont_puntos << "***" << endl;
					coords.open("coords.txt");
					coords << itr->second.y << " " << itr->second.x;
					coords.close();
					objRaster.reproject_coords(map_biomass);
					//cout << "Source = " << itr->second.x << ", " << itr->second.y << endl;
					bestInfo << itr->second.x << ", " << itr->second.y << ",";
					if(algorithm == 'a' || algorithm == 'A') {
						d.inicio_cost_distance(friction, itr->second.x, itr->second.y, biomass, objRaster.getIntervals(), itr->second.xMin, itr->second.xMax, itr->second.yMin, itr->second.yMax, objRaster.getProjection());
						Explore e;
						stringstream s;
						s << cont_puntos << "_A_Star";
						string algName = s.str();
						e.COL = cols;
						e.ROW = rows;
						e.inicio(biomass);
						clock_t begin2 = clock();
						e.explore(d.output_raster, itr->second.x, itr->second.y, demanda, bestInfo, heuristic);
						clock_t end2 = clock();
						double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
						cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;
						clock_t begin3 = clock();
						objRaster.write_image(e.matrix_path, rows, cols, hName, demanda, region, algName);
						objRaster.matrix_to_tiff(outPath, e.matrix_path, rows, cols, hName, demanda, region, algName);
						clock_t end3 = clock();
						double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
						cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
						e.freeMem();
					}
					else if(algorithm == 'b' || algorithm == 'B') {
						d.inicio_cost_distance(friction, itr->second.x, itr->second.y, biomass, objRaster.getIntervals(), itr->second.xMin, itr->second.xMax, itr->second.yMin, itr->second.yMax, objRaster.getProjection());
						stringstream s;
						s << cont_puntos << "_Breadth_First_Search";					string algName = s.str();
						Tree rn;
						rn.COL = cols;
						rn.ROW = rows;
						clock_t begin2 = clock();
						rn.inicio_rutas(biomass, d.output_raster, itr->second.x, itr->second.y, demanda, bestInfo, heuristic);
						clock_t end2 = clock();
                    double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
						cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

						clock_t begin3 = clock();
						cout << "Image creation..." << endl;
						objRaster.write_image(rn.matrix_path, rows, cols, hName, demanda, region, algName);
						objRaster.matrix_to_tiff(outPath, rn.matrix_path, rows, cols, hName, demanda, region, algName);
						clock_t end3 = clock();
						double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
						cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
						rn.freeMem();
					}

					cont_puntos++;
				}else{
					break;
				}
			}
		}
	}
	info.close();
	bestInfo.close();
}

void parseParameters(int argc, const char** argv){
    try {

        // Define the command line object, and insert a message
        // that describes the program. The "Command description message"
        // is printed last in the help text. The second argument is the
        // delimiter (usually space) and the last one is the version number.
        // The CmdLine object parses the argv array based on the Arg objects
        // that it contains.
        TCLAP::CmdLine cmd("Command description message", ' ', "1");

        // Define a value argument and add it to the command line.
        // A value arg defines a flag and a type of value that it expects,
        // such as "-n Bishop".
        //TCLAP::ValueArg<std::string> nameArg("n","name","Name to print",true,"homer","string");
        TCLAP::ValueArg<std::string> biomass("b","biomass","Absolute path to biomass_map.tif",true,"/path/to/image.tif","string");
        TCLAP::ValueArg<std::string> friction("f","friction","Absolute path to friction_map.tif",true,"/path/to/image.tif","string");
        TCLAP::ValueArg<std::string> algor("a","algorithm","Searching algorithm",true,"A","char");
        TCLAP::ValueArg<std::string> stop("s","demand","Biomass demand",true,"5000","float");
        TCLAP::ValueArg<std::string> watts("w","watts","Watts demand",true,"20","float");
        TCLAP::ValueArg<std::string> humedad("i","humidity","Humidity content (%)",false,"40","float");
        TCLAP::ValueArg<std::string> merma("l","loss","Percentage of expected loss in plant (%)",false,"30","float");
        TCLAP::ValueArg<std::string> produccion("p","production","Annual percentage of production / operation of plant (%)",false,"20","float");
        TCLAP::ValueArg<std::string> heur("u","heuristic","Searching heuristic",false,"d","char");
        TCLAP::ValueArg<std::string> reg("r","region","Name of the region/country",false,"Haiti","string");
        TCLAP::ValueArg<std::string> validation("v","validation","Validation option",true,"1","int");
        TCLAP::ValueArg<std::string> grids_validate("g","grids_to_validate","Number of grids to validate",false,"50","int");
        TCLAP::ValueArg<std::string> export_p("o","export_points","Export a number of points",false,"4","int");
        TCLAP::ValueArg<std::string> npa("n","npa","Map of Natural Protected Areas (NPA)",false,"/path/to/npa_map.tif","int");
        TCLAP::ValueArg<std::string> waterbody("m","waterbody","Map of Water bodies",false,"/path/to/water_bodies_map.tif","int");
        TCLAP::ValueArg<std::string> usable("e","usable","Biomass' info represents usable biomass",true,"y/n","char");
        // Add the argument nameArg to the CmdLine object. The CmdLine object
        // uses this Arg to parse the command line.
        cmd.xorAdd(stop, watts);
        cmd.add(biomass);
        cmd.add(friction);
        cmd.add(algor);
        cmd.add(heur);
        cmd.add(reg);
        cmd.add(validation);
        cmd.add(grids_validate);
        cmd.add(humedad);
        cmd.add(merma);
        cmd.add(produccion);
        cmd.add(export_p);
        cmd.add(npa);
        cmd.add(waterbody);
        cmd.add(usable);

        // Parse the argv array.
        cmd.parse( argc, argv );

        // Get the value parsed by each arg.
        map_biomass = biomass.getValue();
        map_friction = friction.getValue();
        string algoritmo = algor.getValue();
        algorithm = algoritmo[0];
        string heuristic2 = heur.getValue();
        heuristic = heuristic2[0];
        region = reg.getValue();
        string validacion = validation.getValue();
        optValidation = atoi(validacion.c_str());
        string grids_a_validar = grids_validate.getValue();
        grids_to_validate = atoi(grids_a_validar.c_str());
        string exp = export_p.getValue();
        export_points = atoi(exp.c_str());
        string demand;
        string usa = usable.getValue();
        is_usable = usa[0];

        if(optValidation > 4 || optValidation < 1) {
            cerr << "Please verify the validation option. (-v):\n 1 -- Best-candidate search.\n 2 -- Candidates where relation >= average validation.\n 3 -- Custom validation.\n 4 -- All-points validation." << endl;
            exit(0);
        }

        if(optValidation == 3 && !grids_validate.isSet()) {
            cerr << "Please indicate the number of grids to validate. (-g)." << endl;
            exit(0);
        }

        if(optValidation == 3 && grids_to_validate == 0) {
            cerr << "Please indicate a positive number of grids to validate. (-g)." << endl;
            exit(0);
        }

        if(optValidation == 2 || optValidation == 3){
            if(export_p.isSet() && export_points <= 0){
                cerr << "Please select a positive number for points to export. (-o)" << endl;
                exit(0);
            }else if(!export_p.isSet()){
                export_points = 1;
            }else if(export_p.isSet() && export_points > grids_to_validate){
                cerr << "Please select a number of points to export less than or equal to the number of grids to validate. (-o)" << endl;
                exit(0);
            }
        } else {
            export_points = 1;
        }


        if(!heur.isSet())
            heuristic = 'x';

        if(!grids_validate.isSet())
            grids_to_validate = 1;

        if(npa.isSet())
            map_npa = npa.getValue();

        if(waterbody.isSet())
            map_waterbodies = waterbody.getValue();

        if ( stop.isSet() ){
            demand = stop.getValue();
            demanda = strtof(demand.c_str(),0);
            if (demanda <= 0) {
                cerr << "Biomass demand must be greater than 0 (-s)." << endl;
                exit(0);
            }
        }
        else if ( watts.isSet() ){
            if(merma.isSet() && humedad.isSet() && produccion.isSet()){
                demand = watts.getValue();
                float w = strtof(demand.c_str(),0);
                if (w <= 0) {
                    cerr << "Watts demand must be greater than 0 (-w)." << endl;
                    exit(0);
                }
                string mer = merma.getValue();
                string hum = humedad.getValue();
                string prod = produccion.getValue();
                float humidity = strtof(hum.c_str(), 0);
                float merm = strtof(mer.c_str(),0);
                float produ = strtof(prod.c_str(),0);
                float v1 = 1 - (humidity / 100), v2 = 1 - (merm / 100), v3 = (produ / 100);
                float hpa = 8760 * v3;
                float sp = w * hpa;
                float eb = sp / v2;
                float biomasa = eb / (5 * v1);
                demanda = biomasa;
            }else{
                cerr << "Please verify that the following flags are set: -i [--humidity] -l [--loss] -p [--production]" << endl;
                exit(0);
            }
        }

    } catch (TCLAP::ArgException &e)  // catch any exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

    // Heuristic selection
    hName = "No_heuristic";
    switch(heuristic){
        case 'e':
            hName = "Euclidean";
            break;
        case 'm':
            hName = "Manhattan";
            break;
        case 'd':
            hName = "Diagonal";
            break;
        case  'a':
            hName = "More_than_median";
            break;
        case 'b':
            hName = "Best_2_nodes";
            break;
    }

}
