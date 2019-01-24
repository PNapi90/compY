#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::ifstream Singles("Gamma_GANIL/DATA_REAL/Singles");
    std::ifstream OFT("Gamma_GANIL/DATA_REAL/OFT");
    std::ifstream MC("OutputFolder/Output_FWHM_5.000000_0_1");

    std::string line;

    int countS = 0,countO = 0,countM = 0;

    while(std::getline(Singles,line))
        ++countS;

    while(std::getline(OFT,line))
        ++countO;

    while(std::getline(MC,line))
        ++countM;
    
    std::ofstream Lens("Lens.dat");

    Lens << countS-1 << " " << countO-1 << " "<< countM<< std::endl;

    Lens.close();

    return 0;
}