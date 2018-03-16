/*
 * main.cpp
 *
 *  Created on: 17/08/2017
 *      Author: lanase
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <float.h>
#include <vector>

#include "DisplayImage.cpp"
#include "distance.h"
//#include "gdal.cpp"
#include "exploracion.h" //A*
#include "tree.h" //Nodos
#include "dijkstra.cpp" //Dijkstra
#include "bellford.cpp" //Bellman-Ford
#include <tclap/CmdLine.h>

int main(int argc, const char** argv){
	string map_biomass, map_friction, hName, region;
		char algorithm, heuristic;
		int demanda, optValidation, grids_to_validate;

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
		string demand;

		if(optValidation > 3 || optValidation < 1) {
			cerr << "Please verify the validation option. (-v):\n 1 -- Best-candidate search.\n 2 -- Candidates where relation >= average validation.\n 3 -- Custom validation." << endl;
			exit(0);
		}

		if(optValidation == 3 && !grids_validate.isSet()) {
			cerr << "Please indicate the number of grids to validate. (-g)." << endl;
			exit(0);
		}

		if(!heur.isSet())
			heuristic = 'x';

		if(!grids_validate.isSet())
			grids_to_validate = 1;

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
				float humedad = strtof(hum.c_str(),0);
				float merm = strtof(mer.c_str(),0);
				float produ = strtof(prod.c_str(),0);
				float v1 = 1 - (humedad/100), v2 = 1 - (merm / 100), v3 = (produ/100);
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


	if(heuristic == 'e')
		hName = "Euclidean";
	else if(heuristic == 'm')
		hName = "Manhattan";
	else if(heuristic == 'd')
		hName = "Diagonal";
	else if(heuristic == 'a')
		hName = "More_than_median";
	else if(heuristic == 'b')
		hName = "Best_2_nodes";
	else
		hName = "No_heuristic";

	Display_image di;

	clock_t begin = clock();
	float** biomass = di.tiff_to_matrix_gdal(map_biomass, true);
	float** friction = di.tiff_to_matrix_gdal(map_friction, false);
	clock_t end = clock();
	int rows = di.ROWS;
	int cols = di.COLS;

	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "TIFF to matrix = " << elapsed_secs << " secs." << endl;

	int xIntervals = 0, yIntervals = 0;
	di.define_intervals(demanda, xIntervals, yIntervals);

	map<float,Grid> grids;
	grids = di.define_grids(rows, cols, xIntervals, yIntervals, biomass, friction);

	if(optValidation == 2)
		grids_to_validate = di.totGridsAvg;
	else if(optValidation == 1)
		grids_to_validate = 1;

	Point2D centroid;
	ofstream info;
	ostringstream ss;
	ss << "centroids_" << demanda << "_" << hName << ".csv";
	//string fileName = "puntos_" + stop + ".csv"
	info.open(ss.str().c_str());
	info << "X, Y, Size, Biomass, Cost" << endl;

	costDistance d;
	d.COL = cols;
	d.ROW = rows;

	int cont = 1;
	float bestCost = 0, bestxMin, bestxMax, bestyMin, bestyMax;
	int bestX, bestY;

	while(cont <= grids_to_validate) {
		cout << "\n\n" << endl;
		centroid = di.find_centroid(grids, biomass, friction);
		if(!grids.empty())
			grids.erase(--grids.end());

		cout << "No. " << cont << " / " << grids_to_validate << endl;
		cout << "Source = " << centroid.x << ", " << centroid.y << endl;
		info << centroid.x << ", " << centroid.y << ",";

		clock_t begin_cd = clock();
		d.inicio_cost_distance(friction, centroid.x, centroid.y, biomass, di.intervals, di.xMin, di.xMax, di.yMin, di.yMax);
		clock_t end_cd = clock();
		double elapsed_secs_cd = double(end_cd - begin_cd) / CLOCKS_PER_SEC;

		cout << "Cost Distance time = " << elapsed_secs_cd << " secs." << endl;


		switch(algorithm) {
			case 'T': //Binary search
			case 't': {
				string algName = "Binary";
				Tree rn;
				rn.COL = cols;
				rn.ROW = rows;
				clock_t begin2 = clock();
				rn.inicio_rutas(biomass, d.output_raster, centroid.x, centroid.y, demanda, info, heuristic);
				clock_t end2 = clock();
				double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
				cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

				if (optValidation == 1) {
					clock_t begin3 = clock();
					cout << "Image creation..." << endl;
					di.write_image(rn.matrix_path, rows, cols, hName, demanda, region, algName);
					//di.matrix_to_tiff(rn.matrix_path, rows, cols);
					clock_t end3 = clock();
					double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
					cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
					exit(0);
				}

				if(rn.cost > bestCost) {
					bestCost = rn.cost;
					bestX = rn.x;
					bestY = rn.y;
					bestxMin = di.xMin;
					bestxMax = di.xMax;
					bestyMin = di.yMin;
					bestyMax = di.yMax;
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
				e.explore(d.output_raster, friction, centroid.x, centroid.y, demanda, info, heuristic);
				clock_t end2 = clock();
				double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
				cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;

				if (optValidation == 1) {
					clock_t begin3 = clock();
					di.write_image(e.matrix_path, rows, cols, hName, demanda, region, algName);
					//di.matrix_to_tiff(e.matrix_path, rows, cols);
					clock_t end3 = clock();
					double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
					cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
					exit(0);
				}

				if(e.cost > bestCost) {
					bestCost = e.cost;
					bestX = e.X;
					bestY = e.Y;
					bestxMin = di.xMin;
					bestxMax = di.xMax;
					bestyMin = di.yMin;
					bestyMax = di.yMax;
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
				di.matrix_to_tiff(gr.matrix_path, rows, cols);
				clock_t end3 = clock();
				double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
				cout << "Matrix to TIFF = " << elapsed_secs3 << endl;*/
				break;
			}

			case 'B': //Bellman-Ford search
			case 'b': {
				Bellmanford bf;
				bf.COL = cols;
				bf.ROW = rows;
				clock_t begin2 = clock();
				bf.bellford_start(biomass, d.output_raster, centroid.x, centroid.y, demanda, heuristic);
				clock_t end2 = clock();
				double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
				cout << "Bellman-Ford time = " << elapsed_secs2 << " secs." << endl;

				/*clock_t begin3 = clock();
				di.matrix_to_tiff(bf.matrix_path, rows, cols);
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
	cout << "Source = " << bestX << ", " << bestY << endl;
	if(algorithm == 'a' || algorithm == 'A') {
		d.inicio_cost_distance(friction, bestX, bestY, biomass, di.intervals, bestxMin, bestxMax, bestyMin, bestyMax);
		Explore e;
		string algName = "AStar";
		e.COL = cols;
		e.ROW = rows;
		e.inicio(biomass);
		clock_t begin2 = clock();
		e.explore(d.output_raster, friction, bestX, bestY, demanda, info, heuristic);
		clock_t end2 = clock();
		double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
		cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;
		clock_t begin3 = clock();
		di.write_image(e.matrix_path, rows, cols, hName, demanda, region, algName);
		//di.matrix_to_tiff(e.matrix_path, rows, cols);
		clock_t end3 = clock();
		double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
		cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
		e.freeMem();
	}
	else if(algorithm == 't' || algorithm == 'T') {
		d.inicio_cost_distance(friction, bestX, bestY, biomass, di.intervals, bestxMin, bestxMax, bestyMin, bestyMax);
		string algName = "Binary";
		Tree rn;
		rn.COL = cols;
		rn.ROW = rows;
		clock_t begin2 = clock();
		rn.inicio_rutas(biomass, d.output_raster, bestX, bestY, demanda, info, heuristic);
		clock_t end2 = clock();
		double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
		cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

		clock_t begin3 = clock();
		cout << "Image creation..." << endl;
		di.write_image(rn.matrix_path, rows, cols, hName, demanda, region, algName);
		//di.matrix_to_tiff(rn.matrix_path, rows, cols);
		clock_t end3 = clock();
		double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
		cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
		rn.freeMem();
	}
	info.close();
}
