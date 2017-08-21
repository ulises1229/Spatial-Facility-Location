//============================================================================
// Name        : distance.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <set>
#include <climits>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

#define ROW 6
#define COL	6


struct cell {
	int x, y;
	float distance;
	cell(int x, int y, float distance) :
			x(x), y(y), distance(distance) {}
};

// Utility method for comparing two cells
static bool operator<(const cell& a, const cell& b){
	if (a.distance == b.distance){
		if (a.x != b.x)
			return (a.x < b.x);
		else
			return (a.y < b.y);
	}
	return (a.distance < b.distance);
}

class cost_distance{
public:
	/*float cost_raster[6][6] = {
				{1,3,4,4,3,2},
				{4,6,2,3,7,6},
				{5,8,7,5,6,6},
				{1,4,5,-1,5,1},
				{4,7,5,-1,2,6},
				{1,2,2,1,3,4},
			};*/
	int ROWS, COLS;

	float** cost_raster;

	int rutas[8][8]{
		{0,0,0,0,0,0,0,0},
		{-1,0,0,1,0,1,-1,0},
		{0,0,0,0,0,0,0,0},
		{0,1,1,0,1,0,0,1},
		{0,0,0,0,0,0,0,0},
		{0,-1,1,0,1,0,0,-1},
		{0,0,0,0,0,0,0,0},
		{-1,0,0,-1,0,-1,-1,0},
	};

	float** output_raster;

	bool** active_raster;

	bool isInsideGrid(int i, int j){
		return (i >= 0 && i < ROWS && j >= 0 && j < COLS && cost_raster[i][j] >= 0);
	}


	set<cell>vecinos(int origen_x, int origen_y){
		set<cell>distancias;
		float dis[ROWS][COLS];

		// initializing distance array by INT_MAX
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++){
				dis[i][j] = INT_MAX;
			}

		// direction arrays for simplification of getting
		// neighbour
		int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
		int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

		set<cell> st;

		// insert (0, 0) cell with 0 distance
		st.insert(cell(origen_x, origen_y, 0));
		//cout << "insert en set<cell>(cell(3,3,0))" << endl;

		// initialize distance of (0, 0) with its grid value
		dis[origen_x][origen_y] = cost_raster[origen_x][origen_y];
		//cout << "dis[3][3] = " << dis[origen_x][origen_y] << "  grid[3][3] = " << grid[origen_x][origen_y] << endl;


		// get the cell with minimum distance and delete
		// it from the set
		cell k = *st.begin();
		st.erase(st.begin());
		//cout << "k.x = " << k.x << " k.y = " << k.y << " k.distance = " << k.distance <<endl;
		// looping through all neighbours
		for (int i = 0; i < 8; i++){
			int x = k.x + dx[i];
			int y = k.y + dy[i];
			cout << "k.x = " << k.x << " k.y = "<< k.y <<" x = " << x << " y = " << y << endl;
			// if not inside boundry, ignore them
			if (!isInsideGrid(x, y)){
				cout << "no inside grid" << endl;
				continue;
			}
			cout << "dentro del grid" << endl;

			if(i % 2 == 0){//par = no es diagonal
				//cout << "N" << endl;
				dis[x][y] = (dis[k.x][k.y] + cost_raster[x][y])/2;
			}
			else{
				//cout << "D" << endl;
				dis[x][y] = 1.414214*(dis[k.x][k.y] + cost_raster[x][y])/2;
			}
			distancias.insert(cell(x, y, dis[x][y]));
			active_raster[x][y] = true;
			output_raster[x][y] = dis[x][y];//son los bordes, no hay necesidad de verificar si es menor al INT_MAX
		}
		output_raster[origen_x][origen_y] = 0;


		return distancias;
	}


	void acumulados(set<cell> active_costs, int origen_x, int origen_y){
		while(!active_costs.empty()){
			set <cell> :: iterator itr;
			//cout << "#######" << endl;

			cell k = *active_costs.begin();//inicio con la distancia menor
			active_costs.erase(active_costs.begin());
			 int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
			 int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
			 cout << "************k.x = " << k.x << " k.y = " << k.y << endl;
			 /*if(k.x == 23 && k.y == 9)
				 exit(0);*/
			 //float acum_inicio = k.distance;
			 for (int i = 0; i < 8; i++){
				 int x = k.x + dx[i];
				 int y = k.y + dy[i];

				 set<float>distancias;
				 //cout << x << y << " " << active_raster[x][y]<< endl;
				 if(i % 2 != 0){// si es movimiento diagonal
					 //cout << "Diagonal" << " "<< x << ", " << y << " " << cost_raster[x][y] << endl;
					 if(isInsideGrid(x,y) && (x != origen_x || y != origen_y) && active_raster[x][y] == 0 ){
						 cout << x << y << " " << active_raster[x][y]<< endl;
						 cout << "Diagonal" << " "<< x << ", " << y << " " << cost_raster[x][y] << endl;
						 float dist=0;
						 int cont_x = k.x; int cont_y = k.y;
						 int cont_x2 = k.x; int cont_y2 = k.y;
						 int caminos = 1;
						 //set<float>distancias;
						 for(int z = 0; z < 8; z=z+2){

							 int mov_x = rutas[i][z];
							 int mov_y = rutas[i][z+1];

							 dist += (cost_raster[cont_x+mov_x][cont_y+mov_y] + cost_raster[cont_x2][cont_y2])/2;
							 //cout << mov_x << " " << mov_y << " " << "cost_raster[cont_x+mov_x][cont_y+mov_y] = "<< cost_raster[cont_x+mov_x][cont_y+mov_y]
							 //<< " cost_raster[cont_x2][cont_y2] = "<< cost_raster[cont_x2][cont_y2] << " /2 =  " << dist << endl;
							 if(caminos == 2 || caminos == 4){
								 //cout << dist+output_raster[k.x][k.y] << endl;
								 distancias.insert(dist+output_raster[k.x][k.y]);
								 cont_x = cont_x2 = k.x;
								 cont_y = cont_y2 = k.y;
								 caminos = 0;
								 dist = 0;
							 }else{
								 cont_x2 = cont_x+mov_x;
								 cont_y2 = cont_y+mov_y;
								 cont_x = cont_x+mov_x;
								 cont_y = cont_y+mov_y;
							 }
							 caminos++;
						 }
						 distancias.insert(1.414214 * ((cost_raster[k.x][k.y]+cost_raster[x][y])/2) + output_raster[k.x][k.y] );
						 float minimo = *distancias.begin();
						 //cout << minimo << endl;
						 if(minimo < output_raster[x][y]){
							 output_raster[x][y] = minimo;
							 active_costs.insert(cell(x,y,minimo));
							 cout << "inserté = " << x << ", " << y << ", " << minimo << endl;
						 }
					 }
				 }
				 else{
					 //cout << "Normal" << " "<< x << ", " << y << " " << cost_raster[x][y] << endl;
					 float dist = 0;
					 if(isInsideGrid(x,y) && (x != origen_x || y != origen_y) && active_raster[x][y] == 0 ){
						 cout << x << y << " " << active_raster[x][y]<< endl;
						 cout << "Normal" << " "<< x << ", " << y << " " << cost_raster[x][y] << endl;
						 dist = ((cost_raster[k.x][k.y] + cost_raster[x][y])/2) + output_raster[k.x][k.y];
						 //cout << dist << endl;
						 if(dist < output_raster[x][y]){
							 output_raster[x][y] = dist;
							 active_costs.insert(cell(x,y,dist));
							 cout << "inserté = " << x << ", " << y << ", " << dist << endl;
						 }
					 }
				 }

			 }
			 active_raster[k.x][k.y] = true;

		}
	}


	void inicio_cost_distance(float** cost_raster) {
		//cout << ROWS << COLS << endl;
		this->cost_raster = new float*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			this->cost_raster[i] = new float[COLS];
		//cout << cost_raster[29][26]<<endl;
		for(int x = 0; x < ROWS; x++){
			for(int y = 0; y < COLS; y++){
				//cout << cost_raster[x][y] << " ";
				this->cost_raster[x][y] = cost_raster[x][y];
				cout << this->cost_raster[x][y] << " ";
			}
			cout << endl;
		}
		cout << "COST_RASTER >>>>>>>" << endl;
		for(int x = 0; x < ROWS; x++){
			for(int y = 0; y < COLS; y++){
				cout << this->cost_raster[x][y] << " ";
			}
			cout << endl;
		}

		this->output_raster = new float*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			this->output_raster[i] = new float[COLS];
		this->active_raster = new bool*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			this->active_raster[i] = new bool[COLS];

		/*int source_raster[6][6] = {
					0,0,0,0,0,0,
					0,0,0,0,0,0,
					0,0,0,0,0,0,
					0,0,0,0,0,0,
					0,0,0,0,0,0,
					1,0,0,0,0,0,
				};*/
		int** source_raster;

		source_raster = new int*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			source_raster[i] = new int[COLS];
		for(int r=0; r<ROWS; r++){
			for(int c=0; c<COLS; c++){
				source_raster[r][c] = 0;
				output_raster[r][c] = INT_MAX;
			}
		}
		//fill(&source_raster[0][0], &source_raster[0][0] + sizeof(source_raster) / sizeof(source_raster[0][0]), 0);
		source_raster[29][0] = 1;
		//fill(&output_raster[0][0],&output_raster[0][0] + sizeof(output_raster) / sizeof(output_raster[0][0]), INT_MAX);

		// the position of a cell that you want to display its neighbours
				int origen_x, origen_y;
				vector<cell> vect_destinos;
				for(int r = 0; r < ROWS; r++){
					for(int c = 0; c < COLS; c++){
						if(source_raster[r][c] == 1){
							origen_x = r;
							origen_y = c;
							vect_destinos.push_back(cell(r,c,0));
							cout << "origen = "<< r << ", " << c << endl;
							this->active_raster[r][c] = 1;
							cout << active_raster[r][c]<<endl;
						}
						else
							active_raster[r][c] = false;
					}
				}


				//se obtienen los vecinos proximos al origen y sus distancias calculadas. ordenas de menor a mayor
				set<cell> distancias = vecinos(origen_x,origen_y);
				set <cell> :: iterator itr;
				for (itr = distancias.begin(); itr != distancias.end(); ++itr){
					cout << (*itr).x << ", " << (*itr).y << " - dist =  " << (*itr).distance << endl;
				}

			acumulados(distancias, origen_x,origen_y);//metodo para calcular demas vecinos.

				for(int r=0; r<ROWS; r++){
					for(int c=0; c<COLS; c++){
						//cout << output_raster[r][c] << " ";
						printf("%f \t", output_raster[r][c]);
					}
					cout << endl;
				}

	}
};
