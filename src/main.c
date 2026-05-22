#include "main.h"
int main(int argc, char *argv[])
{
    
    test_encode_decode();
    test_add_encode_decode();
    test_mult_encode_decode();
    srand(time(NULL));
    user_parameters_t *user_parameters = parser_io(argc, argv);
    if (user_parameters == NULL)
    {
        return 0;
    }
    dataset_t *train_dataset = dataset_read(IMAGES_TRAIN_PATH, LABELS_TRAIN_PATH, TRAIN_SIZE);
    if (train_dataset == NULL)
    {
        parameters_free(user_parameters);

        return 0;
    }
    dataset_t *test_dataset = dataset_read(IMAGES_TEST_PATH, LABELS_TEST_PATH, TEST_SIZE);
    if (test_dataset == NULL)
    {
        parameters_free(user_parameters);
        dataset_free(train_dataset);
        return 0;
    }
    if (user_parameters->mode == TRAINING_MODE)
    {
        nn_parameters_t *parameters = train(train_dataset);
        if (parameters == NULL)
        {
            dataset_free(train_dataset);
            dataset_free(test_dataset);
            free(user_parameters);
            return 0;
        }
        uint8_t error = nn_parameters_write(parameters, user_parameters->input_parameters);
        if (error ==0){
            dataset_free(train_dataset);
            dataset_free(test_dataset);
            free(parameters);
            return 0;
        }
        
    }
    if (user_parameters->mode == PREDICT_MODE)
    {
        nn_parameters_t *parameters = nn_parameters_get(user_parameters->input_parameters);
        if (parameters == NULL)
        {
            dataset_free(train_dataset);
            dataset_free(test_dataset);
            free(user_parameters);
            return 0;
        }
        for (uint8_t i = 0; i < user_parameters->predict_number; i++)
        {
            uint8_t *prediction = predict_image(test_dataset, parameters);
            if (prediction == NULL)
            {
                dataset_free(train_dataset);
                dataset_free(test_dataset);
                free(parameters);
                free(user_parameters);
                return 0;
            }
            fprintf(stdout, "\n Predicted label : %" PRIu8 "\n\n\n", *prediction);
            free(prediction);
        }
        free(parameters);
    }
    dataset_free(train_dataset);
    dataset_free(test_dataset);
    free(user_parameters);
    return 1;
}