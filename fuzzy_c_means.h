

#ifndef FUZZY_MEAN_H
#define FUZZY_MEAN_H

#include <random>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

class fuzzy_c_means{

private:

    const int max_len = 10;
    const double abort_val = 1e-3;
    const double DMAX = 80.;

    bool first_iter;
    int fuzzyness;
    int verbosity;
    int am_points;
    int am_clusters;
    int thr_num,first_pos_i,first_cluster;
    double dmax;
    double epsilon;
    double sum_edep;

	std::ofstream fuzzFile;

    std::vector<int> cluster_to_points;
    std::vector<int> cluster_len;
    
    std::vector<std::vector<double> > x;
    std::vector<std::vector<double> > weights;
    std::vector<std::vector<double> > old_weights;
    std::vector<std::vector<double> > centroids;
    std::vector<std::vector<double> > en_centroid;
    std::vector<std::vector<double> > dist_2;

    std::vector<std::vector<std::vector<double> > > fc_clusters;
    
    std::vector<double> first_interaction;

    void get_am_clusters();
    void set_dmax();
    void init_weights();
    void adjust_old_weights();
    void calc_centroids();
    void calc_dist2();
    void calc_weights();
    void calc_epsilon();
    void set_cluster_to_points();
    void print_info();
    void sort_clusters();
    void set_energy_centroids();

public:
    fuzzy_c_means(double,int,int);
    ~fuzzy_c_means();
    
    void fuzzy_clustering(std::vector<std::vector<double> >&,int,int);
    void reset();
    
    int return_am_clusters();
    int return_cluster_len(int);
    int get_first_cluster_ID();
	double get_edep_in_cluster();

    std::vector<std::vector<double> > return_cluster(int);
    std::vector<std::vector<double> > return_energy_centroids();
};



#endif

