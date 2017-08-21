/*
 * main.cpp
 *
 *  Created on: 17/08/2017
 *      Author: lanase
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <vector>

#include "DisplayImage.cpp"
#include "distance.cpp"

int main(){
	Display_image di;
	float** costos = di.tiff_to_matrix("mytif.tif");

	cost_distance d;
	d.COLS = di.get_imageWidth("mytif.tif");
	d.ROWS = di.get_imageHeight("mytif.tif");
	d.inicio_cost_distance(costos);

}



