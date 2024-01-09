#include "graph.h"
#include <algorithm>
#include <cstdlib>
#include <set>

vector<list<int>> graph;

void initialize_graph(int data_size, int k, const vector<vector<double>> &ar){
    
    //initialize LSH hash tables
    int L=5, k_h=4;
    int number_of_points = (int) ar.size();
    initialize_h_func(L,k_h,data_size);

    TableSize = number_of_points/8;

    initialize_hash_table(L, k_h, ar);

    graph.resize(number_of_points);

    //find neighbors
    for (int p=0; p<number_of_points; p++){
        
        vector<double> point = ar[p];
        vector<pair<int,double>> neighbors;

        for (int i=0; i<L; i++){    
            double id;                                      //ID of query for this hash table
            double g = initialize_g_func(i,point,k_h,id);    //g of query for this hash table

            find_neighbors(neighbors, i, g, id, ar, point, euclidean_distance);
        }
        
        sort( neighbors.begin(), neighbors.end(), sort_by_dist);
        neighbors.erase( unique( neighbors.begin(), neighbors.end() ), neighbors.end() );
        
        for (auto n: neighbors){
            if (n.second > 0){      //if neighbor is not the same point
                graph[p].push_back( n.first );
            }
            if((int)graph[p].size() == k){  //keep first k neighbors only
                break;
            }
        }

        neighbors.clear();
        
    }

    empty_lsh();
    
}

void convert_graph(const vector<string> &graph_data , const vector<vector<double>> &ar){
    
    graph.resize(ar.size());

    for(int i=0; i<(int)graph_data.size(); i++){
        
        int my_int = stoi(graph_data[i]); // int of the point that we want to find its neighbors

        int j = 0; //index inside the string
        
        while(graph_data[i][j] != '>'){
            j++;
        }
        
        j++; //graph_data[i][j] is the first space after the -->

        while(graph_data[i][j] != '\0'){ //while not end of string
            
            while(graph_data[i][j] == ' '){ //skip spaces
                j++;
            }
    
            string neighbor;
            while(graph_data[i][j] != ' ' && graph_data[i][j] != '\0'){ //read the neighbor
                neighbor += graph_data[i][j];
                j++;
            }

            if(graph_data[i][j] == ' '){ //if the neighbor was found
                int neighbor_int = stoi(neighbor);
                graph[my_int].push_back(neighbor_int);
            }

        }

    }

}

set<pair<int,double>,Cmp> gnns(int E, int R, int N, const vector<vector<double>> &points, const vector<double> &query, double (*distance)(const vector<double> &point, const vector<double> &query)){

    set<pair<int, double>,Cmp> S;       //set of candidates
    set<int> used_points;               //set of points chosen as y

    int number_of_points = (int)points.size();
    unsigned seed = 123;
    srand(seed);

    for (int i=0; i<R; i++){    //random restarts

        int y = rand() % number_of_points ;     //random start point

        while(1){

            used_points.insert(y);

            vector<pair<int, double>> y_neighbors;      //neighbors N(y,E,G)
            int exp=0;                                  //counter for expansions

            for (auto p: graph[y]){         //get neighbors of y from graph
                
                auto it = used_points.find(p);          //if point has been already used as y
                if (it != used_points.end()) continue;  // ignore it

                double dist = distance(points[p], query);
                y_neighbors.push_back(make_pair(p,dist));
                S.insert(make_pair(p,dist));

                exp++;
                if (exp == E){      //if E neighbors have been opened
                    break;
                }
            }

            if (exp == 0) break;    //y does not have any neighbors (or its neighbors are already in S)

            sort(y_neighbors.begin(), y_neighbors.end(), sort_by_dist);     //sort neighbors by distance to query

            double dist_y = distance(points[y], query);
            double min_dist = y_neighbors[0].second;
            if (dist_y < min_dist){         //if local optimal is reached (y is better than its neighbors)
                break;
            
            } else {            //else select best neighbor as new y
                y = y_neighbors[0].first;
            }
            
            vector<pair<int, double>>().swap(y_neighbors);
        }
    }

    return S;
}



void mrng_initialize_graph(const vector<vector<double>> &ar, double (*distance)(const vector<double> &point, const vector<double> &query)){
    
    int number_of_points = (int) ar.size();
    graph.resize(number_of_points);

    for (int p=0; p<number_of_points; p++){         //for each point p in S

        vector<double> point = ar[p];
        set<pair<int, double>,Cmp> R;               //set of candidates
        set<pair<int, double>,Cmp> L;               //set of neighbors

        //create R = S\{p}
        for (int i=0; i<number_of_points; i++){
            if (i != p){
                double dist = distance(point, ar[i]);
                R.insert(make_pair(i, dist));
            }    
        }

        double min = R.begin()->second;          //min distance from p
        double dist = min;
        while(dist == min){                     //insert all points with min distance to L
            pair<int,double> r = *R.begin();
            L.insert(r); 
            R.erase(R.begin());
            dist = R.begin()->second;
        }

        for (auto r: R){                            //for each point r in R-L
            bool condition = true;
            
            for (auto t: L){                        //for each point t in L
                double pr = r.second;
                double pt = t.second;
                double rt = distance(ar[r.first], ar[t.first]);

                if ((pr > pt) && (pr > rt)){        //if pr is the longest edge in triangle (prt)
                    condition = false;              //  do not insert point r in L
                    break;
                }
            }

            if (condition == true){
                L.insert(r);
            }
        }

        //update graph
        for (auto n: L){
            graph[p].push_back( n.first );
        }

        //empty sets
        R.clear();
        L.clear();

    }

}



int find_navigating_node(int data_size, const vector<vector<double>> &points, double (*distance)(const vector<double> &point, const vector<double> &query)){

    vector<double> sum(data_size, 0.0);
    int number_of_points = (int)points.size();

    //calculate the centroid of dataset
    for (int i=0; i<number_of_points; i++){
        for (int j=0; j<data_size; j++){
            sum[j] += points[i][j];
        } 
    }
    for (int i=0; i<data_size; i++){
        sum[i] /= number_of_points;
    }

    //find nearest neighbor with brute force
    int min=0;
    double min_dist = distance(sum, points[0]);
    for (int p=1; p<number_of_points; p++){
        double dist = distance(sum, points[p]);
        if (dist < min_dist){
            min = p;
            min_dist = dist;
        }
    }

    return min;
}


set<pair<int,double>,Cmp> search(int l, int N, int start_p, const vector<vector<double>> &points, const vector<double> &query, double (*distance)(const vector<double> &point, const vector<double> &query)){

    set<pair<int,double>,Cmp> R;        //set of candidates
    set<int> used_points;               //marked points

    double dist = distance(points[start_p], query);
    R.insert(make_pair(start_p, dist));
    used_points.insert(start_p);

    int i=1, p=start_p;
    while(i < l){
        
        for (auto r: R){

            auto it = used_points.find(r.first);          //if point has been already used 
            if (it != used_points.end()) continue;        // ignore it
            
            p = r.first;                //unchecked p with min distance from query
            used_points.insert(p);      // mark p as used

            break;
        }
        
        for(auto n: graph[p]){

            pair<int,double> neighbor(n, distance(points[n], query));
            auto it = R.find(neighbor);     //if point is already in R
            if (it != R.end()) continue;    // ignore it

            R.insert(neighbor);     //insert n in R
            i++;    

        }
        
    }
    
    //return N points 
    set<pair<int, double>,Cmp> neighbors;
    for (auto r: R){
        neighbors.insert(r);
        if ((int)neighbors.size() == N) break;
    }
    
    return neighbors;

}


void convert_init(const set<pair<int,double>,Cmp> &neighbors, const vector<vector<double>> &points, const vector<double> &query, vector<pair<int,double>> &conv_neigh){

    for (auto nb: neighbors){
        int p = nb.first;
        double dist = euclidean_distance(points[p], query);
        conv_neigh.push_back(make_pair(p, dist));
    }
}

