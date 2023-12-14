#include "functions.h" 
#include "graph.h"
#include <iostream> 
#include <cstdlib>
#include <algorithm>
#include <chrono>

using namespace std;


int main(int argc, char *argv[]) {
    
    //check command line arguments
    int k=50, E=30, R=1, l=20, N=1, m=2;
    int flag_init = false;      //flag for creating a new graph or reading it from a file 
    int flag_rep = false;       //flag for repeating algorithm

    int flag_d=0, flag_q=0, flag_o=0;

    int nav_node = -1;

    std::string inputFile, queryFile, outputFile;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-d") {             //path for dataset
            inputFile = argv[i + 1];
            flag_d = 1;
        } else if (std::string(argv[i]) == "-q") {      //path for query
            queryFile = argv[i + 1];
            flag_q = 1;
        }  else if (std::string(argv[i]) == "-k") {     //number of k-NN
            k = atoi(argv[i+1]);
        } else if (std::string(argv[i]) == "-E") {      //number of expansions
            E = atoi(argv[i+1]);
        } else if (std::string(argv[i]) == "-o") {      //output file name
            outputFile = argv[i + 1];
            flag_o = 1;
        } else if (std::string(argv[i]) == "-N") {      //number of neighbors
            N = atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-R") {      //number of random restarts
            R = atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-l") {      //number of candidates
            l = atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-m") {      //method
            m = atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-init"){    //initialize new graph
            flag_init = true;
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


            if(flag_init == true){    //create a new graph
                if(m == 1){         //GNNS
                    initialize_graph(data_size, k, ar);
                } else{     //MRNG
                    mrng_initialize_graph(ar, euclidean_distance);
                    nav_node = find_navigating_node(data_size, ar, euclidean_distance);     //find navigating node
                }
            }
            else if (flag_rep == false){
                if(m == 1){     //GNNS
                    //read the file graph_gnns.txt
                    vector<string> graph_data = Read_txt(ar.size(), "graph_gnns.txt");
                    convert_graph(graph_data, ar);
                }
                else{   //MRNG
                    //read the file graph_MRNG.txt
                    vector<string> graph_data = Read_txt(ar.size(), "graph_MRNG.txt");
                    convert_graph(graph_data, ar);
                    nav_node = find_navigating_node(data_size, ar, euclidean_distance);     //find navigating node
                }
            }

            vector<set<pair<int,double>,Cmp>> neighbors(query_size); 
            vector<vector<pair<int,double>>> real_neighbors(query_size);

            vector<double> duration(query_size);
            vector<double> duration_real(query_size);

            vector<double> maf(query_size);

            for(int q=0; q<query_size; q++){

                vector<double> query = ar2[q];
                
                if(m == 1){     //find neighbors using GNNS
                    const std::clock_t start_gnns = std::clock();
                    neighbors[q] = gnns(E, R, N, ar, query, euclidean_distance);    //GNNS
                    const std::clock_t end_gnns = std::clock();
                    duration[q] = (end_gnns - start_gnns) / (double) CLOCKS_PER_SEC;
                }
                else{   //find neighbors using SEARCH-ON-GRAPH

                    const std::clock_t start_search = std::clock();
                    neighbors[q] = search(l,N, nav_node, ar, query, euclidean_distance);    //SEARCH-ON-GRAPH
                    
                    const std::clock_t end_search = std::clock();
                    duration[q] = (end_search - start_search) / (double) CLOCKS_PER_SEC;

                }

                //find real nearest neighbors(brute force)
                const std::clock_t real_start = std::clock();

                for (int i=0; i<number_of_points; i++){
                    double dist = euclidean_distance(ar[i], query);
                    real_neighbors[q].push_back( make_pair(i,dist) );
                }

                sort(real_neighbors[q].begin(), real_neighbors[q].end(), sort_by_dist);     //sort real neighbors in ascending order by distance

                //compute total time for real neighbors(brute force)
                std::clock_t real_end = std::clock();
                duration_real[q] = ( real_end - real_start ) / (double) CLOCKS_PER_SEC;

                //compute approximation factor
                if ((int)neighbors[q].size() > 0){
                    pair<int,double> appr_neighbor = *neighbors[q].begin();  
                    maf[q] = appr_neighbor.second / real_neighbors[q][0].second ;
                } else {
                    maf[q] = -1;
                }

                cout << "\nFinished query " << q+1 << endl;
            
            }

            //compute average approximate time and average true time
            double avg_duration = 0.0, avg_duration_real = 0.0;

            for (int i=0; i<query_size; i++){
                avg_duration += duration[i];
                avg_duration_real += duration_real[i];
            }

            avg_duration /= query_size;
            avg_duration_real /= query_size;


            // Create the output file
            ofstream OutputFile(outputFile);
            if (OutputFile.is_open()) {
                if (m == 1){
                    OutputFile << "GNNS Results" << endl;
                } else if (m == 2){
                    OutputFile << "MRNG Results" << endl;
                }
                for (int q=0; q<query_size; q++){

                    int n = (int)neighbors[q].size();
                    if (N < n) {        //in case GNNS/MRNG finds more neighbors than N
                        n = N;
                    }
                    OutputFile << "Query: image_" << q+1 << endl;
                    int i=0;
                    for (auto nb: neighbors[q]){
                        OutputFile << "Nearest neighbor-" << i+1 << ": image_" << nb.first+1 << endl;
                        OutputFile << "distanceApproximate: " << nb.second << endl;
                        OutputFile << "distanceTrue: " << real_neighbors[q][i].second << endl;
                        i++;
                        if (i == n){
                            break;
                        }
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
            flag_d = 1;
            flag_q = 0;
            flag_o = 0;
            
            flag_init = false;
            flag_rep = true;

            vector<vector<double>>().swap(ar2);

            queryFile.clear();
            outputFile.clear();

            vector<set<pair<int,double>,Cmp>>().swap(neighbors);
            vector<vector<pair<int,double>>>().swap(real_neighbors);
            vector<double>().swap(duration);
            vector<double>().swap(duration_real);
    }
    

    return 0;
}
