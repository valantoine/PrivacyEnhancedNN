#ifndef CKKS_ENCODING_H
#define CKKS_ENCODING_H
#include "encoding_misc.h"
#include "polynomial_operations.h"
#include <gmp.h> //used for later with big integers
#ifndef M_PI
#define M_PI 3.1415926535897932384626433
#endif

// Poorly optimized
extern complex_matrix_t *mult_sigma_basis_anti_identity(size_t size);

extern encoded_polynomial_t *complex_vector_encode(complex_vector_t *vector, complex_matrix_t *precomputed_sigma_basis, mpz_t scaling_factor);

extern complex_matrix_t *sigma_basis_tilde_etoile_init(size_t size);

extern complex_vector_t *recover_vector(encoded_polynomial_t *encoded_pol, complex_matrix_t *sigma_basis_etoile, mpz_t scaling_factor);

extern void float_encode(const float a, complex_vector_t *v, encoded_polynomial_t *encoded, complex_matrix_t *precomputed_sigma_basis, const mpz_t scaling_factor);

float float_decode(encoded_polynomial_t *encoded_pol, complex_matrix_t *sigma_basis_etoile, mpz_t scaling_factor);

#endif /*CKKS_ENCODING_H*/