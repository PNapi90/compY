#include "Geometry_Checker.h"


Geometry_Checker::Geometry_Checker(){

    for(int i = 0;i < 5;++i) lens_ARR[i] = 0;
    for(int i = 0;i < 2;++i) for(int j = 0;j < 3;++j) save_position[i][j] = 0;

    lens_ARR[0] = 57;
    lens_ARR[1] = 20;
    lens_ARR[2] = 15;

    ganil_theta = new double*[3];
    ganil_phi = new double**[3];

    for(int i = 0;i < 3;++i){
    	ganil_theta[i] = new double[lens_ARR[i]];
    	ganil_phi[i] = new double*[lens_ARR[i]];
    	for(int j = 0;j < lens_ARR[i];++j) ganil_phi[i][j] = new double[2];
    }

    set_GANIL();
}

Geometry_Checker::~Geometry_Checker(){

    for(int i = 0;i < 3;++i){
        for(int j = 0;j < lens_ARR[i];++j) delete[] ganil_phi[i][j];
        delete[] ganil_phi[i];
        delete[] ganil_theta[i];
    }
    delete[] ganil_theta;
    delete[] ganil_phi;

}


double Geometry_Checker::outer_shell(std::vector<std::vector<double> > vec){
    
    double vec_tmp1[2][3];

    double distance = 0;
    for(int i = 0;i < 3;++i) distance += pow(vec[0][i+1] - vec[1][i+1],2);
    distance = sqrt(distance);
    
    double dir_vec[2][3];
    for(int i = 0;i < 3;++i){
        dir_vec[0][i] = (vec[1][i+1] - vec[0][i+1])/distance;
        dir_vec[1][i] = (vec[0][i+1] - vec[1][i+1])/distance;
    }
    
    double dist_quant = distance/100.;
    double lambda_t[2];
    
    double r_t[2],th_t[2],phi_t[2];
    bool check_stuff[2] = {true,true};
    bool happend = false;
    for(int i = 0;i < 100;++i){
        lambda_t[0] = (i+1)*dist_quant;
        lambda_t[1] = (i+1)*dist_quant;
        r_t[0] = 0;
        r_t[1] = 0;
        for(int j = 0;j < 3;++j){
            vec_tmp1[0][j] = vec[0][j+1] + lambda_t[0]*dir_vec[0][j];
            vec_tmp1[1][j] = vec[1][j+1] + lambda_t[1]*dir_vec[1][j];
            for(int k = 0;k < 2;++k) r_t[k] += pow(vec_tmp1[k][j],2);
        }
        for(int j = 0;j < 2;++j){
            r_t[j] = sqrt(r_t[j]);
            th_t[j] = acos(vec_tmp1[j][2]/r_t[j]);
            phi_t[j] = atan2(vec_tmp1[j][1],vec_tmp1[j][0]);
        }
        
        for(int j = 0;j < 2;++j) check_stuff[j] = check_Geometry(j,r_t[j],th_t[j],phi_t[j],check_stuff[j]);
        happend = !check_stuff[0] && !check_stuff[1];
        
        if(happend) break;
    }
    if(happend){
        double norm_tmp = 0;
        for(int i = 0;i < 3;++i) norm_tmp += pow(save_position[0][i] - save_position[1][i],2);

        return sqrt(norm_tmp);
    }
    return 0;
}


double Geometry_Checker::get_air_path(std::vector<std::vector<double> > vec){
	double x1[3] = {vec[0][1],vec[0][2],vec[0][3]};
	double x2[3] = {vec[1][1],vec[1][2],vec[1][3]};

	double inner_radius = 235;

	double norm_gamma = 0;
	double norm_x2 = 0;
	double gamma_vec[3] = {0.,0.,0.};
	for(int i = 0;i < 3;i++){
		gamma_vec[i] = x2[i] - x1[i];
		norm_x2 += pow(x1[i],2);
		norm_gamma += pow(gamma_vec[i],2);
	}
	norm_gamma = sqrt(norm_gamma);
	for(int i = 0;i < 3;i++) gamma_vec[i] /= norm_gamma;

	double scalar_product = 0;
	for(int i = 0;i < 3;i++) scalar_product += gamma_vec[i]*x1[i];

	double inside_shell = norm_x2 - pow(inner_radius,2);

	if(pow(scalar_product,2) <= inside_shell) return 0.;

	double lambdas[2];
	for(int i = 0;i < 2;i++){
		lambdas[i] = -scalar_product + pow(-1,i)*sqrt(pow(scalar_product,2.) - inside_shell);
		//if(lambdas < 0 || lambdas > norm_gamma) return 0.;

	}
	if(lambdas[0] >= norm_gamma && lambdas[1] >= norm_gamma) return 0;
	if(lambdas[0] < 0 && lambdas[1] < 0) return 0;
	if(lambdas[0] >= 0 && lambdas[1] >= 0) return 2.*sqrt(pow(scalar_product,2.) - inside_shell);
	return 0;
}


bool Geometry_Checker::check_Geometry(int j,double r,double th,double phi,bool check_previous){
    
	if(!check_previous) return false;
    
    if(th > 70*M_PI/180.){
    	std::cerr << "Angles too large?! " << th*180./M_PI << std::endl;
        exit(0);
    }
    if(th < 6.3*M_PI/180.){
        save_position[j][0] = r*sin(th)*cos(phi);
        save_position[j][1] = r*sin(th)*sin(phi);
        save_position[j][2] = r*cos(th);
        return false;
    }
    int pos[2] = {0,0};
    
    bool broken = false;
    
    for(int i = 0;i < 3;++i){
        for(int k = 0;k < lens_ARR[i];++k){
            if(th >= ganil_theta[i][k] && th < ganil_theta[i][k+1]){
                pos[0] = i;
                pos[1] = k;
                broken = true;
                break;
            }
        }
        if(!broken) continue;
        if(phi >= ganil_phi[pos[0]][pos[1]][0] && phi < ganil_phi[pos[0]][pos[1]][1]){
            save_position[j][0] = r*sin(th)*cos(phi);
            save_position[j][1] = r*sin(th)*sin(phi);
            save_position[j][2] = r*cos(th);
            return false;
        }
        broken = false; 
    }
    
    return true;
}

//set Geometry of GANIL (30 crystals -> 2016 Cs run)
void Geometry_Checker::set_GANIL(){
   
    std::ifstream angle_data;
    char str[100];
    for(int i = 0;i < 3;++i){
        sprintf(str,"inputdata/GANIL_ANGLES_%d.Angles",i+1);
        angle_data.open(str);

        if(angle_data.fail()){
        	std::cerr << "Could not find " << str << std::endl;
        	exit(1);
        }
        for(int j = 0;j < lens_ARR[i];++j){
            angle_data >> ganil_theta[i][j] >> ganil_phi[i][j][0] >> ganil_phi[i][j][1];
            ganil_theta[i][j] *= M_PI/180.;
            ganil_phi[i][j][0] *= M_PI/180.;
            ganil_phi[i][j][1] *= M_PI/180.; 
        }
        angle_data.close();
        angle_data.clear();
    }
}


bool Geometry_Checker::direct_check(std::vector<double> &vec_t){
    double vec[3] = {-vec_t[1],vec_t[2],-vec_t[3]};
    
    double r = 0;
    for(int i = 0;i < 3;++i) r += pow(vec[i],2);
    r = sqrt(r);
    double th = acos(vec[2]/r);
    double phi = atan2(vec[1],vec[0]);

    bool bad_area = check_Geometry(0,r,th,phi,true);
    
    return bad_area;
}
