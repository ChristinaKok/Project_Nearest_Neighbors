import sys
import numpy as np
import struct
from sklearn.model_selection import train_test_split

def readFile(filename):
    with open(filename,'rb') as f:
        magic, size = struct.unpack(">II", f.read(8))
        nrows, ncols = struct.unpack(">II", f.read(8))
        data = np.fromfile(f, dtype=np.dtype(np.uint8).newbyteorder('>'))
        data = data.reshape((size, nrows, ncols))
        return data


###############################################################################################################
#read command line arguments
n = len(sys.argv)
for i in range(1, n):
    if sys.argv[i] == "-d" :                    #dataset file
        datasetFile = sys.argv[i+1]

    elif sys.argv[i] == "-q":                   #queryset file
        queryFile = sys.argv[i+1]

    elif sys.argv[i] == "-od":                  #output dataset file
        outputDataFile = sys.argv[i+1]

    elif sys.argv[i] == "-oq":                  #output queryset file
        outputQueryFile = sys.argv[i+1]


dataset = readFile(datasetFile)
queryset = readFile(queryFile)

#preprocess data
train_data = dataset.reshape(-1, 28,28, 1)      #(60000,28,28,1)
test_data = queryset.reshape(-1, 28,28, 1)      #(10000,28,28,1)
train_data = train_data.astype('float32') / np.max(train_data)
test_data = test_data.astype('float32') / np.max(test_data)

#create validation set
train_X,valid_X,train_real,valid_real = train_test_split(train_data, train_data, test_size=0.2, random_state=13)