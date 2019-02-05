#ifndef ENERGYPARSER_H
#define ENERGYPARSER_H

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <memory>


#include "PsiMerger.hpp"

class EnergyParser
{
private:
    
    int EnergyBin,nBins_d0,factor_d0;

    std::vector<std::shared_ptr<PsiMerger> > PSIs;


public:
    EnergyParser(int _EnergyBin,
                 int _nBins_d0);
    ~EnergyParser();

    bool CallIntersection(std::vector<int> &binsArray,
                          double thetaX,
                          std::vector<double> &E);
};

#endif