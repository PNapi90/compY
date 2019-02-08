#include "EnergyParser.hpp"

//--------------------------------------------------------------

EnergyParser::EnergyParser(int _EnergyBin,
                           int _nBins_d0,
                           bool _debug)
    : EnergyBin(_EnergyBin),
      nBins_d0(_nBins_d0)
{

    PSIs.reserve(nBins_d0);
    int d0_tmp = 0;

    factor_d0 = 600 / nBins_d0;

    for(int i = 0;i < nBins_d0;++i)
    {
        d0_tmp = i*factor_d0;
        PSIs.push_back(std::make_shared<PsiMerger>(EnergyBin,d0_tmp,_debug));
    }
}

//--------------------------------------------------------------

EnergyParser::~EnergyParser()
{}

//--------------------------------------------------------------

bool EnergyParser::CallIntersection(std::vector<int> &binsArray,
                                    double thetaX,
                                    std::vector<double> &E)
{
    int d0_i = binsArray[0];
    int d12_i = binsArray[1];
    int theta_Ei = binsArray[2];

    int trueBin = d0_i / factor_d0;

    int E_i = (int)(E[1] / 4);
    int theta_i = binsArray[3];

    bool P = PSIs[trueBin]->GetP(d12_i, E_i, theta_i);


    return P;
}

//--------------------------------------------------------------