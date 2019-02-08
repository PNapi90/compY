#include "D12_Handler.hpp"

//--------------------------------------------------------------

D12_Handler::D12_Handler(int _d0_i,
                         int _d12_i,
                         int _nBins_theta,
                         int _nBins_Hist,
                         double _SigmaX,
                         bool _debug) 
    : d0_i(_d0_i) ,
      d12_i(_d12_i) , 
      nBins_theta(_nBins_theta) ,
      nBins_Hist(_nBins_Hist) , 
      SigmaX(_SigmaX),
      debug(_debug)
{
    refactor_d0 = 1;
    refactor_d12 = 1;
    
    LOAD();
}

//--------------------------------------------------------------

D12_Handler::~D12_Handler()
{}
//--------------------------------------------------------------

void D12_Handler::LOAD(){

    //only create Histogram array if not created before
    Histogram = std::vector<std::vector<double> >(nBins_theta,std::vector<double>(nBins_Hist,0));
    Maxima = std::vector<double>(nBins_theta,0);

    //if debug mode, no histograms are loaded!
    if(debug)
        return;

    //load histograms from specific file
    std::string d0Folder = Getd0Folder();
    std::string name = d0Folder + "d0_" + std::to_string(d0_i) + "/d12_" + std::to_string(d12_i);

    std::ifstream DATA(name);
    std::string line;

    int theta_i = 0;

    double DELTA = 180.,maxVal = 0;

    if(DATA.fail())
    {
        std::cerr << "\n\n--------------------------------" << std::endl;
        std::cerr << "Could not find " << name << std::endl;
        std::cerr <<"--------------------------------\n" << std::endl;
        exit(1);
    }
    int iter = 0;
    while(std::getline(DATA,line))
    {
        std::istringstream Buffer(line);
        std::vector<std::string> Values(std::istream_iterator<std::string>{Buffer},std::istream_iterator<std::string>());
        theta_i = DELTA*iter/nBins_theta;
        
        maxVal = 0;

        if (nBins_Hist != Values.size())
        {
            std::cerr << "\nUnexpected histogram formats in d0_ " << d0_i << "/d12_" << d12_i << " :" <<std::endl;
            std::cerr << "Exp.: " << nBins_Hist << " delivered: " << Values.size() << std::endl;
            exit(1);
        }

        for(int i = 0;i < Values.size();++i)
        {
            Histogram[theta_i][i] = std::stod(Values[i]);
            maxVal = Histogram[theta_i][i] >= maxVal ? Histogram[theta_i][i] : Histogram[theta_i][i];
        }
        Maxima[theta_i] = maxVal;

        ++iter;
    }
    if(d12_i % 100 == 0 && d12_i > 0 && false)
    {
        //std::cout << "\r";
        std::cout << "\rLoaded " << name << " histogram         ";
        std::cout.flush();
    }
}

//--------------------------------------------------------------

double D12_Handler::GetPValue(int theta_Ei,double thetaX){
    
    int cth_i = (int)((thetaX + 1.)*(nBins_Hist-1)/2.);

    return (Histogram[theta_Ei][cth_i]/Maxima[theta_Ei]);
}

//--------------------------------------------------------------

std::string D12_Handler::Getd0Folder()
{
    int sigX_int = (int) SigmaX;
    
    if(sigX_int - SigmaX != 0)
    {
        std::cerr << "----------------------------------------------" << std::endl;
        std::cerr << "SigmaX = " << SigmaX << " not integer value! " << std::endl;
        std::cerr << "This is not supported yet!" << std::endl;
        std::cerr << "----------------------------------------------" << std::endl;
        exit(1);
    }

    std::string name = "d0_Folder/d0s_" + std::to_string(sigX_int) + "_4/";
    return name;
}

//--------------------------------------------------------------

std::vector<double> D12_Handler::GetHistX(int theta_Ei,double thetaX)
{
    int cth_i = (int)((thetaX + 1.) * nBins_Hist / 2.);

    return Histogram[theta_Ei];
}

//--------------------------------------------------------------