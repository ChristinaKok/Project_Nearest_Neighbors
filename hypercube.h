#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

using namespace std;

extern int table_sz;

static vector<vector<int>> hash_table;

void h_func(const vector<vector<double>> &ar,int k,vector<vector<double>> &h_vec);

extern std::vector<std::map<int, int>> calc_h;

void f_func(const vector<vector<double>> &ar,int k,vector<vector<double>> &h_vec,vector<vector<int>> &f_vec);

std::string concat_help(const vector<int> b,int k);

void concat(int k,const vector<vector<double>> &ar, vector<pair<string,int>> &str , const vector<vector<int>> &f_vec);

void initialize_ht_cube(int k, const vector<pair<string,int>> &str);

int hammingDistance(int n1, int n2);

vector<int> neighbors_func(const pair<string,int> query,int M,int probs,const vector<vector<double>> &ar, const vector<vector<double>> &ar2,double (*distance)(const vector<double> &point, const vector<double> &query));

void find_neighbors_cube(const pair<string,int> query, vector<pair<int,double>> &n_neighbors,int N,int M,int probs, const vector<vector<double>> &ar, const vector<vector<double>> &ar2,double (*distance)(const vector<double> &point, const vector<double> &query));

void range_search_cube(const pair<string,int> query, vector<pair<int,double>> &n_neighbors,int M,int R,int probs, const vector<vector<double>> &ar, const vector<vector<double>> &ar2,double (*distance)(const vector<double> &point, const vector<double> &query));

void empty_cube();

#endif
