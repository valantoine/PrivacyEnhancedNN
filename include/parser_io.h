#ifndef PARSER_IO_H
#define PARSER_IO_H
#include "nn_parameters.h"
#include <getopt.h>


typedef enum{
    BENCHMARK_NO,
    BENCHMARK_YES
} is_benchmark_t;

typedef enum{
    PRIVACY_NO,
    PRIVACY_YES
} is_privacy_t;

typedef enum{
    MODE_NOT_SPECIFIED,
    PREDICT_MODE,
    TRAINING_MODE
} which_mode_t;



typedef struct{
    is_benchmark_t benchmark;
    is_privacy_t privacy;
    which_mode_t mode;
    FILE *input_parameters;
    uint16_t predict_number;
} user_parameters_t;

extern user_parameters_t *parser_io(int argc, char *argv[]);

extern void parameters_free(user_parameters_t *user_parameters);

#endif /*PARSER_IO_H*/