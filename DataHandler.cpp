
#include "DataHandler.h"

//-------------------------------------------------------------------------------------------

DataHandler::DataHandler(std::vector<int> &range,
						 bool type,
						 double FWHM,
						 int thr_num,
						 int maxG,
						 unsigned int SEED,
						 bool NoG,
						 double CRange,
						 int SMEAR,
						 bool GANIL,
						 bool _OFT) 
	: generator(SEED) ,
	  OFT(_OFT)
{
	this->GANIL = GANIL;
	this->SMEAR = SMEAR;
	this->NoG = NoG;
	this->maxG = maxG;
	this->thr_num = thr_num;
	this->CRange = CRange;
	this->FWHM = FWHM;
	sigma_X = this->FWHM/2.355;
	sigma_Scale = 1./(sqrt(2*M_PI)*sigma_X);
	m_iter = 0;
	double_gam_counter = 0;
	this->type = type;
	this->range = std::vector<int>(2,0);
	for(int i = 0;i < 2;++i) 
		this->range[i] = range[i];
	gamma_iter = 0;
	E0 = 0;

	am_GammasFull = 0; 
	
	gammaID = 0;
	
	tmpIter = 0;
	TmpGamma = std::vector<std::vector<std::vector<double> > >(1000,std::vector<std::vector<double> >(100,std::vector<double>(7,0)));
	TmpGammaLen = std::vector<int>(1000,0);

	gamma_iter2 = std::vector<int>(2,0);
	E0_2 = std::vector<double>(2,0);
	Gamma2 = std::vector<std::vector<std::vector<double> > >(2,std::vector<std::vector<double> >(max_len,std::vector<double>(4,0)));

	Signs = std::vector<std::vector<int> >(1000,std::vector<int>(2,0));

	Gamma = std::vector<std::vector<double> >(max_len,std::vector<double>(5,0));
	MergedData = std::vector<std::vector<double> >(max_len,std::vector<double>(5,0));

	UNI = std::uniform_real_distribution<double>(0,sigma_Scale);
	UNI_C = std::uniform_real_distribution<double>(0,1);
	
	GaussX = std::normal_distribution<double>(0,sigma_X);

	std::string tmpDouble = (type) ? "Double" : "";
	std::string tmpName;
	
	if(!OFT) 
		tmpName = "Stored/Gamma"+tmpDouble + "_" + std::to_string(range[0]) + "_" + std::to_string(range[1]) + ".rawSpec";
	else
	{
		tmpName = "Stored/OFT/GammaEvents_OFT";
		if(type)
			tmpName += "_Double";
		tmpName += "." + std::to_string(range[0]) + "_" + std::to_string(range[1]);
	}
	

	Etot_file.open("Stored/Etot.dat");
	majorFile.open(tmpName);
	if(thr_num == 0){
		max_file.open("max_file.dat");
	}
	
	if(!NoG) gammaTmpFile.open("Gammas/gamma_"+std::to_string(range[0]) + std::to_string(range[1])+"_tmp");
	
	if(majorFile.fail()){
		std::cerr << "Could not open " << tmpName << "!" << std::endl;
		exit(1);
	}
}

//-------------------------------------------------------------------------------------------

DataHandler::~DataHandler() 
{
	majorFile.close();
	Etot_file.close();
	if(thr_num == 0) max_file.close();
	if(!NoG) gammaTmpFile.close();
}

//-------------------------------------------------------------------------------------------

void DataHandler::LOAD(){
	std::ifstream file;
	std::string name,line;

	double E;
	double x[3];
	double type;
	int dummy;
	gamma_iter = 0;
	tmpIter = 0;

	bool new_gamma = false;


	bool data_coming = false;
	bool first_gamma = true;
	bool skip = false;
	int iterator = 0;
	int tmpID = 0;

	int lineIter = 0;
	for (int i = range[0]; i < range[1]; ++i) 
	{
		if(!OFT)
			name = "Gamma_Single_Cs/GammaEvents." + EndingName(i);

		else
		{
			if(!this->type)
				name = "Gamma_Single_Cs/GammaEvents." + EndingName(i);
			else
				name = "Gamma_Double_Cs/GammaEvents." + EndingName(i);
		}
				
		int kkk = 0;
		file.open(name);
		if(file.fail()){
			std::cerr << "Could not find " << name << std::endl;
			std::exit(1);
		}

		data_coming = false;

		while(std::getline(file,line,'\n') && am_GammasFull < maxG)
		{
			if(OFT && !data_coming)
				majorFile << line << std::endl;
			
			if(line[0] == '#')
				continue;

			if(line[0] == '$')
			{
				data_coming = true;
				continue;
			}

			if(!data_coming)
				continue;

			std::sscanf(line.c_str(),format,&type,&E,&x[0],&x[1],&x[2],&dummy);
			
			if(type == -1)
			{
				E0 = E;
				oldLine = line;
				
				if(this->type)
					tmpID = dummy;

				new_gamma = true;
			}
			else
			{	
				if(new_gamma)
					ResetGammaBuffer();

				if(gamma_iter == 0)
					GammaLine = oldLine;
				
				Gamma[gamma_iter][0] = E;
				for(int i = 0;i < 3;++i) Gamma[gamma_iter][i+1] = x[i];

				Signs[gamma_iter][0] = type;
				Signs[gamma_iter][1] = this->type ? tmpID : dummy;

				++gamma_iter;
				new_gamma = false;
			}
			

			if(thr_num == 0 && iterator % 200000 == 0)
			{
				std::cout << "\r";
				std::cout << "Handlers: Thread 0 in File " << i << " of [" << range[0] <<"," << range[1] << ") at iteration " << iterator << "\t\t\t\t\t";
				std::cout.flush();
			}
			++iterator;
			
			if(tmpIter == 500 && !OFT)
			{
				SaveTmp();
				tmpIter = 0;
			}
		}
		if(tmpIter > 0 && !OFT)
		{
			SaveTmp();
			tmpIter = 0;
		}
		if(thr_num == 0)
			std::cout << std::endl;
		iterator = 0;
		file.close();
		file.clear();
	}
	
}

//-------------------------------------------------------------------------------------------

void DataHandler::LOAD_Double()
{
	std::ifstream file;
	std::string name,line;

	double E = 0;
	double x[3] = {0};
	double type = 0;
	int dummy = 0;
	gamma_iter = 0;

	int gamma_c = 0;

	bool data_coming = false;
	bool first_gamma = true;
	bool skip = false;
	bool filled_gammas[2] = {false,false};
	int iterator = 0;
	for (int i = range[0]; i < range[1]; ++i) {
		name = "Gamma_Double_Cs/GammaEvents." + EndingName(i);
		file.open(name);
		if(file.fail()){
			std::cerr << "Could not find " << name << std::endl;
			std::exit(1);
		}

		data_coming = false;

		while(std::getline(file,line,'\n'))
		{
			if(line[0] == '#') continue;

			if(line[0] == '$'){
				data_coming = true;
				continue;
			}

			if(!data_coming) continue;

			std::sscanf(line.c_str(),format,&type,&E,&x[0],&x[1],&x[2],&dummy);

			if(type == -1){

				gamma_c = 1 - (dummy % 2);

				if(first_gamma){
					skip = true;
					first_gamma = false;
				}
				else skip = false;

				if(gamma_c == 0){
					bool tmp_bool = filled_gammas[0] && filled_gammas[1];
					ResetGammaBuffer2(tmp_bool);

					for(int k = 0;k < 2;++k) filled_gammas[k] = false;
				}

				E0_2[gamma_c] = E;
			}
			else{
				if(skip) 
					continue;

				filled_gammas[gamma_c] = true;

				Gamma2[gamma_c][gamma_iter2[gamma_c]][0] = E;
				for(int i = 0;i < 3;++i) Gamma2[gamma_c][gamma_iter2[gamma_c]][i+1] = x[i];

				++gamma_iter2[gamma_c];
			}

			//if(skip) std::cout << "Thread " << thr_num << " skipping line -> " << line << std::endl;
			if(thr_num == 0 && iterator % 200000 == 0){
				std::cout << "\r";
				std::cout << "Thread 0 in File " << i << " of [" << range[0] <<"," << range[1] << ") at iteration " << iterator << "\t\t\t\t\t";
				std::cout.flush();
			}
			++iterator;
		}
		file.close();
		file.clear();
	}
	if(thr_num == 0) std::cout << std::endl;
}

//-------------------------------------------------------------------------------------------

void DataHandler::LOAD_GANIL()
{

	if (thr_num > 0)
	{
		std::cerr << "Thread number " << thr_num << " > 0 !" << std::endl;
		std::cerr << "GANIL data set only allows single threaded mode!" << std::endl;
		exit(1);
	}
	std::ifstream file;
	std::string name, line;

	double E;
	double x[3];
	double type;
	int dummy;
	gamma_iter = 0;
	tmpIter = 0;

	bool new_gamma = false;

	bool data_coming = false;
	bool first_gamma = true;
	bool skip = false;
	int iterator = 0;

	bool process = false;

	for (int i = 0; i < 1; ++i)
	{
		name = "Gamma_GANIL/DATA_REAL/FOR_MC";
		//name = "Gamma_GANIL/GammaEvents.GANIL";
		file.open(name);
		if (file.fail())
		{
			std::cerr << "Could not find " << name << std::endl;
			std::exit(1);
		}

		data_coming = false;

		while (std::getline(file, line, '\n') && am_GammasFull < maxG)
		{
			if (line[0] == '#')
				continue;

			if (line[0] == '$')
			{
				data_coming = true;
				continue;
			}

			if (!data_coming)
				continue;

			std::sscanf(line.c_str(), format, &type, &E, &x[0], &x[1], &x[2], &dummy);

			if (type == -1)
			{
				process = (std::abs(E - 661.7) > 2);
				
				process = true;
				E0 = 661.7;
				new_gamma = true;
			}
			else if(type != -1 && process)
			{
				if (new_gamma)
					ResetGammaBuffer();

				Gamma[gamma_iter][0] = E;
				for (int i = 0; i < 3; ++i)
					Gamma[gamma_iter][i + 1] = x[i];

				++gamma_iter;
				new_gamma = false;
			}
			if (thr_num == 0 && iterator % 200000 == 0)
			{
				std::cout << "\r";
				std::cout << "Handlers: Thread 0 in File " << i << " of [" << range[0] << "," << range[1] << ") at iteration " << iterator << "\t\t\t\t\t";
				std::cout.flush();
			}
			++iterator;
			if (tmpIter == 500)
			{
				SaveTmp();
				tmpIter = 0;
			}
		}
		if (tmpIter > 0)
		{
			SaveTmp();
			tmpIter = 0;
		}
		if (thr_num == 0)
			std::cout << std::endl;
		iterator = 0;
		file.close();
		file.clear();
	}

}

//-------------------------------------------------------------------------------------------

inline std::string DataHandler::EndingName(int i)
{
	if(i > 999) return std::to_string(i);
	if(i > 99) return "0" + std::to_string(i);
	std::string returnName = (i > 9) ? "00" : "000";
	return returnName + std::to_string(i);
}

//-------------------------------------------------------------------------------------------

void DataHandler::ResetGammaBuffer2(bool two_gammas)
{

	if(two_gammas){
		double Etot = 0;
		for(int i = 0;i < 2;++i){
			for(int j = 0;j < gamma_iter2[i];++j){
				for(int k = 0;k < 4;++k) Gamma[j][k] = Gamma2[i][j][k];
				Etot += Gamma2[i][j][0];
			}
			gamma_iter = gamma_iter2[i];
			E0 = E0_2[i];
			MergeGammaAndSave();
		}
		Etot_file << Etot << std::endl;
	}

	for(int i = 0;i < 2;++i){
		for(int j = 0;j < max_len;++j){
			for(int k = 0;k < 4;++k){
				if(i == 0){
					Gamma[j][k] = 0;
					MergedData[j][k] = 0;
				}
				Gamma2[i][j][k] = 0;
			}
		}
		E0_2[i] = 0;
		gamma_iter2[i] = 0;
	}
	gamma_iter = 0;
	m_iter = 0;
}

//-------------------------------------------------------------------------------------------

void DataHandler::ResetGammaBuffer()
{

	if(gamma_iter > 0){
		MergeGammaAndSave();
		++am_GammasFull;
	}
	
	if(thr_num == 0){
		double Et = 0;
		for(int i = 0;i < gamma_iter;++i) Et += Gamma[i][0];
		if(Et == 661.7){
			for(int i = 0;i < gamma_iter;++i) max_file << Gamma[i][0] << std::endl;
		}
	}
	
	for(int i = 0;i < max_len;++i){
		for(int j = 0;j < 4;++j){
			Gamma[i][j] = 0;
			MergedData[i][j] = 0;
		}
	}
	gamma_iter = 0;
	m_iter = 0;
}

//-------------------------------------------------------------------------------------------

void DataHandler::MergeGammaAndSave(){

	double distance = 0;

	for(int i = 0;i < 4;++i) MergedData[0][i] = Gamma[0][i];
	m_iter = 0;
	++gammaID;
	
	if(!NoG)
		saveTMP();
	
	double EnergySum = 0;

	for (int i = 0; i < gamma_iter-1; ++i) 
	{
		distance = 0;
		for (int j = 1; j < 4; ++j) distance += pow(MergedData[m_iter][j] - Gamma[i+1][j],2);
		distance = sqrt(distance);

		if(MergeChecker(distance) && ConvolSuppr()){
			EnergySum = MergedData[m_iter][0] + Gamma[i+1][0];
			for(int j = 1;j < 4;++j){
				MergedData[m_iter][j] = MergedData[m_iter][j]*MergedData[m_iter][0];
				MergedData[m_iter][j] += Gamma[i+1][0]*Gamma[i+1][j];
				MergedData[m_iter][j] /= EnergySum;
			}
			MergedData[m_iter][0] = EnergySum;
		}
		else
		{
			++m_iter;
			for (int j = 0; j < 4; ++j) MergedData[m_iter][j] = Gamma[i+1][j];
		}
	}
	
	for(int i = 0;i <= m_iter;++i) MergedData[i][4] = gammaID;

	SaveMerge();
}

//-------------------------------------------------------------------------------------------

void DataHandler::saveTMP(){
	for(int i = 0;i < gamma_iter;++i){
		for(int j = 0;j < 4;++j) gammaTmpFile << Gamma[i][j] << " " ;
		gammaTmpFile << gammaID << " " << gamma_iter <<std::endl;
	}
	for(int i = 0;i < 6;++i) gammaTmpFile << -99999 << " ";
	gammaTmpFile << std::endl;
}

//-------------------------------------------------------------------------------------------

inline double DataHandler::Gaussian(double x){
	return sigma_Scale*exp(-0.5*pow(x/sigma_X,2));
}

//-------------------------------------------------------------------------------------------

inline bool DataHandler::MergeChecker(double distance){
	double UniVal = UNI(generator);
	return (UniVal <= Gaussian(distance));
}

//-------------------------------------------------------------------------------------------

inline bool DataHandler::ConvolSuppr(){
	double UniVal = UNI_C(generator);
	return (UniVal <= CRange);
}

//-------------------------------------------------------------------------------------------

void DataHandler::SaveMerge()
{

	if(!type && !OFT){
		for(int i = 0;i <= m_iter;++i){
			std::normal_distribution<double> GaussE(0,0.002/2.355*MergedData[i][0]);
			TmpGamma[tmpIter][i][0] = MergedData[i][0] + GaussE(generator)*SMEAR;
			for(int j = 1;j < 4;++j) TmpGamma[tmpIter][i][j] = MergedData[i][j] + GaussX(generator)*SMEAR;

			TmpGamma[tmpIter][i][4] = E0;
			TmpGamma[tmpIter][i][5] = gamma_iter;
			TmpGamma[tmpIter][i][6] = MergedData[i][4];
		}
		TmpGammaLen[tmpIter] = m_iter+1;
		++tmpIter;
	}
	else{
		double EdTmp = 0;

		if (OFT)
			majorFile << GammaLine << std::endl;

		for (int i = 0; i <= m_iter; ++i)
		{
			if (OFT) 
				majorFile << "   " << Signs[i][0] << "   ";
			
			for (int j = 0; j < 4; ++j)
			{
				if(j == 0)
				{
					std::normal_distribution<double> GaussE(0,0.002/2.355*MergedData[i][j]);
					MergedData[i][j] += GaussE(generator)*SMEAR;
				}
				else 
					MergedData[i][j] += GaussX(generator)*SMEAR;
				majorFile << MergedData[i][j] << " ";
			}

			EdTmp += MergedData[i][0];

			if(!OFT)
				majorFile << E0 << " " <<  gamma_iter << " " << MergedData[i][4] <<std::endl;
			else
				if(i < m_iter)
					majorFile << 0 <<  Signs[i][1] << std::endl;
				else
				{
					if(std::abs(EdTmp - 661.7) <= 2) 
						majorFile << 0 << 2 << std::endl;
					else
						majorFile << 0 << Signs[i][1] << std::endl;
				}
				
		}
		if(!OFT)
			SetFileEnding();
	}
}

//-------------------------------------------------------------------------------------------

void DataHandler::SetFileEnding(){

	if(type){
		++double_gam_counter;

		if(double_gam_counter == 1){
			for (int i = 0; i < 6; ++i) majorFile << -888 << " ";
			majorFile << std::endl;
			return;
		}
		else if(double_gam_counter > 2){
			std::cerr << "double_gam_counter exception encountered @ " << double_gam_counter << std::endl;
			exit(1);
		}
	}

	double_gam_counter = 0;

	for (int i = 0; i < 6; ++i) majorFile << -99999 << " ";
	majorFile << std::endl;

}

//-------------------------------------------------------------------------------------------

std::thread DataHandler::threading(){
	if(!type || OFT) return std::thread(
		[=]
		{	
			if(!GANIL)
				LOAD();
			else
				LOAD_GANIL();
		}
	);
	return std::thread([=]{LOAD_Double();});
}

//-------------------------------------------------------------------------------------------

void DataHandler::SaveTmp(){
	
	for(int i = 0;i < tmpIter;++i){
		for(int j = 0;j < TmpGammaLen[i];++j){
			for(int k = 0;k < 7;++k){
				if(k < 5) majorFile << TmpGamma[i][j][k] << " ";
				else majorFile << ((int) TmpGamma[i][j][k]) << " ";
				TmpGamma[i][j][k] = 0;
			}
			majorFile << std::endl;
		}
		SetFileEnding();
		TmpGammaLen[i] = 0;
	}
}

//-------------------------------------------------------------------------------------------