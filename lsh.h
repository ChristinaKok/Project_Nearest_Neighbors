#ifndef LSH_H
#define LSH_H

#include <vector>
#include <set>
#include <unordered_set>
#include <random>
#include <iostream>

using namespace std;

struct Cmp{
    bool operator()(const pair<int, double> &a, const pair<int, double> &b){
        return a.second < b.second;
    }
};

extern vector<double> r;

extern vector<vector<vector<double>>> v;

extern vector<vector<double>> t;

const int w = 1000;

extern int TableSize;

static vector<vector<vector<pair<int,double>>>> hashTable;

void initialize_hash_table(int L, int k, const vector<vector<double>> &points);

bool sort_by_dist(const pair<int,double> &a, const pair<int,double> &b);

double euclidean_distance(const vector<double> &point, const vector<double> &query);

void find_neighbors(vector<pair<int, double>> &neighbors, int ht_index, double q_pos, double id, const vector<vector<double>> &points, const vector<double> &query,double (*distance)(const vector<double> &point, const vector<double> &query));

void range_search(vector<pair<int,double>> &neighbors, double R, int ht_index, double q_pos, const vector<vector<double>> &points, const vector<double> &query, double (*distance)(const vector<double> &point, const vector<double> &query));

void initialize_h_func(int L, int k, int data_size);

double initialize_g_func(int i, const vector<double> &point, int k, double &id);

double find_id_func(int i, const vector<double> &point, int k);

void empty_lsh();

#endif
