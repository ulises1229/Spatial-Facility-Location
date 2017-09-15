#include <climits>
#include <iostream>
#include <math.h>

#include <float.h>
#include <stdio.h>
#include <stack>
#include <cstring>
#include <set>
#include <utility>

using namespace std;

//#define ROW 9
//#define COL 10


// Creating a shortcut for int, int pair type
typedef pair<int, int> Pair;
// Creating a shortcut for pair<int, pair<int, int>> type
typedef pair< float, pair<int, int> > pPair;

// A structure to hold the neccesary parameters
struct cell_exp{
    int parent_i, parent_j;
    double f, g, h;
};

class exploracion{
public:
	bool** closedList;

	float** grid;
    int ROW, COL;
	cell_exp** cellDetails;
	cell_exp** cellParents;


bool isValid(int row, int col){
    // Returns true if row number and column number
    // is in range
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}



// A Utility Function to trace the path from the source
// to destination
stack<Pair> tracePath(cell_exp** cellDetails, Pair dest){
    int row = dest.first;
    int col = dest.second;

    stack<Pair> Path;
    while (! (cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col) )
    {
        Path.push (make_pair (row, col));
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }
    Path.push (make_pair (row, col));

    return Path;
}

void printPath(stack<Pair> Path){
	int s = Path.size();
	float n=0.0;
	 while (!Path.empty()){
		pair<int,int> p = Path.top();
		Path.pop();
		printf("-> (%d,%d) ",p.first,p.second);
		cout << grid[p.first][p.second] << endl;
		n += grid[p.first][p.second];
	}
	 cout << "Size: " << s << endl;
	 cout << "Sum: " << n << endl;
}

bool checkFutureInsert(int x, int y, float acum, float parada){
	int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
	int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
	float gNew, hNew, fNew;
	for(int i = 0; i < 8; i++){
		int xx = x + dx[i];
		int yy = y + dy[i];
		if (isValid(xx, yy) == true){
			// If the successor is already on the closed
			// list or if it is blocked, then ignore it.
			// Else do the following
			if (closedList[xx][yy] == false){
				gNew = acum; //grid[i][j];// g = grid[i][j]
				hNew = grid[xx][yy];// h = grid[i-1][j]
				fNew = gNew + hNew;

				if ((cellDetails[xx][yy].f == FLT_MAX || cellDetails[xx][yy].f < fNew) && fNew <= parada && hNew != -9999){
					return true;
				}
			}
		}
	}
	return false;
}

// A Function to find the shortest path between
// a given source cell to a destination cell according
// to A* Search Algorithm
void aStarSearch(Pair src, float stop){
    // If the source is out of range
    if (isValid (src.first, src.second) == false){
        printf ("Source is invalid\n");
        return;
    }

    int i, j;
    for (i=0; i<ROW; i++){
        for (j=0; j<COL; j++){
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
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
     Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
     This open list is implenented as a set of pair of pair.*/
    set<pPair> openList;

    // Put the starting cell on the open list and set its
    // 'f' as 0
    openList.insert(make_pair (grid[i][j], make_pair (i, j)));//celda origen
    float parada = stop;
    int pushes = INT_MAX;
    stack<Pair> maxCost_shortest_path;
    while (!openList.empty()){
    	pPair p;

    	p = *(--openList.end());//tomar el ultimo? seria el mas costoso
    	i = p.second.first;
		j = p.second.second;

        //pPair last = *openList.end();
        if((parada-.005 <= p.first )){
        	if(p.first <= parada){
				Pair dest = make_pair(p.second.first, p.second.second);
				stack<Pair> ruta = tracePath(cellDetails, dest);
				if(ruta.size() < pushes){
					pushes = ruta.size();
					maxCost_shortest_path = ruta;
				}
        	}
        }

        float acum = p.first;

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
        float gNew, hNew, fNew;

        //----------- 1st Successor (North) ------------

        // Only process this cell if this is a valid one
        if (isValid(i-1, j) == true){

            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i-1][j] == false){
                gNew = acum;
                hNew = grid[i-1][j];
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if ((cellDetails[i-1][j].f == FLT_MAX ||
                        cellDetails[i-1][j].f < fNew) && fNew <= parada && hNew != -9999)
                {
					//if(checkFutureInsert(i-1, j, acum, parada)){
						openList.insert( make_pair(fNew, make_pair(i-1, j)));

						// Update the details of this cell
						cellDetails[i-1][j].f = fNew;
						cellDetails[i-1][j].g = gNew;
						cellDetails[i-1][j].h = hNew;
						cellDetails[i-1][j].parent_i = i;
						cellDetails[i-1][j].parent_j = j;
					//}
                }
            }
        }

        //----------- 2nd Successor (South) ------------

        // Only process this cell if this is a valid one
        if (isValid(i+1, j) == true){
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i+1][j] == false){
                gNew = acum;
                hNew = grid[i+1][j];
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if ((cellDetails[i+1][j].f == FLT_MAX ||
                        cellDetails[i+1][j].f < fNew) && fNew <= parada && hNew != -9999){
					//if(checkFutureInsert(i+1, j, acum, parada)){
                    	openList.insert( make_pair (fNew, make_pair (i+1, j)));

						// Update the details of this cell
						cellDetails[i+1][j].f = fNew;
						cellDetails[i+1][j].g = gNew;
						cellDetails[i+1][j].h = hNew;
						cellDetails[i+1][j].parent_i = i;
						cellDetails[i+1][j].parent_j = j;
					//}
					/*else{
						openList.insert( make_pair (fNew, make_pair (i+1, j)));
						cout << "inserté openList " << fNew << " " << i+1 << ", " << j << endl;

						// Update the details of this cell
						cellDetails[i+1][j].f = fNew;
						cellDetails[i+1][j].g = gNew;
						cellDetails[i+1][j].h = hNew;
						cellDetails[i+1][j].parent_i = cellDetails[i][j].parent_i;
						cellDetails[i+1][j].parent_j = cellDetails[i][j].parent_j;
						cout << "South inserté cellDetails[" << i+1 << "][" << j << " = " << i << " " << j << " " << fNew <<  endl;
					}*/
                }
            }
        }

        //----------- 3rd Successor (East) ------------

        // Only process this cell if this is a valid one
        if (isValid (i, j+1) == true){
            // If the destination cell is the same as the
            // current successor


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i][j+1] == false){
                gNew = acum;//grid[i][j];
                hNew = grid[i][j+1];
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if ((cellDetails[i][j+1].f == FLT_MAX ||
                        cellDetails[i][j+1].f < fNew) && fNew <= parada && hNew != -9999){
                	//if(checkFutureInsert(i, j+1, acum, parada)){
						openList.insert( make_pair(fNew, make_pair (i, j+1)));

						// Update the details of this cell
						cellDetails[i][j+1].f = fNew;
						cellDetails[i][j+1].g = gNew;
						cellDetails[i][j+1].h = hNew;
						cellDetails[i][j+1].parent_i = i;
						cellDetails[i][j+1].parent_j = j;
					//}
                	/*else{
                		openList.insert( make_pair(fNew, make_pair (i, j+1)));
						cout << "inserté openList " << fNew << " " << i << ", " << j+1 << endl;

						// Update the details of this cell
						cellDetails[i][j+1].f = fNew;
						cellDetails[i][j+1].g = gNew;
						cellDetails[i][j+1].h = hNew;
						cellDetails[i][j+1].parent_i = cellDetails[i][j].parent_i;
						cellDetails[i][j+1].parent_j = cellDetails[i][j].parent_j;
						cout << "East inserté cellDetails[" << i << "][" << j+1 << " = " << i << " " << j << " " << fNew <<  endl;
                	}*/
                }
            }
        }

        //----------- 4th Successor (West) ------------

        // Only process this cell if this is a valid one
        if (isValid(i, j-1) == true){
            // If the destination cell is the same as the
            // current successor


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i][j-1] == false){
                gNew = acum;//grid[i][j];
                hNew = grid[i][j-1];
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if ((cellDetails[i][j-1].f == FLT_MAX ||
                        cellDetails[i][j-1].f) < fNew && fNew <= parada && hNew != -9999){
                	//if(checkFutureInsert(i, j-1, acum, parada)){
						openList.insert( make_pair (fNew, make_pair (i, j-1)));

						// Update the details of this cell
						cellDetails[i][j-1].f = fNew;
						cellDetails[i][j-1].g = gNew;
						cellDetails[i][j-1].h = hNew;
						cellDetails[i][j-1].parent_i = i;
						cellDetails[i][j-1].parent_j = j;
					//}
                	/*else{
                		openList.insert( make_pair (fNew, make_pair (i, j-1)));
						cout << "inserté openList " << fNew << " " << i << ", " << j-1 << endl;

						// Update the details of this cell
						cellDetails[i][j-1].f = fNew;
						cellDetails[i][j-1].g = gNew;
						cellDetails[i][j-1].h = hNew;
						cellDetails[i][j-1].parent_i = cellDetails[i][j].parent_i;
						cellDetails[i][j-1].parent_j = cellDetails[i][j].parent_j;
						cout << "West inserté cellDetails[" << i << "][" << j-1 << " = " << i << " " << j << " " << fNew <<  endl;
                	}*/
                }
            }
        }

        //----------- 5th Successor (North-East) ------------

        // Only process this cell if this is a valid one
        if (isValid(i-1, j+1) == true){
            // If the destination cell is the same as the


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i-1][j+1] == false){
                gNew = acum;//grid[i][j];
                hNew = grid[i-1][j+1];
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if ((cellDetails[i-1][j+1].f == FLT_MAX ||
                        cellDetails[i-1][j+1].f < fNew) && fNew <= parada && hNew != -9999){
                	//if(checkFutureInsert(i-1, j+1, acum, parada)){
						openList.insert( make_pair (fNew, make_pair(i-1, j+1)));
						// Update the details of this cell
						cellDetails[i-1][j+1].f = fNew;
						cellDetails[i-1][j+1].g = gNew;
						cellDetails[i-1][j+1].h = hNew;
						cellDetails[i-1][j+1].parent_i = i;
						cellDetails[i-1][j+1].parent_j = j;
					//}
                	/*else{
                		openList.insert( make_pair (fNew, make_pair(i-1, j+1)));
						cout << "inserté openList " << fNew << " " << i-1 << ", " << j+1 << endl;

						// Update the details of this cell
						cellDetails[i-1][j+1].f = fNew;
						cellDetails[i-1][j+1].g = gNew;
						cellDetails[i-1][j+1].h = hNew;
						cellDetails[i-1][j+1].parent_i = cellDetails[i][j].parent_i;
						cellDetails[i-1][j+1].parent_j = cellDetails[i][j].parent_j;
						cout << "NE inserté cellDetails[" << i-1 << "][" << j+1 << " = " << i << " " << j << " " << fNew <<  endl;
                	}*/
                }
            }
        }

        //----------- 6th Successor (North-West) ------------

        // Only process this cell if this is a valid one
        if (isValid (i-1, j-1) == true){
            // If the destination cell is the same as the
            // current successor

            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i-1][j-1] == false){
                gNew = acum;//grid[i][j];
                hNew = grid[i-1][j-1];
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if ((cellDetails[i-1][j-1].f == FLT_MAX ||
                        cellDetails[i-1][j-1].f < fNew) && fNew <= parada && hNew != -9999) {
                	//if(checkFutureInsert(i-1, j-1, acum, parada)){
						openList.insert( make_pair (fNew, make_pair (i-1, j-1)));

						// Update the details of this cell
						cellDetails[i-1][j-1].f = fNew;
						cellDetails[i-1][j-1].g = gNew;
						cellDetails[i-1][j-1].h = hNew;
						cellDetails[i-1][j-1].parent_i = i;
						cellDetails[i-1][j-1].parent_j = j;
					//}
                	/*else{
                		openList.insert( make_pair (fNew, make_pair (i-1, j-1)));
						cout << "inserté openList " << fNew << " " << i-1 << ", " << j-1 << endl;

						// Update the details of this cell
						cellDetails[i-1][j-1].f = fNew;
						cellDetails[i-1][j-1].g = gNew;
						cellDetails[i-1][j-1].h = hNew;
						cellDetails[i-1][j-1].parent_i = cellDetails[i][j].parent_i;
						cellDetails[i-1][j-1].parent_j = cellDetails[i][j].parent_j;
						cout << "NW inserté cellDetails[" << i-1 << "][" << j-1 << " = " << i << " " << j << " " << fNew <<  endl;
                	}*/
                }
            }
        }

        //----------- 7th Successor (South-East) ------------

        // Only process this cell if this is a valid one
        if (isValid(i+1, j+1) == true){
            // If the destination cell is the same as the
            // current successor


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i+1][j+1] == false){
                gNew = acum;//grid[i][j];
                hNew = grid[i+1][j+1];
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if ((cellDetails[i+1][j+1].f == FLT_MAX ||
                        cellDetails[i+1][j+1].f < fNew) && fNew <= parada && hNew != -9999){
                	//if(checkFutureInsert(i+1, j+1, acum, parada)){
						openList.insert(make_pair(fNew, make_pair (i+1, j+1)));

						// Update the details of this cell
						cellDetails[i+1][j+1].f = fNew;
						cellDetails[i+1][j+1].g = gNew;
						cellDetails[i+1][j+1].h = hNew;
						cellDetails[i+1][j+1].parent_i = i;
						cellDetails[i+1][j+1].parent_j = j;
                    //}
                	/*else{
                		openList.insert(make_pair(fNew, make_pair (i+1, j+1)));
						cout << "inserté openList " << fNew << " " << i+1 << ", " << j+1 << endl;

						// Update the details of this cell
						cellDetails[i+1][j+1].f = fNew;
						cellDetails[i+1][j+1].g = gNew;
						cellDetails[i+1][j+1].h = hNew;
						cellDetails[i+1][j+1].parent_i = cellDetails[i][j].parent_i;
						cellDetails[i+1][j+1].parent_j = cellDetails[i][j].parent_j;
						cout << "SE inserté cellDetails[" << i+1 << "][" << j+1 << " = " << i << " " << j << " " << fNew <<  endl;
                	}*/
                }
            }
        }

        //----------- 8th Successor (South-West) ------------

        // Only process this cell if this is a valid one
        if (isValid (i+1, j-1) == true){
            // If the destination cell is the same as the
            // current successor


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i+1][j-1] == false){
                gNew = acum;//grid[i][j];
                hNew = grid[i+1][j-1];
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if ((cellDetails[i+1][j-1].f == FLT_MAX ||
                        cellDetails[i+1][j-1].f < fNew) && fNew <= parada && hNew != -9999){
                	//if(checkFutureInsert(i+1, j-1, acum, parada)){
						openList.insert(make_pair(fNew, make_pair(i+1, j-1)));

						// Update the details of this cell
						cellDetails[i+1][j-1].f = fNew;
						cellDetails[i+1][j-1].g = gNew;
						cellDetails[i+1][j-1].h = hNew;
						cellDetails[i+1][j-1].parent_i = i;
						cellDetails[i+1][j-1].parent_j = j;
                    //}
                	/*else{
                		openList.insert(make_pair(fNew, make_pair(i+1, j-1)));
						cout << "inserté openList " << fNew << " " << i+1 << ", " << j-1 << endl;

						// Update the details of this cell
						cellDetails[i+1][j-1].f = fNew;
						cellDetails[i+1][j-1].g = gNew;
						cellDetails[i+1][j-1].h = hNew;
						cellDetails[i+1][j-1].parent_i = cellDetails[i][j].parent_i;
						cellDetails[i+1][j-1].parent_j = cellDetails[i][j].parent_j;
						cout << "SW inserté cellDetails[" << i+1 << "][" << j-1 << " = " << i << " " << j << " " << fNew <<  endl;
                	}*/

                }
            }
        }



    }

    printPath(maxCost_shortest_path);

    return;
}


// Driver program to test above function
void explore(float** grid, float stop){
	this->grid = new float*[this->ROW];
	for(int i = 0; i< ROW; ++i)
		this->grid[i] = new float[COL];

	for(int x = 0; x < ROW; x++){
		for(int y = 0; y < COL; y++){
			this->grid[x][y] = grid[x][y];
		}
	}

	this->cellDetails = new cell_exp*[this->ROW];
	for(int i = 0; i< ROW; ++i)
		this->cellDetails[i] = new cell_exp[COL];

	this->closedList = new bool*[this->ROW];
		for(int i = 0; i< ROW; ++i)
			this->closedList[i] = new bool[COL];
	memset(closedList, false, sizeof (closedList));


    // Source is the left-most bottom-most corner
    Pair src = make_pair(29, 0);

    aStarSearch(src, stop);

    //return(0);
}
};
