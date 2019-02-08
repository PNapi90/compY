/*
 * Merger.cpp
 *
 *  Created on: 22.10.2018
 *      Author: philipp
 */

#include "Merger.h"

//--------------------------------------------------------------

Merger::Merger(int am_thr,
			   bool type,
			   std::vector<int> &from_To,
			   int fileOffset,
			   double _FWHM)
	: FWHM(_FWHM)
{
	this->am_thr = am_thr;
	this->type = type;
	this->from = from_To[0];
	this->to = from_To[1];
	this->fileOffset = fileOffset;
	am_lines = 0;
	merged = 0;
	LOAD();
}

//--------------------------------------------------------------

Merger::~Merger() 
{}

//--------------------------------------------------------------

void Merger::LOAD()
{
	std::string name,line;
	std::string tmpName = type ? "Double" : "";
	std::ifstream data;
	std::vector<double> x(4,0);
	std::vector<int> d(2,0);
	am_lines = 0;

	std::ofstream out("OutputFolder/Output_FWHM_"+std::to_string(FWHM)+"_"+tmpName+std::to_string(from)+"_" + std::to_string(to));

	for(int i = 0;i < am_thr;++i){
		name = "Stored/Bad_Gammas/GammaFalse"+tmpName+"_dists_"+std::to_string(i+fileOffset)+"_"+std::to_string(i+1+fileOffset);
		data.open(name);
		if(data.fail()){
			std::cerr << "Could not find " << name << std::endl;
			exit(1);
		}
		while(std::getline(data,line,'\n')){
			std::sscanf(line.c_str(),"%lf %lf %lf %lf %d %d",&x[0],&x[1],&x[2],&x[3],&d[0],&d[1]);
			++am_lines;
			if(d[1] > d[0]) ++merged;
			for(int j = 0;j < 4;++j) out << x[j] << " ";
			for(int j = 0;j < 2;++j) out << d[j] << " ";
			out << std::endl;
		}
		data.close();
		data.clear();
		std::cout << "\r";
		std::cout << "File # " << i+1 << "/" << am_thr << " written\t\t\t\t";
		std::cout.flush();
	}
	std::cout << std::endl;

	out.close();
}

//--------------------------------------------------------------

void Merger::print_am()
{

	double Rel = ((double) merged)/((double) am_lines);

	std::cout << "Remaining \u03B3s -> " << am_lines << std::endl;
	std::cout << "Merged -> " << merged << std::endl;
	std::cout << "Relative -> " << Rel << std::endl;
}

//--------------------------------------------------------------