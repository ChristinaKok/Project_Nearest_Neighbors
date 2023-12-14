#include "functions.h"

int ReverseInt (int i){
    unsigned char ch1, ch2, ch3, ch4;
    ch1=i&255;
    ch2=(i>>8)&255;
    ch3=(i>>16)&255;
    ch4=(i>>24)&255;
    return((int)ch1<<24)+((int)ch2<<16)+((int)ch3<<8)+ch4;
}

int ReadMNIST(vector<vector<double>> &arr,string filename, int &total_number){

    ifstream file(filename, ios::binary);
    if (file.is_open()){
        int magic_number=0;
        int number_of_images=0;
        int n_rows=0;
        int n_cols=0;

        file.read((char*)&magic_number,sizeof(magic_number));       //magic number
        magic_number= ReverseInt(magic_number);

        file.read((char*)&number_of_images,sizeof(number_of_images));   //number_of_images
        number_of_images= ReverseInt(number_of_images);

        file.read((char*)&n_rows,sizeof(n_rows));       //number of rows
        n_rows= ReverseInt(n_rows);

        file.read((char*)&n_cols,sizeof(n_cols));       //number of columns
        n_cols= ReverseInt(n_cols);

        int DataOfAnImage = n_rows*n_cols;
        arr.resize(number_of_images,vector<double>(DataOfAnImage));
        
        for(int i=0;i<number_of_images;++i){
            for(int r=0; r<n_rows; ++r){
                for(int c=0; c<n_cols; ++c){
                    unsigned char temp=0;
                    file.read((char*)&temp,sizeof(temp));
                    arr[i][(n_rows*r)+c]= (double)temp;
                }
            }
        }

        total_number = number_of_images;

        return DataOfAnImage;
    }

    return 0;
}


void readCluster(int new_ar[6]){

    std::string arr[6];
    int given[6] = {-1,3,4,10,3,2};

    int i=0;
    std::ifstream cFile ("cluster.conf");
    if (cFile.is_open()) {
        std::string line;
        while(getline(cFile, line)){
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            if(line[0] == '/' || line.empty())
                continue;
            auto delimiterPos = line.find(":");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);
            arr[i] = value;
            i++;
        }
        
    }
    else {
        std::cerr << "Couldn't open configuration file for reading.\n";
    }

    for(int i=0;i<6;i++){
        if(arr[i].empty()){
            if(i == 0){
                //Print error
                std::cout << "Error in cluster.conf - k must be given" << endl;
                exit(1);
            }
            new_ar[i] = given[i];
        }
        else{
            new_ar[i] = std::stoi(arr[i]);
        }
    }
}

vector<string> Read_txt(int size, string filename){

    vector<string> graph_data;
    graph_data.reserve(size);
    fstream newfile;
    newfile.open(filename,ios::in); //open a file to perform read operation using file object
    if (newfile.is_open()){ //checking whether the file is open
        string tp;
        while(getline(newfile, tp)){ //read data from file object and put it into string.
            graph_data.push_back(tp);
        }
        newfile.close(); //close the file object.
    }

    return graph_data;
}
