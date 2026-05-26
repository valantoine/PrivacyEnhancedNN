#include "main.h"
//

int main(int argc, char *argv[])
{
    srand(time(NULL));
    // tst_mult_consec();
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    user_parameters_t *user_parameters = parser_io(argc, argv);
    if (user_parameters == NULL)
    {
        return 0;
    }
    dataset_t *test_dataset = dataset_read(IMAGES_TEST_PATH, LABELS_TEST_PATH, TEST_SIZE);
    if (test_dataset == NULL)
    {
        parameters_free(user_parameters);
        return 0;
    }
    if (user_parameters->mode == TRAINING_MODE)
    {
        dataset_t *train_dataset = dataset_read(IMAGES_TRAIN_PATH, LABELS_TRAIN_PATH, TRAIN_SIZE);
        if (train_dataset == NULL)
        {
            parameters_free(user_parameters);

            return 0;
        }
        nn_parameters_t *parameters = train(train_dataset);
        if (parameters == NULL)
        {
            dataset_free(train_dataset);
            dataset_free(test_dataset);
            free(user_parameters);
            return 0;
        }
        uint8_t error = nn_parameters_write(parameters, user_parameters->input_parameters);
        if (error == 0)
        {
            dataset_free(train_dataset);
            dataset_free(test_dataset);
            free(parameters);
            return 0;
        }
        dataset_free(train_dataset);
    }
    if (user_parameters->mode == PREDICT_MODE)
    {

        nn_parameters_t *parameters = nn_parameters_get(user_parameters->input_parameters);
        if (parameters == NULL)
        {
            dataset_free(test_dataset);
            free(user_parameters);
            return 0;
        }
        float accuracy = 0;
        float startTime;
        float endTime;
        float timeElapsed;

        startTime = (float)clock() / CLOCKS_PER_SEC;
        if (user_parameters->privacy == PRIVACY_YES)
        {
            polynomial_t *secret_key = key_generation(POL_DEGREE, state);
            fprintf(stdout, " Key : \n");
            polynomial_print(secret_key);
            for (uint16_t i = 0; i < user_parameters->predict_number; i++)
            {
                uint8_t *prediction = predict_image_private(test_dataset, parameters, secret_key, state);
                fprintf(stdout, "\n Predicted label : %" PRIu8 "\n\n\n", prediction[1]);
                if (prediction[1] == prediction[0])
                {
                    accuracy++;
                }
                free(prediction);
            }
            polynomial_free(secret_key);
        }
        else
        {
            for (uint16_t i = 0; i < user_parameters->predict_number; i++)
            {

                uint8_t *prediction = predict_image(test_dataset, parameters);
                if (prediction == NULL)
                {
                    dataset_free(test_dataset);
                    free(parameters);
                    free(user_parameters);
                    return 0;
                }
                fprintf(stdout, "\n Predicted label : %" PRIu8 "\n\n\n", prediction[1]);
                if (prediction[1] == prediction[0])
                {
                    accuracy++;
                }
                free(prediction);
            }
        }
        endTime = (float)clock() / CLOCKS_PER_SEC;
        timeElapsed = endTime - startTime;
        accuracy /= user_parameters->predict_number;
        fprintf(stdout, " Accuracy : %f\n", accuracy);
        fprintf(stdout, "Exec time for %u predictions : %f\n", user_parameters->predict_number, timeElapsed);
        free(parameters);
    }
    dataset_free(test_dataset);
    free(user_parameters);
    gmp_randclear(state);
    return 1;
}