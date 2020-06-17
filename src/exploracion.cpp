/*
 * exploracion.cpp
 *
 *  Created on: 01/09/2017
 *      Author: lanase
 */

#include "exploracion.h"

using namespace std;


bool Explore::isValid(int row, int col){
    // Returns true if row number and column number
    // is in range
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}


void Explore::printPath(stack<Pair> Path, ofstream& info){
	float costTot = 0;
	this->matrix_path = new float*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->matrix_path[i] = new float[COL];
		for(int j = 0; j < COL ; j++){
			matrix_path[i][j] = 0;
		}
	}

	int s = Path.size();
	float n=0.0;
	while (!Path.empty()){
		pair<int,int> p = Path.top();
		Path.pop();
		printf("-> (%d,%d) ",p.first,p.second);
		n += biomass[p.first][p.second];
		cout << biomass[p.first][p.second] << " " <<  friction[p.first][p.second] << " " << n << endl;
		matrix_path[p.first][p.second] = 1;
		costTot += friction[p.first][p.second];
	}
	cout << "Size: " << s << endl;
	cout << "Sum: " << n << endl;
	cout << "Total Cost: " << costTot << endl;
	info << s << ",";
	info << n << ",";
	info  << costTot << endl;
}

bool Explore::checkFutureInsert(int x, int y, float acum, float parada){
	int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
	int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

	int xx, yy;
	for(int i = 0; i < 8; i++){
		xx = x + dx[i];
		yy = y + dy[i];
		if (isValid(xx, yy)) {
			if (closedList[xx][yy] == false && biomass[xx][yy] != -9999 ){
				return true;
			}
		}
	}
	return false;
}

void Explore::printPath2(vector<info_ruta> p, ofstream& info, Pair src){
	float biomass = 0, cost = 0, frict = 0;
	int size = 0;
	this->X = src.first; this->Y = src.second;
	this->matrix_path = new float*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->matrix_path[i] = new float[COL];
		for(int j = 0; j < COL ; j++){
			matrix_path[i][j] = -9999;
		}
	}
	int xMin = INT_MAX, yMin = INT_MAX;
	int xMax = 0, yMax = 0;

	for (vector<info_ruta>::iterator it = p.begin(); it != p.end(); it++) {
		if(it->x == src.first && it->y == src.second) {
			//matrix_path[it->x][it->y] = this->biomass[it->x][it->y];
			matrix_path[it->x][it->y] = it->friction;
		}
		else {
			matrix_path[it->x][it->y] = it->friction;
			cost += (it->biomass / it->friction);
		}
		biomass += it->biomass;
		frict += it->friction;
		size++;
		if(it->x < xMin)
			xMin = it->x;
		if(it->y < yMin)
			yMin = it->y;
		if(it->x > xMax)
			xMax = it->x;
		if(it->y > yMax)
			yMax = it->y;
		}
		cout << "Size: " << size << endl;
		cout << fixed <<  "Sum: " << biomass << endl;
		cout << "Cost (B/F): " << cost << endl;
		cout << "Cost ($): " << frict << endl;
		info << size << ",";
		info << biomass << ",";
		info  << cost << endl;
		this->cost = cost;
}




// A Function to find the shortest path between
// a given source cell to a destination cell according
// to A* Search Algorithm
void Explore::aStarSearch(Pair src, float stop, ofstream& info, char heuristic){

    int i, j;
    for (i=0; i<ROW; i++){
        for (j=0; j<COL; j++){
            cellDetails[i][j].f = FLT_MIN;
            cellDetails[i][j].g = FLT_MIN;
            cellDetails[i][j].h = FLT_MIN;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    // Initialising the parameters of the starting node
    i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    /*
     Create an open list having information as-
     <f, <i, j>>
     where f = g + h,
     and i, j are the row and column index of that cell
     Note that 0 <= i <= ROWS-1 & 0 <= j <= COLS-1
     This open list is implenented as a set of pair of pair.*/
    set<cell_info> openList;

    // Put the starting cell on the open list and set its
    // 'f' as 0
    extern char is_usable;
    cell_info co;  co.friction = friction[i][j];  co.i = i; co.j = j; co.heuristic = 0;
    if(is_usable == 'n'){
        co.biomass_acum = biomass[i][j] / 40;
        co.relation = biomass[i][j] / 40;
    }
    else{
		co.biomass_acum = biomass[i][j];
		co.relation = biomass[i][j];
    }
    openList.insert(co);

    float parada = stop;

    stack<Pair> maxCost_shortest_path;//pila con ruta final

    float acum;
    cell_info p;
    float gNew, hNew, fNew;
	int x, y;
	int dx[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
	int dy[] = {  0, 0, 1, -1, 1, -1, 1, -1 };
	float biomasa_total = 0, friction_total = 0, heuristicResult = 0, usable_b;
	vector<info_ruta> ruta;
	int pushes = 0;
    while (!openList.empty()){
    	p = *(--openList.end());//tomar el ultimo? seria el mas costoso
    	i = p.i;
		j = p.j;
		if(is_usable == 'n')
			usable_b = biomass[i][j] / 40;
		else
			usable_b = biomass[i][j];
		biomasa_total += usable_b; //biomass[i][j];
		friction_total += friction[i][j];
		ruta.push_back(info_ruta(i, j, usable_b, friction[i][j], biomasa_total, p.heuristic));
		pushes++;
        	if(biomasa_total >= parada){
        		openList.clear();
				goto finish;
        	}

        acum = p.biomass_acum;

        // Remove this vertex from the open list
        openList.erase(--openList.end());//borrar ultimo

        // Add this vertex to the open list
        closedList[i][j] = true;

       /*
        Generating all the 8 successor of this cell

            N.W   N   N.E
              \   |   /
               \  |  /
            W----Cell----E
                 / | \
               /   |  \
            S.W    S   S.E

        Cell-->Popped Cell (i, j)
        N -->  North       (i-1, j)
        S -->  South       (i+1, j)
        E -->  East        (i, j+1)
        W -->  West           (i, j-1)
        N.E--> North-East  (i-1, j+1)
        N.W--> North-West  (i-1, j-1)
        S.E--> South-East  (i+1, j+1)
        S.W--> South-West  (i+1, j-1)*/

        // To store the 'g', 'h' and 'f' of the 8 successors

		for(int k = 0; k < 8; k++){
			x = i + dx[k];
			y = j + dy[k];
			// If the successor is already on the closed
			// list or if it is blocked, then ignore it.
			// Else do the following
			if (isValid(x, y)) {
				if (closedList[x][y] == false && closedopenList[x][y] == false){
					// If it isn’t on the open list, add it to
					// the open list. Make the current square
					// the parent of this square. Record the
					// f, g, and h costs of the square cell
					//                OR
					// If it is on the open list already, check
					// to see if this path to that square is better,
					// using 'f' cost as the measure.

					//cout << cellDetails[i-1][j].f << " " << fNew << " "  << endl;
					if ( (cellDetails[x][y].f == FLT_MIN) &&  biomass[x][y] != -9999 && friction[x][y] != -9999)
					{
						if (heuristic == 'e') {
							heuristicResult = sqrt(pow((src.first - x), 2) + pow((src.second - y), 2));
						}
						else if (heuristic == 'm') {
							heuristicResult = abs(src.first - x) + abs(src.second - y);
						}
						else if(heuristic == 'd') {
							heuristicResult = max(abs(src.first - x), abs(src.second - y));
						}
						gNew = acum;
						if (heuristicResult > 0)
							hNew = biomass[x][y] / (friction[x][y] + heuristicResult);
						else
							hNew = biomass[x][y] / friction[x][y];
						if(is_usable == 'n')
							fNew = gNew + (biomass[x][y] / 40);
						else
							fNew = gNew + biomass[x][y];
						//cout << "multiples" << endl;
						if(checkFutureInsert(x, y, acum, parada)){
							co.biomass_acum = fNew; co.friction = friction[x][y]; co.relation = hNew; co.i = x; co.j = y; co.heuristic = heuristicResult;
							openList.insert(co);
							closedopenList[x][y] = true;
							// Update the details of this cell
							cellDetails[x][y].f = fNew;
							cellDetails[x][y].g = gNew;
							cellDetails[x][y].h = hNew;
							cellDetails[x][y].parent_i = i;
							cellDetails[x][y].parent_j = j;
						}
					}
				}
			}
		}
    }

    finish:
		printPath2(ruta, info, src);

    return;
}


// Driver program to test above function
void Explore::explore(float** friction, int src_X, int src_Y, float stop, ofstream& info, char heuristic){
	this->friction = new float*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->friction[i] = new float[COL];
	}



	this->cellDetails = new cell_exp*[this->ROW];
	for(int i = 0; i< ROW; ++i)
		this->cellDetails[i] = new cell_exp[COL];

	this->closedList = new bool*[this->ROW];
	this->closedopenList = new bool*[this->ROW];
		for(int i = 0; i< ROW; ++i){
			this->closedList[i] = new bool[COL];
			this->closedopenList[i] = new bool[COL];
		}

	for(int x = 0; x < ROW; x++){
		for(int y = 0; y < COL; y++){
			this->friction[x][y] = friction[x][y];
			this->closedList[x][y] = false;
			this->closedopenList[x][y] = false;
		}
	}

    // Source is the left-most bottom-most corner
    Pair src = make_pair(src_X, src_Y);
    X = src_X; Y = src_Y;
    aStarSearch(src, stop, info, heuristic);
}

void Explore::freeMem() {
	for(int m=0; m < ROW; m++){
		delete[] biomass[m];
		delete[] friction[m];
		delete[] closedList[m];
		delete[] matrix_path[m];
		delete[] closedopenList[m];
		delete[] cellDetails[m]; //****!!!!!!!!!!!!!
	}
}

void Explore::inicio(float** biomass){
	this->biomass = new float*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->biomass[i] = new float[COL];
	}

	for(int x = 0; x < ROW; x++){
		for(int y = 0; y < COL; y++){
			this->biomass[x][y] = biomass[x][y];
		}
	}
}
