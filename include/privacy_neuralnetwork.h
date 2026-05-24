#ifndef PRIVACY_NEURALNETWORK_H
#define PRIVACY_NEURALNETWORK_H
#include "CKKS_operations.h"

#define POL_DEGREE 2

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

ciphered_matrix_t *encrypt_image(image_t image, polynomial_t *secret_key, int verbose, mpz_t precision_factor, mpz_t scaling_factor, mpz_t modulo_full, gmp_randstate_t state);

uint8_t decrypt_prediction(ciphered_matrix_t *c, polynomial_t *secret_key, int verbose, mpz_t precision_factor, mpz_t scaling_factor, mpz_t modulo_0);


#endif /*PRIVACY_NEURALNETWORK_H*/