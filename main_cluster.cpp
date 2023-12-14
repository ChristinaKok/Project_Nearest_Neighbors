#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "functions.h"
#include "clusters.h"
#include <chrono>


int main(int argc, char *argv[]){
    
    std::string method;
    int flag_i=0, flag_c=0, flag_o=0 ,flag_m=0;
    bool complete=false;

    std::string inputFile, configurationFile, outputFile;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-i") {             //path for dataset
            inputFile = argv[i + 1];
            flag_i = 1;
        } else if (std::string(argv[i]) == "-c") {      //configuration file
            configurationFile = argv[i + 1];
            flag_c = 1;
        } else if (std::string(argv[i]) == "-o") {      //output file name
            outputFile = argv[i + 1];
            flag_o = 1;
        } else if (std::string(argv[i]) == "-complete") {      //optional
            complete = true;
        } else if (std::string(argv[i]) == "-m") {      // method: Classic OR LSH or Hypercube
            method = argv[i + 1];
            flag_m = 1;
        }
    }
    
    if(flag_i == 0 || flag_c == 0 || flag_o == 0 || flag_m == 0 || (method.compare("Classic") != 0 && method.compare("LSH") != 0 && method.compare("Hypercube") != 0)){
        cout << "Wrong input" << endl;
        exit(1);
    }
    
    //read input file
    vector<vector<double>> ar;
    int number_of_points;
    int data_size = ReadMNIST(ar,inputFile, number_of_points);

    //read cluster.conf
    int cluster_arg[6];
    readCluster(cluster_arg);

    int k = cluster_arg[0];         //k clusters
    int L = cluster_arg[1];         //L hash tables for LSH
    int k_h = cluster_arg[2];       //k_h hash functions for LSH
    int M = cluster_arg[3];         //M max number of points for hypercube
    int d = cluster_arg[4];         //d dimensions for hypercube
    int probes = cluster_arg[5];    //probes for hypercube

    vector<vector<double>> centroids;       //to save centroids index
    const std::clock_t init_start = std::clock();

    //initialize k centroids
    k_means_pp(ar,centroids,k);  
    double duration_init = ( std::clock() - init_start ) / (double) CLOCKS_PER_SEC;
    cout << "\nFinished initializing centroids" << endl;

    vector<Cluster> clusters;
    double duration_method = 0.0;

    if (method.compare("Classic") == 0){
        const std::clock_t method_start = std::clock();
        clusters = lloyds(k, centroids, ar);        //Lloyds algorithm to create clusters
        duration_method = ( std::clock() - method_start ) / (double) CLOCKS_PER_SEC;

    } else if (method.compare("LSH") == 0){
        //Create hashtable  
        initialize_h_func(L,k_h,data_size);
        TableSize = ar.size()/8;
        initialize_hash_table(L,k_h, ar);

        const std::clock_t method_start = std::clock();
        clusters = rev_lsh(L, k_h, ar, centroids);       //reverse assignment with LSH
        duration_method = ( std::clock() - method_start ) / (double) CLOCKS_PER_SEC;
        empty_lsh();

    } else if (method.compare("Hypercube") == 0){
        initialize_h_func(1,d,data_size);       //initialize v,t from LSH
        vector<vector<double>> h_vec;  
        h_vec.resize(number_of_points,vector<double>(d));
        h_func(ar, d, h_vec);          //calculate d h functions for every point
        vector<vector<int>> f_vec; 
        f_vec.resize(number_of_points,vector<int>(d));
        f_func(ar, d, h_vec, f_vec);    //Calculate d f functions for every point
        vector<pair<string,int>> str;
        concat(d, ar, str, f_vec);      //Concatenate f functions to create a string
        initialize_ht_cube(d, str);

        const std::clock_t method_start = std::clock();
        clusters = rev_cube(d,ar, centroids,M,probes);       //reverse assignment with hypercube
        duration_method = ( std::clock() - method_start ) / (double) CLOCKS_PER_SEC;
        empty_cube();
            
    }

    cout << "\nFinished clustering" << endl;
    cout << "\nComputing Silhouette" << endl;

    vector<double> s = silhouette(ar, clusters, method);
    

    //Create the output file
    ofstream OutputFile(outputFile);
    if (OutputFile.is_open()) {

        OutputFile << "Algorithm: " ;
        if (method.compare("Classic") == 0){
            OutputFile << "Lloyds" << endl;

        } else if (method.compare("LSH") == 0){
            OutputFile << "Range Search LSH" << endl;

        } else  if (method.compare("Hypercube") == 0){
            OutputFile << "Range Search Hypercube" << endl;
        }

        for (int i=0; i<k; i++){
            OutputFile << "CLUSTER-" << i+1 << " { size: " << clusters[i].points_index.size() << ", centroid: [";
            for (auto d: clusters[i].centroid){
                OutputFile << d << "  ";
            }
            OutputFile << "] }" << endl;
        }
        OutputFile << "clustering_time: " << duration_init + duration_method << std::endl;

        OutputFile << "Silhouettte: [";
        for (int i=0; i<(int)s.size(); i++){
            OutputFile << s[i] << "  ";
        }
        OutputFile << "]" << endl;
        
        if (complete){
            OutputFile << endl;
            for (int c=0; c<(int)clusters.size(); c++){
                OutputFile << "CLUSTER-" << c+1 << " { centroid: [";
                for (auto d: clusters[c].centroid){
                    OutputFile << d << "  ";
                }
                OutputFile << "], " << endl;
                for (auto p: clusters[c].points_index){
                    OutputFile << "image_" << p+1 << "  ";
                }
                OutputFile << " }" << endl;
            }
        }
                
        
    }
    OutputFile.close();

    vector<Cluster>().swap(clusters);
    vector<vector<double>>().swap(centroids);
    vector<vector<double>>().swap(ar);

    return 0;

}
