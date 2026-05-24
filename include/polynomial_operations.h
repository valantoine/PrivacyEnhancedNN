#ifndef POLYNOMIAL_OPERATIONS_H
#define POLYNOMIAL_OPERATIONS_H
#include "encoding_misc.h"

typedef struct
{
    mpz_t *coeffs;
    size_t degree;
} polynomial_t;

extern void encoded_pol_add(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2);

extern encoded_polynomial_t *encoded_pol_mult_rescaled(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, mpz_t scaling_factor);

extern polynomial_t *polynomial_init(size_t size);

extern void polynomial_free(polynomial_t *pol);

extern void polynomial_print(polynomial_t *pol);

extern void encoded_pol_add_modulo(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, mpz_t modulo);

extern encoded_polynomial_t *encoded_pol_mult_rescaled_modulo(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, mpz_t scaling_factor, mpz_t modulo);

extern void encoded_pol_add_polynomial_modulo(encoded_polynomial_t *pol1, const polynomial_t *pol2, mpz_t modulo);

extern encoded_polynomial_t *encoded_pol_mult_polynomial_modulo(const encoded_polynomial_t *pol1, const polynomial_t *pol2, mpz_t modulo);

void encoded_pol_scalar_mult(encoded_polynomial_t *pol, mpz_t scalar);

extern void encoded_pol_scalar_mult_modulo(encoded_polynomial_t *pol, mpz_t scalar, mpz_t modulo);

extern polynomial_t *polynomial_mult(const polynomial_t *pol1, const polynomial_t *pol2);

extern polynomial_t *polynomial_mult_modulo(const polynomial_t *pol1, const polynomial_t *pol2, mpz_t modulo);

extern void polynomial_scalar_mult(polynomial_t *pol, mpz_t scalar);

extern void polynomial_scalar_mult_modulo(polynomial_t *pol, mpz_t scalar, mpz_t modulo);

extern void encoded_pol_scalar_div_exact(encoded_polynomial_t *scaled_pol, mpz_t scalar);

extern void encoded_pol_add_polynomial(encoded_polynomial_t *pol1, const polynomial_t *pol2);

extern void div_and_round_encoded_polynomial(encoded_polynomial_t *pol, mpz_t divisor);

extern void div_and_round_polynomial(polynomial_t *pol, mpz_t divisor);

extern encoded_polynomial_t *encoded_pol_mult_modulo_no_rescaling(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, mpz_t modulo);

extern polynomial_t *encoded_to_polynomial(encoded_polynomial_t *encoded);

extern encoded_polynomial_t *encoded_pol_mult_polynomial(const encoded_polynomial_t *pol1, const polynomial_t *pol2);

extern void polynomial_add(polynomial_t *pol1, const polynomial_t *pol2);

extern void polynomial_add_modulo(polynomial_t *pol1, const polynomial_t *pol2, mpz_t modulo);

extern void mpz_center_mod(mpz_t result, mpz_t a, mpz_t q);

extern void polynomial_eval_modulo(polynomial_t *pol, mpz_t modulo);

extern void encoded_polynomial_eval_modulo(encoded_polynomial_t *pol, mpz_t modulo);

#endif /*POLYNOMIAL_OPERATIONS_H*/