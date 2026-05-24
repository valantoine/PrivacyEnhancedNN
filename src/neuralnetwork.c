#include "neuralnetwork.h"

void relu(forward_vector_t *vector)
{
    for (int i = 0; i < NB_NEURONS; i++)
    {
        // if (vector->Z1[i] > 0)
        // {
        //     vector->A1[i] = vector->Z1[i];
        // }
        // else
        // {
        //     vector->A1[i] = 0;
        // }
        vector->A1[i] = vector->Z1[i] * vector->Z1[i];
    }
}

void softmax(forward_vector_t *vector)
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

void feed_forward(forward_matrix_t *output, const nn_parameters_t *parameters, const dataset_t *dataset)
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
    // if (f >= 0)
    // {
    //     return 1;
    // }
    // else
    // {
    //     return 0;
    // }
    float z = 2*f;
    return z;
}

void back_propagation(const forward_matrix_t *output, const dataset_t *dataset, const nn_parameters_t *parameters, back_matrix_t *back_parameters, gradients_t *gradients)
{
    // Computing dZ2 (can be done more efficiently)
    for (size_t k = 0; k < dataset->size; k++)
    {
        for (int j = 0; j < NB_CLASSES; j++)
        {

            if (j == dataset->images[k].label)
            {
                back_parameters->vectors[k].dZ2[j] = output->vectors[k].A2[j] - 1;
            }
            else
            {
                back_parameters->vectors[k].dZ2[j] = output->vectors[k].A2[j];
            }
        }
    }

    // Computing dW2 = dZ2 * A1.transpose
    for (size_t k = 0; k < dataset->size; k++)
    {
        for (int i = 0; i < NB_NEURONS; i++)
        {
            float A1_val = output->vectors[k].A1[i];
            for (int j = 0; j < NB_CLASSES; j++)
            {
                gradients->dW2[j][i] += back_parameters->vectors[k].dZ2[j] * A1_val;
            }
        }
    }
    for (int i = 0; i < NB_NEURONS; i++)
    {
        for (int j = 0; j < NB_CLASSES; j++)
        {
            gradients->dW2[j][i] /= dataset->size;
        }
    }

    // Computing db2 sum of dZ2 rows / size
    for (size_t k = 0; k < dataset->size; k++)
    {
        for (int j = 0; j < NB_CLASSES; j++)
        {
            gradients->db2[j] += back_parameters->vectors[k].dZ2[j];
        }
    }
    for (int j = 0; j < NB_CLASSES; j++)
    {
        gradients->db2[j] /= dataset->size;
    }

    // Computing dZ1 = W2.transpose * dZ2 and every elements multiplied by ReLU'(Z1)
    for (size_t k = 0; k < dataset->size; k++)
    {
        for (int j = 0; j < NB_CLASSES; j++)
        {
            float dZ2_val = back_parameters->vectors[k].dZ2[j];
            for (int i = 0; i < NB_NEURONS; i++)
            {

                back_parameters->vectors[k].dZ1[i] += parameters->W2[j][i] * dZ2_val;
            }
        }
        for (int i = 0; i < NB_NEURONS; i++)
        {
            back_parameters->vectors[k].dZ1[i] *= relu_deriv(output->vectors[k].Z1[i]);
        }
    }

    // Computing dW1 = dZ1 * dataset / size
    for (size_t k = 0; k < dataset->size; k++)
    {
        for (int i = 0; i < NB_NEURONS; i++)
        {
            float dZ1_val = back_parameters->vectors[k].dZ1[i];
            for (int j = 0; j < IMAGE_SIZE; j++)
            {
                gradients->dW1[i][j] += dZ1_val * dataset->images[k].image[j];
            }
        }
    }
    for (int i = 0; i < NB_NEURONS; i++)
    {

        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            gradients->dW1[i][j] /= dataset->size;
        }
    }

    // computing db1 = sum of dZ1 rows / size
    for (size_t k = 0; k < dataset->size; k++)
    {
        for (int i = 0; i < NB_NEURONS; i++)
        {
            gradients->db1[i] += back_parameters->vectors[k].dZ1[i];
        }
    }
    for (int i = 0; i < NB_NEURONS; i++)
    {
        gradients->db1[i] /= dataset->size;
    }
}

void stochastich_gradient_descent(nn_parameters_t *parameters, const gradients_t *gradients)
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

void output_vector_print(const forward_vector_t output_vector)
{
    for (int i = 0; i < NB_CLASSES; i++)
    {
        fprintf(stdout, "%f ", output_vector.Z2[i]);
    }
    fprintf(stdout, "\n");
}

uint8_t get_prediction(const forward_vector_t output_vector)
{
    float max = output_vector.A2[0];
    // output_vector_print(output_vector);
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

float accuracy(const dataset_t *dataset, const forward_matrix_t *output)
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

float cross_entropy_loss(const dataset_t *dataset, const forward_matrix_t *output)
{
    float loss = 0;
    for (size_t i = 0; i < dataset->size; i++)
    {
        uint8_t label = dataset->images[i].label;
        loss += logf(output->vectors[i].A2[label]);
    }
    return (-loss / dataset->size);
}

nn_parameters_t *train(dataset_t *dataset)
{

    // Initialization of parameters
    nn_parameters_t *parameters = init_parameters();
    if (parameters == NULL)
    {
        return NULL;
    }

    // Allocating feed forward output memory for one BATCH
    forward_matrix_t output;
    output.vectors = malloc(BATCH_SIZE * sizeof(forward_vector_t));
    if (output.vectors == NULL)
    {
        fprintf(stderr, "Failed to alloc output.vectors in train");
        return NULL;
    }

    // Allocating memory for back propagation parameters for one BATCH
    back_matrix_t back_parameters;
    back_parameters.vectors = malloc(BATCH_SIZE * sizeof(back_vector_t));
    if (back_parameters.vectors == NULL)
    {
        fprintf(stderr, "Failed to alloc back_parameters.vectors in train");
        return NULL;
    }

    // Allocating feed forward output memory for all the dataset (to measure accuracy at the end of one epoch)
    forward_matrix_t output_all;
    output_all.vectors = malloc(dataset->size * sizeof(forward_vector_t));
    if (output_all.vectors == NULL)
    {
        fprintf(stderr, "Failed to alloc output_all.vectors in train");
        return NULL;
    }

    // Allocating memory for gradients used in back propagations
    gradients_t *gradients = malloc(sizeof(gradients_t));
    if (gradients == NULL)
    {
        fprintf(stderr, "Failed to alloc gradients in train");
        return NULL;
    }

    size_t nb_batches = dataset->size / BATCH_SIZE;
    image_t *original_image = dataset->images; // Keep address of the images array
    size_t original_size = dataset->size;

    for (int epoch = 0; epoch < NB_EPOCHS; epoch++)
    {
        // Shuffle dataset to pick different batch at every epoch
        dataset_shuffle(dataset);

        for (size_t b = 0; b < nb_batches; b++)
        {
            // Temporary modification to keep only a batch in dataset
            dataset->images = original_image + b * BATCH_SIZE;
            dataset->size = BATCH_SIZE;

            feed_forward(&output, parameters, dataset);
            memset(gradients, 0, sizeof(gradients_t));                                 // Reset gradient memory from previous back_propagation
            memset(back_parameters.vectors, 0, dataset->size * sizeof(back_vector_t)); // Reset back_propagation parameters
            back_propagation(&output, dataset, parameters, &back_parameters, gradients);
            stochastich_gradient_descent(parameters, gradients);
        }
        if (original_size % BATCH_SIZE != 0)
        {
            // Remainder batch which is not exactly of size BATCH_SIZE
            dataset->images = original_image + nb_batches * BATCH_SIZE;
            dataset->size = (original_size % BATCH_SIZE);
            feed_forward(&output, parameters, dataset);
            memset(gradients, 0, sizeof(gradients_t));
            memset(back_parameters.vectors, 0, dataset->size * sizeof(back_vector_t));
            back_propagation(&output, dataset, parameters, &back_parameters, gradients);
            stochastich_gradient_descent(parameters, gradients);
        }
        // Restore original image address
        dataset->images = original_image;
        dataset->size = original_size;
        feed_forward(&output_all, parameters, dataset); // long operation to measure accuracy on the full dataset
        fprintf(stdout, "Epoch %d | Accuracy : %f | Cross-entropy Loss : %f\n", epoch, accuracy(dataset, &output_all), cross_entropy_loss(dataset, &output_all));
    }
    free(output_all.vectors);
    free(gradients);
    free(back_parameters.vectors);
    free(output.vectors);
    return parameters;
}

// prediction for one random image in dataset
uint8_t *predict_image(dataset_t *test_dataset, const nn_parameters_t *parameters)
{
    // Allocating feed forward output memory for one image
    forward_matrix_t output;
    uint8_t *prediction = (malloc(sizeof(uint8_t)));
    if (prediction == NULL)
    {
        fprintf(stderr, "Failed to allocate prediction");
        return NULL;
    }
    output.vectors = malloc(sizeof(forward_vector_t));
    if (output.vectors == NULL)
    {
        fprintf(stderr, "Failed to alloc output.vectors in train");
        return NULL;
    }
    image_t *original_image = test_dataset->images; // Keep address of the images array
    size_t original_size = test_dataset->size;
    int random_image_index = rand() % TEST_SIZE; // Pick a random number
    test_dataset->images = original_image + random_image_index;
    test_dataset->size = 1;
    fprintf(stdout, "Image picked : \n");
    image_print(&test_dataset->images[0], stdout);
    feed_forward(&output, parameters, test_dataset);

    *prediction = get_prediction(output.vectors[0]);
    free(output.vectors);
    test_dataset->images = original_image;
    test_dataset->size = original_size;
    return prediction;
}