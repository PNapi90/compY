#ifndef PSIMERGER_H
#define PSIMERGER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>
#include <cmath>
#include <cstdlib>

class PsiMerger
{
private:

    int d0;
    std::vector < std::vector<std::vector<int>>> Histograms;

    void LOAD();

public:
    PsiMerger(int _d0);
    ~PsiMerger();

    bool GetP(int d12_i,
                int E_i,
                int theta_i);
};


#endif