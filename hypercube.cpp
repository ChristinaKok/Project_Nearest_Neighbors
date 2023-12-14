#include "functions.h"
#include <cmath>
#include "lsh.h"
#include <map>
#include "hypercube.h"
#include <stdlib.h> 
#include <set>
#include <algorithm>

#include <ctime>


int table_sz;

void h_func(const vector<vector<double>> &ar,int k,vector<vector<double>> &h_vec){

    for (int p=0; p<(int)ar.size(); p++){ //For every point  

        vector<double> point = ar[p];

        for (int j=0; j<k; j++){
            double a = 0;
            for (int z=0; z<(int)point.size(); z++){
                a += point[z]*v[0][j][z];   //there is only 1 hash table (L=1), so use v[0]
            }
            a += t[0][j];
            int h = floor(a/w);

            h_vec[p][j] = h;

        }
                
     }
}



std::vector<std::map<int, int>> calc_h; //Vector of maps to keep h that have been calculated

void f_func(const vector<vector<double>> &ar,int k,vector<vector<double>> &h_vec,vector<vector<int>> &f_vec){

    unsigned seed = 123;
    srand(seed);
    
    calc_h.resize(k); //Positions of vector are the h functions (h1,h2,h3,...,hk)
    
    for (int i = 0; i < (int)ar.size(); i++) {  //For every point
        for (int j = 0; j < (int)h_vec[i].size(); j++) {    //For every h
            if (calc_h[j].find(h_vec[i][j]) == calc_h[j].end()) { //If h has not been calculated yet
                //Calculate this f
                f_vec[i][j] = rand() % 2;
                calc_h[j][h_vec[i][j]] = f_vec[i][j]; //Add to map
            }
            else { //If this value for h has been calculated
                f_vec[i][j] = calc_h[j][h_vec[i][j]];   //Get it from map
            }
        }
    }
    
}

std::string concat_help(const std::vector<int> b, int k) {      //converts a vector<int> to string
    std::string result;
    for (int i = 0; i < k; i++) {
        result += std::to_string(b[i]);
    }
    return result;
}

void concat(int k, const vector<vector<double>> &ar, vector<pair<string,int>> &str , const vector<vector<int>> &f_vec){
    vector<int> b;
    b.resize(k);

    for(int i=0; i<(int)ar.size(); i++){
        for(int j=0;j<k;j++){
            b[j] = f_vec[i][j];
        }
        str.push_back( make_pair(concat_help(b,k), i));
    }

}

void initialize_ht_cube(int k, const vector<pair<string,int>> &str){

    table_sz = round(pow(2,k));      //number of buckets

    hash_table.resize(table_sz);

    for (auto s: str){
        int i = stoi(s.first, nullptr, 2);      //convert string to int (binary to decimal)
        hash_table[i].push_back(s.second);
    }
}


int hammingDistance(int n1, int n2){
    int x = n1 ^ n2;        //xor: 0 if bits are the same, 1 if they are different 
    int diff_bits = 0;      //diff_bits: counter for bits with 1
 
    while (x > 0) {
        diff_bits += x & 1;
        x >>= 1;
    }
 
    return diff_bits;
}

vector<int> neighbors_func(const pair<string,int> query,int M,int probs,const vector<vector<double>> &ar, const vector<vector<double>> &ar2,double (*distance)(const vector<double> &point, const vector<double> &query)){

    vector<int> neighbors;
    int q_index; //Index of bucket that query belongs
    int index; //Index of bucket that we are checking
    int ham_dist = 1;
    int cur_M = 0; //Points that have been checked
    int cur_probs = 1; //Bucket that has been checked (including the bucket that query belongs)
    string q_key = query.first;
    
    //Find in which int q_key belongs
    q_index = stoi(q_key, nullptr, 2);      //convert string to int (binary to decimal)
    
    //Find neighbors
    for(int i=0;i<(int)hash_table[q_index].size();i++){
        neighbors.push_back(hash_table[q_index][i]);        //add neighbors from the same bucket as query
        cur_M++;
        if (cur_M == M) break;
    }
    
    if(cur_M < M && cur_probs < probs){ //Search in other buckets according to hamming distance
        
        //check hash table buckets from the start
        if(q_index == 0){ 
            index = 1;      //index: bucket to be checked
        }
        else{
            index = 0;
        }
        while(cur_M < M && cur_probs < probs){

            int dist = hammingDistance(index,q_index);
            if(dist == ham_dist){
                for(int j=0;j<(int)hash_table[index].size();j++){
                    neighbors.push_back(hash_table[index][j]);
                    cur_M++;    
                }
                if ((int)hash_table[index].size() > 0){      //found empty bucket
                    cur_probs++;
                }
                
            }
            if(index < table_sz && cur_M < M){
                index++;    //Go to next bucket
                if(index == q_index){   //If its the bucket of the query
                    if(index < table_sz){   //if we have not reached the end of hash table
                        index++; //Go to next bucket
                    }
                    else{       //if we have reached the end of hash table
                        ham_dist++; //Increase hamming distance
                        if(q_index == 0){   //check hash table from the start
                            index = 1;
                        }
                        else{
                            index = 0;
                        }
                    }
                }
            }
            else{
                if(cur_M < M){
                    ham_dist++; //Increase hamming distance
                    if(q_index == 0){ 
                        index = 1;
                    }
                    else{
                        index = 0;
                    }
                }
            }
            
        }
    }

    return neighbors;

}



void find_neighbors_cube(const pair<string,int> query, vector<pair<int,double>> &n_neighbors,int N,int M,int probs, const vector<vector<double>> &ar, const vector<vector<double>> &ar2,double (*distance)(const vector<double> &point, const vector<double> &query)){
    
    vector<int> neighbors = neighbors_func(query,M,probs,ar,ar2,distance);
    
    //Calculate euclidean distance for every neighbor
    for(int i=0;i<(int)neighbors.size();i++){ 
        double dist = distance(ar[neighbors[i]], ar2[query.second]);
        n_neighbors.push_back(make_pair(neighbors[i],dist));
    }
        
    //Sort neighbors according to distance
    sort(n_neighbors.begin(), n_neighbors.end(),sort_by_dist);

    //Keep only N neighbors
    if((int)n_neighbors.size() > N){
        n_neighbors.resize(N);
    }
}



void range_search_cube(const pair<string,int> query, vector<pair<int,double>> &range_neighbors,int M,int R,int probs,const vector<vector<double>> &ar, const vector<vector<double>> &ar2,double (*distance)(const vector<double> &point, const vector<double> &query)){

    vector<int> neighbors = neighbors_func(query,M,probs,ar,ar2,distance);

    //Calculate euclidean distance for every neighbor
    for(int i=0;i<(int)neighbors.size();i++){ 
        double dist = distance(ar[neighbors[i]], ar2[query.second]);
        if(dist < R){
            range_neighbors.push_back(make_pair(neighbors[i],dist));
        }
    }

    //Sort neighbors according to distance
    sort(range_neighbors.begin(), range_neighbors.end(),sort_by_dist);

}

void empty_cube(){
    vector<double>().swap(r);
    vector<vector<vector<double>>>().swap(v);
    vector<vector<double>>().swap(t);
    vector<vector<int>>().swap(hash_table);
    vector<map<int, int>>().swap(calc_h);
}
