#ifndef D0_HANDLER
#define D0_HANDLER

#include <vector>
#include <memory>

#include "D12_Handler.hpp"

class D0_Handler{

private:

    int nBins_d12,d0_i,factor_d12,nBins_theta,nBins_Hist;

    bool Loaded;

    std::vector<bool> Called_d12s;
    std::vector<std::shared_ptr<D12_Handler> > D12s;

    void INIT(int d12_i);

public:
  D0_Handler(int _d0_i,
             int _nBins_d12,
             double sigmaX,
             int _nBins_Hist,
             int _nBins_theta);
  ~D0_Handler();

  bool Check_Loader();

  double Call(int d12_i, int theta_Ei, double theta_Xi);
  std::vector<double> GetHistX(int d12_i, int theta_Ei, double thetaX);

};


#endif