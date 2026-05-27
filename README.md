
# Privacy Enhanced Neural Network

Training a neural network using MNIST dataset and making predictions on ciphered images.


## Installation

Once in the PrivacyEnhancedNN directory, type

```bash
make
```

Some errors may appear depending on your system. The GNU MP library must be installed on your computer (already installed on linux)
## Usage/Examples

Print help :
```bash
./penn -h
```

Training the neural network and storing the trained model in a file :

```bash
./penn -t PARAMETERS_PATH_FILE
```

Prediction of one plain image using your trained model:

```bash
./penn -p -i PARAMETERS_PATH_FILE
```

Prediction of one ciphered image using your trained model (you will see all the ciphered image/the final ciphered vector and the decrypted vector in your terminal):
```bash
./penn -p -e -i PARAMETERS_PATH_FILE
```

You can also predict more images at once by adding -n NUMBER_OF_IMAGE for example : 
```bash
./penn -p -e -i PARAMETERS_PATH_FILE -n 10
```
