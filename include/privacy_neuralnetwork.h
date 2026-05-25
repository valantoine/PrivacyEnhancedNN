#ifndef PRIVACY_NEURALNETWORK_H
#define PRIVACY_NEURALNETWORK_H
#include "CKKS_operations.h"

#define POL_DEGREE 2 //terrible security // Can't change it now because code is not adapted yet
#define MULTIPLICATIVE_LEVEL_MAX 3 //i.e we can do 3 homomorphic multiplications (adapated for the network)
#define PRECISION_VALUE 512
#define SCALING_FACTOR_BIT_SIZE 20
#define MODULO_0_BIT_SIZE 80  //Must be way bigger than scaling_factor_size



typedef struct
{
    encoded_polynomial_t *encoded_cell;
    size_t row_size;
    size_t col_size;
} encoded_matrix_t;

typedef struct
{
    ciphered_t *ciphered_matrix;
    size_t row_size;
    size_t col_size;
} ciphered_matrix_t;

#include "privacy_neuralnetwork.h"

encoded_matrix_t *encoded_matrix_init(size_t rows, size_t cols);

void encoded_matrix_free(encoded_matrix_t *mat);

encoded_polynomial_t *encoded_matrix_get(encoded_matrix_t *mat, size_t i, size_t j);

void encoded_matrix_set(encoded_matrix_t *mat, size_t i, size_t j, encoded_polynomial_t *pol);

ciphered_matrix_t *ciphered_matrix_init(size_t rows, size_t cols);

void ciphered_matrix_free(ciphered_matrix_t *mat);

ciphered_t *ciphered_matrix_get(ciphered_matrix_t *mat, size_t i, size_t j);

void ciphered_matrix_set(ciphered_matrix_t *mat, size_t i, size_t j, ciphered_t *c);

extern ciphered_matrix_t *encrypt_image(image_t image, polynomial_t *secret_key, int verbose, mpz_t precision_factor, mpz_t scaling_factor, mpz_t modulo_full, gmp_randstate_t state);

extern uint8_t decrypt_prediction(ciphered_matrix_t *c, polynomial_t *secret_key, int verbose, mpz_t precision_factor, mpz_t scaling_factor, mpz_t modulo_0);

extern ciphered_matrix_t *ciphered_feed_forward(ciphered_matrix_t *input, const nn_parameters_t *parameters, ciphered_t *precomputed_evk_g, mpz_t precomputed_g, mpz_t modulo_full, mpz_t scaling_factor, mpz_t precision_factor);

extern uint8_t predict_image_private(dataset_t *test_dataset, const nn_parameters_t *parameters, polynomial_t *secret_key, int verbose, gmp_randstate_t state);

#endif /*PRIVACY_NEURALNETWORK_H*/