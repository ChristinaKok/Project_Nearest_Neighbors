import keras
from keras.datasets import mnist
import numpy as np
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import (BatchNormalization, SeparableConv2D, MaxPooling2D, Activation, Flatten, Dropout, Dense)
from tensorflow.keras import backend as K

from keras.layers import Input,Reshape,Conv2D,UpSampling2D,Conv2DTranspose
from keras.models import Model,Sequential
from keras.optimizers import Adadelta, RMSprop,SGD,Adam

#load dataset MNIST
(trainX, trainY), (testX, testY) = mnist.load_data()

#preprocessing data
train_data = trainX.reshape(-1, 28,28, 1)      #(60000,28,28,1)
test_data = testX.reshape(-1, 28,28, 1)        #(10000,28,28,1)
train_data = train_data.astype('float32') / np.max(train_data)
test_data = test_data.astype('float32') / np.max(test_data)

#create validation set
train_X,valid_X,train_ground,valid_ground = train_test_split(train_data, train_data, test_size=0.2, random_state=13)