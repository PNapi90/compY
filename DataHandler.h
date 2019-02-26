/*
 * DataHandler.h
 *
 *  Created on: 15.10.2018
 *      Author: philipp
 */

#ifndef DATAHANDLER_H_
#define DATAHANDLER_H_

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <random>
#include <thread>

class DataHandler {

private:

	const char* format = "%lf %lf %lf %lf %lf %d";
	const int max_len = 40;

	const std::string TmpLine = "-1 661.7 0 0 0 0";

	std::string GammaLine,oldLine;

	double sigma_X,FWHM,sigma_Scale,CRange;
	int gammaID;
	bool type,NoG,GANIL;
	int gamma_iter,m_iter,double_gam_counter,thr_num,tmpIter;

	int PhotonID;

	int maxG,am_GammasFull,SMEAR;

	bool OFT,Mimic;

	double E0;

	std::vector<int> range,gamma_iter2,TmpGammaLen,DetIDs;
	std::vector<double> E0_2;
	std::vector<std::vector<double>> Gamma, MergedData;
	std::vector<std::vector<int>> Signs;
	std::vector<std::vector<std::vector<double> > > Gamma2,TmpGamma;

	std::ofstream majorFile,Etot_file,max_file,gammaTmpFile;
	std::default_random_engine generator;

	std::uniform_real_distribution<double> UNI,UNI_C;
	std::normal_distribution<double> GaussX;



	void LOAD();
	void LOAD_Double();
	void LOAD_GANIL();


	void ResetGammaBuffer();
	void ResetGammaBuffer2(bool);
	void MergeGammaAndSave();
	void SaveMerge();
	void SetFileEnding();
	void SaveTmp();
	void saveTMP();
	
	inline double Gaussian(double);
	inline bool MergeChecker(double);
	inline bool ConvolSuppr();
	inline std::string EndingName(int);

public:
    DataHandler(std::vector<int> &range,
			    bool type,
			    double FWHM,
			    int thr_num,
			    int maxG,
			    unsigned int SEED,
			    bool NoG,
			    double CRange,
			    int SMEAR,
			    bool GANIL,
				bool _OFT,
				bool _Mimic);
    ~DataHandler();

    std::thread threading();

};

#endif /* DATAHANDLER_H_ */
