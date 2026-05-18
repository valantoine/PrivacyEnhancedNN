#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include "preprocess_data.h"
#include <math.h>
#include <time.h>
#define NB_NEURONS 64
#define NB_CLASSES 10 // Final output
#define LEARNING_RATE 0.2

// Used for init parameters for backpropagation
typedef struct
{
    float W1[NB_NEURONS][IMAGE_SIZE];
    float b1[NB_NEURONS];
    float W2[NB_CLASSES][NB_NEURONS];
    float b2[NB_CLASSES];
} parameters_t;

// Results of forward pass for only one feature
typedef struct
{
    float Z1[NB_NEURONS];
    float A1[NB_NEURONS];
    float Z2[NB_CLASSES];
    float A2[NB_CLASSES];
} in_vector_t;

typedef struct
{
    in_vector_t *vectors;
} in_matrix_t;

typedef struct
{
    float dW1[NB_NEURONS][IMAGE_SIZE];
    float db1[NB_NEURONS];
    float dW2[NB_CLASSES][NB_NEURONS];
    float db2[NB_CLASSES];
} gradients_t;

typedef struct
{
    float dZ1[NB_NEURONS];
    float dZ2[NB_CLASSES];
} back_vector_t;

typedef struct
{
    back_vector_t *vectors;
} back_matrix_t;

extern void train(const dataset_t *dataset);

#endif /*NEURALNETWORK_H*/