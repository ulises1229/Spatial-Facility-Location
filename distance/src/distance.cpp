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
	bool** active_raster_exploracion;

	bool isInsideGrid(int i, int j){
		return (i >= 0 && i < ROWS && j >= 0 && j < COLS && cost_raster[i][j] >= 0);
	}

	set<cell>vecinos(int origen_x, int origen_y){

		set<cell>distancias;
		float** dis;
		dis = new float*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			dis[i] = new float[COLS];
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

		// insert cell with 0 distance
		st.insert(cell(origen_x, origen_y, 0));

		// initialize distance of source with its grid value
		dis[origen_x][origen_y] = cost_raster[origen_x][origen_y];

		// get the cell with minimum distance and delete
		// it from the set
		cell k = *st.begin();
		st.erase(st.begin());

		// looping through all neighbours
		for (int i = 0; i < 8; i++){
			int x = k.x + dx[i];
			int y = k.y + dy[i];

			// if not inside boundry, ignore them
			if (!isInsideGrid(x, y))
				continue;

			if(i % 2 == 0){//par = no es diagonal
				cout << "N" << endl;
				dis[x][y] = (dis[k.x][k.y] + cost_raster[x][y])/2;
			}
			else{
				cout << "D" << endl;
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

			cell k = *active_costs.begin();//inicio con la distancia menor
			active_costs.erase(active_costs.begin());
			 int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
			 int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

			 for (int i = 0; i < 8; i++){
				 int x = k.x + dx[i];
				 int y = k.y + dy[i];

				 set<float>distancias;
				 if(i % 2 != 0){// si es movimiento diagonal
					 if(isInsideGrid(x,y) && (x != origen_x || y != origen_y) && active_raster[x][y] == 0 ){
						 float dist=0;
						 int cont_x = k.x; int cont_y = k.y;
						 int cont_x2 = k.x; int cont_y2 = k.y;
						 int caminos = 1;

						 for(int z = 0; z < 8; z=z+2){
							 int mov_x = rutas[i][z];
							 int mov_y = rutas[i][z+1];
							 if(cost_raster[cont_x+mov_x][cont_y+mov_y] || cost_raster[cont_x2][cont_y2] < 0)
								 break;
							 dist += (cost_raster[cont_x+mov_x][cont_y+mov_y] + cost_raster[cont_x2][cont_y2])/2;

							 if(caminos == 2 || caminos == 4){
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
						 if((cost_raster[k.x][k.y] || cost_raster[x][y]) < 0)
							 break;
						 distancias.insert(1.414214 * ((cost_raster[k.x][k.y]+cost_raster[x][y])/2) + output_raster[k.x][k.y] );

						 float minimo = *distancias.begin();
						 if(minimo < output_raster[x][y]){
							 output_raster[x][y] = minimo;
							 active_costs.insert(cell(x,y,minimo));
						 }
					 }
				 }
				 else{
					 float dist = 0;
					 if(isInsideGrid(x,y) && (x != origen_x || y != origen_y) && active_raster[x][y] == 0 ){
						 if((cost_raster[k.x][k.y] || cost_raster[x][y]) < 0)
							 break;

						 dist = ((cost_raster[k.x][k.y] + cost_raster[x][y])/2) + output_raster[k.x][k.y];
						 if(dist < output_raster[x][y]){
							 output_raster[x][y] = dist;
							 active_costs.insert(cell(x,y,dist));
						 }
					 }
				 }

			 }
			 active_raster[k.x][k.y] = true;

		}
	}

	set<cell>vecinos_cuadrantes(int origen_x, int origen_y){//busca vecinos iniciales nada mas
			set<cell>distancias;

			int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
			int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

			set<cell> st;

			// insert  cell with 0 distance
			st.insert(cell(origen_x, origen_y, 0));

			cell k = *st.begin();
			st.erase(st.begin());

			// looping through all neighbours
			for (int i = 0; i < 8; i++){
				int x = k.x + dx[i];
				int y = k.y + dy[i];

				// if not inside boundry, ignore them
				if (!isInsideGrid(x, y))
					continue;

				distancias.insert(cell(x, y, 0));
				active_raster_exploracion[x][y] = true;

			}


			return distancias;
		}

		set<cell>vecinos_recalc(set<cell> vecinos){//obtiene vecinos que aun no han sido considerados
			set<cell>distancias;
			int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
			int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

			set <cell> :: iterator itr;
			for (itr = vecinos.begin(); itr != vecinos.end(); ++itr){
				cell k = *itr;
				// looping through all neighbours
				for (int i = 0; i < 8; i++){
					int x = k.x + dx[i];
					int y = k.y + dy[i];
					// if not inside boundry, ignore them
					if (!isInsideGrid(x, y) || active_raster_exploracion[x][y] == 1)
						continue;

					distancias.insert(cell(x, y, 0));
					active_raster_exploracion[x][y] = true;
				}
			}
			return distancias;
		}

	set<cell> explore(int origen_x, int origen_y, float stop){
		set<cell> vecinos = vecinos_cuadrantes(origen_x,origen_y);

		set<cell> celdas;
		celdas.insert(cell(origen_x, origen_y, 0));
		float suma = 0.0;
		int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
		int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
		suma = cost_raster[origen_x][origen_y];

		if(suma < stop){

			while(suma < stop){
				set <cell> :: iterator itr;
				bool inserte = false;
				for (itr = vecinos.begin(); itr != vecinos.end(); ++itr){
					if(suma + cost_raster[(*itr).x][(*itr).y] <= stop){
						for (int i = 0; i < 8; i++){
							int x = (*itr).x + dx[i];
							int y = (*itr).y + dy[i];
							set<cell>::iterator it = celdas.find(cell(x,y,0));
							if(it != celdas.end()){
								suma += cost_raster[(*itr).x][(*itr).y];
								celdas.insert(cell((*itr).x, (*itr).y, 0));
								inserte = true;
								break;
							}
						}

					}
				}
				if(!inserte)//sino insertó niguna celda que sea contigua, la suma no llegará al tope
					break;

				vecinos = vecinos_recalc(vecinos);

			}
		}
		else{//el origen ya cumple con la condicion de parada, stop
			cout << "el origen ya cumple con la condicion de parada, stop" << endl;
		}
		cout << "ORIGEN = " << origen_x << ", " << origen_y << endl;
		cout << "TOPE = " << stop << " SUMA = " << suma << endl;

		return celdas;


	}

	void exploracion(float** cost_raster){//busqueda por cuadrantes
		this->cost_raster = new float*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			this->cost_raster[i] = new float[COLS];

		for(int x = 0; x < ROWS; x++){
			for(int y = 0; y < COLS; y++){
				this->cost_raster[x][y] = cost_raster[x][y];
			}
		}

		this->active_raster_exploracion = new bool*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			this->active_raster_exploracion[i] = new bool[COLS];

		int** source_raster;
		source_raster = new int*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			source_raster[i] = new int[COLS];
		for(int r=0; r<ROWS; r++){
			for(int c=0; c<COLS; c++){
				source_raster[r][c] = 0;
			}
		}
		source_raster[29][0] = 1;

		int origen_x, origen_y;
		for(int r = 0; r < ROWS; r++){
			for(int c = 0; c < COLS; c++){
				if(source_raster[r][c] == 1){
					origen_x = r;
					origen_y = c;
					this->active_raster_exploracion[r][c] = 1;
				}
				else
					active_raster_exploracion[r][c] = false;
			}
		}

		cout << "voy a explore" << endl;
		set<cell> celdas = explore(origen_x, origen_y, 2.5);
	}


	void inicio_cost_distance(float** cost_raster) {
		this->cost_raster = new float*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			this->cost_raster[i] = new float[COLS];
		for(int x = 0; x < ROWS; x++){
			for(int y = 0; y < COLS; y++){
				this->cost_raster[x][y] = cost_raster[x][y];
			}
		}


		this->output_raster = new float*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			this->output_raster[i] = new float[COLS];
		this->active_raster = new bool*[this->ROWS];
		for(int i = 0; i< ROWS; ++i)
			this->active_raster[i] = new bool[COLS];

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

		source_raster[29][0] = 1;

		int origen_x, origen_y;
		vector<cell> vect_destinos;
		for(int r = 0; r < ROWS; r++){
			for(int c = 0; c < COLS; c++){
				if(source_raster[r][c] == 1){
					origen_x = r;
					origen_y = c;
					vect_destinos.push_back(cell(r,c,0));
					this->active_raster[r][c] = 1;
				}
				else
					active_raster[r][c] = false;
			}
		}

		//se obtienen los vecinos proximos al origen y sus distancias calculadas. ordenas de menor a mayor
		set<cell> distancias = vecinos(origen_x,origen_y);

		acumulados(distancias, origen_x,origen_y);//metodo para calcular demas vecinos.

		/*for(int r=0; r<ROWS; r++){
			for(int c=0; c<COLS; c++){
				printf("%f \t", output_raster[r][c]);
			}
			cout << endl;
		}*/

	}
};
