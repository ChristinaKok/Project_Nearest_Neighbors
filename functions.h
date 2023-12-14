#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <list>

using namespace std;

int ReverseInt (int i);
int ReadMNIST(vector<vector<double>> &arr,string filename, int &total_number);
void readCluster(int new_ar[6]);
vector<string> Read_txt(int size, string filename);