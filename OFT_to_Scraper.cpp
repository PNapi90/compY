#include <iostream>
#include <fstream>
#include <string>
#include <vector>


int main()
{
    const char* format = "%lf %lf %lf %lf %lf %lf";

    std::ifstream DATA("Single_OFT_Dist");
    double x[6];

    std::string line;
    int iter = 0;
    int line_Num = 0;

    std::vector<int> RestVals(5,0);
    for(int i = 3;i < 5;++i)
        RestVals[i] = 2;

    std::ofstream OUTPUT("OFT_Single_Scraper");

    std::vector<std::vector<double>> Buffer(2,std::vector<double>(4,0));

    while(std::getline(DATA,line))
    {
        sscanf(line.c_str(),format,&x[0],&x[1],&x[2],&x[3],&x[4],&x[5]);
        if(x[0] < 0)
        {
            if(iter > 0)
            {
                for(int i = 0;i < 2;++i)
                {
                    for(auto B : Buffer[i])
                        OUTPUT << B << " ";
                    for(auto r : RestVals)
                        OUTPUT << r << " ";
                    OUTPUT << line_Num-1 << " " << 6 << std::endl;
                }
                for(int i = 0;i < 11;++i)
                    OUTPUT << -99999 << " ";
                OUTPUT << std::endl;
            }
            ++line_Num;
            iter = 0;
        }
        else
        {
            for(int i = 0;i < 4;++i)
                Buffer[iter][i] = x[i+1];
            
            ++iter;
        }
        
    }    

    OUTPUT.close();


    return 0;
}