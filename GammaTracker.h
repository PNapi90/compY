/*
 * GammaTracker.h
 *
 *  Created on: 15.10.2018
 *      Author: philipp
 */

#ifndef GAMMATRACKER_H_
#define GAMMATRACKER_H_

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <thread>
#include <algorithm>
#include <vector>
#include <random>

#include "MC_Sampler.hpp"

class GammaTracker {

private:

	const char* format = "%lf %lf %lf %lf %lf %d %d";
	const char* formatDouble = "%lf %lf %lf %lf %lf %d";
	const char* formatGANIL = "%d %lf %lf %lf %lf %lf";
	const double source_vec[3] = {0,0,0};
	const double mc2 = 511.;
	const double WRONG_CASE = 100000;
	const double nBinsD = 300;
	const double nBinsT = 180.;

	MC_Sampler* MC;

	bool MC_Calc,ForceMode,GANIL,OFT,DirectOutput,HeaderWritten;
	int binningFactor,order;

	double sigma,lambdaE;
	double thetaX;

	double Ecompton_max,min_delta;
	
	std::vector<std::vector<double> > GammaBuffer;
	std::vector<std::vector<std::vector<double> > > GammaBufferDouble;

	std::vector<std::vector<double> > mu_vec;
	std::vector<std::vector<double> > angle_vec,angle_save;
	std::vector<double> sigmas,delta_arr,Egamma_D,sigma_Eth;
	std::vector<int> range,binsArray;
	
	std::default_random_engine generator;
	std::uniform_real_distribution<double> UNI;


	std::ofstream OUTFILE,dep_file,DIRECT;

	int len,len_orig,NOriginal,thr_num,ID;
	std::vector<int> len_D,len_orig_D,NOriginal_D;


	bool type,Track;
	double FWHM,delta_X,Egamma;
	double MAX_TRACK,MAX_ITER;

	void LOAD();
	void LOAD_GANIL();
	void LOAD_Double();

	void SetBuffer(int pos);
	void WRITE(int iter);
	void WRITE_DIRECT(int iter);
	void PrintDataSet(int iter);
	void get_sigmas(int,int);
	void get_E_angle(double,double,int);
	void GetSigmaE(std::vector<double>&,int);


	bool Tracking(int,int);
	bool Check_Doppler(double);

	inline int Factorial(int);
	inline double Gaussian(double);
	inline double Exponential(double);
	inline bool BinCheck(std::vector<double> &Tmp);
	inline bool CheckGammaBadness(int iter);
	inline std::string GetEnding(int i);



public:
  GammaTracker(std::vector<int> &range,
			   bool type,
			   double FWHM,
			   int MAX_ITER,
			   double MAX_TRACK,
			   int thr_num,
			   bool Track,
			   MC_Sampler *MC,
			   bool _MC_Calc,
			   int _order,
			   bool _ForceMode,
			   bool _GANIL,
			   bool _OFT,
			   bool _DirectOutput);
  ~GammaTracker();

  std::thread threading();
};

#endif /* GAMMATRACKER_H_ */
