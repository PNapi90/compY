#include "D0_Handler.hpp"

//--------------------------------------------------------------

D0_Handler::D0_Handler(int _d0_i,int _nBins_d12,double sigmaX) : nBins_d12(_nBins_d12) , d0_i(_d0_i)
{
    D12s.reserve(nBins_d12);
    int d12factor = 600/nBins_d12;

    for(int i = 0;i < nBins_d12;++i){
        D12s.push_back(std::make_shared<D12_Handler>(d0_i,i*d12factor,181,101,sigmaX));
    }
    Called_d12s = std::vector<bool>(nBins_d12,false);
    Loaded = false;

    factor_d12 = 600/nBins_d12;
}

//--------------------------------------------------------------

D0_Handler::~D0_Handler()
{}

//--------------------------------------------------------------

double D0_Handler::Call(int d12_i,int theta_Ei,double thetaX)
{
    int tmp = factor_d12;
    int trueBin = d12_i/factor_d12;
    //if(!Called_d12s[trueBin]) INIT(trueBin);

    return D12s[trueBin]->GetPValue(theta_Ei,thetaX);
}

//--------------------------------------------------------------

void D0_Handler::INIT(int d12_i)
{
    
    //D12s[d12_i]->LOAD();
    //Called_d12s[d12_i] = true;

}

//--------------------------------------------------------------

bool D0_Handler::Check_Loader()
{
    return Loaded;
}

//--------------------------------------------------------------

std::vector<double> D0_Handler::GetHistX(int d12_i, int theta_Ei, double thetaX)
{
    int tmp = factor_d12;
    int trueBin = d12_i / factor_d12;
    //if(!Called_d12s[trueBin]) INIT(trueBin);

    return D12s[trueBin]->GetHistX(theta_Ei, thetaX);
}

//--------------------------------------------------------------