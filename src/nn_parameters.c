#include "nn_parameters.h"


// Important step in our case : uniform distribution between [-0.5, 0.5]
nn_parameters_t *init_parameters()
{
    nn_parameters_t *parameters = (malloc(sizeof(nn_parameters_t)));
    if (parameters == NULL)
    {
        fprintf(stderr, "Failed to allocate parameters");
        return NULL;
    }
    for (int i = 0; i < NB_NEURONS; i++)
    {
        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            parameters->W1[i][j] = (float)rand() / RAND_MAX - 0.5f; //random float in [-0.5, 0.5]
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

void nn_parameters_print(const nn_parameters_t *parameters, FILE *fd)
{
    for (int i = 0; i < NB_NEURONS; i++)
    {
        for (int j = 0; j < IMAGE_SIZE - 1; j++)
        {
            fprintf(fd, "%f ", parameters->W1[i][j]);
        }
    }
    fprintf(fd, "%f\n", parameters->W1[NB_NEURONS - 1][IMAGE_SIZE - 1]);

    for (int i = 0; i < NB_NEURONS - 1; i++)
    {
        fprintf(fd, "%f ", parameters->b1[i]);
    }
    fprintf(fd, "%f\n", parameters->b1[NB_NEURONS - 1]);

    for (int i = 0; i < NB_CLASSES; i++)
    {
        for (int j = 0; j < NB_NEURONS - 1; j++)
        {
            fprintf(fd, "%f ", parameters->W2[i][j]);
        }
    }
    fprintf(fd, "%f\n", parameters->W2[NB_CLASSES - 1][NB_NEURONS - 1]);
    for (int i = 0; i < NB_CLASSES - 1; i++)
    {
        fprintf(fd, "%f ", parameters->b2[i]);
    }
    fprintf(fd, "%f\n", parameters->b2[NB_CLASSES - 1]);
    fclose(fd);
}

uint8_t nn_parameters_write(const nn_parameters_t *parameters, FILE *fd)
{
    if (1!=fwrite(parameters, sizeof(nn_parameters_t), 1, fd))
    {
        fprintf(stderr, "Failed to write");
        fclose(fd);
        return 0;
    }
    fclose(fd);
    return 1;
}

nn_parameters_t *nn_parameters_get(FILE *fd)
{
    nn_parameters_t *parameters = (malloc(sizeof(nn_parameters_t)));
    if (parameters == NULL)
    {
        fprintf(stderr, "Failed to allocate parameters");
        return NULL;
    }
    if (1!=fread(parameters, sizeof(nn_parameters_t), 1, fd))
    {
        fprintf(stderr, "Failed to read");
        return NULL;
    }
    fclose(fd);
    return parameters;
}
