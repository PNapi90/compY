#include "Uncertainties.hpp"

//--------------------------------------------------------------

Uncertainties::Uncertainties(Binnings &Bins,
                             bool _debug)
    : nBins_d0(Bins.nBins_d0),
      nBins_d12(Bins.nBins_d12),
      nBins_theta(Bins.nBins_theta),
      nBins_Hist(Bins.nBins_Hist),
      MC_Calc(Bins.MC_Calc),
      EComp(Bins.nBinsE, Bins.nBins_Hist)
{   

    if(MC_Calc)
    {
        //initialize d0 vectors
        D0s.reserve(nBins_d0);
        PSIs.reserve(nBins_d0);

        int d0_bins = 0;

        factor_d0 = 600/nBins_d0;

        for(int i = 0;i < nBins_d0;++i)
        {
            d0_bins = i*factor_d0;
            PSIs.push_back(std::make_shared<PsiMerger>(-999,d0_bins,_debug));
            D0s.push_back(std::make_shared<D0_Handler>(d0_bins,nBins_d12,Bins.sigmaX,nBins_Hist,nBins_theta,_debug));
        }
        D0s_Called = std::vector<bool>(nBins_d0,false); 

      
        int nthr_tmp = 5;
        std::thread t[nthr_tmp];
  
        for(int i = 0;i < nthr_tmp;++i)
            t[i] = LoadPsis(i,nthr_tmp);
        
        for(int i = 0;i < nthr_tmp;++i)
            t[i].join();

        EComp.INIT();
    }
}

//--------------------------------------------------------------

Uncertainties::~Uncertainties()
{}

//--------------------------------------------------------------

double Uncertainties::Call(std::vector<int> &binsArray,double thetaX)
{
    if(!MC_Calc)
    {
        std::cerr << "-------------------------------------------------\n";
        std::cerr << "Calling Monte Carlo error method without\n";
        std::cerr << "histograms being set!\n";
        std::cerr << "Check MC_Calc flag behaviour\n";
        std::cerr << "-------------------------------------------------";
        std::cerr << std::endl;
        exit(1); 
    }
    int d0_i = binsArray[0];
    int d12_i = binsArray[1];
    int theta_Ei = binsArray[2];
    
    int trueBin = d0_i/factor_d0;

    return D0s[trueBin]->Call(d12_i,theta_Ei,thetaX);
}

//--------------------------------------------------------------

bool Uncertainties::CallIntersection(std::vector<int> &binsArray, double thetaX,std::vector<double> &E)
{
    if (!MC_Calc)
    {
        std::cerr << "-------------------------------------------------\n";
        std::cerr << "Calling Monte Carlo error method without\n";
        std::cerr << "histograms being set!\n";
        std::cerr << "Check MC_Calc flag behaviour\n";
        std::cerr << "-------------------------------------------------";
        std::cerr << std::endl;
        exit(1);
    }
    
    int d0_i = binsArray[0];
    int d12_i = binsArray[1];
    int theta_Ei = binsArray[2];

    int trueBin = d0_i / factor_d0;

    int E_i = (int) (E[1]/4);
    int theta_i = binsArray[3];

    bool P = PSIs[trueBin]->GetP(d12_i,E_i,theta_i);

    //std::vector<double> HistTmp = D0s[trueBin]->GetHistX(d12_i, theta_Ei, thetaX);

    //double P = EComp.Intersection(HistTmp,E);

    return P;
}

//--------------------------------------------------------------

std::thread Uncertainties::LoadPsis(int i,int nthr)
{
    int perThread = nBins_d0/nthr;
    std::cout << "per  " << perThread << std::endl;   
    return std::thread(
        [=]
        {
	   std::cout << "in lambda" << std::endl;
            for(int j = i*perThread;j < (i+1)*perThread;++j)
                PSIs[j]->LOAD();
        });
    

}

//--------------------------------------------------------------
