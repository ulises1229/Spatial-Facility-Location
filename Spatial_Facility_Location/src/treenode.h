/*
 * treenode.h
 *
 *  Created on: 12/12/2017
 *      Author: lanase
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <queue>
#include <stdio.h>
#include <algorithm>
#include <list>

class TreeNode{
    private:
		int id;
		float acum;
        int x, y;

        TreeNode* parent;

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

		TreeNode* getParent()
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

		void setX(int x) {
			this->x = x;
		}

		int getY()
		{
			return y;
		}

		void setY(int y) {
			this->y = y;
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
