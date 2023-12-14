#include "lsh.h"
#include <cstdlib> 
#include <cmath>
#include <tuple>  
#include <algorithm>

vector<double> r;
vector<vector<vector<double>>> v;
vector<vector<double>> t;
int TableSize;


//initialize vectors v,t,r
void initialize_h_func(int L, int k, int data_size){
    
    unsigned seed = 123;
    default_random_engine generator(seed); 
    normal_distribution<double> normal_distribution(0.0, 1.0);      //uniform distribution for vector v

    v.resize(L,vector<vector<double>>(k,vector<double>(data_size)));
    for (int i = 0; i<L; i++){
       for (int j = 0; j < k; j++) {
            for (int z=0; z<data_size; z++){
                double random_number = normal_distribution(generator); 
                v[i][j][z] = random_number;
            }
        } 
    }

    t.resize(L,vector<double>(k));
    uniform_real_distribution<float> uniform_distribution(0.0, (float)w);       //normal distribution for t
    for (int i=0; i<L; i++){
        for (int j=0; j<k; j++){
            t[i][j] = uniform_distribution(generator);
        }
    } 

    for (int i=0; i<k; i++){            //random r vector
        r.push_back(rand());
    }

}


//find a point's ID for a specific g 
double find_id_func(int i, const vector<double> &point, int k){
    
    double sum = 0;
    double M = pow(2.0, 32.0)-5;
    for (int j=0; j<k; j++){
        double a = 0;
        for (int z=0; z<(int)point.size(); z++){
            a += point[z]*v[i][j][z];
        }
        a += t[i][j];
        int h = floor(a/w);
        int m = fmod((fmod(r[j]*h,M) + M), M);
        sum += m;          //(linear compination r*h for g) mod M

    }

    sum = fmod((fmod(sum,M) + M), M);  //ID

    return sum;
}


//find a point's g(p)
double initialize_g_func(int i, const vector<double> &point, int k, double &id){
    
    id = find_id_func(i,point,k);
    double sum = fmod((fmod(id,(double)TableSize) + (double)TableSize), (double)TableSize);

    return sum;
}


void initialize_hash_table(int L, int k, const vector<vector<double>> &ar){

    hashTable.resize(L, vector<vector<pair<int,double>>>(TableSize));       //hashTable: L hashtables with TableSize buckets

    for (int p=0; p<(int)ar.size(); p++){ //For every point  

        vector<double> point = ar[p];
        
        for (int j=0; j<L; j++){     //Create L g
            double id;
            double g = initialize_g_func(j,point,k, id);  //g = ID mod TS
            
            
            hashTable[j][g].push_back( make_pair(p, id) );
        }
    }
}


//compute Euclidean distance
double euclidean_distance(const vector<double> &point, const vector<double> &query){

    double dist = 0;

    for (int i=0; i<(int)point.size(); i++){
        double x = point[i] - query[i];
        x = pow(x,2.0);
        dist += x;
    }

    dist = sqrt(dist);             

	return dist;
}



void find_neighbors(vector<pair<int, double>> &neighbors, int ht_index, double q_pos, double id, const vector<vector<double>> &points, const vector<double> &query,double (*distance)(const vector<double> &point, const vector<double> &query)){

    vector<pair<int,double>> bucket = hashTable[ht_index][q_pos];       //bucket of query

    for (auto point: bucket){
        if (point.second == id){        //if point.id == query.id
            double dist = distance(points[point.first], query);
            neighbors.push_back( make_pair(point.first, dist) );  //keep <point,dist> for neighbor
        }
    }

    sort( neighbors.begin(), neighbors.end(), sort_by_dist);
    neighbors.erase( unique( neighbors.begin(), neighbors.end() ), neighbors.end() );

}

bool sort_by_dist(const pair<int,double> &a, const pair<int,double> &b){
    // return (a.second < b.second);
    return tie( a.second, a.first ) < tie( b.second, b.first ) ;
}

void range_search(vector<pair<int,double>> &neighbors, double R, int ht_index, double q_pos, const vector<vector<double>> &points, const vector<double> &query ,double (*distance)(const vector<double> &point, const vector<double> &query)){

    vector<pair<int,double>> bucket = hashTable[ht_index][q_pos];       //bucket of query

    for (auto point: bucket){
        double dist = distance(points[point.first], query);
        if (dist < R){        //if distance < range
            neighbors.push_back( make_pair(point.first, dist) );  //keep <point,dist> for neighbor
        }
    }

    sort( neighbors.begin(), neighbors.end(), sort_by_dist);
    neighbors.erase( unique( neighbors.begin(), neighbors.end() ), neighbors.end() );
}


void empty_lsh(){
    vector<double>().swap(r);
    vector<vector<vector<double>>>().swap(v);
    vector<vector<double>>().swap(t);
    vector<vector<vector<pair<int,double>>>>().swap(hashTable);
}