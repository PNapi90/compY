#ifndef COMPTON_HANDLER_H
#define COMPTON_HANDLER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>
#include <cmath>
#include <cstdlib>

class ComptonHandler
{
private:
    int nBinsE,nBinsTh;
    double width;

    std::vector<std::vector<double>> EHist;

    inline double Min(double a,double b);

public:

    ComptonHandler(int _nBinsE, int _nBinsTh);
    ~ComptonHandler();

    double Intersection(std::vector<double> &HistX,
                        std::vector<double> &E);

    void INIT();

};


#endif