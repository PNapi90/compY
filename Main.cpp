/*
 * Main.cpp
 *
 *  Created on: 15.10.2018
 *      Author: philipp
 */

#include <iostream>
#include <string>
#include <thread>
#include <fstream>


#include "DataHandler.h"
#include "GammaTracker.h"
#include "GammaScraper.h"
#include "Merger.h"
#include "MC_Sampler.hpp"
#include "Binnings.hpp"


//--------------------------------------------------------------

void FlagChecker(int argc,char** argv,int &nthr,int &amount_of_sets,
				int &offset,double &CRange,int &maxG,double &fwhm,
				bool &fwhm_flag,bool &type,bool &thr_flag,bool &file_flag,
				bool &track_flag,bool &offset_flag,bool &Tracking,
				bool &SkipHandler,bool &SkipTracker,bool &maxG_B,bool &NoG,
				bool &CRange_F,double &MAX_TRACK,bool &helpCalled,bool &Smear,
				bool &MC_Calc,int &order,bool &Force,bool &GANIL);

void PrintCouts(const double fwhm, const bool type, const int nthr,
				const int offset, const double mtrack, const bool Tracking,
				const bool SkipHandler, const bool SkipTracker, const int maxG,
				const bool NoG, const double CRange, const bool Smear,
				const bool MC_Calc, const int order, const bool Force,
				const bool GANIL);

void PrintHelp();

void getSets(int*,const int,const int);

void SetBins(Binnings &Bins);

//--------------------------------------------------------------

int main(int argc, char **argv) 
{

	int nthr = 1;
	int amount_of_sets = 1;
	int offset = 0;
	
	double CRange = 1;

	int maxG = 1000000000;

	double fwhm = 5.;
	bool fwhm_flag = false;
	bool type = false;
	bool thr_flag = false;
	bool file_flag = false;
	bool track_flag = false;
	bool offset_flag = false;
	bool Tracking = false;
	bool SkipHandler = false;
	bool SkipTracker = false;
	bool maxG_B = false;
	bool NoG = true;
	bool CRange_F = false;
	bool helpCalled = false;
	bool Smear = true;
	bool MC_Calc = false;
	int order = 1;
	bool Force = true;
	bool GANIL = false;
	
	double MAX_TRACK = 3.;

	FlagChecker(argc,argv,nthr,amount_of_sets,offset,CRange,maxG,fwhm,
				fwhm_flag,type,thr_flag,file_flag,track_flag,
				offset_flag,Tracking,SkipHandler,SkipTracker,
				maxG_B,NoG,CRange_F,MAX_TRACK,helpCalled,Smear,MC_Calc,
				order,Force,GANIL);

	if(helpCalled) return 0;

	if(!Force) MC_Calc = true;
	
	
	SkipHandler = SkipTracker ? SkipTracker : SkipHandler;

	//no handling of data needed for GANIL data set
	//(already preprocessed)
	if(GANIL)
	{
		//SkipHandler = true;
		nthr = 1;
		Smear = false;
		CRange = 0;
		//Tracking = true;
	}

	PrintCouts(fwhm,type,nthr,offset,MAX_TRACK,Tracking,
			   SkipHandler,SkipTracker,maxG,NoG,CRange,
			   Smear,MC_Calc,order,Force,GANIL);


	int* sets = new int[2];

	if(nthr >= amount_of_sets) nthr = amount_of_sets;


	getSets(sets,nthr,amount_of_sets);


	int MAX_ITER = 6;
	

	int set_begin = 0;
	int set_end = 0;
	int* from_to = new int[2];
	for(int i = 0;i < 2;++i) from_to[i] = 0;

	DataHandler** Handlers = (SkipHandler) ? nullptr : new DataHandler*[nthr];
	GammaTracker** Tracker = (SkipTracker) ? nullptr : new GammaTracker*[nthr];
	GammaScraper** Scraper = new GammaScraper*[nthr];

	//set binnings 
	Binnings Bins;
	SetBins(Bins);

	Bins.MC_Calc = MC_Calc;
	Bins.sigmaX = fwhm;

    MC_Sampler* MC = new MC_Sampler(Bins);

	for(int i = 0;i < nthr-1;++i){
		set_begin = from_to[1];
		set_end = from_to[1] + sets[0];
		from_to[0] = set_begin;
		from_to[1] = set_end;
		if(i == 0) for(int j = 0;j < 2;++j) from_to[j] += offset;
		if(!SkipHandler) Handlers[i] = new DataHandler(from_to,type,fwhm,i,maxG,((unsigned int) i),NoG,CRange,((int)Smear*1),GANIL);
		if(!SkipTracker) Tracker[i] = new GammaTracker(from_to,type,fwhm,MAX_ITER,MAX_TRACK,i,Tracking,MC,MC_Calc,order,Force,false);
		Scraper[i] = new GammaScraper(from_to,type,i,NoG);
	}


	from_to[0] = from_to[1];
	from_to[1] = from_to[0] + sets[1];
	if(!SkipHandler) Handlers[nthr-1] = new DataHandler(from_to,type,fwhm,nthr-1,maxG,(unsigned int) nthr-1,NoG,CRange,((int)Smear*1),GANIL);
	if(!SkipTracker) Tracker[nthr-1] = new GammaTracker(from_to,type,fwhm,MAX_ITER,MAX_TRACK,nthr-1,Tracking,MC,MC_Calc,order,Force,false);
	Scraper[nthr-1] = new GammaScraper(from_to,type,nthr-1,NoG);
		
	
	std::thread t[nthr];
	
	if(!SkipHandler){
		for(int i = 0;i < nthr;++i) t[i] = Handlers[i]->threading();
		for(int i = 0;i < nthr;++i) t[i].join();

		for(int i = 0;i < nthr;++i) delete Handlers[i];
		delete[] Handlers;
	}
	std::cout << "Handlers: done" << std::endl;
	std::cout << "-----------------" << std::endl;
	

	if(!SkipTracker){
		for(int i = 0;i < nthr;++i) t[i] = Tracker[i]->threading();
		for(int i = 0;i < nthr;++i) t[i].join();

		for(int i = 0;i < nthr;++i) delete Tracker[i];
		delete[] Tracker;
	}
	std::cout << "Trackers: done" << std::endl;
	std::cout << "-----------------" << std::endl;

	for(int i = 0;i < nthr;++i) t[i] = Scraper[i]->threading();
	for(int i = 0;i < nthr;++i) t[i].join();

	int scrapped = 0;
	for(int i = 0;i < nthr;++i) scrapped += Scraper[i]->get_in_bad_dets();

	for(int i = 0;i < nthr;++i) delete Scraper[i];
	delete[] Scraper;


	
	std::cout << "Scrapers: done" << std::endl;
	std::cout << "-----------------" << std::endl;
	
	from_to[0] = offset;
	from_to[1] = offset + amount_of_sets;
	Merger Merge(nthr,type,from_to,offset,fwhm);

	delete[] sets;
	delete[] from_to;
	sets = nullptr;
	
	std::cout << "Merger: done" << std::endl;
	std::cout << "*****************" << std::endl;

	Merge.print_am();
	std::cout << "Scraped Gammas: " << scrapped << std::endl;
	std::cout << "*****************" << std::endl;

    delete MC;
    
	return 0;
}

//--------------------------------------------------------------

void PrintCouts(const double fwhm,const bool type,const int nthr,
				const int offset,const double mtrack,const bool Tracking,
				const bool SkipHandler,const bool SkipTracker,const int maxG,
				const bool NoG,const double CRange,const bool Smear,
				const bool MC_Calc,const int order,const bool Force,
				const bool GANIL)
{
	int MaximumG = 1000000000;
	bool MG_mg = MaximumG == maxG;

	std::string order_string;
	if(order > 2 || order <= 0) order_string = "1st order";
	else order_string = (order == 1) ? "1st order" : "2nd order";
	
	std::string d_or_s[2] = {"\u03B3","\u03B3\u03B3"};
	std::string t_name = nthr == 1 ? "thread" : "threads";
	std::string tr_tmp = Tracking ? "tracked" : "non-tracked";
	
	std::cout << "\n******************************************************" << std::endl;
	std::cout << "\nWelcome to the \u03B3\u03B3/\u03B3 analysis tool \033[1;31mcompY\033[0m" << std::endl;
	std::cout << "\n======================================================" << std::endl;
	std::cout << "Running program with " << nthr << " " << t_name << std::endl;
	std::cout << "FWHM set to: " << fwhm << " mm" <<  std::endl;
	std::cout << "File Offset: " << offset << std::endl;
	std::cout << "Acceptance Level: " << mtrack << "\u03C3" << std::endl;
	std::cout << "Interested in " << tr_tmp << " gammas" << std::endl;
	if(SkipHandler) std::cout << "Skipping DataHandlers" << std::endl;
	if(SkipTracker) std::cout << "Skipping Trackers" << std::endl;
	if(!NoG) std::cout << "Saving Input <-> Output correlated files " << std::endl;
	if(MG_mg) std::cout << "Analyzing all measured gammas per thread" << std::endl;
	else std::cout << "Analyzing <= " << ((double) maxG)/1000000. << "e6 measured gammas per thread" << std::endl;
	if(CRange < 1) std::cout << "Suppression of Multi-hit Convolution set to " << CRange << std::endl;
	if(!Smear) std::cout << "No Gaussian smearing of data" << std::endl;
	if(Force)
	{
		if(MC_Calc) std::cout << "Using Monte Carlo based error propagation" << std::endl;
		else std::cout << "Using Gaussian error propagation of " << order_string << std::endl;
	}
	else std::cout << "Mixing of Monte Carlo and Gaussian error propagation" << std::endl;
	if(GANIL) std::cout << "Analyzing 137 Cs run of e673 at GANIL" << std::endl;
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "Data in " << (type ? d_or_s[1] : d_or_s[0]) << " mode" << std::endl;
	std::cout << "======================================================" << std::endl;
}

//--------------------------------------------------------------

void getSets(int* sets,const int nthr,const int amount){
	sets[0] = 0;
	sets[1] = 0;


	//data point splitting for threading
	int data_points_per_thr = amount/nthr;
	double amount_of_data_points_d = (double) amount;
	double am_threads_d = (double) nthr;
	double remaining = amount_of_data_points_d/am_threads_d - data_points_per_thr;
	int data_points_per_thr_last = ((int) remaining*nthr) + data_points_per_thr;


	sets[0] = data_points_per_thr;
	sets[1] = data_points_per_thr_last;

}

//--------------------------------------------------------------

void PrintHelp(){
	std::cout << std::endl;
	std::cout << "-------------------------------------------------------------------" << std::endl;
	std::cout << "Welcome to help of \033[1;31mcompY\033[0m" << std::endl;
	std::cout << std::endl;
	std::cout << "Possible flags:\n" << std::endl;
	std::cout << "\t -f n\t set n as FWHM in mm (standard: 5 mm) " << std::endl;
	std::cout << "\t -d \t set double gamma decay analysis" << std::endl;
	std::cout << "\t -s \t set single gamma decay analysis (standard)" << std::endl; 
	std::cout << "\t -t n \t set n as amount of threads (standard: 1)" << std::endl;
	std::cout << "\t -x n\t set n as amount of data set per thread" << std::endl; 
	std::cout << "\t -o n \t set n as file offset (standard: 0)" << std::endl;  
	std::cout << "\t -m n \t set n as sigma for maximum tracking probability (standard: 3)" << std::endl; 
	std::cout << "\t -T \t set single gamma output to tracked gammas" << std::endl; 
	std::cout << "\t -sH \t skip DataHandler routine" << std::endl; 
	std::cout << "\t -sT \t skip DataHandler & Tracking routine" << std::endl; 
	std::cout << "\t -mG n \t set n as maximum amount of processed gammas" << std::endl; 
	std::cout << "\t -sG \t Input <-> Output correlated files are written" << std::endl;
	std::cout << "\t -C n \t set n as multi-hit convolution suppression factor (n -> float)" << std::endl; 
	std::cout << "\t -nS \t no Gaussian smearing of data" << std::endl;
	std::cout << "\t -MC \t use Monte Carlo based error propagation" << std::endl;
	std::cout << "\t -oE n \t set n as order of error propagation (1 or 2)" << std::endl;
	std::cout << "\t -fP \t don't force used error propagation method" << std::endl;
	std::cout << "\t -G \t analyze 137 Cs run of e673 experiment at GANIL (May/June 2017)" << std::endl;
	std::cout << "\t -h \t prints this message" << std::endl; 
	std::cout << "-------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
}

//--------------------------------------------------------------

void FlagChecker(int argc,char** argv,int &nthr,int &amount_of_sets,
				int &offset,double &CRange,int &maxG,double &fwhm,
				bool &fwhm_flag,bool &type,bool &thr_flag,bool &file_flag,
				bool &track_flag,bool &offset_flag,bool &Tracking,
				bool &SkipHandler,bool &SkipTracker,bool &maxG_B,bool &NoG,
				bool &CRange_F,double &MAX_TRACK,bool &helpCalled,bool &Smear,
				bool &MC_Calc,int &order,bool &Force,bool &GANIL)
{	
	bool OrderFlag = false;

	for(int i = 0;i < argc;++i){
		
		if(std::string(argv[i]) == "-h"){
			PrintHelp();
			helpCalled = true;
			return;
		}
		
		if(std::string(argv[i]) == "-f"){
			fwhm_flag = true;
			continue;
		}
		if(fwhm_flag){
			fwhm = std::stod(std::string(argv[i]));
			fwhm_flag = false;
			continue;
		}
		if(std::string(argv[i]) == "-d"){
			type = true;
			continue;
		}
		if(std::string(argv[i]) == "-s"){
			type = false;
			continue;
		}
		if(std::string(argv[i]) == "-t"){
			thr_flag = true;
			continue;
		}
		if(thr_flag){
			nthr = std::stoi(std::string(argv[i]));
			thr_flag = false;
			continue;
		}

		if(std::string(argv[i]) == "-x"){
			file_flag = true;
			continue;
		}
		if(file_flag){
			amount_of_sets = std::stoi(std::string(argv[i]));
			file_flag = false;
			continue;
		}
		if(std::string(argv[i]) == "-o"){
			offset_flag = true;
			continue;
		}
		if(offset_flag){
			offset = std::stoi(std::string(argv[i]));
			offset_flag = false;
			continue;
		}
		if(std::string(argv[i]) == "-m"){
			track_flag = true;
			continue;
		}
		if(track_flag){
			MAX_TRACK = std::stod(std::string(argv[i]));
			track_flag = false;
			continue;
		}
		
		if(std::string(argv[i]) == "-T"){
			Tracking = true;
			continue;
		}
		
		if(std::string(argv[i]) == "-sH"){
			SkipHandler = true;
			continue;
		}
		if(std::string(argv[i]) == "-sT"){
			SkipTracker = true;
			continue;
		}

		if(std::string(argv[i]) == "-mG"){
			maxG_B = true;
			continue;
		}

		if(maxG_B){
			maxG = std::stoi(std::string(argv[i]));
			maxG_B = false;
			continue;
		}
		if(std::string(argv[i]) == "-sG"){
			NoG = false;
			continue;
		}
		if(std::string(argv[i]) == "-C"){
			CRange_F = true;
			continue;
		}

		if(CRange_F){
			CRange = std::stod(std::string(argv[i]));
			CRange_F = false;
			continue;
		}
		if(std::string(argv[i]) == "-nS"){
			Smear = false;
			continue;
		}
		if(std::string(argv[i]) == "-MC"){
			MC_Calc = true;
			continue;
		}
		if(std::string(argv[i]) == "-fP"){
			Force = false;
			continue;
		}
		if(std::string(argv[i]) == "-oE"){
			OrderFlag = true;
			continue;
		}
		if(OrderFlag){
			order = std::stoi(std::string(argv[i]));
			OrderFlag = false;
			continue;
		}
		if (std::string(argv[i]) == "-G")
		{
			GANIL = true;
			continue;
		}
	}
}

//--------------------------------------------------------------

void SetBins(Binnings &Bins)
{

	std::ifstream Conf("Config/Binning");
	std::string line;

	std::vector<int> tmp(5,0);

	if(Conf.fail())
	{
		std::cerr << "Could not find binning config file at Config/Binning" << std::endl;
		exit(1);
	}
	while(std::getline(Conf,line))
	{
		if(line[0] == '#')
			continue;
		sscanf(line.c_str(),"%d %d %d %d %d", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4]);
	}

	Bins.nBins_d0 = tmp[0];
	Bins.nBins_d12 = tmp[1];
	Bins.nBins_theta = tmp[2];
	Bins.nBins_Hist = tmp[3];
	Bins.nBinsE = tmp[4];
}

//--------------------------------------------------------------