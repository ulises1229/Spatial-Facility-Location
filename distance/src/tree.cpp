#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <queue>
#include <stdio.h>
//#include "TreeNode.cpp"
using namespace std;


class TreeNode{
    private:
		int id;
		float acum;
        int x, y;

        TreeNode *parent;

        std::vector<TreeNode *> children;

    public:
		TreeNode() {};

		TreeNode(int id, float acum,int x, int y) :
			id(id),
			acum(acum),
			x(x),
			y(y),
			parent(NULL)
		{}

		int countNodesRec(TreeNode *root, int& count)
		{
			TreeNode *parent = root;
			TreeNode *child = NULL;

			for(int it = 0; it < parent->childrenNumber(); it++)
			{
				child = parent->getChild(it);
				count++;
				//std::cout<< child->getTextContent() << ", " << child->getTagName()<<  " Number : "<<count<<std::endl;
				if(child->childrenNumber() > 0)
				{
					countNodesRec(child, count);
				}
			}

			return count;
		}

		void appendChild(TreeNode *child)
		{
			child->setParent(this);
			children.push_back(child);
		}

		void setParent(TreeNode *theParent)
		{
			parent = theParent;
		}

		void popBackChild()
		{
			children.pop_back();
		}

		void removeChild(int pos)
		{
			if(children.size() > 0) {
				children.erase(children.begin()+ pos);
			}
			else {
				children.pop_back();
			}
		}

		bool hasChildren()
		{
			if(children.size() > 0)
				return true;
			else
				return false;
		}

		bool hasParent()
		{
			if(parent != NULL)
				return true;
			else
				return false;
		}

		TreeNode * getParent()
		{
			return parent;
		}

		TreeNode* getChild(int pos)
		{
			if(children.size() < pos)
				return NULL;
			else
				return children[pos];
		}

		int childrenNumber()
		{
			return children.size();
		}

		int grandChildrenNum()
		{
			int t = 0;

			if(children.size() < 1)
			{
				return 0;
			}

			countNodesRec(this, t);

			return t;
		}

		int getX()
		{
			return x;
		}

		int getY()
		{
			return y;
		}

		float getAcum()
		{
			return acum;
		}

		int getId()
		{
			return id;
		}

		TreeNode* search(int id){
			for(int i = 0; i < children.size(); i++){
				if(children[i]->getId() == id)
					return children[i];
			}
			return NULL;
		}
};


struct Node{
	int x, y;
};


class rutas_nodos{
public:
	stack<TreeNode*> Path;
	float** grid;
	int ROW, COL;
	int** matrix_path;

	bool isValid(int row, int col){
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

void getPath(TreeNode* dest){
	if(dest->hasParent()){
		Path.push(dest->getParent());
		getPath(dest->getParent());
	}
}

void printPath(){
	while (!Path.empty()){
		printf("-> (%d,%d) ",Path.top()->getX(), Path.top()->getY());
		Path.pop();
	}
	cout << endl;
}

void printMatrix(stack <TreeNode*> s){
	this->matrix_path = new int*[this->ROW];
	for(int i = 0; i< ROW; ++i){
		this->matrix_path[i] = new int[COL];
		for(int j = 0; j < COL ; j++){
			matrix_path[i][j] = 0;
		}
	}
	//fill(&matrix_path[0][0], &matrix_path[0][0] + sizeof(matrix_path) / sizeof(matrix_path[0][0]), 0);

	while (!s.empty()){
		matrix_path[s.top()->getX()][s.top()->getY()] = 1;
		//cout << matrix_path[s.top()->getX()][s.top()->getY()] << endl;
		s.pop();
	}
	/*for(int i = 0; i < ROW; i++){
		for(int j = 0; j < COL; j++)
			cout << matrix_path[i][j] << "\t";
		cout << endl;
	}*/

}

void ruta(TreeNode* node_dest){
	Path.push(node_dest);
	getPath(node_dest);
	stack<TreeNode*> m = Path;
	printPath();
	printMatrix(m);
}

void rutas(Node src, float stop){
	int x,y;
	int dx[] = { -1, -1, 0, 1, 1,  1,  0,-1 };
	int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

	map<int, Node> openList;
	openList[1].x = src.x;
	openList[1].y = src.y;
	queue <TreeNode*> fila;

	TreeNode *node = new TreeNode(1, grid[src.x][src.y],src.x, src.y);
	TreeNode *tmp;

	int id = 2;
	int id_child = 0;
	TreeNode *node_dest;
	while(!openList.empty()){
		map <int, Node> :: iterator itr;//iterador al mapa
		itr = openList.begin();
		x = itr->second.x;
		y = itr->second.y;

		openList.erase(openList.begin());
		for(int k = 0; k < 8; k++){
			int i = x + dx[k];
			int j = y + dy[k];
			if(node->hasParent()){
				if (isValid(i, j) == true && (i != src.x || j != src.y) && grid[i][j] != -9999 && (node->getParent()->getX() != i || node->getParent()->getY() != j)){
					node->appendChild(new TreeNode(id, node->getAcum() + grid[i][j], i, j));
					//cout << "acum = " << node->getAcum() + grid[i][j] << endl;
					//if(stop - 100 <= (node->getAcum() + grid[i][j]))
						if(node->getAcum() + grid[i][j] >= stop){
							cout << "acum = " << node->getAcum() + grid[i][j] << endl;
							node_dest = node->getChild(id_child);
							goto finish;
						}

					fila.push(node->getChild(id_child));

					openList[id].x = i;
					openList[id].y = j;
					id++;
					id_child++;
				}
			}
			else{//ORIGEN
				if (isValid(i, j) == true && (i != src.x || j != src.y) && grid[i][j] != -9999 ){
					node->appendChild(new TreeNode(id, node->getAcum() + grid[i][j], i, j));
					//if(stop - 100 <= (node->getAcum() + grid[i][j]))
						if(node->getAcum() + grid[i][j] >= stop){
							node_dest = node->getChild(id_child);
							goto finish;
						}
					fila.push(node->getChild(id_child));

					openList[id].x = i;
					openList[id].y = j;
					id++;
					id_child++;
				}
			}

		}
		id_child = 0;

		if(!fila.empty()){
			tmp = fila.front();
			fila.pop();
			node = tmp;
		}
	}
	finish:
		cout << "finish" << endl;
		ruta(node_dest);
}




void inicio_rutas(float** grid, int srcX, int srcY, float stop) {
	this->grid = new float*[this->ROW];
	for(int i = 0; i< ROW; ++i)
		this->grid[i] = new float[COL];

	for(int x = 0; x < ROW; x++){
		for(int y = 0; y < COL; y++){
			this->grid[x][y] = grid[x][y];
		}
	}


	Node src;
	src.x = srcX; src.y = srcY;

	rutas(src, stop);

}
};
