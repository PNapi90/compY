#include "fuzzy_c_means.h"


fuzzy_c_means::fuzzy_c_means(double fuzz,int verbosity,int thr_num){

	this->verbosity = verbosity;
	this->thr_num = thr_num;
    fuzzyness = fuzz;
    epsilon = 10;
    set_dmax();

    weights = std::vector<std::vector<double> >(max_len,std::vector<double>(max_len,0));
    centroids = std::vector<std::vector<double> >(max_len,std::vector<double>(3,0));
    dist_2 = std::vector<std::vector<double> >(max_len,std::vector<double>(max_len,0));
    x = std::vector<std::vector<double> >(max_len,std::vector<double>(5,0));
    old_weights = std::vector<std::vector<double> >(max_len,std::vector<double>(max_len,0));
    
    first_interaction = std::vector<double>(4,0);


    cluster_to_points = std::vector<int>(max_len,0);
    cluster_len = std::vector<int>(max_len,0);

    en_centroid = std::vector<std::vector<double> >(max_len,std::vector<double>(4,0));

    fc_clusters = std::vector<std::vector<std::vector<double> > >(max_len,std::vector<std::vector<double> >(max_len,std::vector<double>(5,0)));

    fuzzFile.open("Stored/Fuzz/Fuzz_" + std::to_string(thr_num));

	first_cluster = -1;
}

fuzzy_c_means::~fuzzy_c_means(){    
    fuzzFile.close();
}



void fuzzy_c_means::init_weights(){
    
    double norm = 0.;
    for(int i = 0;i < am_clusters;i++){
        for(int k = 0;k < am_points;k++) weights[i][k] = ((double) (rand() % 1000 + 1))/1000.;
    }
    for(int k = 0;k < am_points;k++){
		for(int i = 0;i < am_clusters;i++) norm += weights[i][k];
		for(int i = 0;i < am_clusters;i++){
			weights[i][k] /= norm;
			old_weights[i][k] = weights[i][k];
		}
		norm = 0;
	}
}

void fuzzy_c_means::fuzzy_clustering(std::vector<std::vector<double> > &data,int len,int am_clusters){
	
	for(int i = 0;i < 4;++i) first_interaction[i] = data[0][i];
	
    this->am_clusters = am_clusters;
    sum_edep = 0;
    if(len == 1){
        am_clusters = 1;
        am_points = 1;
        cluster_len[0] = 1;
        for(int k = 0;k < am_points;k++){
            for(int j = 0;j < 4;j++) fc_clusters[0][k][j] = data[k][j];
        }
        if(verbosity > 0 && thr_num == 1) print_info();
        return;
    }
    
    if(len == 2){
        am_clusters = 2;
        am_points = 2;
        cluster_len[0] = 1;
        cluster_len[1] = 1;
		
		int randVal = 0;//std::rand() % 2;
        if(randVal == 0){
			for(int i = 0;i < 2;++i){
				for(int j = 0;j < 4;++j) fc_clusters[i][0][j] = data[i][j];
			}
		}
		else{
			for(int i = 0;i < 2;++i){
				for(int j = 0;j < 4;++j) fc_clusters[i][0][j] = data[1-i][j];
			}
		}

        set_energy_centroids();
        return;
    }
    
    for(int i = 0;i < len;i++){
        sum_edep += data[i][0];
        for(int j = 0;j < 5;j++) x[i][j] = data[i][j];
    }

    if(verbosity > 0 && thr_num == 1){
		std::cout << "\n###################################" << std::endl;
		for(int i = 0;i < len;i++){
			for(int j = 0;j < 4;j++) std::cout << x[i][j] << "\t";
			std::cout << std::endl;
		}
		std::cout << "###################################" << std::endl;
		std::cout << "------------------------------\n";
		std::cout << "Edep in Cluster: " << sum_edep << " keV";
		std::cout << "\n------------------------------\n" << std::endl;
    }

    am_points = len;
    //get_am_clusters();
    init_weights();
    first_iter = true;
    epsilon = 10;
    

    while(epsilon > abort_val){
        calc_centroids();
        calc_dist2();
        calc_weights();
        calc_epsilon();
        adjust_old_weights();
    }

    set_cluster_to_points();
}

void fuzzy_c_means::adjust_old_weights(){
    for(int i = 0;i < am_clusters;i++){
        if(!first_iter) for(int k = 0;k < am_points;k++) old_weights[i][k] = weights[i][k];
        else for(int k = 0;k < am_points;k++) old_weights[i][k] = 0.;
    }
}

void fuzzy_c_means::calc_centroids(){
    double sum_weights;
    for(int i = 0;i < am_clusters;i++){
        sum_weights = 0.;
        for(int j = 0;j < 3;j++){
            centroids[i][j] = 0.;
            for(int k = 0;k < am_points;k++){
                centroids[i][j] += pow(weights[i][k],fuzzyness)*x[k][j+1];
                if(j == 0) sum_weights += pow(weights[i][k],fuzzyness);
            }
            centroids[i][j] /= sum_weights;
        }
    }
}

void fuzzy_c_means::calc_dist2(){
    for(int i = 0;i < am_clusters;i++){
        for(int k = 0;k < am_points;k++){
            dist_2[i][k] = 0.;
            for(int j = 0;j < 3;j++) dist_2[i][k] += pow(x[k][j+1] - centroids[i][j],2.);
        }
    }
}

void fuzzy_c_means::calc_weights(){
    double sum_val;
    double exponent = 1./(fuzzyness - 1.);

    for(int i = 0;i < am_clusters;i++){
        for(int k = 0;k < am_points;k++){
            sum_val = 0.;
            for(int j = 0;j < am_clusters;j++){ 
				if(j != i) sum_val += pow(dist_2[i][k]/dist_2[j][k],exponent);
				else sum_val += 1;
			}
            weights[i][k] = 1./sum_val;
        }
    }
}

void fuzzy_c_means::calc_epsilon(){
    if(first_iter){
        first_iter = false;
        //return;
    }
    double norm_val = 0.;
    double max = 0;
    for(int i = 0;i < am_clusters;i++){
        for(int k = 0;k < am_points;k++){
			norm_val = std::abs(weights[i][k] - old_weights[i][k]);
			if(max < norm_val) max = norm_val;
			//std::cout << norm_val << std::endl;
		}
    }
    epsilon = max;
}

void fuzzy_c_means::reset(){
    epsilon = 10.;
    for(int i = 0;i < max_len;i++){
        for(int j = 0;j < 3;j++){
            centroids[i][j] = 0.;
            x[i][j] = 0.;
        }
        x[i][3] = 0.;
        x[i][4] = -1.;
        cluster_to_points[i] = -3;
        cluster_len[i] = 0;
    }
    first_cluster = -1;
    first_pos_i = 0;
}

void fuzzy_c_means::set_cluster_to_points(){

    double max_weight = 0.;
    int max_cl_int = 0;
    
    bool first_int = false;
    std::vector<bool> b_arr(4,false);
    
    for(int k = 0;k < am_points;k++){
        max_weight = 0.;
        max_cl_int = 0;
        for(int i = 0;i < am_clusters;i++){
            if(max_weight < weights[i][k]){
                max_weight = weights[i][k];
                max_cl_int = i;
            }
        }
        cluster_to_points[k] = max_cl_int;
        if(!first_int) first_int = false;
        for(int j = 0;j < 5;j++){
            fc_clusters[max_cl_int][cluster_len[max_cl_int]][j] = x[k][j];
        }
        
        if(first_int) first_pos_i = max_cl_int;
		
        cluster_len[max_cl_int]++;
    }
    //sort_clusters();
    set_energy_centroids();
    if(verbosity > 0 && thr_num == 1) print_info();

}

void fuzzy_c_means::set_energy_centroids(){

	double edep_temp = 0;

	for(int i = 0;i < max_len;++i){
		for(int j = 0;j < 4;++j) en_centroid[i][j] = 0;
	}

	double max_e = 0;
	int max_pos = 0;
	bool mean_or_heavy = true;

	bool smallest_dist = false;

	if(smallest_dist && am_clusters == 2){
		int pos_xy[2] = {0};

		double tmp_dist = 0;
		double min_dist = 100000;
		for(int j = 0;j < cluster_len[0];++j){
			tmp_dist = 0;
			for(int k = 0;k < cluster_len[1];++k){
				tmp_dist = 0;
				for(int o = 1;o <= 3;++o){
					tmp_dist += pow(fc_clusters[0][j][o] - fc_clusters[1][j][o],2);
				}
				tmp_dist = sqrt(tmp_dist);
				if(tmp_dist <= min_dist){
					min_dist = tmp_dist;
					pos_xy[0] = j;
					pos_xy[1] = k;
				}
			}
		}
		//over simplified->
		for(int k = 0;k < 4;++k){
			en_centroid[0][k] = fc_clusters[0][cluster_len[0]-1][k];
			en_centroid[1][k] = fc_clusters[1][0][k];
		}
		//for(int i = 0;i < 2;++i){
		//	for(int k = 0;k < 4;++k) en_centroid[i][k] = fc_clusters[i][pos_xy[i]][k];
		//}


		return;
	}

	if(!mean_or_heavy){
		double edep_tmp = 0;
		for(int i = 0;i < am_clusters;++i){
			max_e = 0;
			max_pos = 0;
			edep_tmp = 0;
			for(int j = 0;j < cluster_len[i];++j){
				edep_tmp += fc_clusters[i][j][0];
				if(max_e < fc_clusters[i][j][0]){
					max_e = fc_clusters[i][j][0];
					max_pos = j;
				}
			}
			for(int k = 1;k < 4;++k) en_centroid[i][k] = fc_clusters[i][max_pos][k];
			en_centroid[i][0] = edep_tmp;
		}
	}
	else{

		for(int i = 0;i < am_clusters;++i){
			edep_temp = 0;
			for(int j = 0;j < cluster_len[i];++j){
				edep_temp += fc_clusters[i][j][0];
				for(int k = 0;k < 3;++k) en_centroid[i][k+1] += fc_clusters[i][j][k+1]*fc_clusters[i][j][0];
			}
			en_centroid[i][0] = edep_temp;
			for(int j = 0;j < 3;++j) en_centroid[i][j+1] /= edep_temp;
			fuzzFile << edep_temp << " " ;
		}
		if(am_clusters > 0) fuzzFile << std::endl;
		
		
		for(int i = 0;i < am_clusters;++i){
			for(int j = 0;j < cluster_len[i];++j){
				if(fc_clusters[i][j][4] == 0){
					first_cluster = i;
					return;
				}
			}
		}
		
	}
}


void fuzzy_c_means::sort_clusters(){
    
    std::vector<std::vector<std::vector<double> > >tmp_cluster(am_clusters,std::vector<std::vector<double> >(am_points,std::vector<double>(4,0)));
    std::vector<int> tmp_len(am_clusters,0);

    int iter = 0;
    for(int i = 0;i < am_clusters;++i){
        if(cluster_len[i] > 0){
            for(int j = 0;j < cluster_len[i];++j){
                for(int k = 0;k < 4;++k) tmp_cluster[iter][j][k] = fc_clusters[i][j][k];
            }
            tmp_len[iter] = cluster_len[i];
            iter += 1;
        }
    }
    double edep_tmp = 0;
    for(int i = 0;i < iter;++i){
        edep_tmp = 0;
        for(int j = 0;j < tmp_len[i];++j){
            for(int k = 0;k < 4;++k) fc_clusters[i][j][k] = tmp_cluster[i][j][k];
            edep_tmp += fc_clusters[i][j][0];
        }
        cluster_len[i] = tmp_len[i];
    }
    am_clusters = iter;
}

void fuzzy_c_means::print_info(){
	std::cout << "*** Amount of fuzzy clusters: " << am_clusters << " ***" << std::endl;
    std::cout << "= = = = = = = = = = = = = = = = = =" << std::endl;
    for(int i = 0;i < am_clusters;i++){
		std::cout << "*************************\n";
        for(int k = 0;k < cluster_len[i];k++){
            for(int j = 0;j < 4;j++){
                std::cout << fc_clusters[i][k][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "*************************\n";
        std::cout << "--------------------------\n";
        std::cout << "len of cluster: " << cluster_len[i];
        std::cout << "\n--------------------------\n";
    }
    std::cout << "= = = = = = = = = = = = = = = = = =" << std::endl;
}

void fuzzy_c_means::set_dmax(){
    dmax = DMAX;
}

void fuzzy_c_means::get_am_clusters(){
    am_clusters = 1;

    double** distances_tmp = new double*[am_points];
    for(int i = 0;i < am_points;i++){
		distances_tmp[i] = new double[am_points];
		for(int j = 0;j < am_points;j++) distances_tmp[i][j] = 0;
	}
	for(int i = 0;i < am_points;i++){
		std::cout << i+1 << " -> ";
		for(int j = 0;j < am_points;j++){
			if(i >= j||true){
				for(int k = 0;k < 3;k++){
					distances_tmp[i][j] += pow(x[i][k] - x[j][k],2);
				}
				distances_tmp[i][j] = sqrt(distances_tmp[i][j]);
				std::cout << distances_tmp[i][j] << "\t";
			}

		}
		std::cout << std::endl;
	}

    for(int i = 0;i < am_clusters;i++) cluster_len[i] = 0;


    for(int i = 0;i < am_points;i++) delete[] distances_tmp[i];
	delete[] distances_tmp;
}

int fuzzy_c_means::return_am_clusters(){
	int amount = 0;
	for(int i = 0;i < am_clusters;++i) if(cluster_len[i] > 0) amount += 1;
	return amount;
}

int fuzzy_c_means::return_cluster_len(int pos){return cluster_len[pos];}

std::vector<std::vector<double> >  fuzzy_c_means::return_cluster(int pos){return fc_clusters[pos];}

double fuzzy_c_means::get_edep_in_cluster(){return sum_edep;}

std::vector<std::vector<double> > fuzzy_c_means::return_energy_centroids(){
	return en_centroid;
}


int fuzzy_c_means::get_first_cluster_ID(){
	return first_cluster*0;
}
