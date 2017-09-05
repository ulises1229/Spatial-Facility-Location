//============================================================================
// Name        : a*Costos.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stack>
#include <cstring>
#include <float.h>
#include <set>
#include <queue>
using namespace std;

#define ROW 9
#define COL 10

// Creating a shortcut for int, int pair type
typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef pair< double, pair<int, int> > pPair;

// A structure to hold the neccesary parameters
struct cell
{
    // Row and Column index of its parent
    // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
    int parent_i, parent_j;
    // f = g + h
    double f, g, h;
};

// A Utility Function to check whether given cell (row, col)
// is a valid cell or not.
bool isValid(int row, int col)
{
    // Returns true if row number and column number
    // is in range
    return (row >= 0) && (row < ROW) &&
           (col >= 0) && (col < COL);
}

// A Utility Function to trace the path from the source
// to destination
stack<Pair> tracePath(cell cellDetails[][COL], Pair dest)
{
	cout << "recibo tracePath " << dest.first << ", " << dest.second << endl;
    printf ("\nThe Path is ");
    int row = dest.first;
    int col = dest.second;

    stack<Pair> Path;

    while (! (cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col) )
    {
        Path.push (make_pair (row, col));
        cout << "push " << row << ", " << col << endl;
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }

    Path.push (make_pair (row, col));
    cout << "push " << row << ", " << col << endl;

    return Path;
}

void printPath(stack<Pair> Path){
	cout << "SIZE " << Path.size() << endl;
	 while (!Path.empty()){
	        pair<int,int> p = Path.top();
	        Path.pop();
	        printf("-> (%d,%d) ",p.first,p.second);
	    }
}

// A Function to find the shortest path between
// a given source cell to a destination cell according
// to A* Search Algorithm
void aStarSearch(int grid[][COL], Pair src, int stop)
{
    // If the source is out of range
    if (isValid (src.first, src.second) == false)
    {
        printf ("Source is invalid\n");
        return;
    }

    // Create a closed list and initialise it to false which means
    // that no cell has been included yet
    // This closed list is implemented as a boolean 2D array
    bool closedList[ROW][COL];
    memset(closedList, false, sizeof (closedList));

    // Declare a 2D array of structure to hold the details
    //of that cell
    cell cellDetails[ROW][COL];

    int i, j;

    for (i=0; i<ROW; i++)
    {
        for (j=0; j<COL; j++)
        {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    // Initialising the parameters of the starting node
    i = src.first, j = src.second;
    cellDetails[i][j].f = grid[i][j];
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
     This open list is implenented as a set of pair of pair.
     si se utiliza set, debemos tomar el ultimo elemento que seria el mas costoso
     si se utiliza prioriry_queue se toma el primero que es el mas costoso.
     */
    set<pPair> openList;
    priority_queue <pPair> pq;
    // Put the starting cell on the open list and set its
    // 'f' as 0
    openList.insert(make_pair (grid[i][j], make_pair (i, j)));//celda origen
    pq.push(make_pair (grid[i][j], make_pair (i, j)));

    int parada = stop;
    int pushes = INT_MAX;
    stack<Pair> maxCost_shortest_path;

    /* ****TRABAJA SOBRE PRIORITY_QUEUE***** */
    while (!pq.empty())
    {
    	pPair p;
    	p =pq.top();
    	//p = *(--openList.end());//tomar el ultimo? seria el mas costoso
    	i = p.second.first;
		j = p.second.second;
    	cout << "***" << p.first << " " << i << " " << j << "***" << endl;
        //pPair last = *openList.end();

		if( p.first == parada){
			cout << "encontré ruta.  = " << p.first << endl;
			Pair dest = make_pair(p.second.first, p.second.second);
			stack<Pair> ruta = tracePath(cellDetails, dest);
			if(ruta.size() < pushes){
				pushes = ruta.size();
				maxCost_shortest_path = ruta;
			}
		}

        int acum = p.first;
        // Remove this vertex from the open list
        pq.pop();
        //openList.erase(--openList.end());//borrar ultimo
        cout << "borré " << p.first << " " << p.second.first << ", " << p.second.second << endl;

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
        double gNew, hNew, fNew;


        //----------- 1st Successor (North) ------------

        // Only process this cell if this is a valid one
        if (isValid(i-1, j) == true)
        {

            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i-1][j] == false)
            {
                gNew = acum; //grid[i][j];// g = grid[i][j]
                hNew = grid[i-1][j];// h = grid[i-1][j]
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
                    openList.insert( make_pair(fNew,
                                               make_pair(i-1, j)));
                    pq.push( make_pair(fNew,
                                                                   make_pair(i-1, j)));
                    // Update the details of this cell
                    cellDetails[i-1][j].f = fNew;
                    cellDetails[i-1][j].g = gNew;
                    cellDetails[i-1][j].h = hNew;
                    cellDetails[i-1][j].parent_i = i;
                    cellDetails[i-1][j].parent_j = j;
                    cout << "North inserté cellDetails[" << i-1 << "][" << j << " = " << i << " " << j << " " << fNew <<  endl;
                }
            }
        }

        //----------- 2nd Successor (South) ------------

        // Only process this cell if this is a valid one
        if (isValid(i+1, j) == true)
        {
            // If the destination cell is the same as the
            // current successor
            /*if (isDestination(i+1, j, dest) == true)
            {
                // Set the Parent of the destination cell
                cellDetails[i+1][j].parent_i = i;
                cellDetails[i+1][j].parent_j = j;
                printf("The destination cell is found\n");
                tracePath(cellDetails, dest);
                foundDest = true;
                return;
            }*/
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i+1][j] == false)
            {
                gNew = acum; //grid[i][j];
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
                        cellDetails[i+1][j].f < fNew) && fNew <= parada && hNew != -9999)
                {
                    openList.insert( make_pair (fNew, make_pair (i+1, j)));
                    pq.push( make_pair (fNew, make_pair (i+1, j)));
                    // Update the details of this cell
                    cellDetails[i+1][j].f = fNew;
                    cellDetails[i+1][j].g = gNew;
                    cellDetails[i+1][j].h = hNew;
                    cellDetails[i+1][j].parent_i = i;
                    cellDetails[i+1][j].parent_j = j;
                    cout << "South inserté cellDetails[" << i+1 << "][" << j << " = " << i << " " << j << " " << fNew <<  endl;
                }
            }
        }

        //----------- 3rd Successor (East) ------------

        // Only process this cell if this is a valid one
        if (isValid (i, j+1) == true)
        {
            // If the destination cell is the same as the
            // current successor


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i][j+1] == false)
            {
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
                        cellDetails[i][j+1].f < fNew) && fNew <= parada && hNew != -9999)
                {
                    openList.insert( make_pair(fNew,
                                        make_pair (i, j+1)));
                    pq.push( make_pair(fNew,
                                                            make_pair (i, j+1)));
                    // Update the details of this cell
                    cellDetails[i][j+1].f = fNew;
                    cellDetails[i][j+1].g = gNew;
                    cellDetails[i][j+1].h = hNew;
                    cellDetails[i][j+1].parent_i = i;
                    cellDetails[i][j+1].parent_j = j;
                    cout << "East inserté cellDetails[" << i << "][" << j+1 << " = " << i << " " << j << " " << fNew <<  endl;
                }
            }
        }

        //----------- 4th Successor (West) ------------

        // Only process this cell if this is a valid one
        if (isValid(i, j-1) == true)
        {
            // If the destination cell is the same as the
            // current successor


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i][j-1] == false)
            {
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
                        cellDetails[i][j-1].f) < fNew && fNew <= parada && hNew != -9999)
                {
                    openList.insert( make_pair (fNew,
                                          make_pair (i, j-1)));
                    pq.push( make_pair (fNew,
                                                              make_pair (i, j-1)));
                    // Update the details of this cell
                    cellDetails[i][j-1].f = fNew;
                    cellDetails[i][j-1].g = gNew;
                    cellDetails[i][j-1].h = hNew;
                    cellDetails[i][j-1].parent_i = i;
                    cellDetails[i][j-1].parent_j = j;
                    cout << "West inserté cellDetails[" << i << "][" << j-1 << " = " << i << " " << j << " " << fNew <<  endl;
                }
            }
        }

        //----------- 5th Successor (North-East) ------------

        // Only process this cell if this is a valid one
        if (isValid(i-1, j+1) == true)
        {
            // If the destination cell is the same as the


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i-1][j+1] == false)
            {
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
                        cellDetails[i-1][j+1].f < fNew) && fNew <= parada && hNew != -9999)
                {
                    openList.insert( make_pair (fNew,
                                    make_pair(i-1, j+1)));
                    pq.push( make_pair (fNew,
                                                        make_pair(i-1, j+1)));
                    //cout << "insert openLIst " << fNew << " " << i-1 << ", " << j+1 << endl;
                    // Update the details of this cell
                    cellDetails[i-1][j+1].f = fNew;
                    cellDetails[i-1][j+1].g = gNew;
                    cellDetails[i-1][j+1].h = hNew;
                    cellDetails[i-1][j+1].parent_i = i;
                    cellDetails[i-1][j+1].parent_j = j;
                    cout << "NE inserté cellDetails[" << i-1 << "][" << j+1 << " = " << i << " " << j << " " << fNew <<  endl;
                }
            }
        }

        //----------- 6th Successor (North-West) ------------

        // Only process this cell if this is a valid one
        if (isValid (i-1, j-1) == true)
        {
            // If the destination cell is the same as the
            // current successor

            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i-1][j-1] == false)
            {
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
                        cellDetails[i-1][j-1].f < fNew) && fNew <= parada && hNew != -9999)
                {
                    openList.insert( make_pair (fNew, make_pair (i-1, j-1)));
                    pq.push(make_pair (fNew, make_pair (i-1, j-1)));
                    // Update the details of this cell
                    cellDetails[i-1][j-1].f = fNew;
                    cellDetails[i-1][j-1].g = gNew;
                    cellDetails[i-1][j-1].h = hNew;
                    cellDetails[i-1][j-1].parent_i = i;
                    cellDetails[i-1][j-1].parent_j = j;
                    cout << "NW inserté cellDetails[" << i-1 << "][" << j-1 << " = " << i << " " << j << " " << fNew <<  endl;
                }
            }
        }

        //----------- 7th Successor (South-East) ------------

        // Only process this cell if this is a valid one
        if (isValid(i+1, j+1) == true)
        {
            // If the destination cell is the same as the
            // current successor


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i+1][j+1] == false)
            {
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
                        cellDetails[i+1][j+1].f < fNew) && fNew <= parada && hNew != -9999)
                {
                    openList.insert(make_pair(fNew,
                                        make_pair (i+1, j+1)));
                    pq.push(make_pair(fNew,
                                                            make_pair (i+1, j+1)));
                    // Update the details of this cell
                    cellDetails[i+1][j+1].f = fNew;
                    cellDetails[i+1][j+1].g = gNew;
                    cellDetails[i+1][j+1].h = hNew;
                    cellDetails[i+1][j+1].parent_i = i;
                    cellDetails[i+1][j+1].parent_j = j;
                    cout << "SE inserté cellDetails[" << i+1 << "][" << j+1 << " = " << i << " " << j << " " << fNew <<  endl;
                }
            }
        }

        //----------- 8th Successor (South-West) ------------

        // Only process this cell if this is a valid one
        if (isValid (i+1, j-1) == true)
        {
            // If the destination cell is the same as the
            // current successor


            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[i+1][j-1] == false)
            {
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
                        cellDetails[i+1][j-1].f < fNew) && fNew <= parada && hNew != -9999)
                {
                    openList.insert(make_pair(fNew,
                                        make_pair(i+1, j-1)));
                    pq.push(make_pair(fNew,
                                                            make_pair(i+1, j-1)));
                    // Update the details of this cell
                    cellDetails[i+1][j-1].f = fNew;
                    cellDetails[i+1][j-1].g = gNew;
                    cellDetails[i+1][j-1].h = hNew;
                    cellDetails[i+1][j-1].parent_i = i;
                    cellDetails[i+1][j-1].parent_j = j;
                    cout << "SW inserté cellDetails[" << i+1 << "][" << j-1 << " = " << i << " " << j << " " << fNew <<  endl;
                }
            }
        }



    }

    printPath(maxCost_shortest_path);
    // When the destination cell is not found and the open
    // list is empty, then we conclude that we failed to
    // reach the destiantion cell. This may happen when the
    // there is no way to destination cell (due to blockages)
    //if (foundDest == false)
        //printf("Failed to find the Destination Cell\n");

    return;
}


// Driver program to test above function
int main()
{
    /* Description of the Grid-
     1--> The cell is not blocked
     0--> The cell is blocked    */


    int grid[ROW][COL] =
        {
            { 1, 2, 1, 1, 1, 1, 3, 1, 1, 1 },
            { 1, 1, 1, 3, 1, 1, 1, 5, 1, 1 },
            { 1, 1, 1, 6, 1, 1, 6, 1, 4, 1 },
            { 7, 4, 1, 5, 1, 7, 3, 5, 7, 1 },
            { 1, 1, 1, 8, 1, 1, 1, 5, 1, 2 },
            { 1, 0, 1, 0, 1, 1, 3, 1, 2, 3 },
            { 0, 3, 5, 2, 6, 1, 1, 9, 6, 1 },
            { 2, 5, -9999, 1, 1, 1, 3, 1, 1, 1 },
            { 1, 1, 0, 3, 6, 8, 1, 8, 0, 1 }
        };

    // Source is the left-most bottom-most corner
    Pair src = make_pair(8, 0);

    // Destination is the left-most top-most corner
    //Pair dest = make_pair(0, 0);
    int stop = 20;
    aStarSearch(grid, src, stop);

    return(0);
}
