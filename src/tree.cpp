#include "tree.h"

using namespace std;


bool Tree::isValid(int row, int col){
	return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

void Tree::printPath(vector<nodeInfo> nodes, ofstream& info){
	float sum = 0, cost = 0;
	int totNodes = 0;
	for (it = nodes.begin(); it != nodes.end(); it++) {
		sum += it->value;
		if(it->fricc > 0)
			cost += it->fricc;//it->value / it->fricc;
		totNodes++;
	}
	cout << fixed << "Biomass: " << sum << "  Cost: " << cost << "  Nodes: " << totNodes << endl;
	info << totNodes << ",";
	info << sum << ",";
	info << cost << endl;
	this->cost = cost;
}

void Tree::printMatrix(vector<nodeInfo> nodes, Node src){
	this->x = src.x; this->y = src.y;
	this->matrix_path = new float*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->matrix_path[i] = new float[COL];
		for(int j = 0; j < COL ; j++){
			matrix_path[i][j] = -9999;
		}
	}

	for (vector<nodeInfo>::iterator it = nodes.begin(); it != nodes.end(); it++) {
		//if(it->fricc > 0)
		matrix_path[it->x][it->y] = it->fricc;
	}
}

void Tree::getExplorationType(vector<nodeInfo> children, char heuristic) {
	float sumRel = 0, median = 0, sumStd = 0, stdDev = 0, dataRange = 0, partitions = 0, max = 0, min = 0;
	int i = 0;
	vector<nodeInfo>::iterator auxIt;

	auxIt = --(children.end());
	max = auxIt->relation;
	min = children.begin()->relation;

	dataRange = max - min;

	partitions = dataRange / children.size();

	for (vector<nodeInfo>::iterator it = children.begin(); it != children.end(); it++) {
		sumRel += it->relation;
		//cout << i << ".- " << it->relation << endl;
		//i++;
	}
	median = sumRel / children.size();
	for (vector<nodeInfo>::iterator it = children.begin(); it != children.end(); it++) {
		sumStd += pow(abs(it->relation - median), 2);
	}
	stdDev = sqrt(sumStd / children.size());
	if(stdDev > (median / 2)) {
		if(heuristic == 'b') {
			for (vector<nodeInfo>::iterator it = --(children.end()); it != children.begin(); it--) {
				bool exp = true;
				if(i > 1)
					exp = false;
				sortedNodes.insert(nodeInfo(it->value, it->fricc, it->relation, it->x, it->y, it->id, exp, 0, it->heuristic));
				i++;
			}
		} else if(heuristic == 'a') {
			for (vector<nodeInfo>::iterator it = --(children.end()); it != children.begin(); it--) {
			if(it->relation >= median)
				sortedNodes.insert(nodeInfo(it->value, it->fricc, it->relation, it->x, it->y, it->id, true, 0, it->heuristic));
			}
		} else {
			for (vector<nodeInfo>::iterator it = children.begin(); it != children.end(); it++) {
				sortedNodes.insert(nodeInfo(it->value, it->fricc, it->relation, it->x, it->y, it->id, true, 0, it->heuristic));
			}
		}

	} else {
		for (vector<nodeInfo>::iterator it = children.begin(); it != children.end(); it++) {
			sortedNodes.insert(nodeInfo(it->value, it->fricc, it->relation, it->x, it->y, it->id, true, 0, it->heuristic));
		}
	}
}

void Tree::ruta(vector<nodeInfo> nodes, Node src, ofstream& info){
	printPath(nodes, info);
	printMatrix(nodes, src);
	nodes.clear();
}

void Tree::rutas(Node src, float stop, ofstream& info, char heuristic){
	cout << "Finding route..." << endl;
	int x,y;
	int dx[] = { -1, -1, 0, 1, 1,  1,  0,-1 };
	int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

	//vector<findElement> visitedNodes;
	vector<nodeInfo> path;
	TreeNode *node = new TreeNode(1, biomass[src.x][src.y], src.x, src.y);
	int id = 2;
	int id_child = 0;
	extern char is_usable;
	float totalAcum, costoTot = friction[src.x][src.y], heuristicResult = 0;
	TreeNode *node_dest;
	if(is_usable == 'n'){
		totalAcum = biomass[src.x][src.y] / 40;
		path.push_back(nodeInfo(biomass[src.x][src.y] / 40, friction[src.x][src.y], 0.0, src.x, src.y, 1, true, 0, 0));
	}
	else{
		totalAcum = biomass[src.x][src.y];
		path.push_back(nodeInfo(biomass[src.x][src.y], friction[src.x][src.y], 0.0, src.x, src.y, 1, true, 0, 0));
	}


	//path.push_back(nodeInfo(biomass[src.x][src.y] / 40, friction[src.x][src.y], 0.0, src.x, src.y, 1, true, 0, 0));
	//visitedNodes.push_back(findElement(src.x, src.y));
	visitedNodes[src.x][src.y] = true;
	sortedNodes.insert(nodeInfo(biomass[src.x][src.y], friction[src.x][src.y], 0.0, src.x, src.y, id, true, 0, 0));
	float relation = 0;
	while(!sortedNodes.empty()){
		x = node->getX();
		y = node->getY();
		vector<nodeInfo> localChildren;
		for(int k = 0; k < 8; k++){
			int i = x + dx[k];
			int j = y + dy[k];
			if (heuristic == 'e') {
				heuristicResult = sqrt(pow((src.x - i), 2) + pow((src.y - j), 2));
			}
			else if (heuristic == 'm') {
				heuristicResult = abs(src.x - i) + abs(src.y - j);
			}
			else if (heuristic == 'd'){
				heuristicResult = max(abs(src.x - i), abs(src.y - j));
			}
			if(node->hasParent()){
				if (isValid(i, j) == true && (i != src.x || j != src.y) && biomass[i][j] != -9999
						&& (node->getParent()->getX() != i || node->getParent()->getY() != j)){
					//relation = (biomass[i][j] + heuristicResult) / friction[i][j] ;
					if(heuristicResult > 0)
						relation = biomass[i][j] / (friction[i][j] + heuristicResult) ;
						//relation = (biomass[i][j] / friction[i][j]) + (biomass[i][j] / heuristicResult);
					else
						relation = biomass[i][j] / friction[i][j];
					//sortedNodes.push_back(nodeInfo(biomass[i][j], friction[i][j], relation, i, j, id));
					localChildren.push_back(nodeInfo(biomass[i][j], friction[i][j], relation, i, j, id, true, 0, heuristicResult));
					id++;
				}
			}
			else{//ORIGEN
				if (isValid(i, j) == true && (i != src.x || j != src.y) && biomass[i][j] != -9999){
					if(heuristicResult > 0)
						relation = biomass[i][j] / (friction[i][j] + heuristicResult) ;
						//relation = (biomass[i][j] / friction[i][j]) + (biomass[i][j] / heuristicResult);
					else
						relation = biomass[i][j] / friction[i][j];

					//sortedNodes.push_back(nodeInfo(biomass[i][j], friction[i][j], relation, i, j, id));
					localChildren.push_back(nodeInfo(biomass[i][j], friction[i][j], relation, i, j, id, true, 0, heuristicResult));
					id++;
				}
			}
		}
		sort(localChildren.begin(), localChildren.end());
		getExplorationType(localChildren, heuristic);
		//sort(sortedNodes.begin(), sortedNodes.end());
		set<nodeInfo>::iterator itSorted;
		//vector<findElement>::iterator itAux;
		for (itSorted = --(sortedNodes.end()); itSorted != sortedNodes.begin(); --itSorted) {
			if(visitedNodes[itSorted->x][itSorted->y] == true) {
				sortedNodes.erase(itSorted);
			}
			else {
				break;
			}
		}

		int x = itSorted->x;
		int y = itSorted->y;
		int last_id = itSorted->id;
		float usable_b;
		//cout << biomass[x][y] << "  -  " << friction[x][y] << endl;
		if (biomass[x][y] > 0 && friction[x][y] >= 0) {
			node->appendChild(new TreeNode(last_id, node->getAcum() + biomass[x][y], x, y));
			if(visitedNodes[x][y] == false) {
				if(is_usable == 'n')
					usable_b = biomass[x][y] / 40;
				else
					usable_b = biomass[x][y];
				totalAcum += usable_b; //biomass[x][y];
				costoTot += usable_b / friction[x][y];
				//cout << totalAcum << endl;
				path.push_back(nodeInfo(usable_b, friction[x][y], itSorted->relation, x, y, last_id, true, totalAcum, itSorted->heuristic));
				if( totalAcum >= stop){
					node_dest = node->getChild(id_child);
					//visitedNodes.clear();
					sortedNodes.clear();
					localChildren.clear();
					delete(node);
					goto finish;
				}

			} else {
				cout << "visited" << endl;
			}
			id_child++;
		}

		if(itSorted->expandable == false) {
			set<nodeInfo>::iterator nodeIt;
			if(sortedNodes.size() == 1) {
				nodeIt = sortedNodes.begin();
				if(nodeIt->expandable == false) {
					sortedNodes.erase(nodeIt);
					goto void_vector;
				}
			}
			for (nodeIt = --(sortedNodes.end()); nodeIt != sortedNodes.begin(); --nodeIt) {
				//nodeIt = find_if(visitedNodes.begin(), visitedNodes.end(), findElement(nodeIt->x, nodeIt->y));
				if (nodeIt->expandable == true && (nodeIt->x != x && nodeIt->y != y)){
					break;
				}
			}
			node->setX(nodeIt->x);
			node->setY(nodeIt->y);
			//visitedNodes.push_back(findElement(itSorted->x, itSorted->y));
			visitedNodes[nodeIt->x][nodeIt->y] = true;
			visitedNodes[itSorted->x][itSorted->y] = true;
			sortedNodes.erase(itSorted);
			sortedNodes.erase(nodeIt);
		} else {
			node->setX(itSorted->x);
			node->setY(itSorted->y);
			visitedNodes[itSorted->x][itSorted->y] = true;
			sortedNodes.erase(itSorted);
		}
	}
	cout << "No More Elements" << endl;
	if (!sortedNodes.empty()) {
		finish:
				ruta(path, src, info);
	} else {
		void_vector:
				cout << "No More Elements" << endl;
		ruta(path, src, info);
	}

}

void Tree::freeMem() {
	for(int m=0; m < this->ROW; m++){
		delete[] this->biomass[m];
		delete[] this->friction[m];
		delete[] this->visitedNodes[m];
		delete[] this->matrix_path[m];
	}
}

void Tree::inicio_rutas(float** grid, float** cost_dist, int srcX, int srcY, float stop, ofstream& info, char heuristic) {
	this->biomass = new float*[this->ROW];
	this->friction = new float*[this->ROW];
	this->visitedNodes = new bool*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->biomass[i] = new float[COL];
		this->friction[i] = new float[COL];
		this->visitedNodes[i] = new bool[COL];
	}

	for(int x = 0; x < ROW; x++){
		for(int y = 0; y < COL; y++){
			this->biomass[x][y] = grid[x][y];
			this->friction[x][y] = cost_dist[x][y];
			this->visitedNodes[x][y] = false;
		}
	}

	Node src;
	src.x = srcX; src.y = srcY;

	rutas(src, stop, info, heuristic);
}
