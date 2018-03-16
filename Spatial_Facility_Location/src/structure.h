/*
 * structure.h
 *
 *  Created on: 12/12/2017
 *      Author: lanase
 */


#ifndef STRUCTURE_H_
#define STRUCTURE_H_


struct cell_exp{
	int parent_i, parent_j;
	double f, g, h;
};

struct Prop{
	int srcX, srcY, size;
	float biomass, total_cost;
};



#endif /* STRUCTURE_H_ */
