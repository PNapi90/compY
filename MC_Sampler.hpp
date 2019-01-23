#ifndef MC_SAMPLER_H
#define MC_SAMPLER_H

#include "Uncertainties.hpp"
#include <mutex>
#include <vector>


class MC_Sampler
{   
private:
    std::mutex MUTEX;
    int binningD;
    bool MC_Calc;

    Uncertainties U;

public:
    
    MC_Sampler(int _binningD,
               bool _MC_Calc,
               double sigmaX);
    ~MC_Sampler();
    
    double GetPValue(std::vector<int> &binsArray,double thetaX);
    bool GetIntersection(std::vector<int> &binsArray, double thetaX, std::vector<double> &E);

    int GetBinning();
};



#endif