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
#include <vector>
#include <memory>


#include "DataHandler.h"
#include "GammaTracker.h"
#include "GammaScraper.h"
#include "Merger.h"
#include "MC_Sampler.hpp"
#include "Binnings.hpp"

//--------------------------------------------------------------

struct FlagsAndVals
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
	bool debug = false;
	bool OFT = false;
	bool OFT_Track = false;
};


//--------------------------------------------------------------

void FlagChecker(int argc,char** argv,FlagsAndVals &F);

void PrintCouts(FlagsAndVals &F);

void PrintHelp();

void getSets(std::vector<int> &sets,
			 const int nthr,
			 const int amount);

void SetBins(Binnings &Bins);

//--------------------------------------------------------------

int main(int argc, char **argv) 
{

	FlagsAndVals F;


	FlagChecker(argc,argv,F);

	if (F.helpCalled)
		return 0;

	if (!F.Force)
		F.MC_Calc = true;

	F.SkipHandler = F.SkipTracker ? F.SkipTracker : F.SkipHandler;

	
	if (F.GANIL)
	{
		F.nthr = 1;
		F.Smear = false;
		F.CRange = 0;
	}

	if (F.debug)
	{
		F.nthr = 1;
		F.Tracking = true;
	}

	if(F.OFT)
	{
		F.MC_Calc = false;
	}

	if(F.OFT_Track)
	{
		F.SkipHandler = true;
		F.OFT = false;
	}


	std::vector<int> sets(2,0);

	if (F.nthr >= F.amount_of_sets)
		F.nthr = F.amount_of_sets;

	PrintCouts(F);

	getSets(sets, F.nthr, F.amount_of_sets);

	int MAX_ITER = 6;
	

	int set_begin = 0;
	int set_end = 0;
	
	std::vector<int> from_to(2,0);
	
	std::vector<std::shared_ptr<DataHandler>> Handlers;
	std::vector<std::shared_ptr<GammaTracker>> Tracker;
	std::vector<std::shared_ptr<GammaScraper>> Scraper;

	if (!F.SkipHandler)
		Handlers.reserve(F.nthr);
	if (!F.SkipTracker)
		Tracker.reserve(F.nthr);

	Scraper.reserve(F.nthr);

	//set binnings 
	Binnings Bins;
	SetBins(Bins);

	Bins.MC_Calc = F.MC_Calc;
	Bins.sigmaX = F.fwhm;

	MC_Sampler MC(Bins,F.debug);

	for (int i = 0; i < F.nthr - 1; ++i)
	{
		set_begin = from_to[1];
		set_end = from_to[1] + sets[0];
		from_to[0] = set_begin;
		from_to[1] = set_end;
		
		if(i == 0)
		{
			for(int j = 0;j < 2;++j)
				from_to[j] += F.offset;
		}
		
		if (!F.SkipHandler)
			Handlers.push_back(std::make_shared<DataHandler>(from_to, F.type, F.fwhm,
															 i, F.maxG, ((unsigned int)i),
															 F.NoG, F.CRange, ((int)F.Smear * 1), 
															 F.GANIL,F.OFT));

		if (!F.SkipTracker)
			Tracker.push_back(std::make_shared<GammaTracker>(from_to, F.type, F.fwhm,
															 MAX_ITER, F.MAX_TRACK, i,
															 F.Tracking, &MC, F.MC_Calc,
															 F.order, F.Force, false, 
															 F.OFT_Track));

		Scraper.push_back(std::make_shared<GammaScraper>(from_to, F.type, i, F.NoG));
	}


	from_to[0] = from_to[1];
	from_to[1] = from_to[0] + sets[1];
	if (!F.SkipHandler)
		Handlers.push_back(std::make_shared<DataHandler>(from_to, F.type, F.fwhm,
														 F.nthr - 1, F.maxG, (unsigned int)F.nthr - 1,
														 F.NoG, F.CRange, ((int)F.Smear * 1), F.GANIL, F.OFT));
	if (!F.SkipTracker)
		Tracker.push_back(std::make_shared<GammaTracker>(from_to, F.type, F.fwhm,
														 MAX_ITER, F.MAX_TRACK, F.nthr - 1,
														 F.Tracking, &MC, F.MC_Calc,
														 F.order, F.Force, false,
														 F.OFT_Track));

	Scraper.push_back(std::make_shared<GammaScraper>(from_to, F.type, F.nthr - 1, F.NoG));

	std::thread t[F.nthr];

	if (!F.SkipHandler)
	{
		for (int i = 0; i < F.nthr; ++i)
			t[i] = Handlers[i]->threading();
		for (int i = 0; i < F.nthr; ++i)
			t[i].join();

		for (int i = 0; i < F.nthr; ++i)
			Handlers.pop_back();
	}
	std::cout << "Handlers: done" << std::endl;
	std::cout << "-----------------" << std::endl;

	

	if (!F.SkipTracker)
	{
		if(!F.OFT)
		{
			for (int i = 0; i < F.nthr; ++i)
				t[i] = Tracker[i]->threading();
			for (int i = 0; i < F.nthr; ++i)
				t[i].join();
		}
		for (int i = 0; i < F.nthr; ++i)
			Tracker.pop_back();
	}
	std::cout << "Trackers: done" << std::endl;
	std::cout << "-----------------" << std::endl;

	int scrapped = 0;

	if(!F.OFT)
	{
		for (int i = 0; i < F.nthr; ++i)
			t[i] = Scraper[i]->threading();
		for (int i = 0; i < F.nthr; ++i)
			t[i].join();
		for (int i = 0; i < F.nthr; ++i)
			scrapped += Scraper[i]->get_in_bad_dets();

	}

	for (int i = 0; i < F.nthr; ++i)
		Scraper.pop_back();


	
	std::cout << "Scrapers: done" << std::endl;
	std::cout << "-----------------" << std::endl;

	from_to[0] = F.offset;
	from_to[1] = F.offset + F.amount_of_sets;
	
	if(!F.OFT)
	{
		Merger Merge(F.nthr, F.type, from_to, F.offset, F.fwhm);
	
		std::cout << "Merger: done" << std::endl;
		std::cout << "*****************" << std::endl;

		Merge.print_am();
		std::cout << "Scraped Gammas: " << scrapped << std::endl;
		std::cout << "*****************" << std::endl;
	}
	return 0;
}

//--------------------------------------------------------------

void PrintCouts(FlagsAndVals &F)
{
	int MaximumG = 1000000000;
	bool MG_mg = MaximumG == F.maxG;

	std::string order_string;
	if (F.order > 2 || F.order <= 0)
		order_string = "1st order";
	else
		order_string = (F.order == 1) ? "1st order" : "2nd order";

	std::string d_or_s[2] = {"\u03B3","\u03B3\u03B3"};
	std::string t_name = F.nthr == 1 ? "thread" : "threads";
	std::string tr_tmp = F.Tracking ? "tracked" : "non-tracked";

	std::cout << "\n******************************************************" << std::endl;
	std::cout << "\nWelcome to the \u03B3\u03B3/\u03B3 analysis tool \033[1;31mcompY\033[0m" << std::endl;
	std::cout << "\n======================================================" << std::endl;
	std::cout << "Running program with " << F.nthr << " " << t_name << std::endl;
	std::cout << "FWHM set to: " << F.fwhm << " mm" << std::endl;
	std::cout << "File Offset: " << F.offset << std::endl;
	std::cout << "Acceptance Level: " << F.MAX_TRACK << "\u03C3" << std::endl;
	std::cout << "Interested in " << tr_tmp << " gammas" << std::endl;
	if (F.SkipHandler)
		std::cout << "Skipping DataHandlers" << std::endl;
	if (F.SkipTracker)
		std::cout << "Skipping Trackers" << std::endl;
	if (!F.NoG)
		std::cout << "Saving Input <-> Output correlated files " << std::endl;
	if (MG_mg)
		std::cout << "Analyzing all measured gammas per thread" << std::endl;
	else
		std::cout << "Analyzing <= " << ((double)F.maxG) / 1000000. << "e6 measured gammas per thread" << std::endl;
	if (F.CRange < 1)
		std::cout << "Suppression of Multi-hit Convolution set to " << F.CRange << std::endl;
	if (!F.Smear)
		std::cout << "No Gaussian smearing of data" << std::endl;
	if (F.Force)
	{
		if (F.MC_Calc)
			std::cout << "Using Monte Carlo based error propagation" << std::endl;
		else std::cout << "Using Gaussian error propagation of " << order_string << std::endl;
	}
	else std::cout << "Mixing of Monte Carlo and Gaussian error propagation" << std::endl;
	if (F.GANIL)
		std::cout << "Analyzing 137 Cs run of e673 at GANIL" << std::endl;
	if(F.OFT)
		std::cout << "Format handled data into OFT Format" << std::endl;
	
	if(F.OFT_Track)
		std::cout << "Tracking of OFT output files" << std::endl;

	if(F.debug)
	{
		std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - -" << std::endl;
		std::cout << "!!!RUNNING IN DEBUG MODE => NO SENSIBLE TRACKING!!!" << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "Data in " << (F.type ? d_or_s[1] : d_or_s[0]) << " mode" << std::endl;
	std::cout << "======================================================" << std::endl;
}

//--------------------------------------------------------------

void getSets(std::vector<int> &sets,
			 const int nthr,
			 const int amount)
{
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
	std::cout << "\t -db \t enable debugging mode -> no histograms are loaded" << std::endl;
	std::cout << "\t -OFT\t generate OFT-formatted data" << std::endl;
	std::cout << "\t -OFT_T\t enable tracking of OFT output" << std::endl;
	std::cout << "\t -h \t prints this message" << std::endl; 
	std::cout << "-------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
}

//--------------------------------------------------------------

void FlagChecker(int argc,char** argv,FlagsAndVals &F)
{	
	bool OrderFlag = false;

	for(int i = 0;i < argc;++i){
		
		if(std::string(argv[i]) == "-h"){
			PrintHelp();
			F.helpCalled = true;
			return;
		}
		
		if(std::string(argv[i]) == "-f"){
			F.fwhm_flag = true;
			continue;
		}
		if (F.fwhm_flag)
		{
			F.fwhm = std::stod(std::string(argv[i]));
			F.fwhm_flag = false;
			continue;
		}
		if(std::string(argv[i]) == "-d"){
			F.type = true;
			continue;
		}
		if(std::string(argv[i]) == "-s"){
			F.type = false;
			continue;
		}
		if(std::string(argv[i]) == "-t"){
			F.thr_flag = true;
			continue;
		}
		if (F.thr_flag)
		{
			F.nthr = std::stoi(std::string(argv[i]));
			F.thr_flag = false;
			continue;
		}

		if(std::string(argv[i]) == "-x"){
			F.file_flag = true;
			continue;
		}
		if (F.file_flag)
		{
			F.amount_of_sets = std::stoi(std::string(argv[i]));
			F.file_flag = false;
			continue;
		}
		if(std::string(argv[i]) == "-o"){
			F.offset_flag = true;
			continue;
		}
		if (F.offset_flag)
		{
			F.offset = std::stoi(std::string(argv[i]));
			F.offset_flag = false;
			continue;
		}
		if(std::string(argv[i]) == "-m"){
			F.track_flag = true;
			continue;
		}
		if (F.track_flag)
		{
			F.MAX_TRACK = std::stod(std::string(argv[i]));
			F.track_flag = false;
			continue;
		}
		
		if(std::string(argv[i]) == "-T"){
			F.Tracking = true;
			continue;
		}
		
		if(std::string(argv[i]) == "-sH"){
			F.SkipHandler = true;
			continue;
		}
		if(std::string(argv[i]) == "-sT"){
			F.SkipTracker = true;
			continue;
		}

		if(std::string(argv[i]) == "-mG"){
			F.maxG_B = true;
			continue;
		}

		if (F.maxG_B)
		{
			F.maxG = std::stoi(std::string(argv[i]));
			F.maxG_B = false;
			continue;
		}
		if(std::string(argv[i]) == "-sG"){
			F.NoG = false;
			continue;
		}
		if(std::string(argv[i]) == "-C"){
			F.CRange_F = true;
			continue;
		}

		if (F.CRange_F)
		{
			F.CRange = std::stod(std::string(argv[i]));
			F.CRange_F = false;
			continue;
		}
		if(std::string(argv[i]) == "-nS"){
			F.Smear = false;
			continue;
		}
		if(std::string(argv[i]) == "-MC"){
			F.MC_Calc = true;
			continue;
		}
		if(std::string(argv[i]) == "-fP"){
			F.Force = false;
			continue;
		}
		if(std::string(argv[i]) == "-oE"){
			OrderFlag = true;
			continue;
		}
		if (OrderFlag)
		{
			F.order = std::stoi(std::string(argv[i]));
			OrderFlag = false;
			continue;
		}
		if (std::string(argv[i]) == "-G")
		{
			F.GANIL = true;
			continue;
		}
		if (std::string(argv[i]) == "-db")
		{
			F.debug = true;
			continue;
		}
		if (std::string(argv[i]) == "-OFT")
		{
			F.OFT = true;
			continue;
		}
		if (std::string(argv[i]) == "-OFT_T")
		{
			F.OFT_Track = true;
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