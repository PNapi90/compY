/*
 * Merger.h
 *
 *  Created on: 22.10.2018
 *      Author: philipp
 */

#ifndef MERGER_H_
#define MERGER_H_

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

class Merger {

private:
	int am_thr,am_lines,from,to,fileOffset;
	int merged;
	double FWHM;
	bool type;

public:
	Merger(int am_thr,
		   bool type,
		   std::vector<int> &from_To,
		   int fileOffset,
		   double _FWHM);
	~Merger();

	void LOAD();
	void print_am();
};

#endif /* MERGER_H_ */
