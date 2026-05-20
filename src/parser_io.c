#include "parser_io.h"

// print_help() : print help commands for parser
void print_help()
{
    fprintf(stdout, "NAME\n\n");
    fprintf(stdout, "Privacy Enhanced Neural Network\n\n");
    fprintf(stdout, "SYNOPSIS\n\n");
    fprintf(stdout, "Usage     ./penn { [-b] [-e] [-t | -p | -h] [-i FILE] [-n NUMBER] }\n\n");
    fprintf(stdout, "DESCRIPTIION\n\n");
    fprintf(stdout, "Implementation of a Privacy Enhanced Neural Network using MNIST dataset and FHE scheme\n\n");
    fprintf(stdout, "OPTIONS\n\n");
    fprintf(stdout, "-b, --benchmark              (OPTIONAL) run the program in benchmark mode to measure time of execution \n");
    fprintf(stdout, "-e, --encrypted              (OPTIONAL) run the program in privacy mode\n");
    fprintf(stdout, "-t, --train                  train the neural network using the MNIST dataset \n");
    fprintf(stdout, "-p , --predict               Predict N random image picked from the MNIST test dataset using a given parameter file \n");
    fprintf(stdout, "-i FILE, --input FILE        input parameters file. In case of training --> write trained parameters in this file | In case of predict --> use parameters in this file to predict\n");
    fprintf(stdout, "-n NUMBER, --number NUMBER   (OPTIONAL) Number of images you want to predict from test dataset (default : 1) max : 255\n");
    fprintf(stdout, "-h, --help                   display this help and exit\n");
}

void parameters_free(user_parameters_t *user_parameters)
{
    if (user_parameters->input_parameters != NULL)
    {
        fclose(user_parameters->input_parameters);
    }
    free(user_parameters);
}

user_parameters_t *parser_io(int argc, char *argv[])
{
    user_parameters_t *user_parameters = malloc(sizeof(user_parameters_t));
    user_parameters->benchmark = BENCHMARK_NO;
    user_parameters->privacy = PRIVACY_NO;
    user_parameters->mode = MODE_NOT_SPECIFIED;
    user_parameters->predict_number = 1;
    user_parameters->input_parameters = NULL;
    static struct option long_options[] = {
        {"benchmark", no_argument, 0, 'b'},
        {"encrypted", no_argument, 0, 'e'},
        {"train", no_argument, 0, 't'},
        {"predict", no_argument, 0, 'p'},
        {"input", required_argument, 0, 'i'},
        {"number", required_argument, 0, 'n'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}};

    int opt = 0;
    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "betpi:n:h", long_options, &long_index)) != -1)
    {
        switch (opt)
        {
        case 'h':
            print_help();
            parameters_free(user_parameters);
            return NULL;
            break;
        case 'b':
            user_parameters->benchmark = BENCHMARK_YES;
            break;
        case 'e':
            user_parameters->privacy = PRIVACY_YES;
            break;
        case 't':
            if (user_parameters->mode == PREDICT_MODE)
            {
                fprintf(stderr, "error : conflicting mode prediction and training");
                parameters_free(user_parameters);
                return NULL;
            }
            user_parameters->mode = TRAINING_MODE;
            break;
        case 'p':
            if (user_parameters->mode == TRAINING_MODE)
            {
                fprintf(stderr, "error : conflicting mode prediction and training");
                parameters_free(user_parameters);
                return NULL;
            }
            user_parameters->mode = PREDICT_MODE;
            break;
        case 'n':
            user_parameters->predict_number = (uint8_t)strtol(optarg, NULL, 10);
            break;
        case 'i':
            if (user_parameters->input_parameters != NULL)
            {
                fprintf(stderr, "an input has already been specified, exiting!!\n");
                parameters_free(user_parameters);
                return NULL;
            }
            if (user_parameters->mode == MODE_NOT_SPECIFIED)
            {
                fprintf(stderr, "you must give the mode before the input file\n");
                parameters_free(user_parameters);
                return NULL;
            }
            if (user_parameters->mode == PREDICT_MODE)
            {
                user_parameters->input_parameters = fopen(optarg, "rb+");
                if (user_parameters->input_parameters == NULL)
                {
                    fprintf(stderr, "Could not open file : %s\n", optarg);
                    parameters_free(user_parameters);
                    return NULL;
                }
                // Check file size
                fseek(user_parameters->input_parameters, 0, SEEK_END);
                size_t file_size = ftell(user_parameters->input_parameters);
                fseek(user_parameters->input_parameters, 0, SEEK_SET); // Place cursor at the beginning of file
                if (file_size != sizeof(nn_parameters_t))
                {
                    fprintf(stderr, "Invalid file size : %ld bytes instead of %zu\n", file_size, sizeof(nn_parameters_t));
                    return NULL;
                }
            }
            if (user_parameters->mode == TRAINING_MODE)
            {
                user_parameters->input_parameters = fopen(optarg, "wb+");
                if (user_parameters->input_parameters == NULL)
                {
                    fprintf(stderr, "Could not open file : %s\n", optarg);
                    parameters_free(user_parameters);
                    return NULL;
                }
            }
            break;
        }
    }
    if (optind != argc)
    {
        fprintf(stderr, "error: wrong arguments\n");
        parameters_free(user_parameters);
        return NULL;
    }
    if (user_parameters->mode == MODE_NOT_SPECIFIED)
    {
        fprintf(stderr, "error: no mode given");
        parameters_free(user_parameters);
        return NULL;
    }
    if (user_parameters->input_parameters == NULL)
    {
        fprintf(stderr, "error: no input file given");
        parameters_free(user_parameters);
        return NULL;
    }
    return user_parameters;
}