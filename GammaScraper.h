/*
 * GammaScraper.h
 *
 *  Created on: 17.10.2018
 *      Author: philipp
 */

#ifndef GAMMASCRAPER_H_
#define GAMMASCRAPER_H_


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <thread>
#include <algorithm>
#include <vector>

#include "fuzzy_c_means.h"
#include "DoubleStore.h"
#include "Geometry_Checker.h"

class GammaScraper {

private:

	bool type,NoG;
	int iter,in_bad_dets,thr_num,ID;

	const char* format = "%lf %lf %lf %lf %lf %lf %lf %d %d %d %lf";
	const int mult_vals[4] = {1,-1,1,-1};

	std::vector<int> range,lens;
	std::vector<std::vector<double> > GammaBuffer;

	double **centroids,*set_values;
	double delta_arr;

	fuzzy_c_means* fc_means;
	Geometry_Checker* Geo;
	DoubleStore* Store;

	std::ofstream SaveFile,tmp_File,Save2;

	void LOAD();
	void LOAD_Double();
	void Save2Ints();
	void StartComparison();
	void tmpFileWrite(double,double);

public:
	GammaScraper(std::vector<int> &range,
				 bool type,
				 int thr_num,
				 bool NoG);
	~GammaScraper();

	std::thread threading();

	int get_in_bad_dets();
};

#endif /* GAMMASCRAPER_H_ */
