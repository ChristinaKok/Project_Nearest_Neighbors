#include "functions.h" 
#include "lsh.h"
#include "hypercube.h"
#include <iostream> 
#include <cstdlib>
#include <algorithm>
#include <bitset>
#include <unordered_map>
#include <ctime>

using namespace std;


int main(int argc, char *argv[]) {
    
    //check command line arguments
    int k=14, M=10, probes=2, N=1;
    double R=10000.0;

    int flag_d=0, flag_q=0, flag_o=0;

    std::string inputFile, queryFile, outputFile;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-d") {                 //path for dataset
            inputFile = argv[i + 1];
            flag_d = 1;
        } else if (std::string(argv[i]) == "-q") {          //path for query
            queryFile = argv[i + 1];
            flag_q = 1;
        }  else if (std::string(argv[i]) == "-k") {         //number of h functions (dimension k)
            k = atoi(argv[i+1]);
        } else if (std::string(argv[i]) == "-M") {          //number of points to be searched
            M = atoi(argv[i+1]);
        } else if (std::string(argv[i]) == "-probes") {     //number of vertices to be searched
            probes = atoi(argv[i+1]);
        } else if (std::string(argv[i]) == "-o") {          //output file name
            outputFile = argv[i + 1];
            flag_o = 1;
        } else if (std::string(argv[i]) == "-N") {          //number of neighbors
            N = atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-R") {          //radius
            R = atof(argv[i + 1]);
        }
    }

    while(1){

            vector<vector<double>> ar;
            vector<vector<double>> ar2;

            int number_of_points;
            int number_of_queries;

            if (flag_d == 0){
                cout << "\nPlease give a path for dataset:  " ;
                cin >> inputFile;
            }

            if (flag_q == 0){
                cout << "\nPlease give a path for query:  " ;
                cin >> queryFile;
            }

            if (flag_o == 0){
                cout << "\nPlease give a path for output file:  " ;
                cin >> outputFile;
            }
            
            //read input files for data set and query
            int data_size = ReadMNIST(ar,inputFile, number_of_points);
            int query_size = ReadMNIST(ar2,queryFile, number_of_queries);

            //initialize v,t from LSH
            initialize_h_func(1,k,data_size);


            vector<vector<double>> h_vec;  
            h_vec.resize(number_of_points,vector<double>(k));

            //calculate k h functions for every point
            h_func(ar, k, h_vec);


            vector<vector<int>> f_vec; 
            f_vec.resize(number_of_points,vector<int>(k));

            //Calculate k f functions for every point
            f_func(ar, k, h_vec, f_vec);

              
            //Concatenate f functions to create a string
            vector<pair<string,int>> str;

            concat(k, ar, str, f_vec);  
           
            initialize_ht_cube(k, str);

        
            //Find where the query belongs
            //calculate k h functions for query
            
            vector<vector<double>> h_vec2;
            h_vec2.resize(number_of_queries,vector<double>(k));
            h_func(ar2,k,h_vec2);

            
            //Calculate k f functions for query
            vector<vector<int>> f_vec2; 
            f_vec2.resize(number_of_queries,vector<int>(k));
            f_func(ar2,k,h_vec2,f_vec2);

            //Concatenate f functions to create a string
            vector<pair<string,int>> query_str;
            concat(k, ar2 , query_str, f_vec2); 


            if (query_size > 10){
                query_size = 10;
            }

            vector<vector<pair<int,double>>> neighbors(query_size);
            vector<vector<pair<int,double>>> real_neighbors(query_size);
            vector<vector<pair<int,double>>> range_neighbors(query_size);

            vector<double> duration_cube(query_size);
            vector<double> duration_real(query_size);

            double avg_duration=0.0, avg_duration_real=0.0, avg_af=0.0;
            vector<double> af(query_size);

            for (int q=0; q<query_size; q++){

                //Start timing for Binary (0/1) hypercube
                std::clock_t cube_start = std::clock();

                // Find N neighbors
                find_neighbors_cube(query_str[q],neighbors[q],N,M,probes,ar,ar2,euclidean_distance);

                //compute total time for Binary (0/1) hypercube
                std::clock_t cube_end = std::clock();
                duration_cube[q] = ( cube_end - cube_start ) / (double) CLOCKS_PER_SEC;
                avg_duration += duration_cube[q];


                //find real nearest neighbors
                //Start timing for real neighbors(brute force)
                std::clock_t start_real = std::clock();

                for (int i=0; i<number_of_points; i++){
                    double dist = euclidean_distance(ar[i], ar2[q]);
                    real_neighbors[q].push_back( make_pair(i,dist) );
                }

                sort(real_neighbors[q].begin(), real_neighbors[q].end(), sort_by_dist);     //sort real neighbors in ascending order by distance

                //compute total time for real neighbors(brute force)
                std::clock_t end_real = std::clock();
                duration_real[q] = ( end_real - start_real ) / (double) CLOCKS_PER_SEC;
                avg_duration_real += duration_real[q];

                //compute approximation factor
                if ((int)neighbors[q].size() > 0){
                    pair<int,double> appr_neighbor = neighbors[q][0];
                    af[q] = appr_neighbor.second / real_neighbors[q][0].second ;
                } else {
                    af[q] = -1;
                }
                avg_af += af[q];
                
                //Range Search
                range_search_cube(query_str[q],range_neighbors[q],M,R,probes,ar,ar2,euclidean_distance);

                
                cout << "\nFinished query " << q+1 << endl;
            
            }

            avg_duration /= query_size;
            avg_duration_real /= query_size;
            avg_af /= query_size;

            //Create the output file
            ofstream OutputFile(outputFile);
            if (OutputFile.is_open()) {
                
                for (int q=0; q<query_size; q++){

                    OutputFile << "Query: image_" << q+1 << endl;
                    int n = (int)neighbors[q].size();
                    if (N < n) {        //in case algorithm finds more neighbors than N
                        n = N;
                    }
                    int i=0;
                    for (auto nb: neighbors[q]){
                        OutputFile << "Nearest neighbor-" << i+1 << ": image_" << nb.first+1 << endl;
                        OutputFile << "distanceHypercube: " << nb.second << endl;
                        OutputFile << "distanceTrue: " << real_neighbors[q][i].second << endl;
                        i++;
                        if (i == n){
                            break;
                        }
                    }
                    OutputFile << "tHypercube: " << duration_cube[q] << std::endl;
                    OutputFile << "tTrue: " << duration_real[q] << std::endl;

                    OutputFile << "R-near neighbors: " << std::endl;
                    for (auto n: range_neighbors[q]){
                        OutputFile << "image_" << n.first+1 << std::endl;
                    }


                }
                OutputFile << "\ntAverageApproximate: " << avg_duration << std::endl;
                OutputFile << "tAverageTrue: " << avg_duration_real << std::endl;
                OutputFile << "AAF: " << avg_af << "  [Average Approximation Factor]" << std::endl;

            }
            OutputFile.close();

            string ans;

            cout << "\nCreated output file. Would you like to repeat algorithm? (y/n)  :  ";
            cin >> ans;

            if (ans == "n"){
                cout << "\nEnding program..." << endl;
                break;
            }

            cout << "------------------------\n" << endl;

            //erase previous results
            flag_d = 0;
            flag_q = 0;
            flag_o = 0;

            vector<vector<double>>().swap(ar);
            vector<vector<double>>().swap(ar2);

            inputFile.clear();
            queryFile.clear();
            outputFile.clear();

            
            vector<vector<double>>().swap(h_vec);
            vector<vector<int>>().swap(f_vec);
            vector<vector<double>>().swap(h_vec2);
            vector<vector<int>>().swap(f_vec2);

            empty_cube();

            vector<vector<pair<int,double>>>().swap(neighbors);
            vector<vector<pair<int,double>>>().swap(real_neighbors);
            vector<vector<pair<int,double>>>().swap(range_neighbors);

            vector<double>().swap(duration_cube);
            vector<double>().swap(duration_real);

    }
    

    return 0;
}
