#ifndef MC_SAMPLER_H
#define MC_SAMPLER_H

#include "Uncertainties.hpp"
#include "EnergyParser.hpp"
#include "Binnings.hpp"

#include <mutex>
#include <vector>


class MC_Sampler
{   
private:
    std::mutex MUTEX;
    int binningD, binningE;
    bool MC_Calc;

    Uncertainties U;

    std::vector<std::shared_ptr<EnergyParser> > Energies;


public:
    
    MC_Sampler(Binnings &Bins,
               bool _debug);
    ~MC_Sampler();
    
    double GetPValue(std::vector<int> &binsArray,
                     double thetaX);
    bool GetIntersection(std::vector<int> &binsArray,
                         double thetaX,
                         std::vector<double> &E);

    bool GetIntersection_661(std::vector<int> &binsArray,
                         double thetaX,
                         std::vector<double> &E);

    int GetBinning();
};



#endif