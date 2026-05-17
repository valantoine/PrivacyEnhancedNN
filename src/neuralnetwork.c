#include "neuralnetwork.h"

parameters_t *init_parameters()
{
    parameters_t *parameters = (malloc(sizeof(parameters_t)));
    if (parameters == NULL)
    {
        fprintf(stderr, "Could not allocate");
        return NULL;
    }
    for (int i = 0; i < NB_NEURONS; i++)
    {
        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            parameters->W1[i][j] = (float)rand() / RAND_MAX - 0.5f;
        }
    }
    for (int i = 0; i < NB_NEURONS; i++)
    {
        parameters->b1[i] = (float)rand() / RAND_MAX - 0.5f;
    }
    for (int i = 0; i < NB_NEURONS; i++)
    {
        for (int j = 0; j < NB_NEURONS; j++)
        {
            parameters->W2[i][j] = (float)rand() / RAND_MAX - 0.5f;
        }
    }
    for (int i = 0; i < NB_NEURONS; i++)
    {
        parameters->b2[i] = (float)rand() / RAND_MAX - 0.5f;
    }
    return parameters;
}