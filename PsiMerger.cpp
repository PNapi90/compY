#include "PsiMerger.hpp"

//--------------------------------------------------------------

PsiMerger::PsiMerger(int _E,
                     int _d0,
                     bool _debug)
    : d0(_d0),
      E(_E),
      debug(_debug)
{

    if(E == -999)
    {
        folderName = "TmpEGamma_661/";
        //folderName = "EGamma_661/";
        Ebins = 175;
    }
    else
    {
        folderName = "EGamma_" + std::to_string(E) + "/";
        Ebins = (int)(E/4) + 1;
    }
    Histograms = std::vector<std::vector<std::vector<int>>>(150,
                 std::vector<std::vector<int>>(Ebins,
                 std::vector<int>(2,0)));

    //if debug mode, no histograms are loaded!
    if(!debug)
        LOAD();
}

//--------------------------------------------------------------

PsiMerger::~PsiMerger()
{}

//--------------------------------------------------------------

bool PsiMerger::GetP(int d12_i,int E_i,int theta_i)
{

    //ugly fix for coarse binned Egamma grid of histograms
    if(E_i >= Ebins)
    {
        std::cerr << "Binning related problem: " << E_i*4 << " >= " << Ebins*4 << std::endl;
        return false;
    }

    int lower_Limit = Histograms[d12_i][E_i][0];
    int upper_Limit = Histograms[d12_i][E_i][1];

    bool a = theta_i >= lower_Limit;
    bool b = theta_i <= upper_Limit;

    return a && b;
}

//--------------------------------------------------------------

void PsiMerger::LOAD()
{
    std::string name;
    std::string folder = "d0_E_Merge/" + folderName + "d0_" + std::to_string(d0);
    std::ifstream DATA;

    int d12_i = 0;

    for(int i = 0;i < 150;++i)
    {   
        d12_i = i*4;
        name = folder + "/d12_" + std::to_string(d12_i) + "_Ranges";

        DATA.open(name);
        if(DATA.fail())
        {
            std::cerr << "Could not open " << name << std::endl;
            exit(1);
        }

        for(int j = 0;j < Ebins;++j)
        {
            for(int k = 0;k < 2;++k)
            {
                DATA >> Histograms[i][j][k];
            }
        }
        
        DATA.close();
        DATA.clear();
    }
    if (true)
    {
        std::cout << "\t\t\t\t\t\t\t\r";
        std::cout << "Loaded " << folder << " E(\u03B8)-X(\u03D1) histograms";
        std::cout.flush();
    }
}

//--------------------------------------------------------------