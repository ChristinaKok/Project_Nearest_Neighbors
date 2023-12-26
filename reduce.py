import sys
import numpy as np
import struct
from sklearn.model_selection import train_test_split
import keras
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import BatchNormalization, SeparableConv2D, MaxPooling2D, Activation, Flatten, Dropout, Dense
from tensorflow.keras import backend as K
from keras.layers import Input,Reshape,Conv2D,UpSampling2D,Conv2DTranspose
from keras.models import Model,Sequential
from keras.optimizers import Adadelta, RMSprop,SGD,Adam

tf.keras.utils.set_random_seed(1)

def readFile(filename):
    with open(filename,'rb') as f:
        magic, size = struct.unpack(">II", f.read(8))
        nrows, ncols = struct.unpack(">II", f.read(8))
        data = np.fromfile(f, dtype=np.dtype(np.uint8).newbyteorder('>'))
        data = data.reshape((size, nrows, ncols))
        return data

def writeFile(filename, data):
    with open(filename, "w") as f:
        n_images = data.shape[0]    #number of images
        dim = data.shape[1]         #dimension
        f.write(str(n_images))
        f.write('\n')
        f.write(str(dim))
        f.write('\n')

        p = 0
        for point in data:
            f.write(str(p))
            f.write(":  ")
            for i in point:
                f.write(str(i))
                f.write("  ")
            f.write('\n')
            p = p+1



#create autoencoder
def encoder(input_img,latent_dim):     #input = 28 x 28 x 1 (wide and thin)
    conv1 = Conv2D(32, (3, 3), activation='relu', padding='same')(input_img) #28 x 28 x 32
    conv1 = BatchNormalization()(conv1)
    conv1 = Conv2D(32, (3, 3), activation='relu', padding='same')(conv1)
    conv1 = BatchNormalization()(conv1)
    pool1 = MaxPooling2D(pool_size=(2, 2))(conv1) #14 x 14 x 32
    conv2 = Conv2D(64, (3, 3), activation='relu', padding='same')(pool1) #14 x 14 x 64
    conv2 = BatchNormalization()(conv2)
    conv2 = Conv2D(64, (3, 3), activation='relu', padding='same')(conv2)
    conv2 = BatchNormalization()(conv2)
    pool2 = MaxPooling2D(pool_size=(2, 2))(conv2) #7 x 7 x 64
    conv3 = Conv2D(128, (3, 3), activation='relu', padding='same')(pool2) #7 x 7 x 128 (small and thick)
    conv3 = BatchNormalization()(conv3)
    conv3 = Conv2D(128, (3, 3), activation='relu', padding='same')(conv3)
    conv3 = BatchNormalization()(conv3)
    conv4 = Conv2D(256, (3, 3), activation='relu', padding='same')(conv3) #7 x 7 x 256 (small and thick)
    conv4 = BatchNormalization()(conv4)
    conv4 = Conv2D(256, (3, 3), activation='relu', padding='same')(conv4)
    conv4 = BatchNormalization()(conv4)
    flat = Flatten()(conv4)
    latent_output = Dense(latent_dim, activation='relu')(flat)  # Latent dimension
    return latent_output

def decoder(latent_output): 
    x = Dense(6272, activation='relu')(latent_output)  # Dense layer to match the flattened shape before decoding
    x = Reshape((7, 7, 128))(x)
    conv5 = Conv2D(128, (3, 3), activation='relu', padding='same')(x) #7 x 7 x 128
    conv5 = BatchNormalization()(conv5)
    conv5 = Conv2D(128, (3, 3), activation='relu', padding='same')(conv5)
    conv5 = BatchNormalization()(conv5)
    conv6 = Conv2D(64, (3, 3), activation='relu', padding='same')(conv5) #7 x 7 x 64
    conv6 = BatchNormalization()(conv6)
    conv6 = Conv2D(64, (3, 3), activation='relu', padding='same')(conv6)
    conv6 = BatchNormalization()(conv6)
    up1 = UpSampling2D((2,2))(conv6) #14 x 14 x 64
    conv7 = Conv2D(32, (3, 3), activation='relu', padding='same')(up1) # 14 x 14 x 32
    conv7 = BatchNormalization()(conv7)
    conv7 = Conv2D(32, (3, 3), activation='relu', padding='same')(conv7)
    conv7 = BatchNormalization()(conv7)
    up2 = UpSampling2D((2,2))(conv7) # 28 x 28 x 32
    decoded = Conv2D(1, (3, 3), activation='sigmoid', padding='same')(up2) # 28 x 28 x 1
    return decoded


def plot_history(nn):
    acc = nn.history["accuracy"]
    loss = nn.history["loss"]
    val_loss = nn.history["val_loss"]
    val_accuracy = nn.history["val_accuracy"]
    
    x = range(1, len(acc) + 1)
    
    plt.figure(figsize=(12,5))
    plt.subplot(1, 2, 1)
    plt.plot(x, acc, "b", label="traning_acc")
    plt.plot(x, val_accuracy, "r", label="traning_acc")
    plt.title("Accuracy")
    
    plt.subplot(1, 2, 2)
    plt.plot(x, loss, "b", label="traning_acc")
    plt.plot(x, val_loss, "r", label="traning_acc")
    plt.title("Loss")
    plt.legend(["training", "validation"]) 

    plt.suptitle("epochs=5, batch_size=128, latent_dim=10")
    plt.savefig('acc_loss_5_128_10.png')


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


#define hyper-parameters
batch_size = 128
epochs = 5
inChannel = 1
x, y = 28, 28
input_img = Input(shape = (x, y, inChannel))
latent_dim = 10

autoencoder = Model(input_img, decoder(encoder(input_img, latent_dim)))
autoencoder.compile(loss='mean_squared_error', optimizer = RMSprop(), metrics='accuracy')
autoencoder.summary()


#train model
autoencoder_train = autoencoder.fit(train_X, train_real, batch_size=batch_size,epochs=epochs,verbose=1,validation_data=(valid_X, valid_real))

#save model
autoencoder.save_weights('autoencoder.h5')

#trained encoder
encode = Model(input_img, encoder(input_img,latent_dim))

for l1,l2 in zip(encode.layers[:19], autoencoder.layers[0:19]):
    l1.set_weights(l2.get_weights())

for layer in encode.layers[0:19]:
    layer.trainable = False

# encode.compile(loss=keras.losses.categorical_crossentropy, optimizer=keras.optimizers.Adam(),metrics=['accuracy'])

#save images in latent dimension
reduced_train = encode.predict(train_data)
reduced_test = encode.predict(test_data)

writeFile(outputDataFile, reduced_train)
writeFile(outputQueryFile, reduced_test)


#plot loss-accuracy
plot_history(autoencoder_train)
