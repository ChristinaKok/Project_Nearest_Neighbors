#include <iostream>
#include <vector>
#include "lsh.h"
#include "hypercube.h"

using namespace std;

class Cluster{
    public:
        vector<double> centroid; //centroid of cluster
        vector<int> points_index; //index of points in cluster
};

static vector<pair<int,double>> near2_cent;

void k_means_pp(vector<vector<double>> ar, vector<vector<double>>&centroids,int k);

vector<Cluster> lloyds(int k, vector<vector<double>> centroids ,vector<vector<double>> ar);

void macqueen(vector<Cluster> &clusters,vector<vector<double>> ar,int p_index,int old_index,int new_index);

void macqueen_all(vector<Cluster> &clusters, vector<vector<double>>& new_centroids, vector<vector<double>> ar);

vector<Cluster> rev_lsh(int L,int k_h,vector<vector<double>> ar,vector<vector<double>> centroids);

vector<Cluster> rev_cube(int k_h,vector<vector<double>> ar,vector<vector<double>> centroids,int M,int probes);

vector<double> silhouette(vector<vector<double>> ar, vector<Cluster> clusters, string method);
