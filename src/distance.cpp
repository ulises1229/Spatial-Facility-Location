/*
 * CostDistance.cpp
 *
 *  Created on: 04/10/2017
 *      Author: lanase
 */

#include "distance.h"



	// Utility method for comparing two cells
	static bool operator<(const CostDistance::cell& a, const CostDistance::cell& b){
		if (a.distance == b.distance){
			if (a.x != b.x)
				return (a.x < b.x);
			else
				return (a.y < b.y);
		}
		return (a.distance < b.distance);
	}

    CostDistance::CostDistance(int cols, int rows){
        ROWS = rows;
        COLS = cols;
    }

	bool CostDistance::isInsideGrid(int i, int j){
		return (i >= 0 && i < ROWS && j >= 0 && j < COLS && costRaster[i][j] >= 0);
	}


	set<CostDistance::cell>CostDistance::vecinos(int origen_x, int origen_y){
		set<cell>distancias;
		float** dis;
		dis = new float*[this->ROWS];
		for(int i = 0; i < ROWS; ++i)
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

		// insert (0, 0) cell with 0 distance
		st.insert(cell(origen_x, origen_y, 0));

		// initialize distance of (0, 0) with its grid value
		dis[origen_x][origen_y] = costRaster[origen_x][origen_y];

		// get the cell with minimum distance and delete
		// it from the set
		cell k = *st.begin();
		st.erase(st.begin());
		int x, y;
		// looping through all neighbours
		for (int i = 0; i < 8; i++){
			x = k.x + dx[i];
			y = k.y + dy[i];
			//cout << "x = " << x << " y = " << y << endl;
			// if not inside boundry, ignore them
			if (!isInsideGrid(x, y)){
				//cout << "no inside grid" << endl;
				continue;
			}

			if(i % 2 == 0){//par = no es diagonal
				//cout << "N" << endl;
				dis[x][y] = (dis[k.x][k.y] + costRaster[x][y]) / 2;
			}
			else{
				//cout << "D" << endl;
				dis[x][y] = 1.414214 * (dis[k.x][k.y] + costRaster[x][y]) / 2;
			}
			distancias.insert(cell(x, y, dis[x][y]));
            activeRaster[x][y] = true;
            outputRaster[x][y] = dis[x][y];//son los bordes, no hay necesidad de verificar si es menor al INT_MAX
		}

        outputRaster[origen_x][origen_y] = 0;

		for(int m=0; m<this->ROWS; m++)
			delete[] dis[m];

		st.clear();
		return distancias;
	}


	void CostDistance::acumulados(set<cell> active_costs, int origen_x, int origen_y, float** biomass, float intervals, float xMin, float xMax, float yMin, float yMax){
			extern char is_usable;
			int exp;

			if(is_usable == 'y')
				exp = intervals * 4;
			else
				exp = intervals * 8;

			xMin = xMin - exp;
			yMin = yMin - exp;
			xMax = xMax + exp;
			yMax = yMax + exp;

			//cout << xMin << " - " << xMax << " - " << yMin << " - " << yMax << endl;

			if(xMin < 0)
				xMin = 0;

			if(yMin < 0)
				yMin = 0;

			int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
			int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
			int cont = 1;
			while(!active_costs.empty()){
				cell k = *active_costs.begin();//inicio con la distancia menor
				active_costs.erase(active_costs.begin());

				//cout << "Estoy en: " << k.x << " - " << k.y << " - " << k.distance << endl;
				if(k.x >= xMin && k.x <= xMax && k.y >= yMin && k.y <= yMax){
					cont++;
					int x, y;
					for (int i = 0; i < 8; i++){
						x = k.x + dx[i];
						y = k.y + dy[i];
						//cout << x << ", " << y << " " << activeRaster[x][y]<< " - " << biomass[x][y] << endl;
						set<float>distancias;
						if(isInsideGrid(x,y)) {
							if(i % 2 != 0){// si es movimiento diagonal
								//cout << "Diagonal" << " "<< x << ", " << y << " " << costRaster[x][y] << endl;
								if((x != origen_x || y != origen_y) && activeRaster[x][y] == 0){
									float dist=0;
									int cont_x = k.x; int cont_y = k.y;
									int cont_x2 = k.x; int cont_y2 = k.y;
									int caminos = 1;
									//set<float>distancias;
									for(int z = 0; z < 8; z=z+2){

										int mov_x = rutas[i][z];
										int mov_y = rutas[i][z+1];

										dist += (costRaster[cont_x + mov_x][cont_y + mov_y] + costRaster[cont_x2][cont_y2]) / 2;
										//cout << mov_x << " " << mov_y << " " << "costRaster[cont_x+mov_x][cont_y+mov_y] = "<< costRaster[cont_x+mov_x][cont_y+mov_y]
										//<< " costRaster[cont_x2][cont_y2] = "<< costRaster[cont_x2][cont_y2] << " /2 =  " << dist << endl;
										if(caminos == 2 || caminos == 4){
											//cout << dist+outputRaster[k.x][k.y] << endl;
											distancias.insert(dist + outputRaster[k.x][k.y]);
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
									distancias.insert(1.414214 * ((costRaster[k.x][k.y] + costRaster[x][y]) / 2) + outputRaster[k.x][k.y] );
									float minimo = *distancias.begin();
									//cout << minimo << endl;
									if(minimo < outputRaster[x][y] && minimo >= 0){
										//cout << "Inserté " << x << ", " << y << " - " << minimo << endl;
										outputRaster[x][y] = minimo;
										active_costs.insert(cell(x,y,minimo));
									}
								}
							}
							else{
								//cout << "Normal" << " "<< x << ", " << y << " " << costRaster[x][y] << endl;
								float dist = 0;
								if((x != origen_x || y != origen_y) && activeRaster[x][y] == 0 && isInsideGrid(x, y)){
									dist = ((costRaster[k.x][k.y] + costRaster[x][y]) / 2) + outputRaster[k.x][k.y];
									if(dist < outputRaster[x][y] && dist >= 0){
										//cout << "Inserté " << x << ", " << y << " - " << dist << endl;
										outputRaster[x][y] = dist;
										active_costs.insert(cell(x,y,dist));
									}
								}

							}
						}
					}
                    activeRaster[k.x][k.y] = true;
				}
				}


			active_costs.clear();
		}




	void CostDistance::inicio_cost_distance(float** grid, int srcX, int srcY, float** biomass, float intervals, float xMin, float xMax, float yMin, float yMax, double projection) {
		//cout << COLS << " " << ROWS << endl;
		this->costRaster = new float*[this->ROWS];
		this->outputRaster = new float*[this->ROWS];
		this->activeRaster = new bool*[this->ROWS];
		for(int i = 0; i < ROWS; ++i){
			this->costRaster[i] = new float[COLS];
			this->outputRaster[i] = new float[COLS];
			this->activeRaster[i] = new bool[COLS];
		}
		for(int x = 0; x < ROWS; x++){
			for(int y = 0; y < COLS; y++){
				if(grid[x][y] == 999999)
					grid[x][y] = -9999;

				this->costRaster[x][y] = grid[x][y];// * projection;
				this->activeRaster[x][y] = false;
				this->outputRaster[x][y] = INT_MAX;
			}
		}


		// the position of a cell that you want to display its neighbours
		cout << srcX << " - " << srcY << endl;
        activeRaster[srcX][srcY] = 1;

		//se obtienen los vecinos proximos al origen y sus distancias calculadas. ordenas de menor a mayor
		set<cell> distancias = vecinos(srcX,srcY);

		/*set <cell> :: iterator itr;
		for (itr = distancias.begin(); itr != distancias.end(); ++itr){
			cout << (*itr).x << ", " << (*itr).y << " - dist =  " << (*itr).distance << endl;
		}*/
		//exit(0);
		acumulados(distancias, srcX, srcY, biomass, intervals, xMin, xMax, yMin, yMax);//metodo para calcular demas vecinos.
		for(int r=0; r < ROWS; r++){
			for(int c=0; c < COLS; c++){
				if(outputRaster[r][c] == INT_MAX)
                    outputRaster[r][c] = -9999;

			}
		}

	}

	void CostDistance::freeMem() {
		for(int m=0; m < ROWS; m++){
			delete[] outputRaster[m];
			delete[] costRaster[m];
			delete[] activeRaster[m];
		}
	}

    float** CostDistance::getOutputRaster() {
	    return outputRaster;
	}

