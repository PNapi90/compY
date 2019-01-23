#include "MC_Sampler.hpp"

//--------------------------------------------------------------

MC_Sampler::MC_Sampler(int _binningD, bool _MC_Calc, double sigmaX) : U(_binningD,_binningD, 181, 101, _MC_Calc, sigmaX, 175),
                                                                      binningD(_binningD), MC_Calc(_MC_Calc)
{
    if(MC_Calc)
    {
        std::cout << std::endl;
        std::cout << "Histograms loaded" << std::endl;
        std::cout << "-----------------" << std::endl;
    }
}

//--------------------------------------------------------------

MC_Sampler::~MC_Sampler()
{}

//--------------------------------------------------------------

double MC_Sampler::GetPValue(std::vector<int> &binsArray,double thetaX)
{
    std::lock_guard<std::mutex> LOCK(MUTEX);
    double P = U.Call(binsArray,thetaX);
    return P;
}

//--------------------------------------------------------------

int MC_Sampler::GetBinning()
{
    return binningD;
}

//--------------------------------------------------------------

bool MC_Sampler::GetIntersection(std::vector<int> &binsArray, double thetaX,std::vector<double> &E)
{
    std::lock_guard<std::mutex> LOCK(MUTEX);
    bool P = U.CallIntersection(binsArray,thetaX,E);
    return P;
}

//--------------------------------------------------------------