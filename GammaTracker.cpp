/*
 * GammaTracker.cpp
 *
 *  Created on: 15.10.2018
 *      Author: philipp
 */

#include "GammaTracker.h"

//--------------------------------------------------------------

GammaTracker::GammaTracker(std::vector<int> &range,
						   bool type,
						   double FWHM,
						   int MAX_ITER,
						   double MAX_TRACK,
						   int thr_num,
						   bool Track,
						   MC_Sampler* MC,
						   bool _MC_Calc,
						   int _order,
						   bool _ForceMode,
						   bool _GANIL,
						   bool _OFT,
						   bool _DirectOutput,
						   bool _Mimic) 
	: generator((unsigned int) thr_num) , 
	  MC_Calc(_MC_Calc) ,
	  ForceMode(_ForceMode),
	  GANIL(_GANIL),
	  OFT(_OFT),
	  DirectOutput(_DirectOutput),
	  HeaderWritten(false),
	  Mimic(_Mimic)
{
	
	if(_order > 2 || _order <= 0) order = 1;
	else order = _order;

	this->MC = MC;
	this->Track = Track;
	this->thr_num = thr_num;
	this->type = type;
	this->range = std::vector<int>(2,0);
	
	for(int i = 0;i < 2;++i)
		this->range[i] = range[i];

	binningFactor = (600/this->MC->GetBinning());

	this->type = type;
	this->FWHM = FWHM;
	this->MAX_ITER = MAX_ITER;
	this->MAX_TRACK = MAX_TRACK;
	
	sigma = this->FWHM/2.355;
	
	Ecompton_max = 661.7/(1.+ 2*661.7/511.);
	
	
	std::string tmp = this->type ? "Double" : "";
	std::string tmpName = "Stored/Bad_Gammas/GammaFalse"+tmp+"_" + std::to_string(range[0]) + "_" + std::to_string(range[1]);
	OUTFILE.open(tmpName);

	if(DirectOutput)
	{
		std::string name = "Stored/DIRECT/GammaOFT"+tmp+"_TRACK." + std::to_string(range[0]) + "_" + std::to_string(range[1]);
		
		if(Mimic)
			name = "Stored/DIRECT/GammaOFT_D_TRACK." + std::to_string(range[0]) + "_" + std::to_string(range[1]);

		DIRECT.open(name);
		if(DIRECT.fail())
		{
			std::cerr << "Could not open " << name << std::endl;
			exit(1);
		}
	}

	lambdaE = 0.30529367125613543;
	UNI = std::uniform_real_distribution<double>(0,lambdaE);


	NOriginal = 0;

	len_D = std::vector<int>(2,0);
	len_orig_D = std::vector<int>(2,0);
	NOriginal_D = std::vector<int>(2,0);

	Egamma = 0;

	Egamma_D = std::vector<double>(2,0);
	
	min_delta = 0;
	
	delta_X = this->FWHM/2.355;
	

	int max_fac = Factorial(MAX_ITER);

	sigmas = std::vector<double>(max_fac,0);
	sigma_Eth = std::vector<double>(max_fac,0);

	binsArray = std::vector<int>(4,0);

	GammaBuffer = std::vector<std::vector<double> >(100,std::vector<double>(4,0));
	mu_vec = std::vector<std::vector<double> >(3,std::vector<double>(3,0));
	angle_vec = std::vector<std::vector<double> >(max_fac,std::vector<double>(2,0));
	angle_save = std::vector<std::vector<double> >(MAX_ITER,std::vector<double>(2,0));
	GammaBufferDouble = std::vector<std::vector<std::vector<double> > >(2,std::vector<std::vector<double> >(100,std::vector<double>(4,0)));

	delta_arr = std::vector<double>(max_fac,0);

	dep_file.open("Stored/After_Track.dat");

	len = 0;
	len_orig = 0;

}

//--------------------------------------------------------------

GammaTracker::~GammaTracker()
{
	OUTFILE.close();
	dep_file.close();
	if(DirectOutput)
		DIRECT.close();
	MC = nullptr;
}

//--------------------------------------------------------------

void GammaTracker::LOAD()
{	

	

	std::string tmpDouble = (type) ? "Double" : "";
	//tmpDouble = "Double_";

	std::string name = "Stored/";

	if(type)
	{
		name += "OFT/Gamma_Double_" + std::to_string(range[0]) + "_" + std::to_string(range[1]) + ".rawSpec";
	}
	else
	{
		name += OFT ? "OFT/" : "";
		name += "Gamma_" + tmpDouble  + std::to_string(range[0]) + "_" + std::to_string(range[1]) + ".rawSpec";
	}

	std::ifstream data(name);
	if(data.fail()){
		std::cerr << "Could not find " << name << std::endl;
		exit(1);
	}
	std::string line;
	double x[5] = {0};
	int N = 0;
	int iter = 0;
	int ID = 0;
	bool successful = true;
	double Efull = 0;

	int gamma_counter = 0;

	while(std::getline(data,line,'\n')){
		successful = true;
		std::sscanf(line.c_str(),format,&x[0],&x[1],&x[2],&x[3],&x[4],&N,&ID);
		if(x[0] > 0){
			for(int j = 0;j < 4;++j) GammaBuffer[iter][j] = x[j];
			Egamma = x[4];
			NOriginal = N;
			this->ID = ID;
			Efull += x[0];
			++iter;
		}
		else{
			if(iter > 1 && iter <= MAX_ITER){
				if(std::abs(Efull - 661.7) <= 2)
				{
					bool BAD = CheckGammaBadness(iter);
					
					if(!BAD)
					{	
						successful = Tracking(iter,0);
						if(successful == Track) 
							WRITE(iter);
					}
				}
			}
			iter = 0;
			Efull = 0;
			++gamma_counter;
		}
		if(thr_num == 0 && gamma_counter % 200000 == 0){
			std::cout << "\r";
			std::cout << "Trackers: Thread 0 in File [" << range[0] << "," << range[1] << ") at gamma " << gamma_counter << "\t\t\t\t\t";
			std::cout.flush();
		}
	}
	if(thr_num == 0) std::cout << std::endl;
}

//--------------------------------------------------------------

void GammaTracker::LOAD_Double()
{
	std::string name = "Stored/";
	if(!OFT)
		name += "GammaDouble_" + std::to_string(range[0]) + "_" + std::to_string(range[1]) + ".rawSpec";
	else
	{
		name += "OFT/Gamma_Double_" + std::to_string(range[0]) + "_" + std::to_string(range[1]) + ".rawSpec";
	}
	
	std::ifstream data(name);
	
	if(data.fail())
	{
		std::cerr << "Could not find " << name << std::endl;
		exit(1);
	}
	
	std::string line;
	double x[5] = {0};
	int N = 0;
	int iter = 0;

	bool successful = true;
	int gamma_counter = 0;

	int count_doubles = 0;
	
	for(int i = 0;i < 2;++i) 
		len_D[i] = 0;

	while(std::getline(data,line))
	{
		std::sscanf(line.c_str(),formatDouble,&x[0],&x[1],&x[2],&x[3],&x[4],&N);
		if(x[0] > 0)
		{
			for(int j = 0;j < 4;++j)
				GammaBufferDouble[count_doubles][iter][j] = x[j];
			Egamma_D[count_doubles] = x[4];
			NOriginal_D[count_doubles] = N;
			
			++iter;
		}
		else
		{
			len_D[count_doubles] = iter;
			iter = 0;
			++count_doubles;
			successful = true;
			if(count_doubles == 2)
			{
				for(int i = 0;i < 2;++i)
				{
					if(len_D[i] >= 1 && len_D[i] <= MAX_ITER)
					{
						SetBuffer(i);
						successful = successful && Tracking(len_D[i],i);
					}
				}
				if(successful)
					WRITE(iter);
				
				iter = 0;
				++gamma_counter;

				count_doubles = 0;
				for(int i = 0;i < 2;++i)
					len_D[i] = 0;
			}
		}
	}
}

//--------------------------------------------------------------

void GammaTracker::LOAD_GANIL()
{
	
	if(thr_num > 0)
	{
		std::cerr << "Thread number " << thr_num << " > 0 !" << std::endl;
		std::cerr << "GANIL data set only allows single threaded mode!" << std::endl;
		exit(1);
	}

	std::string name = "Gamma_GANIL/GammaEvents.GANIL";
	std::ifstream DATA(name);

	if(DATA.fail())
	{
		std::cerr << "Could not open " << name << std::endl;
		exit(1);
	} 

	std::string line;
	double x[6];
	int N = 0;
	int iter = 0;
	int ID = 0;
	bool successful = true;
	double Efull = 0;

	int gamma_counter = 0;

	while(std::getline(DATA,line))
	{
		sscanf(line.c_str(), formatGANIL, &ID,&x[0], &x[1], &x[2], &x[3], &x[4]);
		
		if(line[0] == '$')
			continue;

		if (ID > 0)
		{
			for (int j = 0; j < 4; ++j)
				GammaBuffer[iter][j] = x[j];
			Egamma = x[4];
			NOriginal = 0;
			this->ID = 0;
			Efull += x[0];
			++iter;
		}
		else
		{
			if (iter > 1 && iter <= MAX_ITER)
			{
				if (std::abs(Efull - 661.7) <= 2)
				{
					successful = Tracking(iter, 0);
					if (successful == Track)
						WRITE(iter);
				}
			}
			iter = 0;
			Efull = 0;
			++gamma_counter;
		}
		if (thr_num == 0 && gamma_counter % 200000 == 0)
		{
			std::cout << "\r";
			std::cout << "Trackers: Thread 0 in File [" << range[0] << "," << range[1] << ") at gamma " << gamma_counter << "\t\t\t\t\t";
			std::cout.flush();
		}
	}
}

//--------------------------------------------------------------

std::thread GammaTracker::threading()
{
	if(!type || OFT) return std::thread(
		[=]{
			if(!GANIL)
				LOAD();
			else
				LOAD_GANIL();
		}
	);
	return std::thread([=]{LOAD_Double();});
}

//--------------------------------------------------------------

void GammaTracker::SetBuffer(int pos)
{
	for(int i = 0;i < len_D[pos];++i){
		for(int j = 0;j < 4;++j) GammaBuffer[i][j] = GammaBufferDouble[pos][i][j];
	}
}

//--------------------------------------------------------------

void GammaTracker::WRITE(int iter)
{
	if(DirectOutput)
		WRITE_DIRECT(iter);

	if(type && !OFT){
		double edep_t = 0;
		for(int o = 0;o < 2;++o){
			edep_t = 0;
			for(int i = 0;i < len_D[o];++i){
				for(int j = 0;j < 4;++j) OUTFILE << GammaBufferDouble[o][i][j] << " ";
				for(int j = 0;j < 2;++j) OUTFILE << 0 << " ";
				OUTFILE << Egamma_D[o] << " " << NOriginal_D[o] << " " << len_D[o] << " " << ID << std::endl;
				edep_t += GammaBufferDouble[o][i][0];
			}
			for(int i = 0;i < 10;++i) OUTFILE << -99999 << " ";
			OUTFILE << std::endl;
			dep_file << edep_t << " ";
		}
		dep_file << std::endl;
		return;
	}
	
	//gamma -----------------------------------------------------------------------------------
	for(int i = 0;i < iter;++i){
		for(int j = 0;j < 4;++j) 
			OUTFILE << GammaBuffer[i][j] << " ";
		for(int j = 0;j < 2;++j)
			OUTFILE << angle_save[i][j]*180./M_PI << " ";
		OUTFILE << Egamma << " " << NOriginal << " " << iter << " "<< ID  << " " << min_delta << std::endl;
	}
	for(int i = 0;i < 11;++i) OUTFILE << -99999 << " ";
	OUTFILE << std::endl;
}

//--------------------------------------------------------------

void GammaTracker::WRITE_DIRECT(int iter)
{
	
	if(!HeaderWritten)
	{
		std::string name = type ? "Gamma_Double_Cs/" : "Gamma_Single_Cs/";
		
		if(Mimic)
			name = "Gamma_Double_Cs/Merged/";
	
		name += "GammaEvents";
		name += Mimic ? "_D." : ".";
		
		name += GetEnding(range[0]);

		std::ifstream TMP_INPUT(name);

		if(TMP_INPUT.fail())
		{
			std::cerr << "Could not open " <<  name << std::endl;
			exit(1);
		}

		std::string line;


		//write file header
		while(std::getline(TMP_INPUT,line))
		{
			DIRECT << line << std::endl;
			if(line[0] == '$')
			{
				HeaderWritten = true;
				break;
			}
		}
	}

	//write incident information
	DIRECT << "   " << -1 << "   " << 661.7 << " " << 0 << " " << 0 << " " << 0 << " " << ID << std::endl;
	
	for (int i = 0; i < iter; ++i)
	{
		DIRECT <<"   " <<13 << "   ";
		for(int j = 0;j < 4;++j)
			DIRECT << GammaBuffer[i][j] << " ";
		DIRECT << 1 << std::endl;
	}	
}

//--------------------------------------------------------------

inline std::string GammaTracker::GetEnding(int i)
{

	int n_Zeros = 0;

	std::string tmp = "";

	if(i > 999)
		n_Zeros = 0;
	else if(i > 99)
		n_Zeros = 1;
	else if(i > 9)
		n_Zeros = 2;
	else
		n_Zeros = 3;

	for(int j = 0;j < n_Zeros;++j)
		tmp += "0";

	return tmp + std::to_string(i);
}

//--------------------------------------------------------------

inline int GammaTracker::Factorial(int n)
{
	int k = 1;
	for(int i = 1;i <= n;++i) k *= i;
	return k;
}

//--------------------------------------------------------------

void GammaTracker::PrintDataSet(int iter)
{
	std::cout << "===========================================" << std::endl;
	for(int i = 0;i < iter;++i){
		for(int j = 0;j < 4;++j) std::cout << GammaBuffer[i][j] << " ";
		std::cout << std::endl;
	}
	std::cout << "===========================================" << std::endl;
}

//--------------------------------------------------------------

bool GammaTracker::Tracking(int iter,int pos_d)
{

	std::vector<int> sortarray(iter,0);
	for(int i = 0;i < iter;++i) sortarray[i] = i;

	int start_val = 0;
	int perm_iter = 0;

	double Estart = 0;

	if(!GANIL) Estart = type ? Egamma_D[pos_d] : Egamma;
	else Estart = 661.7;

	if(OFT)
		Estart = 661.7;

	double delta_tmp = 0;

	std::sort(sortarray.begin(),sortarray.end());

	//PrintDataSet(iter);
	

	std::vector<double> edeps(iter,0);
	double tmpS = 0,PValue = 0;

	std::vector<double> EtmpVec(2,0);

	bool FirstStep = true;

	//Permutations of all convoluted points
	do
	{
		if(!GANIL)
			Estart = type ? Egamma_D[pos_d] : Egamma;
		//else 
		Estart = 661.7;
		
		delta_tmp = 0;
		delta_arr[perm_iter] = 0;
		PValue = 0;

		for(int i = 0;i < iter;++i)
			edeps[i] = GammaBuffer[sortarray[i]][0];
	
		GetSigmaE(edeps,iter);

		FirstStep = true;

		//loop over all points of permutation set (not last point since
		//no scattering information available)		
		for(int i = 0;i < iter-1;++i)
		{
			//for first point, set source position as start
			start_val = (i == 0) ? 1 : 0;
			
			if(i == 0)
			{
				for(int k = 0;k < 3;++k)
					mu_vec[0][k] = source_vec[k];
			}

			//set (remaining) points for scattering angle comparison
			for(int j = start_val;j < 3;++j)
			{
				for(int k = 0;k < 3;++k)
					mu_vec[j][k] = GammaBuffer[sortarray[i+j-1]][k+1];
			}

			//reset scattering angles (j*10000 to ensure large difference if not used)
			for(int j = 0;j < 2;++j)
				angle_vec[i][j] = j*10000;

			//get sigmas from position error propagation
			get_sigmas(i+1,perm_iter);
			//get Compton scattering angles from energies
			get_E_angle(Estart,GammaBuffer[sortarray[i]][0],i);


			//Get MC simulated possibilities
			if(MC_Calc)
			{
				if (angle_vec[i][1] != WRONG_CASE)
				{

					EtmpVec[0] = Estart;
					EtmpVec[1] = GammaBuffer[sortarray[i]][0];


						
					//Get intersected area
					if (std::abs(Estart - 661.7) <= 2)
						PValue = MC->GetIntersection_661(binsArray, thetaX, EtmpVec);	
					//else if(Estart < 275 && BinCheck(EtmpVec))
					//	PValue = MC->GetIntersection(binsArray, thetaX, EtmpVec);
					else
						PValue = MC->GetPValue(binsArray, thetaX);
				}	
				else PValue = exp(-10);

				if(!FirstStep) delta_tmp = (PValue >= exp(-0.5*MAX_TRACK*MAX_TRACK)) ? 0.1 : 100.; 
				else delta_tmp = (PValue >= 0.1) ? 0.1 : 100.;
			}

			//use 1st/2nd order Gaussian uncertainty propagation
			else
			{
				tmpS = pow(sigma_Eth[i],2) + pow(sigmas[i],2);
				if(tmpS != tmpS)
				{
					tmpS = 0;
					delta_tmp = WRONG_CASE;
				}
				else delta_tmp = std::abs(angle_vec[i][0] - angle_vec[i][1])/std::sqrt(tmpS);
			}

			//comparison between position and energy scattering angles
			delta_arr[perm_iter] = (delta_arr[perm_iter] <= delta_tmp) ? delta_tmp : delta_arr[perm_iter];

			if(perm_iter == 0)
			{
				for(int j = 0;j < 2;++j)
					angle_save[i][j] = angle_vec[i][j];
			}
			//set for intersection only after first iteration
			FirstStep = false;

			//if untrackable part reached -> break
			//(no additional calculations needed for permutation)
			if(delta_tmp >= MAX_TRACK)
				break;

			//set "incident" photon energy for next iteration
			Estart -= GammaBuffer[sortarray[i]][0];
			
		}
		
		//increment permutation iterator
		++perm_iter;

	}while(std::next_permutation(sortarray.begin(),sortarray.end()));
		
	//get minimum of delta_arr
	min_delta = 1000000;
	for(int i = 0;i < perm_iter;++i) min_delta = (delta_arr[i] <= min_delta) ? delta_arr[i] : min_delta;
	
	//check if one permutation allows for successful tracking
	bool possible_tracking = false;
	for(int i = 0;i < perm_iter;++i){
		possible_tracking = (delta_arr[i] <= MAX_TRACK);
		
		if(possible_tracking)
			return true;
	}

	return false;

}

//--------------------------------------------------------------

inline double GammaTracker::Gaussian(double x)
{
	return 1./(sqrt(2*M_PI)*sigma)*exp(-0.5*pow((Ecompton_max-x)/sigma,2));
}

//--------------------------------------------------------------

inline double GammaTracker::Exponential(double x)
{
	//lambda calculated by using 500 keV as energy endpoint of 
	//exponential function starting at Ecompton
	return 10;
	return lambdaE*exp(-(x-Ecompton_max)*lambdaE);
}

//--------------------------------------------------------------

bool GammaTracker::Check_Doppler(double Ed)
{
	double UniVal = UNI(generator);
	return (UniVal <= Exponential(Ed));
}

//--------------------------------------------------------------

void GammaTracker::get_E_angle(double Estart,double Ed,int pos)
{

	double cth = 0;

	if (Estart < Ed)
	{
		angle_vec[pos][1] = WRONG_CASE;
		return;
	}

	cth = 1. - mc2/(Estart-Ed) + mc2/Estart;

	//only use MC if big or small angles are used
	if(!ForceMode) 
		MC_Calc = (std::abs(cth) >= 0.8);
	
	//skip energies above Compton edge if not simulated via MC yet

	std::vector<double> EtmpVec(2, 0);
	EtmpVec[0] = Estart;
	EtmpVec[1] = Ed;

	if (cth < -1 && std::abs(Estart - 661.7) > 2)
	{
		//if(Estart >= 275 || (Estart < 275 && !BinCheck(EtmpVec)))
		//{
		//	angle_vec[pos][1] = WRONG_CASE;
		//	return;
		//}
		angle_vec[pos][1] = WRONG_CASE;
		return;
	}



	double tmpV = acos(cth)*180./M_PI;

	binsArray[2] = (int) tmpV;

}

//--------------------------------------------------------------

void GammaTracker::get_sigmas(int pos,int perm_iter)
{

	int o = 0;

	double source[3] = {0,0,0};

	for(int i = 0;i < 3;++i) source[i] = mu_vec[0][i];

	double x_s[2][3];
	for(int i = 1;i < 3;++i) for(int j = 0;j < 3;++j) x_s[i-1][j] = mu_vec[i][j];

	//norm of vector x0 - source
	double norm1 = 0;
	//norm of vector x1 - x0
	double norm2 = 0;

	double scalar = 0;
	double c_th = 0,delta_cos_th= 0;
	double Da_1 = 0,Da_2 = 0,Db_1 = 0,Db_2 = 0,Da_0 = 0,Db_0 = 0;
	double delta_t1 = 0,delta_t2 = 0,delta_t0 = 0;

	
	for(int i = 0;i < 3;++i){
		norm1 += pow(x_s[o][i] - source[i],2.);
		norm2 += pow(x_s[1-o][i] - x_s[o][i],2.);
		scalar += (x_s[o][i] - source[i])*(x_s[1-o][i] - x_s[o][i]);
	}
	norm1 = sqrt(norm1);
	norm2 = sqrt(norm2);

	c_th = scalar/(norm1*norm2);

	//Data for MC method
	binsArray[0] = (int) (norm1/binningFactor); 
	binsArray[1] = (int) (norm2/binningFactor);

	//binsArray[0] -= binsArray[0] % binningFactor;
	//binsArray[1] -= binsArray[1] % binningFactor;

	binsArray[3] = (int) (acos(c_th)*180/M_PI);
	if(binsArray[3] == 180) --binsArray[3];

	thetaX = c_th;

	if(binsArray[3] > 180 || binsArray[3] < 0)
	{
		std::cout << "Upsi" << std::endl;
	}

	//skip error propagation if MC method used
	if(MC_Calc) return;

	
	double Da2_0 = 0,Da2_1 = 0,Da2_2 = 0;
	double Db2_0 = 0,Db2_1 = 0,Db2_2 = 0;

	int factor = 1;

	double DD0 = 0,DD1 = 0,DD2 = 0;

	double a = scalar;
	double b = norm1*norm2;

	angle_vec[pos-1][0] = c_th;

	thetaX = c_th;
	
    for(int i = 0;i < 3;++i){
        Da_0 = x_s[0][i] - x_s[1][i];
        Db_0 = norm2/norm1*(-x_s[0][i] + source[i]);
        
        if(order == 2)
		{	
			Da2_0 = 0;
        	Db2_0 = norm2/norm1*(1 + pow(-x_s[0][i] + source[i],2)/pow(norm1,2));
        	DD0 = ((Da_0*b - a*Db_0)/pow(b,2))*delta_X + factor*(0.5/pow(b,2)*(Da2_0 - 2*Da_0*Db_0 + a*(Db2_0 - 2*pow(Db_0,2)/b))*pow(delta_X,2));
		}
		
		else DD0 = ((Da_0*b - a*Db_0)/pow(b,2))*delta_X; 

        delta_t0 += DD0*DD0;

        Da_1 = x_s[1][i] - 2*x_s[0][i] + source[i];
        Db_1 = norm2/norm1*(-source[i] + x_s[0][i]) + norm1/norm2*(-x_s[1][i] + x_s[0][i]);

		if(order == 2)
		{
        	Da2_1 = -2;
        	Db2_1 = norm2/norm1*(1 + pow(x_s[0][i] - source[i],2)/pow(norm1,2)) + norm1/norm2*(1 + pow(x_s[0][i] - x_s[1][i],2)/pow(norm2,2));

        	DD1 = ((Da_1*b - a*Db_1)/pow(b,2))*delta_X + factor*(0.5/pow(b,2)*(Da2_1 - 2*Da_1*Db_1 + a*(Db2_1 - 2*pow(Db_1,2)/b))*pow(delta_X,2));
		}
		else DD1 = ((Da_1*b - a*Db_1)/pow(b,2))*delta_X;

        delta_t1 += DD1*DD1;

        Da_2 = x_s[0][i] - source[i];
        Db_2 = norm1/norm2*(-x_s[0][i] + x_s[1][i]);

		if(order == 2)
		{
        	Da2_2 = 0;
        	Db2_2 = norm1/norm2*(1 + pow(x_s[1][i] - x_s[0][i],2)/pow(norm2,2));

        	DD2 = ((Da_2*b - a*Db_2)/pow(b,2))*delta_X + factor*(0.5/pow(b,2)*(Da2_2 - 2*Da_2*Db_2 + a*(Db2_2 - 2*pow(Db_2,2)/b))*pow(delta_X,2));
		}
		else DD2 = ((Da_2*b - a*Db_2)/pow(b,2))*delta_X;

        delta_t2 += DD2*DD2;
    }
	
	delta_cos_th = sqrt(delta_t1 + delta_t2 + delta_t0);
	
	sigmas[pos-1] = std::abs(delta_cos_th);///sin(angle_vec[pos-1][0]));
}

//--------------------------------------------------------------

void GammaTracker::GetSigmaE(std::vector<double> &Edeps,int len)
{
	//skip error propagation if MC method used
	if(MC_Calc) return;

	std::vector<double> EdepDelta(len,0);
	std::vector<double> E(len,0);
	std::vector<double> Edelta(len,0);
	std::vector<double> delta_cth_E(len-1,0);
	for(int i = 0;i < len;++i){
		EdepDelta[i] = 0.002*Edeps[i];
		E[i] = (i == 0) ? Egamma : E[i-1] - Edeps[i-1];
		Edelta[i] = 0;
		for(int j = 0;j < i;++j) Edelta[i-1] += pow(EdepDelta[j],2);
	}
	for(int i = 0;i < len;++i) Edelta[i] = sqrt(Edelta[i]);
	std::vector<double> cth_E(len-1,0);
	for(int i = 1;i < len;++i) cth_E[i-1] = 1. - mc2/E[i] + mc2/E[i-1];
	
	double tmp_Val = 0,DE = 0,DEd = 0;
	
	for(int i = 0;i < len-1;++i){
		
		DE = mc2/pow(E[i]-Edeps[i],2) + mc2/pow(E[i],2);
		DEd = mc2/pow(E[i]-Edeps[i],2);
		
		tmp_Val = pow(Edelta[i]*DE,2) + pow(EdepDelta[i]*DEd,2);
		delta_cth_E[i] = sqrt(tmp_Val);
		
		sigma_Eth[i] = delta_cth_E[i];///sqrt(1. - pow(cth_E[i],2));
	}
}

//--------------------------------------------------------------

inline bool GammaTracker::BinCheck(std::vector<double> &Tmp)
{

	return false;

	int a = (int) Tmp[0]/25;
	int b = (int) Tmp[1]/25;

	return (a - b) > 0;
}

//--------------------------------------------------------------

inline bool GammaTracker::CheckGammaBadness(int iter)
{
	double norm = 0;
	for(int i = 0;i < iter-1;++i)
	{
		norm = 0;
		for(int j = 1;j < 4;++j)
			norm += std::abs(GammaBuffer[i][j] - GammaBuffer[i+1][j]);
		
		if(!norm)
			return true;
	}
	return false;
}

//--------------------------------------------------------------