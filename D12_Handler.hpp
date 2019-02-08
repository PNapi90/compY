#ifndef D12_HANDLER
#define D12_HANDLER

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>
#include <cmath>
#include <cstdlib>

class D12_Handler{

private:

    double SigmaX;

    bool debug;
    int d0_i,d12_i;
    int nBins_theta,nBins_Hist;

    int refactor_d0,refactor_d12;

    std::vector<std::vector<double> > Histogram;
    std::vector<double> Maxima;

    void LOAD();

    std::string Getd0Folder();

 
public:
    D12_Handler(int _d0_i,
                int _d12_i,
                int _nBins_theta,
                int _nBins_Hist,
                double _SigmaX,
                bool _debug);
    ~D12_Handler();
    
    double GetPValue(int theta_Ei,double thetaX);
    std::vector<double> GetHistX(int theta_Ei, double thetaX);
};

#endif