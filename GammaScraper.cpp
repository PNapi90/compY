/*
 * GammaScraper.cpp
 *
 *  Created on: 17.10.2018
 *      Author: philipp
 */

#include "GammaScraper.h"

//--------------------------------------------------------------

GammaScraper::GammaScraper(std::vector<int> &range,
						   bool type,
						   int thr_num,
						   bool NoG)
	: Geo(),
	  Store(),
	  fc_means(1.5,0,thr_num+1)
{
	
	this->NoG = NoG;
	this->thr_num = thr_num;
	this->type = type;
	this->range = std::vector<int>(2,0);
	lens = std::vector<int>(2,0);
	for(int i = 0;i < 2;++i) this->range[i] = range[i];

	in_bad_dets = 0;

	set_values = nullptr;

	GammaBuffer = std::vector<std::vector<double> >(50,std::vector<double>(5,0));
	

	iter = 0;
	centroids = new double*[2];
	for(int i = 0;i < 2;++i){
		centroids[i] = new double[3];
		for(int j = 0;j < 3;++j) centroids[i][j] = 0;
	}

	std::string tmp = this->type ? "Double" : "";
	std::string tmpName = "Stored/Bad_Gammas/GammaFalse"+tmp+"_dists_" + std::to_string(range[0]) + "_" + std::to_string(range[1]);
	SaveFile.open(tmpName);
	
	if(!NoG) tmp_File.open("Gammas/tmpXYZ" + std::to_string(range[0]) + "_" + std::to_string(range[1]));
	
	Save2.open("Gammas/TwoInts/Two_" + std::to_string(range[0]) + "_" + std::to_string(range[1]));

	tmpFile_Dist.open("TmpFile_S");
}

//--------------------------------------------------------------

GammaScraper::~GammaScraper() {
	
	for(int i = 0;i < 2;++i) delete[] centroids[i];
	delete[] centroids;

	if(!NoG) tmp_File.close();
	SaveFile.close();
	Save2.close();
	tmpFile_Dist.close();
}

//--------------------------------------------------------------

void GammaScraper::LOAD(){

	const char* format2 = "%lf %lf %lf %lf %lf %lf %lf";

	std::string name = "Stored/Bad_Gammas/GammaFalse_" + std::to_string(range[0]) + "_" + std::to_string(range[1]);
	//std::string name = "Stored/Gamma_" + std::to_string(range[0]) + "_" + std::to_string(range[1]);
	//name = "OFT_Single_Scraper";//"Stored/GammaOFT_D_OUT";
	std::ifstream data(name);
	if(data.fail()){
		std::cerr << "Scraper: Could not find " << name << std::endl;
		exit(1);
	}
	std::string line;
		
	double x[7] = {0};
	int N = 0;
	int Nt = 0;
	int ID = 0;

	double delta_val = 0;
	bool direct = true;

	iter = 0;

	while(std::getline(data,line,'\n'))
	{
		std::sscanf(line.c_str(),format,&x[0],&x[1],&x[2],&x[3],&x[4],&x[5],&x[6],&Nt,&N,&ID,&delta_val);
		//std::sscanf(line.c_str(),format2,&x[0],&x[1],&x[2],&x[3],&x[4],&x[5],&x[6]);
		
		if(x[0] > 0 && direct)
		{
			lens[0] = Nt;
			lens[1] = N;
			this->ID = ID;
					
			delta_arr = delta_val;
			for(int i = 0;i < 4;++i) GammaBuffer[iter][i] = x[i];
			GammaBuffer[iter][4] = iter;

			//skip photons in bad part of detector
			direct = direct && Geo.direct_check(GammaBuffer[iter]);

			++iter;
		}
		else if(x[0] > 0 && !direct)
			continue;

		if(x[0] < 0)
		{
			if(direct)
			{
				fc_means.reset();
				fc_means.fuzzy_clustering(GammaBuffer,iter,2);
				if(iter == 2) Save2Ints();
				StartComparison();
			}
			else ++in_bad_dets;

			direct = true;
			iter = 0;
		}
	}
}

//--------------------------------------------------------------

void GammaScraper::LOAD_Double(){

	std::string name = "Stored/Bad_Gammas/GammaFalseDouble_" + std::to_string(range[0]) + "_" + std::to_string(range[1]);
	std::ifstream data(name);
	if(data.fail()){
		std::cerr << "Could not find " << name << std::endl;
		exit(1);
	}
	std::string line;
	double x[7] = {0};
	int N = 0;
	int Nt = 0;

	bool direct = true;
	bool cent_set = false;
	int ID = 0;
	double delta_val = 0;

	iter = 0;

	int gamma_iter = 0;
	Store.Reset();

	while(std::getline(data,line,'\n'))
	{
		std::sscanf(line.c_str(),format,&x[0],&x[1],&x[2],&x[3],&x[4],&x[5],&x[6],&Nt,&N,&ID,&delta_val);
		if(x[0] > 0 && direct)
		{
			lens[0] = Nt;
			lens[1] = N;
			for(int i = 0;i < 4;++i) GammaBuffer[iter][i] = x[i];

			//skip photons in bad part of detector
			direct = direct && Geo.direct_check(GammaBuffer[iter]);

			++iter;
		}
		else if(x[0] > 0 && !direct)
			continue;

		if(x[0] < 0)
		{
			if(direct)
			{
				Store.SetGamma(GammaBuffer,iter);
				cent_set = Store.CheckSetCentroids();

				if(cent_set){
					double* set_values = Store.Compare();
					for(int k = 0;k < 6;++k) SaveFile << set_values[k] << " ";
					SaveFile << std::endl;
					set_values = nullptr;
				}
				cent_set = false;
			}

			++gamma_iter;

			if(gamma_iter == 2)
			{
				gamma_iter = 0;
				Store.Reset();
				if(!direct) ++in_bad_dets;
			}

			for(int i = 0;i < iter;++i) 
				for(int j = 0;j < 4;++j)
					GammaBuffer[i][j] = 0;

			direct = true;
			iter = 0;
			cent_set = false;

		}
	}
}

//--------------------------------------------------------------

void GammaScraper::StartComparison(){

	double distance = 0;

	std::vector<std::vector<double> > centroids_t = fc_means.return_energy_centroids();
	int first_pos = fc_means.get_first_cluster_ID();

	for(int i = 0;i < 2;++i){
		for(int j = 0;j < 3;++j){
			centroids[i][j] = mult_vals[j]*centroids_t[i][j+1];
		}
	}
	double En_tmp[2] = {centroids_t[0][0],centroids_t[1][0]};
	

	for(int i = 0;i < 3;++i) distance += pow(centroids_t[0][i+1] - centroids_t[1][i+1],2);
	distance = sqrt(distance);
	
	double tmpVals[2][4] = {0};

	for(int i = 0;i < 2;++i){
		for(int j = 0;j < 4;++j){
			tmpVals[i][j] = centroids_t[i][j];// *= mult_vals[j];
		}
	}
	int position_tmp = 0;
	
	
	for(int i = 0;i < 2;++i){
		for(int j = 0;j < 4;++j){
			if(first_pos == 1) centroids_t[i][j] = tmpVals[1-i][j]*mult_vals[j];
			else if(first_pos == 0) centroids_t[i][j] = tmpVals[i][j]*mult_vals[j];
			else{
				std::cerr << "bad value encountered " << first_pos << std::endl;
				exit(1);
			}
		}
	}
	
	double air = Geo.outer_shell(centroids_t);
	air += Geo.get_air_path(centroids_t);

	if(distance - air < 160 && false)
	{
		for(int i = 0;i < 2;++i)
		{
			for(auto c : centroids_t[i])
				tmpFile_Dist << c << " ";
			tmpFile_Dist << distance << " " << air << std::endl;
		}
		for(int i = 0;i < 6;++i)
			tmpFile_Dist << -99999 << " ";
		tmpFile_Dist << std::endl;
	}
	
	if(std::abs(En_tmp[0] + En_tmp[1] - 661.7) <= 2 && !NoG) tmpFileWrite(centroids_t[0][0],distance-air);

	SaveFile << distance << " " << air << " ";
	for(int i = 0;i < 2;++i) SaveFile << centroids_t[i][0] << " ";
	SaveFile << lens[1] << " " << lens[0] << " " << std::endl;
}

//--------------------------------------------------------------

void GammaScraper::tmpFileWrite(double EEE,double dair){
	std::vector<std::vector<double> > tmp_cluster;
	int len_tmp = 0;
	for(int i = 0;i < 2;++i){
		tmp_cluster = fc_means.return_cluster(i);
		len_tmp = fc_means.return_cluster_len(i);
		for(int j = 0;j < len_tmp;++j){
			tmp_File << EEE << " ";
			for(int k = 0;k < 4;++k) tmp_File << tmp_cluster[j][k] << " ";
			tmp_File << lens[0] << " " << lens[1] << " " << ID << " " << delta_arr << " " << dair <<  std::endl;
		}
		for(int k = 0;k < 10;++k) tmp_File << -888 << " ";
		tmp_File << std::endl;
	}
	for(int k = 0;k < 10;++k) tmp_File << -99999 << " ";
	tmp_File << std::endl;
}

//--------------------------------------------------------------

std::thread GammaScraper::threading(){
	if(!type) return std::thread([=]{LOAD();});
	return std::thread([=]{LOAD_Double();});
}

//--------------------------------------------------------------

int GammaScraper::get_in_bad_dets(){
	return in_bad_dets;
}

//--------------------------------------------------------------

void GammaScraper::Save2Ints(){
	
	if(std::abs(GammaBuffer[0][0] + GammaBuffer[1][0] - 661.7 ) > 2) return;
	
	for(int i = 0;i < 2;++i){
		for(int k = 0;k < 4;++k) Save2 << GammaBuffer[i][k] << " ";
		Save2 << std::endl;
	}
	for(int k = 0;k < 4;++k) Save2 << -99999 << " ";
	Save2 << std::endl;
}

//--------------------------------------------------------------
