#include "neuralnetwork.h"

// Peut être rajouter un sqrt
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
    for (int i = 0; i < NB_CLASSES; i++)
    {
        for (int j = 0; j < NB_NEURONS; j++)
        {
            parameters->W2[i][j] = (float)rand() / RAND_MAX - 0.5f;
        }
    }
    for (int i = 0; i < NB_CLASSES; i++)
    {
        parameters->b2[i] = (float)rand() / RAND_MAX - 0.5f;
    }
    return parameters;
}

void relu(in_vector_t *vector)
{
    for (int i = 0; i < NB_NEURONS; i++)
    {
        if (vector->Z1[i] > 0)
        {
            vector->A1[i] = vector->Z1[i];
        }
        else
        {
            vector->A1[i] = 0;
        }
    }
}

void softmax(in_vector_t *vector)
{
    float sum = 0;
    float max = vector->Z2[0];
    // Max finder
    for (int i = 1; i < NB_CLASSES; i++)
    {
        if (vector->Z2[i] > max)
        {
            max = vector->Z2[i];
        }
    }
    for (int i = 0; i < NB_CLASSES; i++)
    {
        vector->A2[i] = exp(vector->Z2[i] - max);
        sum += vector->A2[i];
    }
    for (int i = 0; i < NB_CLASSES; i++)
    {
        vector->A2[i] /= sum;
    }
}

void feed_forward(in_matrix_t *output, const parameters_t *parameters, const dataset_t *dataset)
{
    float temp;
    for (size_t i = 0; i < dataset->size; i++)
    {

        for (int j = 0; j < NB_NEURONS; j++)
        {
            temp = 0;
            // Computing Z1 = W1 * X + b1

            for (int k = 0; k < IMAGE_SIZE; k++)
            {
                temp += parameters->W1[j][k] * dataset->images[i].image[k];
            }
            output->vectors[i].Z1[j] = temp + parameters->b1[j];
        }
        // Computing A1 = relu(Z1)
        relu(&output->vectors[i]);

        // Computing Z2 = W2 * A1 + b2

        for (int j = 0; j < NB_CLASSES; j++)
        {
            temp = 0;
            for (int k = 0; k < NB_NEURONS; k++)
            {
                temp += parameters->W2[j][k] * output->vectors[i].A1[k];
            }
            output->vectors[i].Z2[j] = temp + parameters->b2[j];
        }

        // Computing A2 = softmax(Z2)

        softmax(&output->vectors[i]);
    }
}

float relu_deriv(const float f)
{
    if (f >= 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

gradients_t *back_propagation(const in_matrix_t *output, const dataset_t *dataset, const parameters_t *parameters, back_matrix_t *back_parameters)
{
    gradients_t *gradients = malloc(sizeof(gradients_t));
    if (gradients == NULL)
    {
        fprintf(stderr, "Could not allocate");
        return NULL;
    }
    float temp_mult;
    float temp_sum;
    // Computing dZ2 (can be done more efficiently)
    for (size_t i = 0; i < dataset->size; i++)
    {
        for (int j = 0; j < NB_CLASSES; j++)
        {

            if (j == dataset->images[i].label)
            {
                back_parameters->vectors[i].dZ2[j] = output->vectors[i].A2[j] - 1;
            }
            else
            {
                back_parameters->vectors[i].dZ2[j] = output->vectors[i].A2[j];
            }
        }
    }

    // Computing dW2 = dZ2 * A1.transpose / size and db2 = sum of dZ2 rows / size
    for (int i = 0; i < NB_CLASSES; i++)
    {
        temp_sum = 0;
        for (int j = 0; j < NB_NEURONS; j++)
        {
            temp_mult = 0;
            for (size_t k = 0; k < dataset->size; k++)
            {
                temp_mult += back_parameters->vectors[k].dZ2[i] * output->vectors[k].A1[j];
                if (j == 0)
                {
                    temp_sum += back_parameters->vectors[k].dZ2[i];
                }
            }
            temp_mult /= dataset->size;
            gradients->dW2[i][j] = temp_mult;
        }
        temp_sum /= dataset->size;
        gradients->db2[i] = temp_sum;
    }
    // Computing dZ1 = W2.transpose * dZ2 and every elements multiplied by ReLU'(Z1)
    for (int i = 0; i < NB_NEURONS; i++)
    {
        for (size_t j = 0; j < dataset->size; j++)
        {
            temp_mult = 0;
            for (int k = 0; k < NB_CLASSES; k++)
            {
                temp_mult += parameters->W2[k][i] * back_parameters->vectors[j].dZ2[k];
            }
            temp_mult *= relu_deriv(output->vectors[j].Z1[i]);
            back_parameters->vectors[j].dZ1[i] = temp_mult;
        }
    }

    // Computing dW1 = dZ1 * dataset / size and d1 = sum of dZ1 rows / size
    for (int i = 0; i < NB_NEURONS; i++)
    {
        temp_sum = 0;
        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            temp_mult = 0;
            for (size_t k = 0; k < dataset->size; k++)
            {
                temp_mult += back_parameters->vectors[k].dZ1[i] * dataset->images[k].image[j];
                if (j == 0)
                {
                    temp_sum += back_parameters->vectors[k].dZ1[i];
                }
            }
            temp_mult /= dataset->size;
            gradients->dW1[i][j] = temp_mult;
        }
        temp_sum /= dataset->size;
        gradients->db1[i] = temp_sum;
    }
    return gradients;
}

void stochastich_gradient_descent(parameters_t *parameters, const gradients_t *gradients)
{
    // Updating parameters W1, b1, W2, d2
    for (int i = 0; i < NB_NEURONS; i++)
    {
        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            parameters->W1[i][j] -= gradients->dW1[i][j] * LEARNING_RATE;
        }
        parameters->b1[i] -= gradients->db1[i] * LEARNING_RATE;
    }
    for (int i = 0; i < NB_CLASSES; i++)
    {
        for (int j = 0; j < NB_NEURONS; j++)
        {
            parameters->W2[i][j] -= gradients->dW2[i][j] * LEARNING_RATE;
        }
        parameters->b2[i] -= gradients->db2[i] * LEARNING_RATE;
    }
}

void output_vector_print(const in_vector_t output_vector)
{
    for (int i = 0; i < NB_CLASSES; i++)
    {
        fprintf(stdout, "%f ", output_vector.Z2[i]);
    }
    fprintf(stdout, "\n");
}
uint8_t get_prediction(const in_vector_t output_vector)
{
    float max = output_vector.A2[0];
    //output_vector_print(output_vector);
    uint8_t index = 0;
    for (int i = 1; i < NB_CLASSES; i++)
    {
        if (output_vector.A2[i] > max)
        {
            max = output_vector.A2[i];
            index = i;
        }
    }
    // fprintf(stdout, "%" PRIu8 "\n", index);
    return index;
}

float accuracy(const dataset_t *dataset, const in_matrix_t *output)
{
    float temp_sum = 0;
    for (size_t i = 0; i < dataset->size; i++)
    {
        if (get_prediction(output->vectors[i]) == dataset->images[i].label)
        {
            temp_sum++;
        }
    }
    return temp_sum / dataset->size;
}

void train(const dataset_t *dataset)
{
    parameters_t *parameters = init_parameters();
    in_matrix_t output;
    output.vectors = malloc(dataset->size * sizeof(in_vector_t));
    back_matrix_t back_parameters;
    back_parameters.vectors = malloc(dataset->size * sizeof(back_vector_t));
    for (int i = 0; i < 1000; i++)
    {
        feed_forward(&output, parameters, dataset);

        fprintf(stdout, "Iteration %d : accuracy : %f\n", i, accuracy(dataset, &output));

        gradients_t *gradients = back_propagation(&output, dataset, parameters, &back_parameters); // Changer la def pour opti
        stochastich_gradient_descent(parameters, gradients);
        free(gradients);
    }
    free(back_parameters.vectors);
    free(output.vectors);
    free(parameters);
}