#include "clusters.h"
#include <set>
#include <algorithm>
#include <cmath>

void k_means_pp(vector<vector<double>> ar, vector<vector<double>>&centroids,int k){
    
    int datasize = ar.size(); //Size of dataset

    set<int> centroids_index; //to save centroids index

    int first_centoid = rand() % (datasize);  //First centroid

    centroids_index.insert(first_centoid); //Save first centroid index

    centroids.push_back(ar[first_centoid]); 

    int t = 1;

    while(t != k){ //While not all centroids have been calculated 

        vector<double> distances(datasize); //to save distances from centroids --> D(i)
        vector<double> prob(datasize); //Vector to save probabilities --> P(r)
        int max_index = 0; //Index of max distance

        vector<int> non_centr_index;      //indexes of non-centroid points

        distances.resize(datasize-t); //Resize distances vector

        for(int i=0;i<datasize;i++){ //For every point
            if(centroids_index.find(i) == centroids_index.end()){   //if i not in centroids_index
                double dist = euclidean_distance(ar[i], centroids[0]); //Calculate distance from first centroid
                for(int j=0;j<t;j++){ //For every centroid
                    double dist2 = euclidean_distance(ar[i], centroids[j]); //Calculate distance from centroid
                    if(dist2 < dist){ //If distance is smaller than the previous
                        dist = dist2; //Save the new distance
                    }
                }
                distances[i] = dist; //Save min distance from centroids

                non_centr_index.push_back(i);
            }
        }
        
        //Find max distance
        double max = distances[0];
        max_index = 0;
        for(int i=1;i<(int)distances.size();i++){
            if(distances[i] > max){
                max = distances[i];
                max_index = i;
            }
        }


        prob.resize((datasize-t),0.0); //Vector to save probabilities
        // for(int i=0;i<(datasize);i++){
        //     if(centroids_index.find(i) == centroids_index.end()){   //if i not in centroids_index
        //         for(int j=0;j<i;j++){   //Calculate probability
        //             double d = distances[j]/distances[max_index];
        //             prob[i] = pow(d,2);
        //         }
        //     }
        // }
        for(int i=0;i<(int)non_centr_index.size();i++){
            // if(centroids_index.find(i) == centroids_index.end()){   //if i not in centroids_index
                for(int j=0;j<i;j++){   //Calculate probability
                    int k = non_centr_index[j];
                    double d = distances[k]/distances[max_index];
                    prob[i] += pow(d,2);
                }
            // }
        }
       
        //New centroid
        // double sum = 0;
        // for(int i=0;i<(int)prob.size();i++){
        //     sum += prob[i]; //All probabilities 
        // }

        //Random number from 0 to sum with uniform distribution
        double lower_bound = 0;
        // double upper_bound = sum;
        int i = (int)prob.size()-1;
        double upper_bound = prob[i];
    
        uniform_real_distribution<double> unif(lower_bound,upper_bound);
    
        default_random_engine re;
    
        // Getting a random double value
        double random = unif(re);
        
        int index = 0;
        double p = prob[index];
        while(p < random){
            index++; //index of the new centroid
            p = prob[index];
        }

        //Save new centroid
        int r = non_centr_index[index];
        centroids.push_back(ar[r]);
        centroids_index.insert(r);
        t++;

        distances = vector<double>(datasize - t);
        prob = vector<double>(datasize - t);
        non_centr_index  = vector<int>(datasize - t);
    }

    centroids_index.clear();

}

vector<Cluster> lloyds(int k, vector<vector<double>> centroids ,vector<vector<double>> ar){

    near2_cent.resize(ar.size());       //2nd nearest centroid for each point

    bool flag = true; //Flag to see if there are changes in clusters

    //Create clusters
    vector<Cluster> clusters; //vector of clusters

    vector<int> cl(ar.size());  
    for(int i=0;i<(int)ar.size();i++){     //Initialize v with -1
        cl[i] = -1;
    }

    int counter = 0;

    Cluster cluster;
    for(int i=0;i<k;i++){
        cluster.centroid = centroids[i]; 
        clusters.push_back(cluster);
    }
    

    while(flag == true){
        int changes =0; //Number of changes in clusters
        
        //Assign points to clusters (in nearest centroid)
        for(int i=0;i<(int)ar.size();i++){ //For every point

            double min_dist = euclidean_distance(ar[i], clusters[0].centroid);      //find min distance from nearest centroid
            int min_index = 0;
            near2_cent[i].first = 1;
            near2_cent[i].second = euclidean_distance(ar[i], clusters[1].centroid);
            if (min_dist > near2_cent[i].second){
                double temp = near2_cent[i].second;
                near2_cent[i].second = min_dist ;
                near2_cent[i].first = 0;
                min_dist = temp;
                min_index = 1;
            }
            for(int j=2;j<k;j++){ //For every centroid (for every cluster)
                double dist = euclidean_distance(ar[i], clusters[j].centroid);
                if(dist < min_dist){
                    int temp1 = min_index;
                    double temp2 = min_dist;
                    min_dist = dist;
                    min_index = j;
                    near2_cent[i].first = temp1;
                    near2_cent[i].second = temp2;
                }
                else if (dist < near2_cent[i].second ) {    //compute 2nd nearest centroid
                    near2_cent[i].second = dist;
                    near2_cent[i].first = j;
                }
            }
            if(cl[i] != min_index){ //if point changed cluster
                clusters[min_index].points_index.push_back(i);      //add point to new cluster
                if(cl[i] != -1){
                    auto it = find(clusters[cl[i]].points_index.begin(), clusters[cl[i]].points_index.end(), i); //Find point in previous cluster
                    clusters[cl[i]].points_index.erase(it); //Remove point from previous cluster 
                }
                
                macqueen(clusters,ar,i,cl[i],min_index);
                changes++;
            }

            cl[i] = min_index;      //save new cluster index

        }


        //See if there are changes in clusters
        if(changes < 1){    //condition for convergence
            flag = false;
        }

        counter++;

    }

    vector<int>().swap(cl);

    return clusters;
}



vector<Cluster> rev_lsh(int L,int k_h,vector<vector<double>> ar,vector<vector<double>> centroids){    

    bool flag = true;
    int counter = 0;

    vector<Cluster> clusters; //vector of clusters
   
    vector<pair<int,double>> cl(ar.size());     //save <cluster index, R>
    for(int i=0;i<(int)ar.size();i++){      //Initialize cluster index with -1
        cl[i].first = -1;
    }


    //Find min distance between centroids to calculate R
    double min_dist = euclidean_distance(centroids[0], centroids[1]);
    for(int i=0;i<(int)centroids.size();i++){
        for(int j=0;j<(int)centroids.size();j++){
            if(i != j){
                double dist = euclidean_distance(centroids[i], centroids[j]);
                if(dist < min_dist){
                    min_dist = dist;
                }
            }
        }
    }

    Cluster cluster;
    for(int i=0;i<(int)centroids.size();i++){
        cluster.centroid = centroids[i]; 
        clusters.push_back(cluster);
    }

    double R = min_dist/2;

    vector<vector<pair<int,double>>> range_neighbors(centroids.size());

    while(flag == true){

        int changes = 0;
        for(int c=0;c<(int)centroids.size();c++){ //For every centroid
            for (int i=0; i<L; i++){
                double id;
                double g = initialize_g_func(i,centroids[c],k_h,id);        //g of query(centroid) for this hash table
                range_search(range_neighbors[c], R, i, g, ar, centroids[c] , euclidean_distance); 
            }

            for (auto r: range_neighbors[c]){ //For every point in range

                if (cl[r.first].first == -1){       //first time point is assigned to cluster
                    clusters[c].points_index.push_back(r.first); 
                    changes++;
                    cl[r.first].first = c;
                    cl[r.first].second = R;
                }
                else {
                    if (cl[r.first].second == R){   //if point is in 2 cluster ranges with same R
                        
                        //find nearest centroid to point
                        int prev = cl[r.first].first;
                        double dist1 = euclidean_distance(ar[r.first], clusters[prev].centroid);
                        double dist2 = euclidean_distance(ar[r.first], clusters[c].centroid);

                        if(dist1 > dist2){  //if new c is closer to point

                            clusters[c].points_index.push_back(r.first); 

                            auto it = find(clusters[prev].points_index.begin(), clusters[prev].points_index.end(), r.first);
                            clusters[prev].points_index.erase(it);

                            cl[r.first].first = c;
                            
                            changes++;
                        }
                    }
                }
            }

            range_neighbors[c].clear();
        }
        
        if(changes < 1){        //condition for convergence
            flag = false;
        }

        macqueen_all(clusters, centroids, ar);      //update all centroids
        
        R = R * 2;
        counter++;
    }


    //assign points that are not in any cluster
    for (int i=0; i<(int)cl.size(); i++){
        if (cl[i].first == -1){
            double min_dist = euclidean_distance(ar[i], clusters[0].centroid);
            int min_index = 0;
            for(int j=0;j<(int)centroids.size();j++){ //For every centroid (for every cluster)
                double dist = euclidean_distance(ar[i], clusters[j].centroid);
                if(dist < min_dist){
                    min_dist = dist;
                    min_index = j;
                }
            }
            clusters[min_index].points_index.push_back(i);
        }
    }

    vector<pair<int,double>>().swap(cl);
    
    return clusters;

}



vector<Cluster> rev_cube(int k_h,vector<vector<double>> ar,vector<vector<double>> centroids,int M,int probes){    

    bool flag = true;
    int counter = 0;

    vector<Cluster> clusters; //vector of clusters

    vector<pair<int,double>> cl(ar.size());   //save <cluster index, R>
    for(int i=0;i<(int)ar.size();i++){      //Initialize cluster index with -1
        cl[i].first = -1;
    }


    //Find min distance between centroids to calculate R
    double min_dist = euclidean_distance(centroids[0], centroids[1]);
    for(int i=0;i<(int)centroids.size();i++){
        for(int j=0;j<(int)centroids.size();j++){
            if(i != j){
                double dist = euclidean_distance(centroids[i], centroids[j]);
                if(dist < min_dist){
                    min_dist = dist;
                }
            }
        }
    }

    Cluster cluster;
    for(int i=0;i<(int)centroids.size();i++){
        cluster.centroid = centroids[i]; 
        clusters.push_back(cluster);
    }

    double R = min_dist/2;
    vector<vector<pair<int,double>>> range_neighbors(centroids.size());

    vector<vector<double>> h_vec2;
    h_vec2.resize((int)centroids.size(),vector<double>(k_h));
    
    vector<vector<int>> f_vec2; 
    f_vec2.resize((int)centroids.size(),vector<int>(k_h));

    while(flag == true){
        
        //calculate k h functions for centroids
        h_func(centroids,k_h,h_vec2);
        
        //Calculate k f functions for cetroids
        f_func(centroids,k_h,h_vec2,f_vec2);

        //Concatenate f functions to create a string
        vector<pair<string,int>> query_str;
        concat(k_h, centroids , query_str, f_vec2);


        int changes = 0;
        for(int c=0;c<(int)centroids.size();c++){ //For every centroid
            range_search_cube(query_str[c],range_neighbors[c],M,R,probes,ar,centroids,euclidean_distance);
            
            for (auto r: range_neighbors[c]){ //For every point in range

                if (cl[r.first].first == -1){       //first time point is assigned to cluster
                    clusters[c].points_index.push_back(r.first); 
                    changes++;
                    cl[r.first].first = c;
                    cl[r.first].second = R;
                }
                else {
                    if (cl[r.first].second == R){   //if point is in 2 cluster ranges with same R
                        
                        //find nearest centroid to point
                        int prev = cl[r.first].first;
                        double dist1 = euclidean_distance(ar[r.first], clusters[prev].centroid);
                        double dist2 = euclidean_distance(ar[r.first], clusters[c].centroid);

                        if(dist1 > dist2){  //if new c is closer to point

                            clusters[c].points_index.push_back(r.first); 

                            auto it = find(clusters[prev].points_index.begin(), clusters[prev].points_index.end(), r.first);
                            clusters[prev].points_index.erase(it);

                            cl[r.first].first = c;
                            
                            changes++;
                        }
                    }
                }
            }

            range_neighbors[c].clear();

        }

        
        if(changes < 1){        //condition for convergence
            flag = false;
        }

        macqueen_all(clusters, centroids, ar);      //update all centroids
        
        R = R * 2;
        counter++;
    }

    //assign points that are not in any cluster
    for (int i=0; i<(int)cl.size(); i++){
        if (cl[i].first == -1){
            double min_dist = euclidean_distance(ar[i], clusters[0].centroid);
            int min_index = 0;
            for(int j=0;j<(int)centroids.size();j++){ //For every centroid (for every cluster)
                double dist = euclidean_distance(ar[i], clusters[j].centroid);
                if(dist < min_dist){
                    min_dist = dist;
                    min_index = j;
                }
            }
            clusters[min_index].points_index.push_back(i);
        }
    }

    vector<pair<int,double>>().swap(cl);
    vector<vector<double>>().swap(h_vec2);
    vector<vector<int>>().swap(f_vec2);
    
    return clusters;

}



void macqueen(vector<Cluster> &clusters, vector<vector<double>> ar, int p_index, int old_index, int new_index){

    //New centroid update
    if (old_index != -1){
        int len_new = clusters[new_index].points_index.size() - 1;
        int len_old = clusters[old_index].points_index.size() + 1;

        for (int i = 0; i < (int)ar[p_index].size(); i++) { //For every coordinate of point    
            if(len_new > 0){ 
                //new centroid for cluster to which the point was added
                clusters[new_index].centroid[i] = ((clusters[new_index].centroid[i] * len_new) / (len_new + 1)) + (ar[p_index][i] / (len_new + 1));
            }
            if(len_old > 1){
                //new centroid for cluster from which the point was removed
                clusters[old_index].centroid[i] = (ceil(clusters[old_index].centroid[i] * len_old) / (len_old - 1)) - (ar[p_index][i] / (len_old - 1));     //use ceil to avoid negative coordinates
            }
        }

    } 
    else { //first time point is assigned to cluster
        int len_new = clusters[new_index].points_index.size() - 1;
        for (int i = 0; i < (int)ar[p_index].size(); i++) { //For every coordinate of point    
            clusters[new_index].centroid[i] = ((clusters[new_index].centroid[i] * len_new) / (len_new + 1)) + (ar[p_index][i] / (len_new + 1));
        }
    }

}



void macqueen_all(vector<Cluster> &clusters, vector<vector<double>>& new_centroids, vector<vector<double>> ar){
    
    //Calculate new centroids 
    for(int i=0;i<(int)clusters.size();i++){ //For every cluster
        vector<double> sum(ar[0].size(), 0.0); // Initialize sum vector

        if ((int)clusters[i].points_index.size() > 0){      //if no point has been assigned to cluster

            for(int j=0;j<(int)clusters[i].points_index.size();j++){ //For every point in cluster i
                for(int k=0;k<(int)ar[clusters[i].points_index[j]].size();k++){ //For every coordinate of point
                    sum[k] += ar[clusters[i].points_index[j]][k] / clusters[i].points_index.size(); //Sum of coordinates
                }
            }
            
            new_centroids[i] = sum;  //new centroid
            clusters[i].centroid = sum; 
        } 
    }
}


vector<double> silhouette(vector<vector<double>> ar, vector<Cluster> clusters, string method){

    if (method.compare("Classic") != 0 ){       //if method is reverse assignent
        near2_cent.resize(ar.size());           //find second nearest centroid for all points

        for (int p=0; p<(int)ar.size(); p++){
            double min_dist = euclidean_distance(ar[p], clusters[0].centroid);
            int min = 0;
            near2_cent[p].first = 1;
            near2_cent[p].second = euclidean_distance(ar[p], clusters[1].centroid);
            if (min_dist > near2_cent[p].second){
                double temp = near2_cent[p].second;
                near2_cent[p].second = min_dist;
                near2_cent[p].first = 0;
                min_dist = temp;
                min = 1;
            }
            for (int c=2; c<(int)clusters.size(); c++){
                double dist = euclidean_distance(ar[p], clusters[c].centroid);
                if (dist < min_dist){
                    int temp1 = min;
                    double temp2 = min_dist;
                    min_dist = dist;
                    min = c;
                    near2_cent[p].first = temp1;
                    near2_cent[p].second = temp2;
                } else if (dist < near2_cent[p].second) {
                    near2_cent[p].first = c;
                    near2_cent[p].second = dist;
                }
            }

            //check if p is in cluster with min distance
            auto it = find(clusters[min].points_index.begin(), clusters[min].points_index.end(), p);        
            if ( it == clusters[min].points_index.end() ){       //if p is not assigned to real closest cluster
                near2_cent[p].first = min;
                near2_cent[p].second = min_dist;
            }
        }
    }

    vector<double> s(clusters.size()+1, 0.0);        //size k+1
    vector<double> a(ar.size(), 0.0);
    vector<double> b(ar.size(), 0.0);
    int k = (int)clusters.size();

    for (int c=0; c<(int)clusters.size(); c++){

        for (auto p1: clusters[c].points_index){

            //compute a[p1] = average distance of p1 to objects in same cluster
            for (auto p2: clusters[c].points_index){
                if ( p1 != p2 ){
                    double dist = euclidean_distance(ar[p1], ar[p2]);
                    a[p1] += dist;
                }
            }
            a[p1] /= (int)clusters[c].points_index.size();

            //compute b[p1] = average distance of p1 to objects in next best (neighbor) cluster
            int j=near2_cent[p1].first;

            for (auto p2: clusters[j].points_index){
                double dist = euclidean_distance(ar[p1], ar[p2]);
                b[p1] += dist;
            }
            b[p1] /= (int)clusters[j].points_index.size();

            int max_num = max(a[p1], b[p1]);

            s[c] += (b[p1] - a[p1])/max_num;

        }

        s[c] /= clusters[c].points_index.size();        // s[c] = average s(p) of points in cluster c
        s[k] += s[c];

    }

    s[k] /= k;      // s[k] = stotal = average s(p) of points in dataset
    
    vector<double>().swap(a);
    vector<double>().swap(b);
    vector<pair<int,double>>().swap(near2_cent);

    return s;

}