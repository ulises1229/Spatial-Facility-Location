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

#include "dijkstra.h" //Dijkstra
#include "bell-ford.h" //Bellman-Ford

#include <tclap/CmdLine.h>
#include "raster.h"
#include "distance.h"

using namespace std;

// Variable declaration
struct points_export{
	int x, y, xMin, xMax, yMin, yMax;
};

// TODO: Evaluate which variables can be local and passed as argument instead of global.
char is_usable;
string map_biomass, map_friction, hName, region, map_npa, map_waterbodies;
char algorithm, heuristic;
int demanda, optValidation, gridsToValidate, exportPoints, rows = 0, cols = 0;


// TODO: Use a common class for all structures
Point2D centroid;

float** biomass;
float** friction;

// Object definition
Raster objRaster;

// Create files to export info
ofstream info, bestInfo, coords;

string path, outPath;



// Methods declaration
void parseParameters(int argc, const char** argv);
void runValidation4();
void runCommonValidation(CostDistance objDist, map<float,Grid> grids);

int main(int argc, const char** argv){

	//parse command-line parameters
    parseParameters(argc, argv);

	// Import biomass and friction
    cout << "-------------------------------------------------------"<< endl;
    cout << "Importing data ... " << endl;

    //FIXME:  add a directive to detect OS and use a relative path
    path = "/Users/ulisesolivares2/Documents/GitHub/PowerPlantOptimization/src/input/"; // MACBOOK
    outPath = "/Users/ulisesolivares2/Documents/GitHub/PowerPlantOptimization/src/output/";

	map_biomass =  path + map_biomass;
	map_friction = path + map_friction;

	clock_t begin = clock();    // start timer

	// Import input maps
	biomass = objRaster.importRaster(map_biomass, true);
	friction = objRaster.importRaster(map_friction, false);

	// In case of a non-supported projection
	//cout << objRaster.epsg << endl;
	//objRaster.reproject_coords(map_biomass);

	clock_t end = clock();
	rows = objRaster.getRows();
	cols = objRaster.getCols();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Total importation time: " << elapsed_secs << " secs." << endl;
    cout << "-------------------------------------------------------"<< endl;

	ostringstream ss, bestSs;
	ss << "centroids_" << demanda << "_" << hName << ".csv";
	bestSs << "Exported_points.csv";


	//string fileName = "puntos_" + stop + ".csv"

	info.open(ss.str().c_str());
	info << "X, Y, Size, Biomass, Cost" << endl;
	bestInfo.open(bestSs.str().c_str());

	// Verify if there is a map of Natural Protected Areas
	if(map_npa != "") {
		float** npa_matrix = objRaster.importRaster(map_npa, false);
		objRaster.check_npa(npa_matrix, biomass);
	}

    // Verify if there is a map of Water bodies
	if(map_waterbodies != "") {
		float** water_matrix = objRaster.importRaster(map_waterbodies, false);
		objRaster.check_waterbodies(water_matrix, biomass);
	}

	// Define the number of grids for each axis
	int sizeX = 0, sizeY = 0;
    objRaster.defineGridSize(demanda, sizeX, sizeY);

    map<float,Grid> grids;
    string validation; //really necessary?

    // CostDistance object
    CostDistance objDist(cols, rows);

    // Write in the CSV file
    switch(algorithm){
        case 'a':
            bestInfo << "A*,";
            break;

        case 't':
            bestInfo << "Depth Search";
            break;
    }

    // write information to .CSV file
    bestInfo << demanda << "," << hName << "," << region << "," << validation << "," << gridsToValidate << "," << exportPoints << endl;

    // Obatain all girds in sorted by key summation(biomass / firction)
    grids = objRaster.defineGrids(rows, cols, sizeX, sizeY, biomass, friction);

    switch (optValidation){
	    case 1: // Run in the best grid
            gridsToValidate = 1;
            validation = "Best-candidate search";
            runCommonValidation(objDist, grids);
	        break;
	    case 2: // Run the optimization algorithm for a set of grids
            gridsToValidate = objRaster.getTotalGrids();
            validation = "Candidates where relation >= average validation";
            runCommonValidation(objDist, grids);
            break;
	    case 3: // Run the optimization algorithm for a custom number of grids
            validation = "Custom validation";
            runCommonValidation(objDist, grids);

            break;
	    case 4: // Global validation: Run all valid grids
            runValidation4();
	        break;
	}

	info.close();
	bestInfo.close();
}




void runValidation4(){
    CostDistance objDist(cols, rows);

    float bestCost = 0, bestxMin, bestxMax, bestyMin, bestyMax;
    int bestX, bestY, cont = 1;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            centroid.x = i; centroid.y = j;
            if (biomass[i][j] > 0) {
                cout << biomass[i][j] << endl;
                coords.open("coords.txt");
                cout << "Power Plant index: " << centroid.x << ", " << centroid.y << endl;
                cout << "No. " << cont << " / " << objRaster.getValidPoints() << endl;
                coords << centroid.y << " " << centroid.x;
                coords.close();
                objRaster.reproject_coords(map_biomass);
                clock_t begin_cd = clock();

                objDist.inicio_cost_distance(friction, centroid.x, centroid.y, biomass, objRaster.getIntervals(), i - 80, i + 80, j - 80, j + 80, objRaster.getProjection());


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
                        rn.inicio_rutas(biomass, objDist.getOutputRaster(), centroid.x, centroid.y, demanda, info, heuristic);
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

                        objDist.freeMem();
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
                        e.explore(objDist.getOutputRaster(), centroid.x, centroid.y, demanda, info, heuristic);
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
                        objDist.freeMem();
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

    // FIXME: use a switch
    if(algorithm == 'a' || algorithm == 'A') {
        objDist.inicio_cost_distance(friction, bestX, bestY, biomass, objRaster.getIntervals(), bestxMin, bestxMax, bestyMin, bestyMax, objRaster.getProjection());
        Explore e;
        string algName = "AStar";
        e.COL = cols;
        e.ROW = rows;
        e.inicio(biomass);
        clock_t begin2 = clock();
        e.explore(objDist.getOutputRaster(), bestX, bestY, demanda, bestInfo, heuristic);
        clock_t end2 = clock();
        double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
        cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;
        clock_t begin3 = clock();
        objRaster.writeImage(e.matrix_path, rows, cols, hName, demanda, region, algName);
        objRaster.exportTiff(outPath, e.matrix_path, rows, cols, hName, demanda, region, algName);
        clock_t end3 = clock();
        double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
        cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
        e.freeMem();
    }
    else if(algorithm == 'b' || algorithm == 'B') {
        objDist.inicio_cost_distance(friction, bestX, bestY, biomass, objRaster.getIntervals(), bestxMin, bestxMax, bestyMin, bestyMax, objRaster.getProjection());
        string algName = "Breadth_First_Search";
        Tree rn;
        rn.COL = cols;
        rn.ROW = rows;
        clock_t begin2 = clock();
        rn.inicio_rutas(biomass, objDist.getOutputRaster(), bestX, bestY, demanda, bestInfo, heuristic);
        clock_t end2 = clock();
        double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
        cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

        clock_t begin3 = clock();
        cout << "Image creation..." << endl;
        objRaster.writeImage(rn.matrix_path, rows, cols, hName, demanda, region, algName);
        objRaster.exportTiff(outPath, rn.matrix_path, rows, cols, hName, demanda, region, algName);
        clock_t end3 = clock();
        double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
        cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
        rn.freeMem();
    }
}

void runCommonValidation(CostDistance objDist, map<float,Grid> grids){

    int count = 1;
    float bestCost = 0, bestxMin, bestxMax, bestyMin, bestyMax;
    int bestX, bestY;

    map <float, points_export> mapa_puntos;

    // TODO: Use a new method from here
    while(count <= gridsToValidate) {
        coords.open("coords.txt");

        //cout << "\n\n" << endl;
        // TODO:Insert EM algorithm here
        objRaster.runEM(grids, biomass, friction, outPath);


        centroid = objRaster.findCentroid(grids, biomass, friction);
        if(!grids.empty())
            grids.erase(--grids.end());

        cout << "No. "  << " / " << gridsToValidate << endl;
        //centroid.x = 49; centroid.y = 93;
        //cout << "Source = " << centroid.x << ", " << centroid.y << endl;
        info << centroid.x << ", " << centroid.y << ",";
        coords << centroid.y << " " << centroid.x;
        coords.close();
        objRaster.reproject_coords(map_biomass);

        clock_t begin_cd = clock();
        objDist.inicio_cost_distance(friction, centroid.x, centroid.y, biomass, objRaster.getIntervals(), objRaster.getXMin(), objRaster.getXMax(), objRaster.getYMin(), objRaster.getYMax(), objRaster.getProjection());
        //cout << centroid.x << ", " << centroid.y << " - " << objRaster.getXMin() << " - " << objRaster.getXMax() << " - " << objRaster.getYMin() << " - " << objRaster.getYMax() << endl;
        //objDist.inicio_cost_distance(friction, centroid.x, centroid.y, biomass, objRaster.getIntervals(), 0, 498, 0, 256, objRaster.getProjection());
        clock_t end_cd = clock();
        double elapsed_secs_cd = double(end_cd - begin_cd) / CLOCKS_PER_SEC;

        cout << count << "Cost Distance time = " << elapsed_secs_cd << " secs." << endl;

        //objRaster.exportTiff(objDist.getOutputRaster(), rows, cols, "", 0, "", "cost_distance");

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
                    rn.inicio_rutas(biomass, objDist.getOutputRaster(), centroid.x, centroid.y, demanda, bestInfo, heuristic);
                }
                else
                    rn.inicio_rutas(biomass, objDist.getOutputRaster(), centroid.x, centroid.y, demanda, info, heuristic);
                clock_t end2 = clock();
                double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
                cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

                if (optValidation == 1) {
                    clock_t begin3 = clock();
                    cout << "---------------------------" << endl;
                    cout << "Image creation..." << endl;
                    objRaster.writeImage(rn.matrix_path, rows, cols, hName, demanda, region, algName);
                    objRaster.exportTiff(outPath, rn.matrix_path, rows, cols, hName, demanda, region, algName);
                    clock_t end3 = clock();
                    double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
                    cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
                    exit(0);
                }

                if(exportPoints == 1){
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

                objDist.freeMem();
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
                    e.explore(objDist.getOutputRaster(), centroid.x, centroid.y, demanda, bestInfo, heuristic);
                }
                else
                    e.explore(objDist.getOutputRaster(), centroid.x, centroid.y, demanda, info, heuristic);
                clock_t end2 = clock();
                double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
                cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;

                if (optValidation == 1) {
                    clock_t begin3 = clock();
                    //cout << "Tiff path: " << e.matrix_path << endl;
                    objRaster.writeImage(e.matrix_path, rows, cols, hName, demanda, region, algName);
                    objRaster.exportTiff(outPath, e.matrix_path, rows, cols, hName, demanda, region, algName);
                    clock_t end3 = clock();
                    double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
                    cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
                    exit(0);
                }

                if(exportPoints == 1){
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
                objDist.freeMem();
                break;
            }

            case 'D': //Dijkstra search
            case 'd': {
                Dijkstra objDijkstra;
                objDijkstra.COL = cols;
                objDijkstra.ROW = rows;
                clock_t begin2 = clock();
                objDijkstra.initDijkstra(biomass, objDist.getOutputRaster(), centroid.x, centroid.y, demanda, heuristic);
                clock_t end2 = clock();
                double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
                cout << "Dijkstra time = " << elapsed_secs2 << " secs." << endl;

                /*clock_t begin3 = clock();
                objRaster.exportTiff(objDijkstra.matrix_path, rows, cols);
                clock_t end3 = clock();
                double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
                cout << "Matrix to TIFF = " << elapsed_secs3 << endl;*/
                break;
            }

            case 'F': //Bellman-Ford search
            case 'f': {
                BellmanFord bf;
                bf.COL = cols;
                bf.ROW = rows;
                clock_t begin2 = clock();
                bf.bellford_start(biomass, objDist.getOutputRaster(), centroid.x, centroid.y, demanda, heuristic);
                clock_t end2 = clock();
                double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
                cout << "Bellman-Ford time = " << elapsed_secs2 << " secs." << endl;

                /*clock_t begin3 = clock();
                objRaster.exportTiff(bf.matrix_path, rows, cols);
                clock_t end3 = clock();
                double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
                cout << "Matrix to TIFF = " << elapsed_secs3 << endl;*/
                break;
            }
            default:
                exit(0);
                break;
        }
        count++;
    }
    map <float, points_export> :: reverse_iterator itr;

    int cont_puntos = 1;
    if(exportPoints == 1){
        cout << "*** Best point ***" << endl;
        coords.open("coords.txt");
        coords << bestY << " " << bestX;
        coords.close();
        objRaster.reproject_coords(map_biomass);
        //cout << "Source = " << bestX << ", " << bestY << endl;
        bestInfo << bestX << ", " << bestY << ",";
        if(algorithm == 'a' || algorithm == 'A') {
            objDist.inicio_cost_distance(friction, bestX, bestY, biomass, objRaster.getIntervals(), bestxMin, bestxMax, bestyMin, bestyMax, objRaster.getProjection());
            Explore e;
            string algName = "AStar";
            e.COL = cols;
            e.ROW = rows;
            e.inicio(biomass);
            clock_t begin2 = clock();
            e.explore(objDist.getOutputRaster(), bestX, bestY, demanda, bestInfo, heuristic);
            clock_t end2 = clock();
            double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
            cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;
            clock_t begin3 = clock();
            objRaster.writeImage(e.matrix_path, rows, cols, hName, demanda, region, algName);
            objRaster.exportTiff(outPath, e.matrix_path, rows, cols, hName, demanda, region, algName);
            clock_t end3 = clock();
            double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
            cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
            e.freeMem();
        }
        else if(algorithm == 'b' || algorithm == 'B') {
            objDist.inicio_cost_distance(friction, bestX, bestY, biomass, objRaster.getIntervals(), bestxMin, bestxMax, bestyMin, bestyMax, objRaster.getProjection());
            string algName = "Breadth_First_Search";
            Tree rn;
            rn.COL = cols;
            rn.ROW = rows;
            clock_t begin2 = clock();
            rn.inicio_rutas(biomass, objDist.getOutputRaster(), bestX, bestY, demanda, bestInfo, heuristic);
            clock_t end2 = clock();
            double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
            cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

            clock_t begin3 = clock();
            cout << "Image creation..." << endl;
            objRaster.writeImage(rn.matrix_path, rows, cols, hName, demanda, region, algName);
            objRaster.exportTiff(outPath, rn.matrix_path, rows, cols, hName, demanda, region, algName);
            clock_t end3 = clock();
            double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
            cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
            rn.freeMem();
        }

    }else{
        for(itr = mapa_puntos.rbegin(); itr != mapa_puntos.rend(); itr++){
            if(cont_puntos <= exportPoints){
                cout << "Best points *** " << cont_puntos << "***" << endl;
                coords.open("coords.txt");
                coords << itr->second.y << " " << itr->second.x;
                coords.close();
                objRaster.reproject_coords(map_biomass);
                //cout << "Source = " << itr->second.x << ", " << itr->second.y << endl;
                bestInfo << itr->second.x << ", " << itr->second.y << ",";
                if(algorithm == 'a' || algorithm == 'A') {
                    objDist.inicio_cost_distance(friction, itr->second.x, itr->second.y, biomass, objRaster.getIntervals(), itr->second.xMin, itr->second.xMax, itr->second.yMin, itr->second.yMax, objRaster.getProjection());
                    Explore e;
                    stringstream s;
                    s << cont_puntos << "_A_Star";
                    string algName = s.str();
                    e.COL = cols;
                    e.ROW = rows;
                    e.inicio(biomass);
                    clock_t begin2 = clock();
                    e.explore(objDist.getOutputRaster(), itr->second.x, itr->second.y, demanda, bestInfo, heuristic);
                    clock_t end2 = clock();
                    double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
                    cout << "A* Search time = " << elapsed_secs2 << " secs." << endl;
                    clock_t begin3 = clock();
                    objRaster.writeImage(e.matrix_path, rows, cols, hName, demanda, region, algName);
                    objRaster.exportTiff(outPath, e.matrix_path, rows, cols, hName, demanda, region, algName);
                    clock_t end3 = clock();
                    double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
                    cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;
                    e.freeMem();
                }
                else if(algorithm == 'b' || algorithm == 'B') {
                    objDist.inicio_cost_distance(friction, itr->second.x, itr->second.y, biomass, objRaster.getIntervals(), itr->second.xMin, itr->second.xMax, itr->second.yMin, itr->second.yMax, objRaster.getProjection());
                    stringstream s;
                    s << cont_puntos << "_Breadth_First_Search";					string algName = s.str();
                    Tree rn;
                    rn.COL = cols;
                    rn.ROW = rows;
                    clock_t begin2 = clock();
                    rn.inicio_rutas(biomass, objDist.getOutputRaster(), itr->second.x, itr->second.y, demanda, bestInfo, heuristic);
                    clock_t end2 = clock();
                    double elapsed_secs2 = double(end2 - begin2) / CLOCKS_PER_SEC;
                    cout << "Nodes time = " << elapsed_secs2 << " secs." << "\n\n";

                    clock_t begin3 = clock();
                    cout << "Image creation..." << endl;
                    objRaster.writeImage(rn.matrix_path, rows, cols, hName, demanda, region, algName);
                    objRaster.exportTiff(outPath, rn.matrix_path, rows, cols, hName, demanda, region, algName);
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
        TCLAP::ValueArg<std::string> grids_validate("g","gridsToValidate","Number of grids to validate",false,"50","int");
        TCLAP::ValueArg<std::string> export_p("o","exportPoints","Export a number of points",false,"4","int");
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
        gridsToValidate = atoi(grids_a_validar.c_str());
        string exp = export_p.getValue();
        exportPoints = atoi(exp.c_str());
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

        if(optValidation == 3 && gridsToValidate == 0) {
            cerr << "Please indicate a positive number of grids to validate. (-g)." << endl;
            exit(0);
        }

        if(optValidation == 2 || optValidation == 3){
            if(export_p.isSet() && exportPoints <= 0){
                cerr << "Please select a positive number for points to export. (-o)" << endl;
                exit(0);
            }else if(!export_p.isSet()){
                exportPoints = 1;
            }else if(export_p.isSet() && exportPoints > gridsToValidate){
                cerr << "Please select a number of points to export less than or equal to the number of grids to validate. (-o)" << endl;
                exit(0);
            }
        } else {
            exportPoints = 1;
        }


        if(!heur.isSet())
            heuristic = 'x';

        if(!grids_validate.isSet())
            gridsToValidate = 1;

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
