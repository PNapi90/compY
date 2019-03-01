#ifndef GEOMETRY_CHECKER_H
#define GEOMETRY_CHECKER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>

class Geometry_Checker{
private:

    int lens_ARR[5],GANIL_FILES;

    std::vector<std::vector<double>> ganil_theta;
    std::vector<std::vector<std::vector<double>>> ganil_phi;

    double save_position[2][3];

    std::ofstream Thrown;

    void set_GANIL();

    bool check_Geometry(int,double,double,double,bool);


public:
    Geometry_Checker();
    ~Geometry_Checker();
    
    double outer_shell(std::vector<std::vector<double> >);
    double get_air_path(std::vector<std::vector<double> >);
    
    bool direct_check(std::vector<double>&);
};


#endif
