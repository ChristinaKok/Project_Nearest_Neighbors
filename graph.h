#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <list>
#include "lsh.h"
#include "hypercube.h"

using namespace std;

extern vector<list<int>> graph;

void initialize_graph(int data_size, int k, const vector<vector<double>> &ar);

void convert_graph(const vector<string> &graph_data , const vector<vector<double>> &ar);

set<pair<int,double>,Cmp> gnns(int E, int R, int N, const vector<vector<double>> &points, const vector<double> &query, double (*distance)(const vector<double> &point, const vector<double> &query));

void mrng_initialize_graph(const vector<vector<double>> &ar, double (*distance)(const vector<double> &point, const vector<double> &query));

int find_navigating_node(int data_size, const vector<vector<double>> &points, double (*distance)(const vector<double> &point, const vector<double> &query));

set<pair<int,double>,Cmp> search(int l, int N, int start_p, const vector<vector<double>> &points, const vector<double> &query, double (*distance)(const vector<double> &point, const vector<double> &query));

#endif
