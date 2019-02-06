#include "MC_Sampler.hpp"

//--------------------------------------------------------------

MC_Sampler::MC_Sampler(Binnings &Bins)
    : U(Bins),
      binningD(Bins.nBins_d0),
      MC_Calc(Bins.MC_Calc)
{

    binningE = 25;

    if(MC_Calc)
    {
        int nPsiE = 10;

        Energies.reserve(nPsiE);
        int Etmp = 0;
    
        for(int i = 0;i < nPsiE;++i)
        {   
            Etmp = (i+1)*25;
            Energies.push_back(std::make_shared<EnergyParser>(Etmp, binningD));
        }

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

bool MC_Sampler::GetIntersection(std::vector<int> &binsArray,
                                 double thetaX,
                                 std::vector<double> &E)
{

    std::lock_guard<std::mutex> LOCK(MUTEX);
    int EnergyBin = (int)(E[0] / binningE - 1);
    bool P = Energies[EnergyBin]->CallIntersection(binsArray, thetaX, E);

    return P;
}

//--------------------------------------------------------------

bool MC_Sampler::GetIntersection_661(std::vector<int> &binsArray,
                                     double thetaX,
                                     std::vector<double> &E)
{
    std::lock_guard<std::mutex> LOCK(MUTEX);
    bool P = U.CallIntersection(binsArray, thetaX, E);
    return P;

} 

//--------------------------------------------------------------