#ifndef POLYNOMIAL_OPERATIONS_H
#define POLYNOMIAL_OPERATIONS_H
#include "encoding_misc.h"

typedef struct
{
    int64_t *coeffs;
    size_t degree;
} polynomial_t;

extern void encoded_pol_add(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2);

extern encoded_polynomial_t *encoded_pol_mult(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, int64_t scaling_factor);

extern polynomial_t *polyonimal_init(size_t size);

extern void polynomial_free(polynomial_t *pol);

extern void polynomial_print(polynomial_t *pol);

extern void encoded_pol_add_modulo(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, int64_t modulo);

extern encoded_polynomial_t *encoded_pol_mult_modulo(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, int64_t scaling_factor, int64_t modulo);

extern void encoded_pol_add_polynomial_modulo(encoded_polynomial_t *pol1, const polynomial_t *pol2, int64_t modulo);

extern encoded_polynomial_t *encoded_pol_mult_polynomial_modulo(const encoded_polynomial_t *pol1, const polynomial_t *pol2, int64_t modulo);

void encoded_pol_scalar_mult(encoded_polynomial_t *pol, int64_t scalar);

extern void encoded_pol_scalar_mult_modulo(encoded_polynomial_t *pol, int64_t scalar, int64_t modulo);

extern polynomial_t *polynomial_mult(const polynomial_t *pol1, const polynomial_t *pol2);

extern polynomial_t *polynomial_mult_modulo(const polynomial_t *pol1, const polynomial_t *pol2, int64_t modulo);

extern void polynomial_scalar_mult(polynomial_t *pol, int64_t scalar);

extern void polynomial_scalar_mult_modulo(polynomial_t *pol, int64_t scalar, int64_t modulo);

extern void encoded_pol_scalar_div(encoded_polynomial_t *scaled_pol, int64_t scalar);

extern void encoded_pol_add_polynomial(encoded_polynomial_t *pol1, const polynomial_t *pol2);

extern void div_and_round_encoded_polynomial(encoded_polynomial_t *pol, int64_t divisor);

extern void div_and_round_polynomial(polynomial_t *pol, int64_t divisor);

extern encoded_polynomial_t *encoded_pol_mult_modulo_no_rescaling(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, int64_t modulo);

extern polynomial_t *encoded_to_polynomial(encoded_polynomial_t *encoded);

extern encoded_polynomial_t *encoded_pol_mult_polynomial(const encoded_polynomial_t *pol1, const polynomial_t *pol2);

extern void polynomial_add(polynomial_t *pol1, const polynomial_t *pol2);

extern void polynomial_add_modulo(polynomial_t *pol1, const polynomial_t *pol2, int64_t modulo);

#endif /*POLYNOMIAL_OPERATIONS_H*/