#ifndef UNCERTAINTIES_H
#define UNCERTAINTIES_H

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <memory>

#include "D0_Handler.hpp"
#include "ComptonHandler.hpp"
#include "PsiMerger.hpp"
#include "Binnings.hpp"

class Uncertainties{

private:

    const double d0MAX = 600.;
    const double d12MAX = 600.;


    int nBins_d0,nBins_d12,nBins_theta,nBins_Hist,l_iter;
    int factor_d0;

    bool MC_Calc;

    std::vector<std::shared_ptr<D0_Handler> > D0s;
    std::vector<std::shared_ptr<PsiMerger> > PSIs;
    std::vector<bool> D0s_Called;

    ComptonHandler EComp;

public:
    Uncertainties(Binnings &Bins);

    ~Uncertainties();

    double Call(std::vector<int> &binsArray,double thetaX);
    bool CallIntersection(std::vector<int> &binsArray, double thetaX, std::vector<double> &E);
};


#endif