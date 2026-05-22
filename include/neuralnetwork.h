#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include "nn_parameters.h"
#include <math.h>

#include <string.h>

#define LEARNING_RATE 0.1
#define NB_EPOCHS 10
#define BATCH_SIZE 64


// Output of forward pass for only one feature
typedef struct
{
    float Z1[NB_NEURONS];
    float A1[NB_NEURONS];
    float Z2[NB_CLASSES];
    float A2[NB_CLASSES];
} forward_vector_t;

typedef struct
{
    forward_vector_t *vectors;
} forward_matrix_t;

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



extern nn_parameters_t *train(dataset_t *dataset);

extern void feed_forward(forward_matrix_t *output, const nn_parameters_t *parameters, const dataset_t *dataset);

extern float accuracy(const dataset_t *dataset, const forward_matrix_t *output);

extern uint8_t *predict_image(dataset_t *test_dataset, const nn_parameters_t *parameters);

#endif /*NEURALNETWORK_H*/