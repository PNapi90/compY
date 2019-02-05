#include "ComptonHandler.hpp"

//--------------------------------------------------------------

ComptonHandler::ComptonHandler(int _nBinsE,
                               int _nBinsTh)
    : nBinsE(_nBinsE) ,
      nBinsTh(_nBinsTh)
{}

//--------------------------------------------------------------

ComptonHandler::~ComptonHandler()
{}

//--------------------------------------------------------------

void ComptonHandler::INIT()
{
    std::ifstream DATA("ComptonHists/Hist_Cs");
    if(DATA.fail())
    {
        std::cerr << "Could not find ComptonHists/Hist_Cs" << std::endl;
        exit(1);
    }

    EHist = std::vector<std::vector<double> >(nBinsE,std::vector<double>(nBinsTh,0));

    std::string line;
    int iter = 0;

    while(std::getline(DATA,line))
    {
        if(iter > nBinsE)
        {
            std::cerr << "Wrong file format in Ehist file" << std::endl;
            std::cerr << "# binsE in file = " << iter << " <-> # binsE defined = " << nBinsE << std::endl;
            exit(1);
        }
        std::istringstream Buffer(line);
        std::vector<std::string> Words(std::istream_iterator<std::string>{Buffer},
                                       std::istream_iterator<std::string>());
        
        if(Words.size() != nBinsTh)
        {
            std::cerr << "Wrong file format in Ehist file" << std::endl;
            std::cerr << "# binsTh in file = " << Words.size() << " <-> # binsTh defined = " << nBinsTh << std::endl;
            exit(1);
        }

        for(int i = 0;i < Words.size();++i)
        {
            EHist[iter][i] = std::stod(Words[i]);
        }

        ++iter;
    }
}

//--------------------------------------------------------------

double ComptonHandler::Intersection(std::vector<double> &HistX,std::vector<double> &E)
{
    double P = 0;

    double Ein = E[0];
    double Edep = E[1];

    if(HistX.size() != EHist[0].size())
    {
        std::cerr << "X and E histogram sizes don't match!" << std::endl;
        std::cerr << "X.size() = " << HistX.size() << " <-> E.size() = "<< EHist[0].size() << std::endl;
        exit(1);
    }
    
    //get Energy bin 
    int Ebin = (int) (Edep/(700./nBinsE));

    //calculate area of intersection between HistX and EHist
    for(int i = 0;i < HistX.size();++i)
    {
        P += width*Min(HistX[i],EHist[Ebin][i]);
    }

    return P;
}

//--------------------------------------------------------------

inline double ComptonHandler::Min(double a,double b)
{
    return ((a <= b) ? a : b);
} 

//--------------------------------------------------------------