#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include "preprocess_data.h"
#include <math.h>

#include <string.h>
#define NB_NEURONS 32
#define NB_CLASSES 10 // Final output
#define LEARNING_RATE 0.2
#define NB_EPOCHS 10
#define BATCH_SIZE 64

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

extern void train(dataset_t *dataset, parameters_t *parameters);

extern parameters_t *init_parameters();

extern void feed_forward(in_matrix_t *output, const parameters_t *parameters, const dataset_t *dataset);

extern float accuracy(const dataset_t *dataset, const in_matrix_t *output);


#endif /*NEURALNETWORK_H*/