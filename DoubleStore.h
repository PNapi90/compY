/*
 * DoubleStore.h
 *
 *  Created on: 18.10.2018
 *      Author: philipp
 */

#ifndef DOUBLESTORE_H_
#define DOUBLESTORE_H_

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>

#include "Geometry_Checker.h"

class DoubleStore {

private:

	const int mult_vals[3] = {-1,1,-1};

	bool centroids_set;

	int pos;
	std::vector<int> lens;

	std::vector<std::vector<double> > centroids;
	double *return_arr;

	Geometry_Checker* Geo;

	std::ofstream centroid_File;

	std::vector<double> centroid_E;
	std::vector<std::vector<std::vector<double> > > StoredGammas;


	void SetCentroids();

public:
	DoubleStore();
	~DoubleStore();

	void Reset();
	void SetGamma(std::vector<std::vector<double> > &Gamma,int len);

	bool CheckSetCentroids();
	double* Compare();
};

#endif /* DOUBLESTORE_H_ */
