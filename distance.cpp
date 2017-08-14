//============================================================================
// Name        : distance.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <set>
#include <climits>
using namespace std;

#define ROW 6
#define COL	6

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

bool active_raster[6][6];


/*class status_raster{
public:
	int x, y;
	bool active;
	status_raster(int x, int y, bool active){
		this->x = x;
		this->y = y;
		this->active = active;
	}
};*/


struct cell {
    int x, y;
    float distance;
    cell(int x, int y, float distance) :
            x(x), y(y), distance(distance) {}
    /*bool operator==(const coords &o) const{
        return x == o.x && y == o.y;
    }

    bool operator<(const coords &o) const{
        return x < o.x || (x == o.x && y < o.y);
    }*/
};
/*struct status_raster{
	int x, y;
	bool active;

	status_raster(int x, int y, bool active) :
	            x(x), y(y), active(active) {}
};*/

struct bordes {
    int x, y;
    bordes(int x, int y) :
            x(x), y(y){}
    /*bool operator==(const coords &o) const{
        return x == o.x && y == o.y;
    }

    bool operator<(const coords &o) const{
        return x < o.x || (x == o.x && y < o.y);
    }*/
};

// Utility method for comparing two cells
bool operator<(const cell& a, const cell& b)
{
    if (a.distance == b.distance)
    {
        if (a.x != b.x)
            return (a.x < b.x);
        else
            return (a.y < b.y);
    }
    return (a.distance < b.distance);
}


bool isInsideGrid(int i, int j)
{
    return (i >= 0 && i < COL && j >= 0 && j < ROW);
}


set<cell>vecinos(float grid[ROW][COL], int origen_x, int origen_y, bool active_raster[ROW][COL])
{
	set<cell>distancias;
    float dis[ROW][COL];

    // initializing distance array by INT_MAX
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++){
            dis[i][j] = INT_MAX;
        }

    // direction arrays for simplification of getting
    // neighbour
    int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
    int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

    set<cell> st;

    // insert (0, 0) cell with 0 distance
    st.insert(cell(origen_x, origen_y, 0));
    cout << "insert en set<cell>(cell(3,3,0))" << endl;

    // initialize distance of (0, 0) with its grid value
    dis[origen_x][origen_y] = grid[origen_x][origen_y];
    cout << "dis[3][3] = " << dis[origen_x][origen_y] << "  grid[3][3] = " << grid[origen_x][origen_y] << endl;


	// get the cell with minimum distance and delete
	// it from the set
	cell k = *st.begin();
	st.erase(st.begin());
	cout << "k.x = " << k.x << " k.y = " << k.y << " k.distance = " << k.distance <<endl;
	// looping through all neighbours
	for (int i = 0; i < 8; i++){
		int x = k.x + dx[i];
		int y = k.y + dy[i];
		cout << "x = " << x << " y = " << y << endl;
		// if not inside boundry, ignore them
		if (!isInsideGrid(x, y))
			continue;

		if(i % 2 == 0){//par = no es diagonal
			//cout << "N" << endl;
			dis[x][y] = (dis[k.x][k.y] + grid[x][y])/2;
		}
		else{
			//cout << "D" << endl;
			dis[x][y] = 1.414214*(dis[k.x][k.y] + grid[x][y])/2;
		}
		distancias.insert(cell(x, y, dis[x][y]));
		active_raster[x][y] = true;
	}


    return distancias;
}



void acumulados(set<cell> active_costs, float cost_raster[ROW][COL], int origen_x, int origen_y, bool active_raster[ROW][COL]){
	 cell k = *active_costs.begin();//inicio con la distancia menor
	 int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
	 int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

	 //float acum_inicio = k.distance;
	 for (int i = 0; i < 8; i++){
		 int x = k.x + dx[i];
		 int y = k.y + dy[i];
		 cout << x << y << " " << active_raster[x][y]<< endl;
		 if((x != origen_x && y != origen_y) && active_raster[x][y] == 0){
			 cout << "entre" << " "<< x << ", " << y << " " << cost_raster[x][y] <<  " " << active_raster[x][y] <<  endl;
			 /*float dist=0;
			 for(int z = 0; z < 8; z=z+2){
				 int mov_x = rutas[i][z];
				 int mov_y = rutas[i][z+1];
				 dist += (cost_raster[mov_x][mov_y] + cost_raster[k.x][k.y])/2;
				 cout << dist << endl;
			 }*/
		 }
	 }
}


int main() {

	float cost_raster[6][6] = {
	        1,3,4,4,3,2,
	        4,6,2,3,7,6,
	        5,8,7,5,6,6,
	        1,4,5,1,5,1,
	        4,7,5,1,2,6,
	        1,2,2,1,3,4,
	    };

	int source_raster[6][6] = {
		        0,0,0,0,0,0,
		        0,0,0,0,0,0,
		        0,0,0,0,0,0,
		        0,0,0,1,0,0,
		        0,0,0,0,0,0,
		        0,0,0,0,0,0,
		    };



	// the position of a cell that you want to display its neighbours
			int origen_x, origen_y;
	        vector<cell> vect_destinos;
	        for(int r=0; r<6; r++){
	        	for(int c=0; c<6; c++){
	        		if(source_raster[r][c] == 1){
	        			origen_x = r;
	        			origen_y = c;
						vect_destinos.push_back(cell(r,c,0));
						cout << r << c << endl;
						active_raster[r][c] = true;
	        		}
	        		active_raster[r][c] = false;
	        	}
	        }

	        //se obtienen los vecinos proximos al origen y sus distancias calculadas. ordenas de menor a mayor
	        set<cell> distancias = vecinos(cost_raster, origen_x,origen_y, active_raster);
	        set <cell> :: iterator itr;
			for (itr = distancias.begin(); itr != distancias.end(); ++itr){
				cout << (*itr).x << ", " << (*itr).y << " - dist =  " << (*itr).distance << endl;
			}

		acumulados(distancias, cost_raster, origen_x,origen_y, active_raster);//metodo para calcular demas vecinos.


	    //cout << shortest(grid, ROW, COL) << endl;
	    return 0;
}
