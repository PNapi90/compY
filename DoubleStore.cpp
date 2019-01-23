/*
 * DoubleStore.cpp
 *
 *  Created on: 18.10.2018
 *      Author: philipp
 */

#include "DoubleStore.h"

//-------------------------------------------------------------------------------------------

DoubleStore::DoubleStore() 
{
	StoredGammas = std::vector<std::vector<std::vector<double> > >(2,std::vector<std::vector<double> >(50,std::vector<double>(4,0)));

	pos = 0;

	Geo = new Geometry_Checker();

	return_arr = new double[6];

	centroids = std::vector<std::vector<double> >(2,std::vector<double>(3,0));
	lens = std::vector<int>(2,0);
	centroid_E = std::vector<double>(2,0);
	centroids_set = false;

	centroid_File.open("Stored/centroid_file.dat");

}

//-------------------------------------------------------------------------------------------

DoubleStore::~DoubleStore()
{
	delete Geo;
	Geo = nullptr;
	delete[] return_arr;
	centroid_File.close();
}

//-------------------------------------------------------------------------------------------

void DoubleStore::Reset()
{
	for(int i = 0;i < 50;++i)
	{
		for(int j = 0;j < 4;++j){
			StoredGammas[0][i][j] = 0;
			StoredGammas[1][i][j] = 0;
 		}
	}
	for(int i = 0;i < 2;++i) lens[i] = 0;
	pos = 0;
}

//-------------------------------------------------------------------------------------------

void DoubleStore::SetGamma(std::vector<std::vector<double> > &Gamma,int len)
{
	lens[pos] = len;
	for(int i = 0;i < len;++i)
	{
		for(int j = 0;j < 4;++j) StoredGammas[pos][i][j] = Gamma[i][j];
	}
	++pos;
	centroids_set = false;
	if(pos == 2) SetCentroids();
}

//-------------------------------------------------------------------------------------------

void DoubleStore::SetCentroids()
{

	for(int i = 0;i < 2;++i){
		centroid_E[i] = 0;
		for(int j = 0;j < 3;++j) centroids[i][j] = 0;
	}

	double edep_tmp = 0;
	for(int i = 0;i < 2;++i){
		edep_tmp = 0;
		for(int j = 0;j < lens[i];++j){
			edep_tmp += StoredGammas[i][j][0];
			for(int k = 1;k < 4;++k){
				centroids[i][k-1] += StoredGammas[i][j][k]*StoredGammas[i][j][0];
			}
		}
		for(int k = 1;k < 4;++k){
			centroids[i][k-1] /= edep_tmp;
		}
		centroid_E[i] = edep_tmp;
	}
	centroids_set = true;
	pos = 0;
}

//-------------------------------------------------------------------------------------------

double* DoubleStore::Compare()
{

	double distance = 0;

	for(int i = 0;i < 2;++i)
	{
		for(int j = 0;j < 3;++j) centroids[i][j] *= mult_vals[j];
	}

	for(int i = 0;i < 3;++i)
	{
		distance += std::pow(centroids[0][i] - centroids[1][i],2);
	}
	distance = sqrt(distance);

	std::vector<std::vector<double> > centroids_t(2,std::vector<double>(4,0));
	for(int i = 0;i < 2;++i)
	{
		for(int j = 0;j < 3;++j) centroids_t[i][j+1] = centroids[i][j];
	}

	double air = Geo->outer_shell(centroids_t);
	air += Geo->get_air_path(centroids_t);

	return_arr[0] = distance;
	return_arr[1] = air;
	for(int i = 2;i < 4;++i){
		return_arr[i] = centroid_E[i-2];
		//return_arr[i+2] = lens[i-2];
	}
	return_arr[4] = lens[0] + lens[1];
	return_arr[5] = lens[0];

	centroid_File << centroid_E[0] << " " << centroid_E[1]  << " " << lens[0]+lens[1] << " " << lens[0] << std::endl;

	return return_arr;

}

//-------------------------------------------------------------------------------------------

bool DoubleStore::CheckSetCentroids()
{
	return centroids_set;
}

//-------------------------------------------------------------------------------------------