#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include "preprocess_data.h"
#include <time.h>
#define NB_NEURONS 10

typedef struct{
    float W1[NB_NEURONS][IMAGE_SIZE];
    float b1[NB_NEURONS];
    float W2[NB_NEURONS][NB_NEURONS];
    float b2[NB_NEURONS];
} parameters_t;


#endif /*NEURALNETWORK_H*/