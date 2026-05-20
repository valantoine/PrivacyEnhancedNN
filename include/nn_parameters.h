#ifndef NN_PARAMETERS_H
#define NN_PARAMETERS_H
#include "preprocess_data.h"

#define NB_NEURONS 64
#define NB_CLASSES 10 // Final output



// Weights and bias parameters (updated after each training iterations)
typedef struct
{
    float W1[NB_NEURONS][IMAGE_SIZE];
    float b1[NB_NEURONS];
    float W2[NB_CLASSES][NB_NEURONS];
    float b2[NB_CLASSES];
} nn_parameters_t;

extern nn_parameters_t *init_parameters();

extern void nn_parameters_print(const nn_parameters_t *parameters, FILE *fd);

extern uint8_t nn_parameters_write(const nn_parameters_t *parameters, FILE *fd);

extern nn_parameters_t *nn_parameters_get(FILE *fd);


#endif /*NN_PARAMETERS_H*/