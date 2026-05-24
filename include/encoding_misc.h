#ifndef ENCODING_MISC_H
#define ENCODING_MISC_H
#include <complex.h>
#include "preprocess_data.h"
#include "neuralnetwork.h"
#include <gmp.h> //used for later with big integers
typedef struct
{
    float complex *vector;
    size_t size;
} complex_vector_t;


typedef struct
{
    float complex *matrix;
    size_t size;
} complex_matrix_t;

typedef struct
{
    mpz_t *coeffs; 
    size_t size;
} encoded_polynomial_t;

extern void encoded_polynomial_print(encoded_polynomial_t *pol);

extern complex_vector_t *complex_vector_init(size_t size);

extern void complex_vector_free(complex_vector_t *cv);

extern complex_matrix_t *complex_matrix_init(size_t size);

extern void complex_matrix_free(complex_matrix_t *cm);

extern encoded_polynomial_t *encoded_pol_init(size_t size);

extern void encoded_pol_free(encoded_polynomial_t *pol);

// to correct
extern float complex complex_matrix_get(complex_matrix_t *cm, uint16_t i, uint16_t j);

extern void complex_matrix_set(complex_matrix_t *cm, uint16_t i, uint16_t j, float complex z);

extern void float_complex_print(float complex z);

extern void complex_matrix_print(complex_matrix_t *complex_matrix);

extern void complex_vector_print(complex_vector_t *vector);

extern void encoded_pol_init_in_place(encoded_polynomial_t *pol, size_t size);

extern void float_to_complex_vector(float a, complex_vector_t *v);

extern float complex_vector_to_float(complex_vector_t *v);

#endif /*ENCODING_MISC_H*/