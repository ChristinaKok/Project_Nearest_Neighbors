#include "functions.h" 
#include "lsh.h"
#include <iostream> 
#include <cstdlib>
#include <algorithm>
#include <chrono>

using namespace std;


int main(int argc, char *argv[]) {
    
    //check command line arguments
    int k=4, L=5 , N=1;
    double R=10000.0;

    int flag_d=0, flag_q=0, flag_o=0;

    std::string inputFile, queryFile, outputFile;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-d") {             //path for dataset
            inputFile = argv[i + 1];
            flag_d = 1;
        } else if (std::string(argv[i]) == "-q") {      //path for query
            queryFile = argv[i + 1];
            flag_q = 1;
        }  else if (std::string(argv[i]) == "-k") {     //number of h functions
            k = atoi(argv[i+1]);
        } else if (std::string(argv[i]) == "-L") {      //number of hash tables
            L = atoi(argv[i+1]);
        } else if (std::string(argv[i]) == "-o") {      //output file name
            outputFile = argv[i + 1];
            flag_o = 1;
        } else if (std::string(argv[i]) == "-N") {      //number of neighbors
            N = atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-R") {      //radius
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

            if (query_size > 10){
                query_size = 10;
            }


            //initialize functions(h,g) and hashtable for LSH 
            initialize_h_func(L,k,data_size);

            TableSize = number_of_points/8;

            initialize_hash_table(L, k, ar);
            
            vector<vector<pair<int,double>>> neighbors(query_size); 
            vector<vector<pair<int,double>>> real_neighbors(query_size);
            vector<vector<pair<int,double>>> range_neighbors(query_size);

            vector<double> duration_lsh(query_size);
            vector<double> duration_real(query_size);

            double avg_duration=0.0, avg_duration_real=0.0;
            vector<double> maf(query_size);

            for (int q=0; q<query_size; q++){

                vector<double> query = ar2[q];
                const std::clock_t lsh_start = std::clock();

                for (int i=0; i<L; i++){    
                    double id;                                     //ID of query for this hash table
                    double g = initialize_g_func(i,query,k,id);    //g of query for this hash table

                    find_neighbors(neighbors[q], i, g, id, ar, query, euclidean_distance);
                }

                //compute total time for LSH
                std::clock_t lsh_end = std::clock();
                duration_lsh[q] = ( lsh_end - lsh_start ) / (double) CLOCKS_PER_SEC;
                avg_duration += duration_lsh[q];


                //find real nearest neighbors
                const std::clock_t real_start = std::clock();

                for (int i=0; i<number_of_points; i++){
                    double dist = euclidean_distance(ar[i], query);
                    real_neighbors[q].push_back( make_pair(i,dist) );
                }

                sort(real_neighbors[q].begin(), real_neighbors[q].end(), sort_by_dist);     //sort real neighbors in ascending order by distance

                //compute total time for real neighbors(brute force)
                std::clock_t real_end = std::clock();
                duration_real[q] = ( real_end - real_start ) / (double) CLOCKS_PER_SEC;
                avg_duration_real += duration_real[q];

                //compute approximation factor
                if ((int)neighbors[q].size() > 0){
                    pair<int,double> appr_neighbor = *neighbors[q].begin();  
                    maf[q] = appr_neighbor.second / real_neighbors[q][0].second ;
                } else {
                    maf[q] = -1;
                }
                

                //find nearest neighbors with range search
                for (int i=0; i<L; i++){
                    double id;
                    double g = initialize_g_func(i,query,k,id);        //g of query for this hash table
                    range_search(range_neighbors[q], R, i, g, ar, query , euclidean_distance);
                }

                sort( range_neighbors[q].begin(), range_neighbors[q].end(), sort_by_dist);
                range_neighbors[q].erase( unique( range_neighbors[q].begin(), range_neighbors[q].end() ), range_neighbors[q].end() );

                vector<double>().swap(query);
                cout << "\nFinished query " << q+1 << endl;
            }

            avg_duration /= query_size;
            avg_duration_real /= query_size;
            

            //Create the output file
            ofstream OutputFile(outputFile);
            if (OutputFile.is_open()) {
                for (int q=0; q<query_size; q++){

                    int n = (int)neighbors[q].size();
                    if (N < n) {        //in case LSH finds more neighbors than N
                        n = N;
                    }
                   OutputFile << "Query: image_" << q+1 << endl;
                    int i=0;
                    for (auto nb: neighbors[q]){
                        OutputFile << "Nearest neighbor-" << i+1 << ": image_" << nb.first+1 << endl;
                        OutputFile << "distanceLSH: " << nb.second << endl;
                        OutputFile << "distanceTrue: " << real_neighbors[q][i].second << endl;
                        i++;
                        if (i == n){
                            break;
                        }
                    }
                    OutputFile << "tLSH: " << duration_lsh[q] << std::endl;
                    OutputFile << "tTrue: " << duration_real[q] << std::endl;

                    OutputFile << "R-near neighbors: " << std::endl;
                    for (auto n: range_neighbors[q]){
                        OutputFile << "image_" << n.first+1 << std::endl;
                    }

                }
                OutputFile << "\ntAverageApproximate: " << avg_duration << std::endl;
                OutputFile << "tAverageTrue: " << avg_duration_real << std::endl;
                OutputFile << "MAF: " << *max_element(maf.begin(), maf.end()) << "  [Maximum Approximation Factor]" << std::endl;
            }
            cout << "------------------------\n" << endl;


            string ans;

            cout << "\nCreated output file. Would you like to repeat algorithm? (y/n)  :  ";
            cin >> ans;

            if (ans == "n"){
                cout << "\nEnding program..." << endl;
                break;
            }

            //erase previous results
            flag_d = 0;
            flag_q = 0;
            flag_o = 0;

            vector<vector<double>>().swap(ar);
            vector<vector<double>>().swap(ar2);

            inputFile.clear();
            queryFile.clear();
            outputFile.clear();

            empty_lsh();

            vector<vector<pair<int,double>>>().swap(neighbors);
            vector<vector<pair<int,double>>>().swap(real_neighbors);
            vector<vector<pair<int,double>>>().swap(range_neighbors);
            vector<double>().swap(duration_lsh);
            vector<double>().swap(duration_real);
            
    }
    

    return 0;
}
